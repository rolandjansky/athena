/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGDECISIONMAKER_ILVL1RESULTACCESSTOOL_H
#define TRIGDECISIONMAKER_ILVL1RESULTACCESSTOOL_H

#include "TrigDecisionMaker/Lvl1ItemsAndRoIs.h"

#include "GaudiKernel/IAlgTool.h"

#include <vector>

// forward declarations
namespace LVL1CTP {
   class Lvl1Result;
   class Lvl1Item;
}

namespace ROIB {
   class RoIBResult;
}

namespace HLT {

/**
      @class ILvl1ResultAccessTool
      this class provides the interface to the Lvl1 result access tool.
      Full description of the tool is given in Lvl1ResultAccessTool.

   */
   class ILvl1ResultAccessTool : public virtual IAlgTool
   {
   public:
      DeclareInterfaceID (ILvl1ResultAccessTool, 1, 0);

      virtual const std::vector<LVL1CTP::Lvl1Item>& getDecisionItems() = 0;

      virtual const std::vector< MuonRoI >& getMuonRoIs() const = 0;
      virtual const std::vector< EMTauRoI >& getEMTauRoIs()  const = 0;
      virtual const std::vector< JetEnergyRoI >& getJetEnergyRoIs() const = 0;

      virtual bool isCalibrationEvent(const ROIB::RoIBResult& result) const = 0;
      virtual std::vector< const LVL1CTP::Lvl1Item*>
      createL1Items(const ROIB::RoIBResult& result,
                    LVL1CTP::Lvl1Result const** lvl1ResultOut = nullptr) = 0;
      virtual std::vector< const LVL1CTP::Lvl1Item*>
      createL1Items(const std::vector< std::unique_ptr<LVL1CTP::Lvl1Item> >& lvl1ItemConfig,
                    const ROIB::RoIBResult& result,
                    LVL1CTP::Lvl1Result const** lvl1ResultOut = nullptr) const = 0;

      virtual const std::vector< MuonRoI >&      createMuonThresholds(const ROIB::RoIBResult& result) = 0;
      virtual const std::vector< EMTauRoI >&     createEMTauThresholds(const ROIB::RoIBResult& result, bool updateCaloRoIs=false) = 0;
      virtual const std::vector< JetEnergyRoI >& createJetEnergyThresholds(const ROIB::RoIBResult& result, bool updateCaloRoIs) = 0;

      virtual std::bitset<3> lvl1EMTauJetOverflow(const ROIB::RoIBResult& result) = 0;

      virtual StatusCode updateConfig(bool useL1Calo = true,
                                      bool useL1Muon = true,
                                      bool useL1JetEnergy = true) = 0;

      virtual std::vector< std::unique_ptr<LVL1CTP::Lvl1Item> > makeLvl1ItemConfig() const = 0;
      virtual std::vector< std::unique_ptr<LVL1CTP::Lvl1Item> > makeLvl1ItemConfig(const EventContext& context) const = 0;
      virtual StatusCode updateItemsConfigOnly() = 0;

      virtual StatusCode updateResult(const ROIB::RoIBResult& result,
                                      bool updateCaloRoIs = false) = 0;

      virtual StatusCode updateResult(const LVL1CTP::Lvl1Result& result) = 0;
   };
}

#endif
