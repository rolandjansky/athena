/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArMphysOverMcalSubset_p1.h
 *
 * @brief This file defines the class used for persistent container of
 * LArMphysOverMcalSubset
 *
 * @author Walter Lampl <walter.lampl@ cern.ch>
 *
 * $Id: LArMphysOverMcalSubset_p1.h,v 1.1 2008-01-25 17:38:29 wlampl Exp $
 */
#ifndef LARCONDTPCNV_LARSMPHYSOVERMCALSUBSET_P1
#define LARCONDTPCNV_LARSMPHYSOVERMCALSUBSET_P1

#include "LArCondTPCnv/LArConditionsSubset_p1.h"

/**
 * @class LArMphysOverMcalSubset_p1
 *
 * @brief persistent class container of LArConditionsSubset for
 * LArMphysOverMcal data.
 *
 *
**/

class LArMphysOverMcalSubset_p1
{

public:
    LArMphysOverMcalSubset_p1()  {} 
    LArConditionsSubset_p1  m_subset;
    std::vector<float>      m_values;
};


#endif // LARCONDTPCNV_LARSHAPESUBSET_P1
