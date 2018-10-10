/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  ***************************************************************************
//  *   Author: John Morris (john.morris@cern.ch)                             *
//  *           Queen Mary University of London                               *
//  *                                                                         *

#ifndef _TRIGGER_TRIGT1_TRIGT1CALOCALIBTOOLS_TRIGGERTOWERTHINNINGALG_H_
#define _TRIGGER_TRIGT1_TRIGT1CALOCALIBTOOLS_TRIGGERTOWERTHINNINGALG_H_

/**
   @class TriggerTowerThinningAlg
   @brief Thinning algorithm for the L1Calo Trigger Towers
   @brief Updated for xAOD::TriggerTowers

   This algorithm can be used to keep only TriggerTowers which have either:
   1.) An ADC value > m_minADC  OR
   2.) A Calo Cell energy > m_minCaloCellEnergy

   For a random (TRandom3) m_minRandom % of events, all Trigger Towers are saved.
   This is very useful for pedestal and noise studies

**/

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

#include <vector>
#include <TRandom3.h>


class IThinningSvc;

namespace DerivationFramework {

  class TriggerTowerThinningAlg : public AthAlgTool , public IThinningTool{

  public:
    TriggerTowerThinningAlg(const std::string& t, const std::string& n, const IInterface* p);
    virtual ~TriggerTowerThinningAlg();

    StatusCode initialize();
    StatusCode finalize();
    virtual StatusCode doThinning() const;

  private:
    ServiceHandle<IThinningSvc> m_thinningSvc;
               
    // StoreGate locations
    std::string m_triggerTowerLocation;

    // python configurables
    double m_minCaloCellET;
    int m_minADC;
    bool m_useRandom;
    double m_minRandom;

    // Counters
    mutable unsigned long m_nEventsProcessed;
    mutable unsigned long m_nEventsAllTriggerTowersKeptByRandom;
    mutable unsigned long m_nTriggerTowersProcessed;
    mutable unsigned long m_nTriggerTowersKept;
    mutable unsigned long m_nTriggerTowersRejected;

    // Tools and random number generator
    TRandom3* m_random;
  };	
} // namespace

#endif
