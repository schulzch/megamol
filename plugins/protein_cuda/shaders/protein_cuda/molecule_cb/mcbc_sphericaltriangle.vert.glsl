#version 130

#include "protein_cuda/molecule_cb/mcbc_common.glsl"

uniform vec4 viewAttr; // TODO: check fragment position if viewport starts not in (0, 0)
uniform vec3 zValues;
uniform vec3 fogCol;
uniform vec2 texOffset;

#ifndef CALC_CAM_SYS
uniform vec3 camIn;
uniform vec3 camUp;
uniform vec3 camRight;
#endif // CALC_CAM_SYS
uniform mat4 mvp;
uniform mat4 invview;

attribute vec4 attribVec1;
attribute vec4 attribVec2;
attribute vec4 attribVec3;
attribute vec3 attribColors;
attribute vec3 attribTexCoord1;
attribute vec3 attribTexCoord2;
attribute vec3 attribTexCoord3;

varying vec4 objPos;
varying vec4 camPos;
varying vec4 lightPos;

varying vec4 inVec1;
varying vec4 inVec2;
varying vec4 inVec3;
varying vec3 inColors;
varying vec3 texCoord1;
varying vec3 texCoord2;
varying vec3 texCoord3;

void main(void) {
    inVec1 = attribVec1;
    inVec2 = attribVec2;
    inVec3 = attribVec3;
    inColors = attribColors;
    
    texCoord1 = attribTexCoord1;
    texCoord2 = attribTexCoord2;
    texCoord3 = attribTexCoord3;
    
    // remove the sphere radius from the w coordinates to the rad varyings
    vec4 inPos = gl_Vertex;
    inVec1.w = inPos.w;
    inVec2.w = inPos.w * inPos.w;
    inPos.w = 1.0;
    
    // DEBUG
    //texCoord2.xy = vec2( attribVec1.w, attribVec2.w);
    
    // object pivot point in object space    
    objPos = inPos; // no w-div needed, because w is 1.0 (Because I know)
    
    // calculate cam position
    camPos = invview[3]; // (C) by Christoph
    camPos.xyz -= objPos.xyz; // cam pos to glyph space
    
    // calculate light position in glyph space
    // USE THIS LINE TO GET POSITIONAL LIGHTING
    //lightPos = invview * gl_LightSource[0].position - objPos;
    // USE THIS LINE TO GET DIRECTIONAL LIGHTING
    lightPos = invview * normalize( gl_LightSource[0].position);
    
    // Sphere-Touch-Plane-Approach
    vec2 winHalf = 2.0 / viewAttr.zw; // window size
    
    vec2 d, p, q, h, dd;
    
    // get camera orthonormal coordinate system
    vec4 tmp;
    
    #ifdef CALC_CAM_SYS
    // camera coordinate system in object space
    tmp = invview[3] + invview[2];
    vec3 camIn = normalize(tmp.xyz);
    tmp = invview[3] + invview[1];
    vec3 camUp = tmp.xyz;
    vec3 camRight = normalize(cross(camIn, camUp));
    camUp = cross(camIn, camRight);
    #endif // CALC_CAM_SYS
    
    vec2 mins, maxs;
    vec3 testPos;
    vec4 projPos;
    
    // projected camera vector
    vec3 c2 = vec3(dot(camPos.xyz, camRight), dot(camPos.xyz, camUp), dot(camPos.xyz, camIn));
    
    vec3 cpj1 = camIn * c2.z + camRight * c2.x;
    vec3 cpm1 = camIn * c2.x - camRight * c2.z;
    
    vec3 cpj2 = camIn * c2.z + camUp * c2.y;
    vec3 cpm2 = camIn * c2.y - camUp * c2.z;
    
    d.x = length(cpj1);
    d.y = length(cpj2);
    
    dd = vec2(1.0) / d;
    
    p = inVec2.w * dd;
    q = d - p;
    h = sqrt(p * q);
    //h = vec2(0.0);
    
    p *= dd;
    h *= dd;
    
    cpj1 *= p.x;
    cpm1 *= h.x;
    cpj2 *= p.y;
    cpm2 *= h.y;
    
    // TODO: rewrite only using four projections, additions in homogenous coordinates and delayed perspective divisions.
    testPos = objPos.xyz + cpj1 + cpm1;
    projPos = mvp * vec4(testPos, 1.0);
    projPos /= projPos.w;
    mins = projPos.xy;
    maxs = projPos.xy;
    
    testPos -= 2.0 * cpm1;
    projPos = mvp * vec4(testPos, 1.0);
    projPos /= projPos.w;
    mins = min(mins, projPos.xy);
    maxs = max(maxs, projPos.xy);
    
    testPos = objPos.xyz + cpj2 + cpm2;
    projPos = mvp * vec4(testPos, 1.0);
    projPos /= projPos.w;
    mins = min(mins, projPos.xy);
    maxs = max(maxs, projPos.xy);
    
    testPos -= 2.0 * cpm2;
    projPos = mvp * vec4(testPos, 1.0);
    projPos /= projPos.w;
    mins = min(mins, projPos.xy);
    maxs = max(maxs, projPos.xy);
    
    gl_Position = vec4((mins + maxs) * 0.5, 0.0, 1.0);
    gl_PointSize = max((maxs.x - mins.x) * winHalf.x, (maxs.y - mins.y) * winHalf.y) * 0.5;
    
    #ifdef SMALL_SPRITE_LIGHTING
    // for normal crowbaring on very small sprites
    lightPos.w = (clamp(gl_PointSize, 1.0, 5.0) - 1.0) / 4.0;
    #endif // SMALL_SPRITE_LIGHTING
    
}
