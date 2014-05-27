/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArDSPThresholdsSubset_p1.h
 *
 * @brief This file defines the class used for persistent container of
 * LArDSPThresholdsSubset
 *
 * @author Walter Lampl <walter.lampl@ cern.ch>
 *
 * $Id: LArDSPThresholdsSubset_p1.h,v 1.1 2008-10-08 14:03:50 wlampl Exp $
 */
#ifndef LARCONDTPCNV_LARDSPTHRESHOLDSUBSET_P1_H
#define LARCONDTPCNV_LARDSPTHRESHOLDSUBSET_P1_H

#include "LArCondTPCnv/LArConditionsSubset_p1.h"

/**
 * @class LArDSPThresholdsSubset_p1
 *
 * @brief persistent class container of LArConditionsSubset for
 * LArDSPThresholds data.
 *
 *
**/

class LArDSPThresholdsSubset_p1
{

public:
    LArDSPThresholdsSubset_p1()  {} 
    LArConditionsSubset_p1  m_subset;
    std::vector<float>      m_tQThr;
    std::vector<float>      m_samplesThr;
    std::vector<float>      m_trigSumThr;

};


#endif 
