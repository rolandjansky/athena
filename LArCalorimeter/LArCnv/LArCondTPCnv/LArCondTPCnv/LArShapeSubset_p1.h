/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArShapeSubset_p1.h
 *
 * @brief This file defines the class used for persistent container of
 * LArShapeSubset
 *
 * @author RD Schaffer  <R.D.Schaffer@cern.ch>
 *
 * $Id: LArShapeSubset_p1.h,v 1.1 2007-10-26 16:13:34 schaffer Exp $
 */
#ifndef LARCONDTPCNV_LARSHAPESUBSET_P1
#define LARCONDTPCNV_LARSHAPESUBSET_P1

#include "LArCondTPCnv/LArConditionsSubset_p1.h"

/**
 * @class LArShapeSubset_p1
 *
 * @brief persistent class container of LArConditionsSubset for
 * LArShape data.
 *
 *
**/

class LArShapeSubset_p1
{

public:
    LArShapeSubset_p1()
      : m_vShapeSize(0), m_vShapeDerSize(0) {} 
    LArConditionsSubset_p1  m_subset;
    std::vector<float>      m_vShape;
    std::vector<float>      m_vShapeDer;
    unsigned int            m_vShapeSize;
    unsigned int            m_vShapeDerSize;
};

     
#endif // LARCONDTPCNV_LARSHAPESUBSET_P1
