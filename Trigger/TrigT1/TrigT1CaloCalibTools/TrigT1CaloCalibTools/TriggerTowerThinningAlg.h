//  ***************************************************************************
//  *                                                                         *
//  *   This program is free software; you can redistribute it and/or modify  *
//  *   it under the terms of the GNU General Public License as published by  *
//  *   the Free Software Foundation; either version 2 of the License, or     *
//  *   (at your option) any later version.                                   *
//  *                                                                         *
//  *   Author: John Morris (john.morris@cern.ch)                             *
//  *           Queen Mary University of London                               *
//  *                                                                         *
//  ***************************************************************************/

#ifndef _TRIGGER_TRIGT1_TRIGT1CALOCALIBTOOLS_TRIGGERTOWERTHINNINGALG_H_
#define _TRIGGER_TRIGT1_TRIGT1CALOCALIBTOOLS_TRIGGERTOWERTHINNINGALG_H_

/**
   @class TriggerTowerThinningAlg
   @brief Thinning algorithm for the L1Calo Trigger Towers

   This algorithm can be used to keep only TriggerTowers which have either:
   1.) An ADC value > m_minADC  OR
   2.) A Calo Cell energy > m_minCaloCellEnergy

   For a random (TRandom3) m_minRandom % of events, all Trigger Towers are saved.
   This is very useful for pedestal and noise studies

**/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaKernel/IThinningSvc.h"
#include "GaudiKernel/Property.h"
#include "DataModel/DataVector.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"


#include "CaloEvent/CaloCellContainer.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "TileEvent/TileDigitsContainer.h"


#include "TrigT1CaloCalibToolInterfaces/IL1CaloOfflineTriggerTowerTools.h"

#include <vector>
#include <TRandom3.h>

class TriggerTowerThinningAlg : public AthAlgorithm{

public:
  TriggerTowerThinningAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TriggerTowerThinningAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  ServiceHandle<IThinningSvc> m_thinningSvc;
  ToolHandle<LVL1::IL1CaloOfflineTriggerTowerTools> m_tools;    
      
  // StoreGate locations
  std::string m_triggerTowerLocation;
  std::string m_caloCellsLocation;
  std::string m_larDigitLocation;
  std::string m_tileDigitsLocation;
  std::string m_tileTTL1ContainerName;

  // python configurables
  double m_minCaloCellEnergy;
  int m_minADC;
  bool m_useRandom;
  double m_minRandom;

  // Counters
  unsigned long m_nEventsProcessed;
  unsigned long m_nEventsAllTriggerTowersKeptByRandom;
  unsigned long m_nTriggerTowersProcessed;
  unsigned long m_nTriggerTowersKept;
  unsigned long m_nTriggerTowersRejected;

  // Tools and random number generator
  TRandom3* m_random;
};	

#endif
