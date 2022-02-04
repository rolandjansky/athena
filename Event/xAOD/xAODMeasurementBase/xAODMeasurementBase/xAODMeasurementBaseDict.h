/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMEASUREMENTBASE_DICT_H
#define XAODMEASUREMENTBASE_DICT_H

#include "xAODMeasurementBase/UncalibratedMeasurement.h"
#include "xAODMeasurementBase/UncalibratedMeasurementContainer.h"
#include "xAODMeasurementBase/versions/UncalibratedMeasurement_v1.h"
#include "xAODMeasurementBase/versions/UncalibratedMeasurementContainer_v1.h"

#include "xAODCore/tools/DictHelpers.h"

// Instantiate all necessary types for the dictionary.
namespace {
    struct GCCXML_DUMMY_INSTANTIATION_XAODMEASUREMENTBASE {
        // Type(s) needed for the dictionary generation to succeed.
        XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, UncalibratedMeasurementContainer_v1 );
    };
}


#endif // XAODMEASUREMENTBASE_DICT_H
