/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
//  ***************************************************************************
//  *   Author: John Morris (john.morris@cern.ch)                             *
//  *           Queen Mary University of London                               *
//  *                                                                         *

#ifndef TRIGT1CALOCALIBTOOLS_TRIGGERTOWERTHINNINGALG_H
#define TRIGT1CALOCALIBTOOLS_TRIGGERTOWERTHINNINGALG_H

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
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "StoreGate/ThinningHandleKey.h"

#include <vector>
#include <TRandom3.h>


namespace DerivationFramework {

  class TriggerTowerThinningAlg : public extends<AthAlgTool, IThinningTool> {

  public:
    TriggerTowerThinningAlg(const std::string& t, const std::string& n, const IInterface* p);
    virtual ~TriggerTowerThinningAlg();

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    virtual StatusCode doThinning() const override;

  private:
    StringProperty m_streamName
    { this, "StreamName", "", "Name of the stream being thinned" };
    SG::ThinningHandleKey<xAOD::TriggerTowerContainer> m_triggerTowerLocation
      { this, "TriggerTowerLocation", LVL1::TrigT1CaloDefs::xAODTriggerTowerLocation, "" };

    // python configurables
    double m_minCaloCellET;
    int m_minADC;
    bool m_useRandom;
    double m_minRandom;

    // Counters
    mutable std::atomic<unsigned long> m_nEventsProcessed;
    mutable std::atomic<unsigned long> m_nEventsAllTriggerTowersKeptByRandom;
    mutable std::atomic<unsigned long> m_nTriggerTowersProcessed;
    mutable std::atomic<unsigned long> m_nTriggerTowersKept;
    mutable std::atomic<unsigned long> m_nTriggerTowersRejected;

    // Tools and random number generator
    TRandom3* m_random;
  };	
} // namespace

#endif
