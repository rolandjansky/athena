/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALO_TRIGT1CALODICT_H
#define TRIGT1CALO_TRIGT1CALODICT_H

#include "DataModel/DataVector.h"
#include "TrigT1CaloEvent/TriggerTower.h"
#include "TrigT1CaloEvent/CPMTower.h"
#include "TrigT1CaloEvent/JetElement.h"
#include "TrigT1CaloEvent/CMXCPTob.h"
#include "TrigT1CaloEvent/CMXCPHits.h"
#include "TrigT1CaloEvent/CPMTobRoI.h"
#include "TrigT1CaloEvent/CMMCPHitsCollection.h"
#include "TrigT1CaloEvent/CMMEtSumsCollection.h"
#include "TrigT1CaloEvent/CMMJetHitsCollection.h"
#include "TrigT1CaloEvent/CMMRoI.h"
#include "TrigT1CaloEvent/CPMHitsCollection.h"
#include "TrigT1CaloEvent/CPMRoICollection.h"
#include "TrigT1CaloEvent/CPMTowerCollection.h"
#include "TrigT1CaloEvent/JEMEtSumsCollection.h"
#include "TrigT1CaloEvent/JEMHitsCollection.h"
#include "TrigT1CaloEvent/JEMRoICollection.h"


struct TrigT1CaloEventDict {
  DataVector<LVL1::TriggerTower > m_Trig;
  DataVector<LVL1::CPMTower> m_CpmTrig;
  DataVector<LVL1::JetElement> m_JetTrig;
  DataVector<LVL1::CMXCPTob> m_CmxCPTob;
  DataVector<LVL1::CMXCPHits> m_CmxCPHits;
  DataVector<LVL1::CPMTobRoI> m_CpmTobRoI;
};

#endif
