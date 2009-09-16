/*
 * AbstractRectangularPyramidalFrustum.h
 *
 * Copyright (C) 2006 - 2009 by Visualisierungsinstitut Universitaet Stuttgart. 
 * Alle Rechte vorbehalten.
 */

#ifndef VISLIB_ABSTRACTRECTANGULARPYRAMIDALFRUSTUM_H_INCLUDED
#define VISLIB_ABSTRACTRECTANGULARPYRAMIDALFRUSTUM_H_INCLUDED
#if (defined(_MSC_VER) && (_MSC_VER > 1000))
#pragma once
#endif /* (defined(_MSC_VER) && (_MSC_VER > 1000)) */
#if defined(_WIN32) && defined(_MANAGED)
#pragma managed(push, off)
#endif /* defined(_WIN32) && defined(_MANAGED) */


#include "vislib/AbstractPyramidalFrustum.h"
#include "vislib/AbstractViewFrustum.h"
#include "vislib/memutils.h"
#include "vislib/Plane.h"
#include "vislib/Point.h"
#include "vislib/ShallowVector.h"
#include "vislib/Vector.h"


namespace vislib {
namespace math {


    /**
     * TODO: comment class
     */
    template<class T, class S> 
    class AbstractRectangularPyramidalFrustum 
            : public AbstractPyramidalFrustum<T> {

    public:

        /** Dtor. */
        virtual ~AbstractRectangularPyramidalFrustum(void);

        /**
         * TODO: Documentation
         */
        inline Point<T, 3> GetApex(void) const {
            VLSTACKTRACE("AbstractRectangularPyramidalFrustum::GetApex", 
                __FILE__, __LINE__);
            return Point<T, 3>(this->values + IDX_APEX_X);
        }

        /**
         * Answer the points that form the bottom base of the frustum.
         *
         * @param outPoints An array that will receive the points. All existing 
         *                  content will be replaced.
         */
        virtual void GetBottomBasePoints(
            vislib::Array<Point<T, 3> >& outPoints) const;

        /**
         * TODO: Documentation
         */
        inline Vector<T, 3> GetBaseNormal(void) const {
            VLSTACKTRACE("AbstractRectangularPyramidalFrustum::GetBaseNormal",
                __FILE__, __LINE__);
            return Vector<T, 3>(this->values + IDX_NORMAL_X);
        }

        /**
         * TODO: Documentation
         */
        inline const Plane<T>& GetBottomBase(void) const {
            VLSTACKTRACE("AbstractRectangularPyramidalFrustum::GetBottomBase",
                __FILE__, __LINE__);
            // Note: This is NOT a bug: The bottom base of the frustum is the
            // far clipping plane as seen from the apex.
            return this->fillPlaneCache(false)[IDX_FAR];
        }

        /**
         * TODO: Documentation
         */
        inline const Plane<T>& GetTopBase(void) const {
            VLSTACKTRACE("AbstractRectangularPyramidalFrustum::GetTopBase",
                __FILE__, __LINE__);
            // Note: This is NOT a bug: The bottom base of the frustum is the
            // near clipping plane as seen from the apex.
            return this->fillPlaneCache(false)[IDX_NEAR];
        }

        /**
         * Answer the points that form the top base of the frustum.
         *
         * @param outPoints An array that will receive the points. All existing 
         *                  content will be replaced.
         */
        virtual void GetTopBasePoints(
            vislib::Array<Point<T, 3> >& outPoints) const;

        /**
         * TODO: Documentation
         */
        inline Vector<T, 3> GetBaseUp(void) const {
            VLSTACKTRACE("AbstractRectangularPyramidalFrustum::GetBaseUp", 
                __FILE__, __LINE__);
            return Vector<T, 3>(this->values + IDX_UP_X);
        }

        /**
         * TODO: Documentation
         */
        void Set(const T left, const T right, 
                const T bottom, const T top, 
                const T zNear, const T zFar,
                const T ax, const T ay, const T az,
                const T nx, const T ny, const T nz,
                const T ux, const T uy, const T uz);

        /**
         * TODO: Documentation
         */
        template<class Sp1, class Sp2, class Sp3, class Sp4>
        void Set(const AbstractViewFrustum<T, Sp1>& frustum,
                const AbstractPoint<T, 3, Sp2>& apex,
                const AbstractVector<T, 3, Sp3>& baseNormal,
                const AbstractVector<T, 3, Sp4>& baseUp);

        /**
         * TODO: Documentation
         */
        template<class Sp>
        inline void SetApex(const AbstractPoint<T, 3, Sp>& apex) {
            VLSTACKTRACE("AbstractRectangularPyramidalFrustum::SetApex", 
                __FILE__, __LINE__);
            this->SetApex(apex.X(), apex.Y(), apex.Z());
        }

        /**
         * TODO: Documentation
         */
        inline void SetApex(const T x, const T y, const T z) {
            VLSTACKTRACE("AbstractRectangularPyramidalFrustum::SetApex", 
                __FILE__, __LINE__);
            this->values[IDX_APEX_X] = x;
            this->values[IDX_APEX_Y] = y;
            this->values[IDX_APEX_Z] = z;
        }

        /**
         * Set the normal of the base planes. The normal is assumed to point 
         * from the apex of the pyramid towards the two planes. It therefore
         * determines the orientation of the frustum.
         *
         * If necessary, the up vector is corrected to be perpendicular to
         * the normal.
         *
         * @param normal The normal vector of the top and bottom base. The 
         *               direction of the normal vector is from the apex of
         *               the pyramid to the planes. The vector will be 
         *               normalised by the method.
         */
        template<class Sp>
        inline void SetBaseNormal(const AbstractVector<T, 3, Sp>& normal) {
            VLSTACKTRACE("AbstractRectangularPyramidalFrustum::SetBaseNormal",
                __FILE__, __LINE__);
            this->SetBaseNormal(normal.X(), normal.Y(), normal.Z());
        }

        /**
         * TODO
         */
        void SetBaseNormal(const T x, const T y, const T z);

        /**
         * Set the up vector of the base planes. The up vector determines the
         * rotation around the normal vector. The up vector and the normal
         * vector must be perpendicular.
         *
         * @param up The up vector within the top or bottom base. The vector
         *           is corrected if it is not perpendicular to the base normal.
         */
        template<class Sp>
        inline void SetBaseUp(const AbstractVector<T, 3, Sp>& up) {
            VLSTACKTRACE("AbstractRectangularPyramidalFrustum::SetBaseUp",
                __FILE__, __LINE__);
            this->SetBaseUp(up.X(), up.Y(), up.Z());
        }

        /**
         * TODO
         */
        void SetBaseUp(const T x, const T y, const T z);

    protected:

        /** Superclass typedef. */
        typedef AbstractPyramidalFrustum<T> Super;

        /** The number of elements in 'offset'. */
        static const UINT_PTR CNT_ELEMENTS;

        /** Index of the x-coordinate of the apex of the pyramid. */
        static const UINT_PTR IDX_APEX_X;

        /** Index of the y-coordinate of the apex of the pyramid. */
        static const UINT_PTR IDX_APEX_Y;

        /** Index of the y-coordinate of the apex of the pyramid. */
        static const UINT_PTR IDX_APEX_Z;

        /** The index of the bottom plane offset. */
        static const UINT_PTR IDX_BOTTOM;

        /** The index of the far plane offset. */
        static const UINT_PTR IDX_FAR;

        /** The index of the left plane offset. */
        static const UINT_PTR IDX_LEFT;

        /** The index of the near plane offset. */
        static const UINT_PTR IDX_NEAR;

        /** The index of the x-coordinate of the bottom/top normal vector. */
        static const UINT_PTR IDX_NORMAL_X;

        /** The index of the x-coordinate of the bottom/top normal vector. */
        static const UINT_PTR IDX_NORMAL_Y;

        /** The index of the x-coordinate of the bottom/top normal vector. */
        static const UINT_PTR IDX_NORMAL_Z;

        /** The index of the right plane offset. */
        static const UINT_PTR IDX_RIGHT;

        /** The index of the top plane offset. */
        static const UINT_PTR IDX_TOP;

        /** The index of the x-coordinate of the up vector. */
        static const UINT_PTR IDX_UP_X;

        /** The index of the x-coordinate of the up vector. */
        static const UINT_PTR IDX_UP_Y;

        /** The index of the x-coordinate of the up vector. */
        static const UINT_PTR IDX_UP_Z;

        /** 
         * Disallow instances.
         */
        AbstractRectangularPyramidalFrustum(void);

        inline void invalidateCaches(void) {
            ARY_SAFE_DELETE(this->cachePoints);
            ARY_SAFE_DELETE(this->cachePlanes);
            ASSERT(this->cachePoints == NULL);
            ASSERT(this->cachePlanes == NULL);
        }

        //Point<T, 3> *fillPointCache(const bool forcheUpdate = false) const;

        /**
         * TODO Documentation
         */
        Plane<T> *fillPlaneCache(const bool forceUpdate = false) const;

        /**
         * TODO Documentation
         */
        ShallowVector<T, 3>& safeUpVector(ShallowVector<T, 3>& inOutUp);

        /**
         * Stores the values defining the frustum. This must provide at least
         * CNT_ELEMENT * sizeof(T) entries. Indices are given by the IDX_*
         * constants
         */
        S values;

    private:

        /** An array for caching the corner points of the frustum. */
        mutable Point<T, 3> *cachePoints;

        /** An array for caching the planes of the frustum. */
        mutable Plane<T> *cachePlanes;
    };


