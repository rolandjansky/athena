/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-

//////////////////////////////////////////////////////////////////
//
// TopInputMapTrigger_p1.h 
// Header file for class TopInputMap_p1
// persistent form of TopInputMap
//
/////////////////////////////////////////////////////////////////// 

#ifndef TOP_INPUT_MAP_TRIGGER_P1_H
#define TOP_INPUT_MAP_TRIGGER_P1_H
 
// AthenaPoolCnvSvc includes
#include "TopInputsTPCnv/TopOverlapTriggerCnv_p1.h"
#include "DataModelAthenaPool/DataLink_p1.h"
#include "TopInputsTPCnv/TopOverlapTriggerVector_p1.h"

struct TopInputMapTrigger_p1{ 
  ElementLinkIntVector_p1 	m_eliv_p1;
  TopOverlapTriggerVector_p1	m_tov_p1;
};

#endif // TOP_INPUT_MAP_TRIGGER_P1_H
