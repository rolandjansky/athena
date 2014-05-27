/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArShapeSubset_p2.h
 *
 * @brief This file defines the class used for persistent container of
 * LArShapeSubset
 *
 * @author Marco Delmastro <Marco.Delmastro@cern.ch>
 *       
 * $Id: LArShapeSubset_p2.h,v 1.1 2008-06-19 10:14:53 mdelmast Exp $
 */
#ifndef LARCONDTPCNV_LARSHAPESUBSET_P2
#define LARCONDTPCNV_LARSHAPESUBSET_P2

#include "LArCondTPCnv/LArConditionsSubset_p1.h"

/**
 * @class LArShapeSubset_p2
 *
 * @brief persistent class container of LArConditionsSubset for
 * LArShape data.
 *
 *
**/

class LArShapeSubset_p2
{

public:
    LArShapeSubset_p2()
      : m_nPhases(0), m_nSamples(0) {} 
    LArConditionsSubset_p1  m_subset;
    std::vector<float>      m_vShape;
    std::vector<float>      m_vShapeDer;
    std::vector<float>      m_timeOffset;
    std::vector<float>      m_timeBinWidth;
    unsigned int            m_nPhases;
    unsigned int            m_nSamples;
};

     
#endif // LARCONDTPCNV_LARSHAPESUBSET_P2