    /*
     * ...Frustum<T, S>::~AbstractRectangularPyramidalFrustum
     */
    template<class T, class S> 
    AbstractRectangularPyramidalFrustum<T, S>
            ::~AbstractRectangularPyramidalFrustum(void) {
        VLSTACKTRACE("AbstractRectangularPyramidalFrustum::"
            "AbstractRectangularPyramidalFrustum", __FILE__, __LINE__);
        // Must not do anything about 'values' due to the Crowbar pattern(TM).
        ARY_SAFE_DELETE(this->cachePoints);
        ARY_SAFE_DELETE(this->cachePlanes);
    }


    /*
     * AbstractRectangularPyramidalFrustum<T, S>::GetBottomBasePoints
     */
    template<class T, class S> 
    void AbstractRectangularPyramidalFrustum<T, S>::GetBottomBasePoints(
            vislib::Array<Point<T, 3> >& outPoints) const {
        VLSTACKTRACE("AbstractRectangularPyramidalFrustum::GetBottomBasePoints",
            __FILE__, __LINE__);
        ASSERT(this->GetBaseNormal().IsNormalised());

        Point<T, 3> center = this->GetApex() + this->GetBaseNormal() 
            * this->values[IDX_FAR];
        //Vector<T, 3> offset

        outPoints.Clear();

        // TODO
        throw MissingImplementationException("GetBottomBasePoints", __FILE__, __LINE__);
    }


