// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSEDM_MEASUREMENT_H
#define ACTSEDM_MEASUREMENT_H 1

#include "Acts/EventData/Measurement.hpp"

#include "AthContainers/DataVector.h"

namespace ActsTrk {
  using Measurement = Acts::BoundVariantMeasurement;
  using MeasurementContainer = DataVector<Acts::BoundVariantMeasurement>;
}

// Set up a CLID for the type:                                                                                                                                                                 
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( ActsTrk::Measurement, 207128991, 1 )
CLASS_DEF( ActsTrk::MeasurementContainer, 1261318992, 1)

#endif

