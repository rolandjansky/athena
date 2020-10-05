// This file's extension implies that it's C, but it's really -*- C++ -*-.
/* Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration */

/**
 * @file xAODEventFormat/test/versions/AVec_v1.h
 * @author frank berghaus <fberghaus@anl.gov>
 * @date May, 2020
 * @brief Class used for testing xAOD::EventFormat data reading/writing.
 */
#ifndef XAODEVENTFORMATCNV_TEST_AVEC_V1_H
#define XAODEVENTFORMATCNV_TEST_AVEC_V1_H

#include "xAODEventFormatCnv/test/versions/A_v1.h"
#include "AthContainers/DataVector.h"

namespace xAODMakerTest { typedef DataVector<A_v1> AVec_v1; } 

#endif  // XAODEVENTFORMATCNV_TEST_AVEC_V1_H
