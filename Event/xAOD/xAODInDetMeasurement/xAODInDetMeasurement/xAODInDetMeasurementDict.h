/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODINDETMEASUREMENT_DICT_H
#define XAODINDETMEASUREMENT_DICT_H

#include "xAODInDetMeasurement/PixelCluster.h"
#include "xAODInDetMeasurement/PixelClusterContainer.h"
#include "xAODInDetMeasurement/versions/PixelCluster_v1.h"
#include "xAODInDetMeasurement/versions/PixelClusterContainer_v1.h"
#include "xAODInDetMeasurement/versions/PixelClusterAuxContainer_v1.h"

#include "xAODInDetMeasurement/StripCluster.h"
#include "xAODInDetMeasurement/StripClusterContainer.h"
#include "xAODInDetMeasurement/versions/StripCluster_v1.h"
#include "xAODInDetMeasurement/versions/StripClusterContainer_v1.h"
#include "xAODInDetMeasurement/versions/StripClusterAuxContainer_v1.h"

#include "xAODCore/tools/DictHelpers.h"

// Instantiate all necessary types for the dictionary.
namespace {
    struct GCCXML_DUMMY_INSTANTIATION_XAODMEASUREMENTBASE {
        // Type(s) needed for the dictionary generation to succeed.
        XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, PixelClusterContainer_v1 );
        XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, StripClusterContainer_v1 );
    };
}


#endif // XAODINDETMEASUREMENT_DICT_H

