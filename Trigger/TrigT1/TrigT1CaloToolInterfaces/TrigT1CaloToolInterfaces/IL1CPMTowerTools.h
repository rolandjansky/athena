/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IL1CPMTowerTools.h, 
///////////////////////////////////////////////////////////////////
#ifndef ILVL1L1CPMTOWERTOOLS_H
#define ILVL1L1CPMTOWERTOOLS_H

#include "GaudiKernel/IAlgTool.h"
#include "TrigT1CaloEvent/TriggerTower.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "TrigT1CaloEvent/CPMTower.h"
#include "xAODTrigL1Calo/CPMTower.h"
#include <map>

namespace LVL1 
{

/**
Interface definition for L1CPMTowerTools
*/

  static const InterfaceID IID_IL1CPMTowerTools("LVL1::IL1CPMTowerTools", 1, 0);

  class IL1CPMTowerTools : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    // enter declaration of your interface-defining member functions here
    virtual void makeCPMTowers(const DataVector<TriggerTower>* triggerTowers, DataVector<CPMTower>* cpmTowers, bool zeroSuppress = true) = 0;
    virtual void makeCPMTowers(const DataVector<xAOD::TriggerTower>* triggerTowers, DataVector<CPMTower>* cpmTowers, bool zeroSuppress = true) = 0;
    virtual void makeCPMTowers(const DataVector<xAOD::TriggerTower>* triggerTowers, DataVector<xAOD::CPMTower>* cpmTowers, bool zeroSuppress = true) = 0;
    
  };

  inline const InterfaceID& LVL1::IL1CPMTowerTools::interfaceID()
    { 
      return IID_IL1CPMTowerTools;
    }

} // end of namespace

#endif 
