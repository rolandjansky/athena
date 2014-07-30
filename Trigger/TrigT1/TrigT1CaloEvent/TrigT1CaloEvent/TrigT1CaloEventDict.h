#ifndef TRIGT1CALO_TRIGT1CALODICT_H
#define TRIGT1CALO_TRIGT1CALODICT_H

#include "DataModel/DataVector.h"
#include "TrigT1CaloEvent/TriggerTower.h"
#include "TrigT1CaloEvent/JetElement.h"

struct temp {
  DataVector<LVL1::TriggerTower > m_Trig;
  DataVector<LVL1::JetElement > mTrig2;
};

#endif
