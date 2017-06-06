// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file xAODForward/AFPDefs.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2017
 * @brief Provide definitions for constants.
 *
 * A name defined in class scope like
 *   static const int foo = 1;
 *
 * still needs a separate definition if it is ever bound to a reference.
 * This can cause link failures, but often only in debug builds.
 * See <https://stackoverflow.com/questions/5391973/undefined-reference-to-static-const-int>
 */

#include "xAODForward/AFPClusterAlgID.h"
#include "xAODForward/AFPPixelLayerID.h"
#include "xAODForward/AFPStationID.h"

// These are all initialized in the headers; can't repeat it here.


const int xAOD::AFPClusterAlgID::singleHit;
const int xAOD::AFPClusterAlgID::nearestNeighbour;
const int xAOD::AFPPixelLayerID::firstFromIP;
const int xAOD::AFPPixelLayerID::secondFromIP;
const int xAOD::AFPPixelLayerID::thirdFromIP;
const int xAOD::AFPPixelLayerID::fourthFromIP;
const int xAOD::AFPStationID::farA;
const int xAOD::AFPStationID::nearA;
const int xAOD::AFPStationID::nearC;
const int xAOD::AFPStationID::farC;

