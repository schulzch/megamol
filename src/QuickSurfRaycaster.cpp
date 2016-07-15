/*
 *	QuickSurfRaycaster.cpp
 *
 *	Copyright (C) 2016 by Universitaet Stuttgart (VISUS).
 *	All rights reserved
 */

#include "stdafx.h"

//#define _USE_MATH_DEFINES 1

#include "QuickSurfRaycaster.h"
#include "mmcore/CoreInstance.h"
#include <gl/GLU.h>

#include "mmcore/param/IntParam.h"
#include "mmcore/param/FloatParam.h"
#include "mmcore/param/StringParam.h"

#include "vislib/StringTokeniser.h"

#include <channel_descriptor.h>
#include <driver_functions.h>

using namespace megamol;
using namespace megamol::core;
using namespace megamol::core::moldyn;
using namespace megamol::protein_cuda;
using namespace megamol::protein_calls;



// Load raw data from disk
void *loadRawFile(char *filename, size_t size)
{
	FILE *fp = fopen(filename, "rb");

	if (!fp)
	{
		fprintf(stderr, "Error opening file '%s'\n", filename);
		return 0;
	}

	void *data = malloc(size);
	size_t read = fread(data, 1, size, fp);
	fclose(fp);

	printf("Read '%s', %d bytes\n", filename, (int)read);

	size_t count = size / sizeof(unsigned char);
	size_t fsize = count * sizeof(float);
	void *floatdata = malloc(fsize);
	for (size_t i = 0; i < count; i++) {
		((float*)floatdata)[i] = ((float)(((unsigned char*)data)[i])) / 255.0f;
	}

	return floatdata;
}


/*
 *	QuickSurfRaycaster::QuickSurfRaycaster
 */
QuickSurfRaycaster::QuickSurfRaycaster(void) : Renderer3DModule(),
	particleDataSlot("getData", "Connects the surface renderer with the particle data storage"),
	qualityParam("quicksurf::quality", "Quality"),
	radscaleParam("quicksurf::radscale", "Radius scale"),
	gridspacingParam("quicksurf::gridspacing", "Grid spacing"),
	isovalParam("quicksurf::isoval", "Isovalue"),
	selectedIsovals("quicksurf::selectedIsovals", "Semicolon seperated list of normalized isovalues we want to ray cast the isoplanes from"),
	scalingFactor("quicksurf::scalingFactor", "Scaling factor for the density values and particle radii"),
	setCUDAGLDevice(true),
	particlesSize(0)
{
	this->particleDataSlot.SetCompatibleCall<MultiParticleDataCallDescription>();
	this->particleDataSlot.SetCompatibleCall<MolecularDataCallDescription>();
	this->MakeSlotAvailable(&this->particleDataSlot);

	this->qualityParam.SetParameter(new param::IntParam(1, 0, 4));
	this->MakeSlotAvailable(&this->qualityParam);

	this->radscaleParam.SetParameter(new param::FloatParam(1.0f, 0.0f));
	this->MakeSlotAvailable(&this->radscaleParam);

	//this->gridspacingParam.SetParameter(new param::FloatParam(0.01953125f, 0.0f));
	this->gridspacingParam.SetParameter(new param::FloatParam(0.1f, 0.0f));
	this->MakeSlotAvailable(&this->gridspacingParam);

	this->isovalParam.SetParameter(new param::FloatParam(0.5f, 0.0f));
	this->MakeSlotAvailable(&this->isovalParam);

	this->selectedIsovals.SetParameter(new param::StringParam("0.5,0.9"));
	this->MakeSlotAvailable(&this->selectedIsovals);
	this->selectedIsovals.ForceSetDirty(); // necessary for initial update

	this->scalingFactor.SetParameter(new param::FloatParam(1.0f, 0.0f));
	this->MakeSlotAvailable(&this->scalingFactor);

	lastViewport.Set(0, 0);

	volumeExtent = make_cudaExtent(0, 0, 0);

	cudaqsurf = nullptr;
	cudaImage = nullptr;
	volumeArray = nullptr;
	particles = nullptr;
	texHandle = 0;
}

/*
 *	QuickSurfRaycaster::~QuickSurfRaycaster
 */
