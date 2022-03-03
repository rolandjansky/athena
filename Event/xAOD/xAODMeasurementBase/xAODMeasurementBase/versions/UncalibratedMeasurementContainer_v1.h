/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMEASUREMENTBASE_VERSION_UNCALIBRATEDMEASUREMENTCONTAINER_V1_H
#define XAODMEASUREMENTBASE_VERSION_UNCALIBRATEDMEASUREMENTCONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"
#include "xAODMeasurementBase/versions/UncalibratedMeasurement_v1.h"

namespace xAOD {
    /// The container is a simple typedef for now
    typedef DataVector< xAOD::UncalibratedMeasurement_v1 > UncalibratedMeasurementContainer_v1;
}

#endif // XAODMEASUREMENTBASE_VERSION_UNCALIBRATEDMEASUREMENTCONTAINER_V1_H
