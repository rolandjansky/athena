/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArPedestalSubset_p1.h
 *
 * @brief This file defines the class used for persistent container of
 * LArPedestalSubset
 *
 * @author RD Schaffer  <R.D.Schaffer@cern.ch>
 *
 * $Id: LArPedestalSubset_p1.h,v 1.1 2007-12-03 18:35:32 schaffer Exp $
 */
#ifndef LARCONDTPCNV_LARPEDESTALSUBSET_P1
#define LARCONDTPCNV_LARPEDESTALSUBSET_P1

#include "LArCondTPCnv/LArConditionsSubset_p1.h"

/**
 * @class LArPedestalSubset_p1
 *
 * @brief persistent class container of LArConditionsSubset for
 * LArPedestal data.
 *
 *
**/

class LArPedestalSubset_p1
{

public:
    LArPedestalSubset_p1() 
      : m_vPedestalSize(0), m_vPedestalRMSSize(0) {} 
    LArConditionsSubset_p1  m_subset;
    std::vector<float>      m_vPedestal;
    std::vector<float>      m_vPedestalRMS;
    unsigned int            m_vPedestalSize;
    unsigned int            m_vPedestalRMSSize;
};

     
#endif // LARCONDTPCNV_LARPEDESTALSUBSET_P1