    /*
     * AbstractRectangularPyramidalFrustum<T, S>::GetTopBasePoints
     */
    template<class T, class S> 
    void AbstractRectangularPyramidalFrustum<T, S>::GetTopBasePoints(
            vislib::Array<Point<T, 3> >& outPoints) const {
        VLSTACKTRACE("AbstractRectangularPyramidalFrustum::GetTopBasePoints",
            __FILE__, __LINE__);
        // TODO
        throw MissingImplementationException("GetTopBasePoints", __FILE__, __LINE__);
    }


    /*
     * AbstractRectangularPyramidalFrustum<T, S>::SetBaseNormal
     */
    template<class T, class S> 
    void AbstractRectangularPyramidalFrustum<T, S>::SetBaseNormal(
            const T x, const T y, const T z) {
        VLSTACKTRACE("AbstractRectangularPyramidalFrustum::SetBaseNormal",
            __FILE__, __LINE__);
        this->values[IDX_NORMAL_X] = x;
        this->values[IDX_NORMAL_Y] = y;
        this->values[IDX_NORMAL_Z] = z;
        ShallowVector<T, 3>(this->values + IDX_NORMAL_X).Normalise();
        this->safeUpVector(ShallowVector<T, 3>(this->values + IDX_UP_X));
    }


    /*
     * AbstractRectangularPyramidalFrustum<T, S>::SetBaseNormal
     */
    template<class T, class S> 
    void AbstractRectangularPyramidalFrustum<T, S>::SetBaseUp(
            const T x, const T y, const T z) {
        VLSTACKTRACE("AbstractRectangularPyramidalFrustum::SetBaseUp",
            __FILE__, __LINE__);
        this->values[IDX_UP_X] = x;
        this->values[IDX_UP_Y] = y;
        this->values[IDX_UP_Z] = z;
        this->safeUpVector(ShallowVector<T, 3>(this->values + IDX_UP_X));
    }