QuickSurfRaycaster::~QuickSurfRaycaster(void) {
	if (cudaqsurf) {
		CUDAQuickSurf *cqs = (CUDAQuickSurf *)cudaqsurf;
		delete cqs;
		cqs = nullptr;
		cudaqsurf = nullptr;
	}
	
	this->Release();
}

/*
 *	QuickSurfRaycaster::release
 */
void QuickSurfRaycaster::release(void) {

}

bool QuickSurfRaycaster::calcVolume(float3 bbMin, float3 bbMax, float* positions, int quality, float radscale, float gridspacing,
	float isoval, float minConcentration, float maxConcentration, bool useCol) {

	float x = bbMax.x - bbMin.x;
	float y = bbMax.y - bbMin.y;
	float z = bbMax.z - bbMin.z;

	int numVoxels[3];
	numVoxels[0] = (int)ceil(x / gridspacing);
	numVoxels[1] = (int)ceil(y / gridspacing);
	numVoxels[2] = (int)ceil(z / gridspacing);

	x = (numVoxels[0] - 1) * gridspacing;
	y = (numVoxels[1] - 1) * gridspacing;
	z = (numVoxels[2] - 1) * gridspacing;

	//printf("vox %i %i %i \n", numVoxels[0], numVoxels[1], numVoxels[2]);

	volumeExtent = make_cudaExtent(numVoxels[0], numVoxels[1], numVoxels[2]);

	float gausslim = 2.0f;
	switch (quality) { // TODO adjust values
	case 3: gausslim = 4.0f; break;
	case 2: gausslim = 3.0f; break;
	case 1: gausslim = 2.5f; break;
	case 0:
	default: gausslim = 2.0f; break;
	}

	float origin[3] = { bbMin.x, bbMin.y, bbMin.z };

	if (cudaqsurf == NULL) {
		cudaqsurf = new CUDAQuickSurf();
	}

	CUDAQuickSurf *cqs = (CUDAQuickSurf*)cudaqsurf;

	int result = -1;
	result = cqs->calc_map((long)numParticles, positions, colorTable.data(), 1, 
	//result = cqs->calc_map((long)numParticles, positions, NULL, 0,
		origin, numVoxels, maxConcentration, radscale, gridspacing, 
		//origin, numVoxels, 2.0f, radscale, gridspacing,
		isoval, gausslim);

	checkCudaErrors(cudaDeviceSynchronize());

	volumeExtent = make_cudaExtent(cqs->getMapSizeX(), cqs->getMapSizeY(), cqs->getMapSizeZ());

	return (result == 0);
}

/*
 *	QuickSurfRaycaster::create
 */
bool QuickSurfRaycaster::create(void) {
	return initOpenGL();
}

/*
 *	QuickSurfRaycaster::GetCapabilities
 */
bool QuickSurfRaycaster::GetCapabilities(Call& call) {
	view::AbstractCallRender3D *cr3d = dynamic_cast<view::AbstractCallRender3D *>(&call);
	if (cr3d == NULL) return false;

	cr3d->SetCapabilities(view::AbstractCallRender3D::CAP_RENDER
		| view::AbstractCallRender3D::CAP_LIGHTING
		| view::AbstractCallRender3D::CAP_ANIMATION);

	return true;
}

/*
 *	QuickSurfRaycaster::GetExtents
 */
bool QuickSurfRaycaster::GetExtents(Call& call) {
	view::AbstractCallRender3D *cr3d = dynamic_cast<view::AbstractCallRender3D *>(&call);
	if (cr3d == NULL) return false;

	MultiParticleDataCall *mpdc = this->particleDataSlot.CallAs<MultiParticleDataCall>();
	MolecularDataCall *mdc = this->particleDataSlot.CallAs<MolecularDataCall>();

	if (mpdc == NULL && mdc == NULL) return false;

	// MultiParticleDataCall in use
	if (mpdc != NULL) {
		if (!(*mpdc)(1)) return false;

		float scale;
		if (!vislib::math::IsEqual(mpdc->AccessBoundingBoxes().ObjectSpaceBBox().LongestEdge(), 0.0f)) {
			scale = 2.0f / mpdc->AccessBoundingBoxes().ObjectSpaceBBox().LongestEdge();
		}
		else {
			scale = 1.0f;
		}
		cr3d->AccessBoundingBoxes() = mpdc->AccessBoundingBoxes();
		cr3d->AccessBoundingBoxes().MakeScaledWorld(scale);
		cr3d->SetTimeFramesCount(mpdc->FrameCount());
	} // MolecularDataCall in use
	else if (mdc != NULL) {
		if (!(*mdc)(1)) return false;

		float scale;
		if (!vislib::math::IsEqual(mdc->AccessBoundingBoxes().ObjectSpaceBBox().LongestEdge(), 0.0f)) {
			scale = 2.0f / mdc->AccessBoundingBoxes().ObjectSpaceBBox().LongestEdge();
		}
		else {
			scale = 1.0f;
		}
		cr3d->AccessBoundingBoxes() = mdc->AccessBoundingBoxes();
		cr3d->AccessBoundingBoxes().MakeScaledWorld(scale);
		cr3d->SetTimeFramesCount(mdc->FrameCount());
	}

	return true;
}

