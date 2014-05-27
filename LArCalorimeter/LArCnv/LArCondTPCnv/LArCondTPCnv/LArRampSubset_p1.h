/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArRampSubset_p1.h
 *
 * @brief This file defines the class used for persistent container of
 * LArRampSubset
 *
 * @author RD Schaffer  <R.D.Schaffer@cern.ch>
 *
 * $Id: LArRampSubset_p1.h,v 1.1 2007-12-03 18:35:32 schaffer Exp $
 */
#ifndef LARCONDTPCNV_LARRAMPSUBSET_P1
#define LARCONDTPCNV_LARRAMPSUBSET_P1

#include "LArCondTPCnv/LArConditionsSubset_p1.h"

/**
 * @class LArRampSubset_p1
 *
 * @brief persistent class container of LArConditionsSubset for
 * LArRamp data.
 *
 *
**/

class LArRampSubset_p1
{

public:
    LArRampSubset_p1()
      : m_vRampSize(0)  {} 
    LArConditionsSubset_p1  m_subset;
    std::vector<float>      m_vRamp;
    unsigned int            m_vRampSize;
};

     
#endif // LARCONDTPCNV_LARRAMPSUBSET_P1
