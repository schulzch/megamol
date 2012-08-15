//
// SphereRendererMouse.h
//
// Copyright (C) 2012 by University of Stuttgart (VISUS).
// All rights reserved.
//

#ifndef MMPROTEINPLUGIN_SPHERERENDERERMOUSE_H_INCLUDED
#define MMPROTEINPLUGIN_SPHERERENDERERMOUSE_H_INCLUDED

#if (defined(_MSC_VER) && (_MSC_VER > 1000))
#pragma once
#endif /* (defined(_MSC_VER) && (_MSC_VER > 1000)) */

#include "ModuleAutoDescription.h"
#include "Renderer3DModuleMouse.h"
#include "Call.h"
#include "view/MouseFlags.h"
#include "vislib/GLSLShader.h"
#include "vislib/CameraParameters.h"
#include "MolecularDataCall.h"
#include "CallerSlot.h"
#include "param/ParamSlot.h"
#include "vislib/Vector.h"

namespace megamol {
namespace protein {

/**
 * A simple 3D sphere renderer to demonstrate mouse functionality
 */
class SphereRendererMouse : public Renderer3DModuleMouse {
public:

	/**
	 * The class name for the factory
	 *
	 * @return The class name
	 */
	static const char *ClassName(void) {
		return "SphereRendererMouse";
	}

	/**
	 * A human-readable description string for the module
	 *
	 * @return The description string
	 */
	static const char *Description(void) {
		return "3D renderer to test mouse functionality";
	}

	/**
	 * Test if the module can be instanziated
	 *
	 * @return 'true'
	 */
	static bool IsAvailable(void) {
		return true;
	}

	/**
	 * Disallow usage in quickstarts
	 *
	 * @return false
	 */
	static bool SupportQuickstart(void) {
		return false;
	}

	/**
	 * ctor
	 */
	SphereRendererMouse();

	/**
	 * dtor
	 */
	virtual ~SphereRendererMouse();

protected:

	/**
	 * Initializes the module directly after instanziation
	 *
	 * @return 'true' on success
	 */
	virtual bool create(void);

    /**
     * The get capabilities callback. The module should set the members
     * of 'call' to tell the caller its capabilities.
     *
     * @param[in] call The calling call.
     * @return The return value of the function.
     */
    virtual bool GetCapabilities(megamol::core::Call& call);

    /**
     * The get extents callback. The module should set the members of
     * 'call' to tell the caller the extents of its data (bounding boxes
     * and times).
     *
     * @param[in] call The calling call.
     * @return The return value of the function.
     */
    virtual bool GetExtents(megamol::core::Call& call);

    /**
     * The render callback.
     *
     * @param[in] call The calling call.
     * @return The return value of the function.
     */
    virtual bool Render(megamol::core::Call& call);

	/**
	 * Callback for mouse events (move, press, and release)
	 *
	 * @param[in] x The x coordinate of the mouse in screen space
	 * @param[in] y The y coordinate of the mouse in screen space
	 * @param[in] flags The mouse flags
	 * @return 'true' on success
	 */
	virtual bool MouseEvent(int x, int y, core::view::MouseFlags flags);

	/**
	 * Releases all resources of the module
	 */
	virtual void release(void);

private:

    /// The data caller slot
    core::CallerSlot molDataCallerSlot;

    /// Parameter slot for the sphere radius
    core::param::ParamSlot sphereRadSclParam;

	/// The current mouse coordinates
	int mouseX, mouseY;

	/// Camera information
	vislib::SmartPtr<vislib::graphics::CameraParameters> cameraInfo;

	/// The shader for raycasting spheres
	vislib::graphics::gl::GLSLShader sphereShader;

	/// Array to determine which atoms are selected
	vislib::Array<bool> atomSelect;

	/// Array with atom colors
	vislib::Array<float> atomColor;

	/// Starting point for the selecting rectangle
	vislib::math::Vector<int, 2> startSelect;

	/// End point for the selecting rectangle
	vislib::math::Vector<int, 2> endSelect;

	/// Toggle whether mouse is in drag mode
	bool drag;

	/// The current rectangle representing the selection (start)
	vislib::math::Vector<int, 2> startSelectCurr;

	/// The current rectangle representing the selection (end)
	vislib::math::Vector<int, 2> endSelectCurr;

	/// Starting point of the rectangle in object space
	double startSelectCurrObj[3];

	/// End point of the rectangle in object space
	double endSelectCurrObj[3];

	/// Flag whether filter has to be used
	bool filter;
};

} // end namespace protein
} // end namespace megamol


#endif // MMPROTEINPLUGIN_SPHERERENDERERMOUSE_H_INCLUDED