/*
 *	QuickSurfRaycaster::initCuda
 */
bool QuickSurfRaycaster::initCuda(view::CallRender3D& cr3d) {
	
	// set cuda device
	if (setCUDAGLDevice) {
#ifdef _WIN32
		if (cr3d.IsGpuAffinity()) {
			HGPUNV gpuId = cr3d.GpuAffinity<HGPUNV>();
			int devId;
			cudaWGLGetDevice(&devId, gpuId);
			cudaGLSetGLDevice(devId);
		}
		else {
			cudaGLSetGLDevice(cudaUtilGetMaxGflopsDeviceId());
		}
#else
		cudaGLSetGLDevice(cudaUtilGetMaxGflopsDeviceId());
#endif
		cudaError err = cudaGetLastError();
		if (err != 0) {
			printf("cudaGLSetGLDevice: %s\n", cudaGetErrorString(err));
			return false;
		}
		setCUDAGLDevice = false;
	}

	//char *volumeFilename = "T:\MegaMol\Megamol_Binaries\x64\Debug\Bucky.raw";
	//cudaExtent volumeSize;
	//volumeSize.width = 32;
	//volumeSize.height = 32;
	//volumeSize.depth = 32;
	//size_t size = volumeSize.width * volumeSize.height * volumeSize.depth * sizeof(unsigned char);
	//void *h_volume = loadRawFile(volumeFilename, size);
	//if (!h_volume)  return false;
	//// create 3D array
	//cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc<float>();
	//checkCudaErrors(cudaMalloc3DArray(&tmpCudaArray, &channelDesc, volumeSize, cudaArraySurfaceLoadStore));
	//// copy data to 3D array
	//cudaMemcpy3DParms copyParams = { 0 };
	//copyParams.srcPtr = make_cudaPitchedPtr(h_volume, volumeSize.width*sizeof(float), volumeSize.width, volumeSize.height);
	//copyParams.dstArray = tmpCudaArray;
	//copyParams.extent = volumeSize;
	//copyParams.kind = cudaMemcpyHostToDevice;
	//checkCudaErrors(cudaMemcpy3D(&copyParams));

	return true;
}

/*
 *	QuickSurfRaycaster::initPixelBuffer
 */
