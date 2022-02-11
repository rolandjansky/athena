/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMEASUREMENTBASE_UNCALIBRATEDMEASUREMENTCONTAINER_H
#define XAODMEASUREMENTBASE_UNCALIBRATEDMEASUREMENTCONTAINER_H

#include "xAODMeasurementBase/UncalibratedMeasurement.h"
#include "xAODMeasurementBase/versions/UncalibratedMeasurementContainer_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
    /// Define the version of the uncalibrated measurement container
    typedef UncalibratedMeasurementContainer_v1 UncalibratedMeasurementContainer;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::UncalibratedMeasurementContainer, 1194368759, 1 )

#endif // XAODMEASUREMENTBASE_UNCALIBRATEDMEASUREMENTCONTAINER_H