    /*
     * ...AbstractRectangularPyramidalFrustum<T, S>::CNT_ELEMENTS
     */
    template<class T, class S>
    const UINT_PTR AbstractRectangularPyramidalFrustum<T, S>::CNT_ELEMENTS = 15;


    /*
     * ...AbstractRectangularPyramidalFrustum<T, S>::IDX_APEX_X
     */
    template<class T, class S>
    const UINT_PTR AbstractRectangularPyramidalFrustum<T, S>::IDX_APEX_X = 6;


    /*
     * ...AbstractRectangularPyramidalFrustum<T, S>::IDX_APEX_Y
     */
    template<class T, class S>
    const UINT_PTR AbstractRectangularPyramidalFrustum<T, S>::IDX_APEX_Y = 7;

    
    /*
     * ...AbstractRectangularPyramidalFrustum<T, S>::IDX_APEX_Z
     */
    template<class T, class S>
    const UINT_PTR AbstractRectangularPyramidalFrustum<T, S>::IDX_APEX_Z = 8;


    /*
     * ...AbstractRectangularPyramidalFrustum<T, S>::IDX_BOTTOM
     */
    template<class T, class S>
    const UINT_PTR AbstractRectangularPyramidalFrustum<T, S>::IDX_BOTTOM = 0;


    /*
     * ...AbstractRectangularPyramidalFrustum<T, S>::IDX_FAR
     */
    template<class T, class S>
    const UINT_PTR AbstractRectangularPyramidalFrustum<T, S>::IDX_FAR = 4;


    /*
     * ...AbstractRectangularPyramidalFrustum<T, S>::IDX_LEFT
     */
    template<class T, class S>
    const UINT_PTR AbstractRectangularPyramidalFrustum<T, S>::IDX_LEFT = 2;


    /*
     * ...AbstractRectangularPyramidalFrustum<T, S>::IDX_NEAR
     */
    template<class T, class S>
    const UINT_PTR AbstractRectangularPyramidalFrustum<T, S>::IDX_NEAR = 5;


    /*
     * ...AbstractRectangularPyramidalFrustum<T, S>::IDX_NORMAL_X
     */    
    template<class T, class S>
    const UINT_PTR AbstractRectangularPyramidalFrustum<T, S>::IDX_NORMAL_X = 9;


    /*
     * ...AbstractRectangularPyramidalFrustum<T, S>::IDX_NORMAL_Y
     */    
    template<class T, class S>
    const UINT_PTR AbstractRectangularPyramidalFrustum<T, S>::IDX_NORMAL_Y = 10;


    /*
     * ...AbstractRectangularPyramidalFrustum<T, S>::IDX_NORMAL_Z
     */    
    template<class T, class S>
    const UINT_PTR AbstractRectangularPyramidalFrustum<T, S>::IDX_NORMAL_Z = 11;


