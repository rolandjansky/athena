/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArSingleIntSubset_p1.h
 *
 * @brief This file defines the class used for persistent container of
 * LArSingleIntSubset
 *
 * @author Walter Lampl <walter.lampl@ cern.ch>
 *
 * $Id: LArSingleIntSubset_p1.h,v 1.1 2008-03-31 15:41:26 wlampl Exp $
 */
#ifndef LARCONDTPCNV_LARSINGLEINTSUBSET_P1
#define LARCONDTPCNV_LARSINGLEINTSUBSET_P1

#include "LArCondTPCnv/LArConditionsSubset_p1.h"

/**
 * @class LArSingleIntSubset_p1
 *
 * @brief persistent class container of LArConditionsSubset for
 * LArSingleInt data.
 *
 *
**/

class LArSingleIntSubset_p1
{

public:
    LArSingleIntSubset_p1()  {} 
    LArConditionsSubset_p1  m_subset;
    std::vector<int>      m_values;
};


#endif 
