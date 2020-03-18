#pragma once

#include "mmcore/Call.h"
#include "mmcore/factories/CallAutoDescription.h"

#include <cstdint>

namespace megamol {
namespace molsurfmapcluster {

/**
 * Struct containing the data sent over CallClustering_2
 */
struct ClusteringData {};

/**
 * Struct containing the metadata sent over CallClustering_2
 */
struct ClusteringMetaData {};

/**
 * Call for transporting clustering information
 */
class CallClustering_2 : public core::Call {
public:
    /** Index of the 'GetData' function */
    static const unsigned int CallForGetData;

    /** Index of the 'GetExtent' function */
    static const unsigned int CallForGetExtent;

    /**
     * Answer the name of the objects of this description.
     *
     * @return The name of the objects of this description.
     */
    static const char* ClassName(void) { return "CallClustering_2"; }

    /**
     * Gets a human readable description of the module.
     *
     * @return A human readable description of the module.
     */
    static const char* Description(void) { return "Call to get Clustering-Data"; }

    /** Ctor. */
    CallClustering_2(void);

    /** Dtor. */
    virtual ~CallClustering_2(void);

    /**
     * Answer the number of functions used for this call.
     *
     * @return The number of functions used for this call.
     */
    static unsigned int FunctionCount(void) { return 2; }

    /**
     * Answer the name of the function used for this call.
     *
     * @param idx The index of the function to return it's name.
     *
     * @return The name of the requested function.
     */
    static const char* FunctionName(unsigned int idx) {
        switch (idx) {
        case 0:
            return "GetData";
        case 1:
            return "GetExtent";
        }
        return nullptr;
    }

private:
    /** The sent data */
    ClusteringData data;

    /** The sent metadata */
    ClusteringMetaData metadata;
};

/** Description class typedef */
typedef megamol::core::factories::CallAutoDescription<CallClustering_2> CallClustering_2Description;

} // namespace molsurfmapcluster
} // namespace megamol
