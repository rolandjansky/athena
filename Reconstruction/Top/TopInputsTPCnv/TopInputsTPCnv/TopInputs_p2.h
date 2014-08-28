/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-

#ifndef TOP_INPUTS_P2_H
#define TOP_INPUTS_P2_H

//
#include "TopInputsTPCnv/TopInputMap_p1.h"
#include "TopInputsTPCnv/TopInputMapTrigger_p1.h"
#include "DataModelAthenaPool/DataLink_p1.h"

struct TopInputs_p2 {
  TopInputMapTrigger_p1 m_muonMap_p1;
  TopInputMapTrigger_p1 m_electronMap_p1;
  TopInputMap_p1 m_photonMap_p1;
  TopInputMap_p1 m_tauJetMap_p1;
  TopInputMap_p1 m_jetMap_p1;
  TopInputMap_p1 m_bJetMap_p1;
  TopInputMap_p1 m_trackMap_p1;
  DataLink_p1 m_metLink_p1;
};

#endif // TOP_INPUTS_P2_H
 
