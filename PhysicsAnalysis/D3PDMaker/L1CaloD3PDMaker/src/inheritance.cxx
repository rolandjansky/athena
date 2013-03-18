/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: inheritance.cxx 349769 2011-03-07 11:00:26Z morrisj $

// Gaudi/Athena include(s):
#include "SGTools/BaseInfo.h"

// EDM include(s):
#include "TrigT1CaloEvent/CPMHitsCollection.h"
#include "TrigT1CaloEvent/CPMRoICollection.h"
#include "TrigT1CaloEvent/CPMTowerCollection.h"

#include "TrigT1CaloEvent/JEMEtSumsCollection.h"
#include "TrigT1CaloEvent/JEMHitsCollection.h"
#include "TrigT1CaloEvent/JEMRoICollection.h"
#include "TrigT1CaloEvent/JetElementCollection.h"

#include "TrigT1CaloEvent/CMMCPHitsCollection.h"
#include "TrigT1CaloEvent/CMMEtSumsCollection.h"
#include "TrigT1CaloEvent/CMMJetHitsCollection.h"
#include "TrigT1CaloEvent/CMMRoI.h"

#include "TrigT1CaloEvent/RODHeaderCollection.h"

SG_ADD_BASE( CPMHitsCollection,DataVector<LVL1::CPMHits> );
SG_ADD_BASE( CPMRoICollection,DataVector<LVL1::CPMRoI> );
SG_ADD_BASE( CPMTowerCollection,DataVector<LVL1::CPMTower> );

SG_ADD_BASE( JEMEtSumsCollection,DataVector<LVL1::JEMEtSums> );
SG_ADD_BASE( JEMHitsCollection,DataVector<LVL1::JEMHits> );
SG_ADD_BASE( JEMRoICollection,DataVector<LVL1::JEMRoI> );
SG_ADD_BASE( JetElementCollection,DataVector<LVL1::JetElement> );

SG_ADD_BASE( CMMCPHitsCollection,DataVector<LVL1::CMMCPHits> );
SG_ADD_BASE( CMMEtSumsCollection,DataVector<LVL1::CMMEtSums> );
SG_ADD_BASE( CMMJetHitsCollection,DataVector<LVL1::CMMJetHits> );
SG_ADD_BASE( CMMRoI,LVL1::CMMRoI);

SG_ADD_BASE( RODHeaderCollection, DataVector<LVL1::RODHeader> );
