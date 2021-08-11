/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGDECISIONMAKER_LVL1RESULTACCESSTOOL_H
#define TRIGDECISIONMAKER_LVL1RESULTACCESSTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigDecisionMaker/ILvl1ResultAccessTool.h"
#include "TrigConfData/L1PrescalesSet.h"
#include "TrigT1Interfaces/CPRoIDecoder.h"
#include "TrigT1Interfaces/JEPRoIDecoder.h"

#include <vector>
#include <bitset>
#include <string>

// forward declarations
namespace TrigConf {
   class ILVL1ConfigSvc;
   class TriggerThreshold;
}

namespace LVL1CTP {
   class Lvl1Result;
   class Lvl1Item;
}

namespace HLT {

   /**
      @class Lvl1ResultAccessTool
      This tool is used to handle all LVL1 results in a coherent way, taking the LVL1
      configuration into account to fill the raw bits with more meaningful data.

      This was used by the Lvl1Converter in the Run-1&2 HLT Steering. Now the only client
      is the trigger decision tool (class TrigDecisionTool) which makes all
      trigger information available to the end-users.

      @author Till Eifert     <Till.Eifert@cern.ch>
      @author Nicolas Berger  <Nicolas.Berger@cern.ch>
   */
   class Lvl1ResultAccessTool : public extends<AthAlgTool, ILvl1ResultAccessTool>
   {
   public:

      Lvl1ResultAccessTool(const std::string& name, const std::string& type,
                           const IInterface* parent); //!< std Gaudi constructor

      virtual StatusCode initialize() override;

      virtual std::vector< std::unique_ptr<LVL1CTP::Lvl1Item>> makeLvl1ItemConfig(const EventContext& context) const override;

      /** @brief checks if we have calibration items fired in this event
       * @return true if calibration items are present in this event
       */
      virtual bool isCalibrationEvent(const ROIB::RoIBResult& result) const override;


      /** @brief Extract LVL1 items from given RoIBResult.
       *  @param result reference to RoIBResult object, holding all LVL1 RoIs and items
       *  @param lvl1ResultOut If non-null, create (in addition) a LVL1 container holding all Lvl1 results,
       *                       and return through this argument.
       *
       *  @return vector holding pointers to all LVL1 items
       */
      virtual
      std::vector< const LVL1CTP::Lvl1Item* >
      createL1Items(const std::vector< std::unique_ptr<LVL1CTP::Lvl1Item> >& lvl1ItemConfig,
                    const ROIB::RoIBResult& result,
                    LVL1CTP::Lvl1Result const** lvl1ResultOut = nullptr) const override;

      /** @brief Check if there was an overflow in the TOB transmission to CMX
       *  @param result reference to RoIBResul object
       *
       *  @return overflow bits for EM, Tau and Jet threshold
       */
      virtual
      std::bitset<3> lvl1EMTauJetOverflow(const ROIB::RoIBResult& result) override;

   private:

      // L1 decoders
      LVL1::JEPRoIDecoder m_jepDecoder;
      LVL1::CPRoIDecoder m_cpDecoder;

      Gaudi::Property<bool> m_useNewConfig { this, "UseNewConfig", true, "When true, read the menu from detector store, when false use the LVL1ConfigSvc" };

      /// access to L1Prescales
      SG::ReadCondHandleKey<TrigConf::L1PrescalesSet> m_l1PrescaleSetInputKey{ this, "L1Prescales", "L1Prescales", "L1 prescales set"};

      SG::WriteHandleKey<LVL1CTP::Lvl1Result> m_l1ResultKey{ this, "L1Result", "L1Result", "L1 result"};

      ServiceHandle<TrigConf::ILVL1ConfigSvc> m_lvl1ConfigSvc{ this, "LVL1ConfigSvc", "LVL1ConfigSvc", "LVL1 Config Service"}; //!< handle for the LVL1 configuration service
   };
} // end namespace


#endif
