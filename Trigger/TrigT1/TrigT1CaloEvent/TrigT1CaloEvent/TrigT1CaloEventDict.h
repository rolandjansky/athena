#ifndef TRIGT1CALO_TRIGT1CALODICT_H
#define TRIGT1CALO_TRIGT1CALODICT_H

#include "DataModel/DataVector.h"
#include "TrigT1CaloEvent/TriggerTower.h"
#include "TrigT1CaloEvent/CPMTower.h"
#include "TrigT1CaloEvent/JetElement.h"
#include "TrigT1CaloEvent/CMXCPTob.h"
#include "TrigT1CaloEvent/CMXCPHits.h"
#include "TrigT1CaloEvent/CPMTobRoI.h"


struct temp {
  DataVector<LVL1::TriggerTower > m_Trig;
  DataVector<LVL1::CPMTower> m_CpmTrig;
  DataVector<LVL1::JetElement> m_JetTrig;
  DataVector<LVL1::CMXCPTob> m_CmxCPTob;
  DataVector<LVL1::CMXCPHits> m_CmxCPHits;
  DataVector<LVL1::CPMTobRoI> m_CpmTobRoI;
};

#endif