    /*
     * ...AbstractRectangularPyramidalFrustum<T, S>::IDX_RIGHT
     */    
    template<class T, class S>
    const UINT_PTR AbstractRectangularPyramidalFrustum<T, S>::IDX_RIGHT = 3;


    /*
     * ...AbstractRectangularPyramidalFrustum<T, S>::IDX_TOP
     */
    template<class T, class S>
    const UINT_PTR AbstractRectangularPyramidalFrustum<T, S>::IDX_TOP = 1;


    /*
     * ...AbstractRectangularPyramidalFrustum<T, S>::IDX_UP_X
     */    
    template<class T, class S>
    const UINT_PTR AbstractRectangularPyramidalFrustum<T, S>::IDX_UP_X = 12;


    /*
     * ...AbstractRectangularPyramidalFrustum<T, S>::IDX_UP_Y
     */    
    template<class T, class S>
    const UINT_PTR AbstractRectangularPyramidalFrustum<T, S>::IDX_UP_Y = 13;


    /*
     * ...AbstractRectangularPyramidalFrustum<T, S>::IDX_UP_Z
     */    
    template<class T, class S>
    const UINT_PTR AbstractRectangularPyramidalFrustum<T, S>::IDX_UP_Z = 14;


    /*
     * ...Frustum<T, S>::~AbstractRectangularPyramidalFrustum
     */
    template<class T, class S> 
    AbstractRectangularPyramidalFrustum<T, S>
            ::AbstractRectangularPyramidalFrustum(void) 
            : Super(), cachePoints(NULL), cachePlanes(NULL) {
        VLSTACKTRACE("AbstractRectangularPyramidalFrustum::"
            "AbstractRectangularPyramidalFrustum", __FILE__, __LINE__);
    }


    /*
     * vislib::math::AbstractRectangularPyramidalFrustum<T, S>::Set
     */
    template<class T, class S>
    void AbstractRectangularPyramidalFrustum<T, S>::Set(
            const T left, const T right, 
            const T bottom, const T top, 
            const T zNear, const T zFar,
            const T ax, const T ay, const T az,
            const T nx, const T ny, const T nz,
            const T ux, const T uy, const T uz) {
        VLSTACKTRACE("AbstractRectangularPyramidalFrustum::Set", 
            __FILE__, __LINE__);

        this->values[IDX_BOTTOM] = bottom;
        this->values[IDX_TOP] = top;
        this->values[IDX_LEFT] = left;
        this->values[IDX_RIGHT] = right;
        this->values[IDX_NEAR] = zNear;
        this->values[IDX_FAR] = zFar;

        this->SetApex(ax, ay, az);
        this->SetBaseNormal(nx, ny, nz);
        this->SetBaseUp(ux, uy, uz);
    }


    /*
     * vislib::math::AbstractRectangularPyramidalFrustum<T, S>::Set
     */
    template<class T, class S>
    template<class Sp1, class Sp2, class Sp3, class Sp4>
    void AbstractRectangularPyramidalFrustum<T, S>::Set(
            const AbstractViewFrustum<T, Sp1>& frustum,
            const AbstractPoint<T, 3, Sp2>& apex,
            const AbstractVector<T, 3, Sp3>& baseNormal,
            const AbstractVector<T, 3, Sp4>& baseUp) {
        VLSTACKTRACE("AbstractRectangularPyramidalFrustum::Set", 
            __FILE__, __LINE__);
        this->Set(frustum.GetLeftDistance(), frustum.GetRightDistance(),
            frustum.GetBottomDistance(), frustum.GetTopDistance(),
            frustum.GetNearDistance(), frustum.GetFarDistance(),
            apex.X(), apex.Y(), apex.Z(),
            baseNormal.X(), baseNormal.Y(), baseNormal.Z(),
            baseUp.X(), baseUp.Y(), baseUp.Z());
    }


    /*
     * ...AbstractRectangularPyramidalFrustum<T, S>::fillPointCache
     */
    //template<class T, class S>
    //void AbstractRectangularPyramidalFrustum<T, S>::fillPointCache(
    //        const bool forcheUpdate) const {
    //    VLSTACKTRACE("AbstractRectangularPyramidalFrustum::fillPointCache",
    //        __FILE__, __LINE__);

