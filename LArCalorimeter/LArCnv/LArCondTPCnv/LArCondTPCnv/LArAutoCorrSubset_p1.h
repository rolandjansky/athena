/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArAutoCorrSubset_p1.h
 *
 * @brief This file defines the class used for persistent container of
 * LArAutoCorrSubset
 *
 * @author RD Schaffer  <R.D.Schaffer@cern.ch>
 *
 * $Id: LArAutoCorrSubset_p1.h,v 1.1 2007-12-03 18:35:31 schaffer Exp $
 */
#ifndef LARCONDTPCNV_LARAUTOCORRSUBSET_P1
#define LARCONDTPCNV_LARAUTOCORRSUBSET_P1

#include "LArCondTPCnv/LArConditionsSubset_p1.h"

/**
 * @class LArAutoCorrSubset_p1
 *
 * @brief persistent class container of LArConditionsSubset for
 * LArAutoCorr data.
 *
 *
**/

class LArAutoCorrSubset_p1
{

public:
    LArAutoCorrSubset_p1()
      : m_vAutoCorrSize(0) {} 
    LArConditionsSubset_p1  m_subset;
    std::vector<float>      m_vAutoCorr;
    unsigned int            m_vAutoCorrSize;
};

     
#endif // LARCONDTPCNV_LARAUTOCORRSUBSET_P1
