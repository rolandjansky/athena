/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArPedestalSubset_p2.h
 *
 * @brief This file defines the class used for persistent container of
 * LArPedestalSubset
 *
 * @author RD Schaffer  <R.D.Schaffer@cern.ch>
 *
 * $Id: LArPedestalSubset_p2.h,v 1.1 2009-04-27 15:22:43 gunal Exp $
 */
#ifndef LARCONDTPCNV_LARPEDESTALSUBSET_P2
#define LARCONDTPCNV_LARPEDESTALSUBSET_P2

#include "LArCondTPCnv/LArConditionsSubset_p1.h"

/**
 * @class LArPedestalSubset_p2
 *
 * @brief persistent class container of LArConditionsSubset for
 * LArPedestal data.
 *
 *
**/

class LArPedestalSubset_p2
{

public:
    LArPedestalSubset_p2()  {} 
    LArConditionsSubset_p1  m_subset;
    std::vector<float>      m_vPedestal;
    std::vector<float>      m_vPedestalRMS;
};

     
#endif // LARCONDTPCNV_LARPEDESTALSUBSET_P1