bool QuickSurfRaycaster::initPixelBuffer(view::CallRender3D& cr3d) {

	auto viewport = cr3d.GetViewport().GetSize();

	if (lastViewport == viewport) {
		return true;
	} else {
		lastViewport = viewport;
	}

	if (!texHandle) {
		glGenTextures(1, &texHandle);
		glActiveTexture(GL_TEXTURE15);
		glBindTexture(GL_TEXTURE_2D, texHandle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, viewport.GetWidth(), viewport.GetHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	if (cudaImage) {
		checkCudaErrors(cudaFreeHost(cudaImage));
		cudaImage = NULL;
	}

	checkCudaErrors(cudaMallocHost((void**)&cudaImage, viewport.GetWidth() * viewport.GetHeight() * sizeof(unsigned int)));

	return true;
}

/*
 *	QuickSurfRaycaster::initOpenGL
 */
bool QuickSurfRaycaster::initOpenGL() {

	Vertex v0(-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	Vertex v1(-1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	Vertex v2(1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
	Vertex v3(1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);

	std::vector<Vertex> verts = { v0, v2, v1, v3 };

	glGenVertexArrays(1, &textureVAO);
	glGenBuffers(1, &textureVBO);

	glBindVertexArray(textureVAO);
	glBindBuffer(GL_ARRAY_BUFFER, textureVBO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)* verts.size(), verts.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, r));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	using namespace vislib::sys;
	using namespace vislib::graphics::gl;

	ShaderSource vertSrc;
	ShaderSource fragSrc;

	if (!this->GetCoreInstance()->ShaderSourceFactory().MakeShaderSource("quicksurfraycast::texture::textureVertex", vertSrc)) {
		Log::DefaultLog.WriteMsg(Log::LEVEL_ERROR, "Unable to load vertex shader source for texture shader");
		return false;
	}

	if (!this->GetCoreInstance()->ShaderSourceFactory().MakeShaderSource("quicksurfraycast::texture::textureFragment", fragSrc)) {
		Log::DefaultLog.WriteMsg(Log::LEVEL_ERROR, "Unable to load fragment shader source for texture shader");
		return false;
	}

	this->textureShader.Compile(vertSrc.Code(), vertSrc.Count(), fragSrc.Code(), fragSrc.Count());
	this->textureShader.Link();

	return true;
}

/*
 *	QuickSurfRaycaster::Render
 */
bool QuickSurfRaycaster::Render(Call& call) {
	view::CallRender3D *cr3d = dynamic_cast<view::CallRender3D *>(&call);
	if (cr3d == NULL) return false;

	this->cameraInfo = cr3d->GetCameraParameters();

	callTime = cr3d->Time();

	MultiParticleDataCall * mpdc = particleDataSlot.CallAs<MultiParticleDataCall>();
	MolecularDataCall * mdc = particleDataSlot.CallAs<MolecularDataCall>();

	float3 bbMin;
	float3 bbMax;

	float3 clipBoxMin;
	float3 clipBoxMax;

	float concMin = FLT_MAX;
	float concMax = FLT_MIN;

	if (mpdc == NULL && mdc == NULL) return false;

	if (mpdc != NULL) {
		mpdc->SetFrameID(static_cast<int>(callTime));
		if (!(*mpdc)(1)) return false;
		if (!(*mpdc)(0)) return false;

		numParticles = 0;
		for (unsigned int i = 0; i < mpdc->GetParticleListCount(); i++) {
			numParticles += mpdc->AccessParticles(i).GetCount();
		}

		if (numParticles == 0) {
			return true;
		}

		if (this->particlesSize < this->numParticles * 4) {
			if (this->particles) {
				delete[] this->particles;
				this->particles = nullptr;
			}
			this->particles = new float[this->numParticles * 4];
			this->particlesSize = this->numParticles * 4;
		}
		memset(this->particles, 0, this->numParticles * 4 * sizeof(float));

		UINT64 particleCnt = 0;
		this->colorTable.clear();
		this->colorTable.reserve(numParticles * 4);

		auto bb = mpdc->GetBoundingBoxes().ObjectSpaceBBox();
		bbMin = make_float3(bb.Left(), bb.Bottom(), bb.Back());
		bbMax = make_float3(bb.Right(), bb.Top(), bb.Front());
		bb = mpdc->GetBoundingBoxes().ClipBox();
		clipBoxMin = make_float3(bb.Left(), bb.Bottom(), bb.Back());
		clipBoxMax = make_float3(bb.Right(), bb.Top(), bb.Front());

		for (unsigned int i = 0; i < mpdc->GetParticleListCount(); i++) {
			MultiParticleDataCall::Particles &parts = mpdc->AccessParticles(i);
			const float *pos = static_cast<const float*>(parts.GetVertexData());
			const float *colorPos = static_cast<const float*>(parts.GetColourData());
			unsigned int posStride = parts.GetVertexDataStride();
			unsigned int colStride = parts.GetColourDataStride();
			float globalRadius = parts.GetGlobalRadius();
			//globalRadius /= 2.0f;
			bool useGlobRad = (parts.GetVertexDataType() == megamol::core::moldyn::MultiParticleDataCall::Particles::VERTDATA_FLOAT_XYZ);
			int numColors = 0;

			switch (parts.GetColourDataType()) {
			case MultiParticleDataCall::Particles::COLDATA_FLOAT_I: numColors = 1; break;
			case MultiParticleDataCall::Particles::COLDATA_FLOAT_RGB: numColors = 3; break;
			case MultiParticleDataCall::Particles::COLDATA_FLOAT_RGBA: numColors = 4; break;
			case MultiParticleDataCall::Particles::COLDATA_UINT8_RGB: numColors = 0; break; // TODO
			case MultiParticleDataCall::Particles::COLDATA_UINT8_RGBA: numColors = 0; break; // TODO
			}

			// if the vertices have no type, take the next list
			if (parts.GetVertexDataType() == megamol::core::moldyn::MultiParticleDataCall::Particles::VERTDATA_NONE) { 
				continue;
			}
			if (useGlobRad) { // TODO is this correct?
				if (posStride < 12) posStride = 12;
			}
			else {
				if (posStride < 16) posStride = 16;
			}

			for (UINT64 j = 0; j < parts.GetCount(); j++, pos = reinterpret_cast<const float*>(reinterpret_cast<const char*>(pos) + posStride),
				colorPos = reinterpret_cast<const float*>(reinterpret_cast<const char*>(colorPos) + colStride)) {

				particles[particleCnt * 4 + 0] = pos[0] - bbMin.x;
				particles[particleCnt * 4 + 1] = pos[1] - bbMin.y;
				particles[particleCnt * 4 + 2] = pos[2] - bbMin.z;
				if (useGlobRad) {
					//particles[particleCnt * 4 + 3] = colorPos[3]; // concentration

					//if (colorPos[3] < concMin) concMin = colorPos[3];
					//if (colorPos[3] > concMax) concMax = colorPos[3];

					particles[particleCnt * 4 + 3] = globalRadius;
				}
				else {
					particles[particleCnt * 4 + 3] = pos[3];
				}
				if (particles[particleCnt * 4 + 3] < concMin) concMin = particles[particleCnt * 4 + 3];
				if (particles[particleCnt * 4 + 3] > concMax) concMax = particles[particleCnt * 4 + 3];

				this->colorTable.push_back(1.0f);
				this->colorTable.push_back(1.0f);
				this->colorTable.push_back(1.0f);
				this->colorTable.push_back(1.0f);

				// TODO delete that, when possible
				if (numColors > 3)
					numColors = 3;

				for (int k = 0; k < numColors; k++) {
					for (int l = 0; l < 3 - k; l++) {
						this->colorTable[particleCnt * 3 + k + l] = colorPos[k];
					}
				}

				particleCnt++;
			}
		}

		glPushMatrix();
		float scale = 1.0f;
		if (!vislib::math::IsEqual(mpdc->AccessBoundingBoxes().ObjectSpaceBBox().LongestEdge(), 0.0f)) {
			scale = 2.0f / mpdc->AccessBoundingBoxes().ObjectSpaceBBox().LongestEdge();
		}
		glScalef(scale, scale, scale);

	} else if (mdc != NULL) {
		// TODO
		printf("MolecularDataCall currently not supported\n");
		return false;
	}

	initCuda(*cr3d);
	initPixelBuffer(*cr3d);

	float factor = scalingFactor.Param<param::FloatParam>()->Value();

	// normalize concentration
	//for (UINT64 i = 0; i < numParticles; i++) {
	//	particles[i * 4 + 3] = ((particles[i * 4 + 3] - concMin) / (concMax - concMin)) * factor;
	//}

	auto viewport = cr3d->GetViewport().GetSize();

    GLfloat m[16];
	GLfloat m_proj[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, m);
	glGetFloatv(GL_PROJECTION_MATRIX, m_proj);
    Mat4f modelMatrix(&m[0]);
	Mat4f projectionMatrix(&m_proj[0]);
    modelMatrix.Invert();
	projectionMatrix.Invert();
	float3 camPos = make_float3(modelMatrix.GetAt(0, 3), modelMatrix.GetAt(1, 3), modelMatrix.GetAt(2, 3));
	float3 camDir = norm(make_float3(-modelMatrix.GetAt(0, 2), -modelMatrix.GetAt(1, 2), -modelMatrix.GetAt(2, 2)));
	float3 camUp = norm(make_float3(modelMatrix.GetAt(0, 1), modelMatrix.GetAt(1, 1), modelMatrix.GetAt(2, 1)));
	float3 camRight = norm(make_float3(modelMatrix.GetAt(0, 0), modelMatrix.GetAt(1, 0), modelMatrix.GetAt(2, 0)));
	// the direction has to be negated because of the right-handed view space of OpenGL

	auto cam = cr3d->GetCameraParameters();

	float fovy = (float)(cam->ApertureAngle() * M_PI / 180.0f);

	float aspect = (float)viewport.GetWidth() / (float)viewport.GetHeight();
	if (viewport.GetHeight() == 0)
		aspect = 0.0f;

	float fovx = 2.0f * atan(tan(fovy / 2.0f) * aspect);
	float zNear = (2.0f * projectionMatrix.GetAt(2, 3)) / (2.0f * projectionMatrix.GetAt(2, 2) - 2.0f);

	float density = 0.05f;
	float brightness = 1.0f;
	float transferOffset = 0.0f;
	float transferScale = 1.0f;

	/*printf("min: %f %f %f \n", clipBoxMin.x, clipBoxMin.y, clipBoxMin.z);
	printf("max: %f %f %f \n\n", clipBoxMax.x, clipBoxMax.y, clipBoxMax.z);*/

	dim3 blockSize = dim3(8, 8);
	dim3 gridSize = dim3(iDivUp(viewport.GetWidth(), blockSize.x), iDivUp(viewport.GetHeight(), blockSize.y));

	if (cudaqsurf == NULL) {
		cudaqsurf = new CUDAQuickSurf();
	}

	// parse selected isovalues if needed
	if (selectedIsovals.IsDirty()) {
		std::vector<float> isoVals;
		vislib::TString valString = selectedIsovals.Param<param::StringParam>()->Value();
		vislib::StringA vala = T2A(valString);

		vislib::StringTokeniserA sta(vala, ',');
		while (sta.HasNext()) {
			vislib::StringA t = sta.Next();
			if (t.IsEmpty()) {
				continue;
			}
			isoVals.push_back((float)vislib::CharTraitsA::ParseDouble(t));
		}

		/*for (float f: isoVals)
			printf("value: %f\n", f);*/

		transferIsoValues(isoVals.data(), (int)isoVals.size());

		selectedIsovals.ResetDirty();
	}

	bool suc = this->calcVolume(bbMin, bbMax, particles, 
					this->qualityParam.Param<param::IntParam>()->Value(),
					this->radscaleParam.Param<param::FloatParam>()->Value(),
					this->gridspacingParam.Param<param::FloatParam>()->Value(),
					this->isovalParam.Param<param::FloatParam>()->Value(), 
					concMin, concMax, true);

	//if (!suc) return false;

	CUDAQuickSurf * cqs = (CUDAQuickSurf*)cudaqsurf;
	float * map = cqs->getMap();

	//printf("size: %i %i %i\n", cqs->getMapSizeX(), cqs->getMapSizeY(), cqs->getMapSizeZ());

	transferNewVolume(map, volumeExtent);

	//if (!suc)
		render_kernel(gridSize, blockSize, cudaImage, viewport.GetWidth(), viewport.GetHeight(), fovx, fovy, camPos, camDir, camUp, camRight, zNear, density, brightness, transferOffset, transferScale, bbMin, bbMax);
	//else
		//renderArray_kernel(cqs->getMap(), gridSize, blockSize, cudaImage, viewport.GetWidth(), viewport.GetHeight(), fovx, fovy, camPos, camDir, camUp, camRight, zNear, density, brightness, transferOffset, transferScale, bbMin, bbMax, dim3(cqs->getMapSizeX(), cqs->getMapSizeY(), cqs->getMapSizeZ()));
	
	getLastCudaError("kernel failed");

	checkCudaErrors(cudaDeviceSynchronize());

	glActiveTexture(GL_TEXTURE15);
	glBindTexture(GL_TEXTURE_2D, texHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, viewport.GetWidth(), viewport.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, cudaImage);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glDisable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);

	textureShader.Enable();
	
	glBindVertexArray(textureVAO);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
	textureShader.Disable();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	if (mpdc)
		mpdc->Unlock();
	if (mdc)
		mdc->Unlock();

	return true;
}