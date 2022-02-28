/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMEASUREMENTBASE_UNCALIBRATEDMEASUREMENT_H
#define XAODMEASUREMENTBASE_UNCALIBRATEDMEASUREMENT_H

#include "xAODMeasurementBase/versions/UncalibratedMeasurement_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
    /// Define the version of the uncalibrated measurement class
    typedef UncalibratedMeasurement_v1 UncalibratedMeasurement;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::UncalibratedMeasurement, 15699721, 1 )

#endif // XAODMEASUREMENTBASE_UNCALIBRATEDMEASUREMENT_H
