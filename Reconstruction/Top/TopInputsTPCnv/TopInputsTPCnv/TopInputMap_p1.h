/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-

//////////////////////////////////////////////////////////////////
//
// TopInputMap_p1.h 
// Header file for class TopInputMap_p1
// persistent form of TopInputMap
//
/////////////////////////////////////////////////////////////////// 

#ifndef TOP_INPUT_MAP_P1_H
#define TOP_INPUT_MAP_P1_H
 
// AthenaPoolCnvSvc includes
#include "TopInputsTPCnv/TopOverlapCnv_p1.h"
#include "DataModelAthenaPool/DataLink_p1.h"
#include "TopInputsTPCnv/TopOverlapVector_p1.h"

struct TopInputMap_p1{ 
  ElementLinkIntVector_p1 m_eliv_p1;
  TopOverlapVector_p1     m_tov_p1;
};

#endif // TOP_INPUT_MAP_P1_H
