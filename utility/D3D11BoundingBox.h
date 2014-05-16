/*
 * D3D11BoundingBox.h
 *
 * Copyright (C) 2013 by Visualisierungsinstitut der Universitšt Stuttgart.
 * Alle Rechte vorbehalten.
 */

#ifndef MEGAMOLCORE_D3D11BOUNDINGBOX_H_INCLUDED
#define MEGAMOLCORE_D3D11BOUNDINGBOX_H_INCLUDED
#if (defined(_MSC_VER) && (_MSC_VER > 1000))
#pragma once
#endif /* (defined(_MSC_VER) && (_MSC_VER > 1000)) */

#include "mmd3d.h"

#include "utility/AbstractD3D11RenderObject.h"
#include "utility/ShaderSourceFactory.h"

#include "vislib/Cuboid.h"
#include "vislib/ColourRGBAu8.h"
#include "vislib/graphicstypes.h"


namespace megamol {
namespace core {
namespace utility {

#ifdef MEGAMOLCORE_WITH_DIRECT3D11

    /**
     * Holds the resources for drawing a bounding box in Direct3D 11.
     */
    class MEGAMOLCORE_API D3D11BoundingBox : public AbstractD3D11RenderObject {

    public:

        D3D11BoundingBox(void);

        virtual ~D3D11BoundingBox(void);

        virtual HRESULT Draw(const bool frontSides = true);

        virtual HRESULT Finalise(void);

        virtual HRESULT Initialise(ID3D11Device *device,
            utility::ShaderSourceFactory& factory);

        // http://msdn.microsoft.com/en-us/library/windows/desktop/ee418728(v=vs.85).aspx#Call_Conventions
        virtual HRESULT Update(const XMMATRIX& viewMatrix,
            const XMMATRIX& projMatrix,
            const vislib::graphics::SceneSpaceCuboid& bbox,
            const vislib::graphics::ColourRGBAu8& colour);

    protected:

        typedef AbstractD3D11RenderObject Base;

        /** cbuffer structure from the shader. */
#pragma pack(push, 16)
        typedef struct Constants_t {
            XMMATRIX ViewMatrix;
            XMMATRIX ProjMatrix;
            XMFLOAT4 Colour;
        } Constants;
#pragma pack(pop)

        /** Constant buffer for per-frame constants. */
        ID3D11Buffer *cbConstants;

        /** The rasteriser state for back faces. */
        ID3D11RasterizerState *rasteriserStateBack;

        /** The rasteriser state for front faces. */
        ID3D11RasterizerState *rasteriserStateFront;

    };

#endif /* MEGAMOLCORE_WITH_DIRECT3D11 */

} /* end namespace utility */
} /* end namespace core */
} /* end namespace megamol */

#endif /* MEGAMOLCORE_D3D11BOUNDINGBOX_H_INCLUDED */
