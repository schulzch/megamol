/*
 * OSPRayGeometryTest.h
 * Copyright (C) 2009-2017 by MegaMol Team
 * Alle Rechte vorbehalten.
 */
#pragma once

#include "mmcore/CallerSlot.h"
#include "mmcore/param/ParamSlot.h"
#include "mmospray/AbstractOSPRayStructure.h"

namespace megamol {
namespace ospray {

class OSPRayGeometryTest : public AbstractOSPRayStructure {

public:
    /**
     * Answer the name of this module.
     *
     * @return The name of this module.
     */
    static const char* ClassName(void) {
        return "OSPRayGeometryTest";
    }

    /**
     * Answer a human readable description of this module.
     *
     * @return A human readable description of this module.
     */
    static const char* Description(void) {
        return "Creator for OSPRay no overhead sphere geometries.";
    }

    /**
     * Answers whether this module is available on the current system.
     *
     * @return 'true' if the module is available, 'false' otherwise.
     */
    static bool IsAvailable(void) {
        return true;
    }

    /** Dtor. */
    virtual ~OSPRayGeometryTest(void);

    /** Ctor. */
    OSPRayGeometryTest(void);

protected:
    /**
     * color transfer helper
     * @param array with gray scales
     * @param transferfunction table/texture
     * @param transferfunction table/texture size
     * @param target array (rgba)
     */
    //void colorTransferGray(std::vector<float> &grayArray, float const* transferTable, unsigned int tableSize, std::vector<float> &rgbaArray);

    virtual bool create();
    virtual void release();

    virtual bool readData(core::Call& call);
    virtual bool getExtends(core::Call& call);


    bool InterfaceIsDirty();
};

} // namespace ospray
} // namespace megamol
