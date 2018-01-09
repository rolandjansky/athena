/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IL1JetElementTools.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ILVL1L1JETELEMENTTOOLS_H
#define ILVL1L1JETELEMENTTOOLS_H

#include "GaudiKernel/IAlgTool.h"
#include "AthContainers/DataVector.h"
// ============================================================================
// xAOD
// ============================================================================
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "xAODTrigL1Calo/JetElementContainer.h"
// ============================================================================
// LVL1
// ============================================================================
#include "TrigT1CaloEvent/JetElement.h"
#include "TrigT1CaloEvent/TriggerTower.h"
// ============================================================================
#include <map>

namespace LVL1 
{
class TriggerTower;
class JetElement;

/**
Interface definition for L1JetTools
*/

  static const InterfaceID IID_IL1JetElementTools("LVL1::IL1JetElementTools", 1, 0);

  class IL1JetElementTools : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    // enter declaration of your interface-defining member functions here
    virtual void makeJetElements(const xAOD::TriggerTowerContainer* tts, xAOD::JetElementContainer* jetElements) const = 0;
    virtual void mapJetElements(const xAOD::JetElementContainer* jetElements, xAOD::JetElementMap_t* map) const = 0;

    // depricated methods
    virtual void makeJetElements(const DataVector<TriggerTower>* tts, DataVector<JetElement>* jetElements) const = 0;
    
  };

  inline const InterfaceID& LVL1::IL1JetElementTools::interfaceID()
    { 
      return IID_IL1JetElementTools;
    }

} // end of namespace

#endif 
