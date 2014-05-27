/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArOFCSubset_p1.h
 *
 * @brief This file defines the class used for persistent container of
 * LArOFCSubset
 *
 * @author RD Schaffer  <R.D.Schaffer@cern.ch>
 *
 * $Id: LArOFCSubset_p1.h,v 1.1 2008-01-23 13:52:08 schaffer Exp $
 */
#ifndef LARCONDTPCNV_LAROFCSUBSET_P1
#define LARCONDTPCNV_LAROFCSUBSET_P1

#include "LArCondTPCnv/LArConditionsSubset_p1.h"

/**
 * @class LArOFCSubset_p1
 *
 * @brief persistent class container of LArConditionsSubset for
 * LArOFC data.
 *
 *
**/

class LArOFCSubset_p1
{

public:
    LArOFCSubset_p1()
      : m_nPhases(0), m_nSamples(0) {} 
    LArConditionsSubset_p1  m_subset;
    std::vector<float>      m_vOFC_a;
    std::vector<float>      m_vOFC_b;
    std::vector<float>      m_timeOffset;
    std::vector<float>      m_timeBinWidth;
    unsigned int            m_nPhases;
    unsigned int            m_nSamples;
};

     
#endif // LARCONDTPCNV_LAROFCSUBSET_P1
