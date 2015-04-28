// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L1CaloTriggerTowerDecoratorAlg.h 652501 2015-03-08 15:06:35Z morrisj $
#ifndef TRIGGER_TRIGT1_TRIGT1CALOXAODCALIBTOOLS_DECORATETRIGGERTOWERSALG_H
#define TRIGGER_TRIGT1_TRIGT1CALOXAODCALIBTOOLS_DECORATETRIGGERTOWERSALG_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AsgTools/ToolHandle.h"

// Local include(s):
#include "TrigT1CaloCalibToolInterfaces/IL1CaloxAODOfflineTriggerTowerTools.h"

namespace LVL1
{
  class L1CaloTriggerTowerDecoratorAlg : public AthAlgorithm{
    public:
      L1CaloTriggerTowerDecoratorAlg( const std::string& name, ISvcLocator* svcLoc );
      
      /// Function initialising the algorithm
      virtual StatusCode initialize();
      /// Function executing the algorithm
      virtual StatusCode execute();   
      
    private:
      std::string m_sgKey_TriggerTowers;   
      
      /// Decoration strings
      std::string m_caloCellEnergy;
      std::string m_caloCellET;
      std::string m_caloCellEnergyByLayer;
      std::string m_caloCellETByLayer;   
      std::string m_caloCellsQuality;
      
      
      ToolHandle< LVL1::IL1CaloxAODOfflineTriggerTowerTools > m_ttTools;
  };
}
#endif