    //    if (this->cachePoints == NULL) {
    //        this->cachePoints = new Point<T, 3>[8];

    //    } else if (!forceUpdate) {
    //        return;
    //    }
    //}


    /*
     * ...AbstractRectangularPyramidalFrustum<T, S>::fillPlaneCache
     */
    template<class T, class S>
    Plane<T> *AbstractRectangularPyramidalFrustum<T, S>::fillPlaneCache(
            const bool forceUpdate) const {
        VLSTACKTRACE("AbstractRectangularPyramidalFrustum::fillPlaneCache",
            __FILE__, __LINE__);

        if (this->cachePlanes == NULL) {
            this->cachePlanes = new Plane<T>[6];

        } else if (!forceUpdate) {
            return this->cachePlanes;
        }

        Point<T, 3> apex = this->GetApex();             // Cache apex.
        Vector<T, 3> normal = this->GetBaseNormal();    // Cache normal.
        Vector<T, 3> up = this->GetBaseUp();            // Cache up.
        Vector<T, 3> right = normal.Cross(up);          // Compute right.
        right.Normalise();
        ASSERT(normal.IsNormalised());
        ASSERT(up.IsNormalised());
        ASSERT(right.IsNormalised());

        /* Compute intersection of base normal with bases. */
        Point<T, 3> nearIntersect = apex + this->values[IDX_NEAR] * normal;
        Point<T, 3> farIntersect = apex + this->values[IDX_FAR] * normal;
        
        this->cachePlanes[IDX_NEAR].Set(nearIntersect, -normal);
        this->cachePlanes[IDX_FAR].Set(farIntersect, normal);

        //this->cachePlanes[IDX_BOTTOM].Set(

	//Vec3 aux,normal;

	//aux = (nc - Y*nh) - p;
	//aux.normalize();
	//normal = X * aux;
	//pl[BOTTOM].setNormalAndPoint(normal,nc-Y*nh);



	//aux = (nc + Y*nh) - p;
	//aux.normalize();
	//normal = aux * X;
	//pl[TOP].setNormalAndPoint(normal,nc+Y*nh);


	//
	//aux = (nc - X*nw) - p;
	//aux.normalize();
	//normal = aux * Y;
	//pl[LEFT].setNormalAndPoint(normal,nc-X*nw);

	//aux = (nc + X*nw) - p;
	//aux.normalize();
	//normal = Y * aux;
	//pl[RIGHT].setNormalAndPoint(normal,nc+X*nw);



        return this->cachePlanes;
    }


    /*
     * ...AbstractRectangularPyramidalFrustum<T, S>::safeUpVector
     */
    template<class T, class S>
    ShallowVector<T, 3>& AbstractRectangularPyramidalFrustum<T, S>
            ::safeUpVector(ShallowVector<T, 3>& inOutUp) {
        VLSTACKTRACE("AbstractRectangularPyramidalFrustum::safeUpVector",
            __FILE__, __LINE__);

        const ShallowVector<T, 3> normal(this->values + IDX_NORMAL_X);
        ASSERT(normal.IsNormalised());
        if (inOutUp.IsParallel(normal)) {
            // Cannot fix non-perpendicular up vector if vectors are parallel.
            throw IllegalParamException("inOutUp", __FILE__, __LINE__);
        }
        
        Vector<T, 3> right = normal.Cross(inOutUp);
        right.Normalise();
        inOutUp = right.Cross(normal);
        inOutUp.Normalise();
    }
    
} /* end namespace math */
} /* end namespace vislib */

#if defined(_WIN32) && defined(_MANAGED)
#pragma managed(pop)
#endif /* defined(_WIN32) && defined(_MANAGED) */
#endif /* VISLIB_ABSTRACTRECTANGULARPYRAMIDALFRUSTUM_H_INCLUDED */

