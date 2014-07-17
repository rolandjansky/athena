/*  -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPCOMPS_UTILITYFUNCS
#define PILEUPCOMPS_UTILITYFUNCS 1
/** @file UtilityFuncs.h
 * @brief Holds helper functions used by FixedArrayBM and ArrayBM
 *
 * @author John Chapman - ATLAS Collaboration
 */
#include<limits>    /*epsilon*/
static bool IsNonZero(float lumi) { return (lumi>std::numeric_limits<float>::epsilon()); }
#endif

