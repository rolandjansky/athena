/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArSingleFloatSubset_p1.h
 *
 * @brief This file defines the class used for persistent container of
 * LArSingleFloatSubset
 *
 * @author Walter Lampl <walter.lampl@ cern.ch>
 *
 * $Id: LArSingleFloatSubset_p1.h,v 1.1 2008-04-10 13:04:48 wlampl Exp $
 */
#ifndef LARCONDTPCNV_LARSINGLEFLOATSUBSET_P1
#define LARCONDTPCNV_LARSINGLEFLOATSUBSET_P1

#include "LArCondTPCnv/LArConditionsSubset_p1.h"

/**
 * @class LArSingleFloatSubset_p1
 *
 * @brief persistent class container of LArConditionsSubset for
 * LArSingleFloat data.
 *
 *
**/

class LArSingleFloatSubset_p1
{

public:
    LArSingleFloatSubset_p1()  {} 
    LArConditionsSubset_p1  m_subset;
    std::vector<float>      m_values;
};


#endif 
