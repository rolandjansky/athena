// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @class  : ILvl1ResultAccessTool, Lvl1ResultAccessTool
 *
 * @brief This file contains the tool to unpack RoI and threshold info from the LVL1 RoI words.
 *
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 * @author Till Eifert     <Till.Eifert@cern.ch>
 *
 * File and Version Information:
 * $Id: Lvl1ResultAccessTool.h,v 1.20 2009-05-11 12:25:22 tbold Exp $
 **********************************************************************************/

#ifndef TRIGSTEERING_Lvl1ResultAccessTool_H
#define TRIGSTEERING_Lvl1ResultAccessTool_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigSteering/Lvl1ItemsAndRoIs.h"
#include "TrigConfData/L1PrescalesSet.h"

#include <vector>
#include <bitset>
#include <string>

// forward declarations
namespace TrigConf {
   class ILVL1ConfigSvc;
   class TriggerThreshold;
   class L1PrescalesSet;
}

namespace LVL1CTP {
   class CTPMuonConfig;
   class CTPCaloConfig;
   class CTPJetEnergyConfig;
   class Lvl1Result;
   class Lvl1Item;
}

namespace LVL1 {
   class RecMuonRoiSvc;
   class JEPRoIDecoder;
   class CPRoIDecoder;
}

namespace ROIB {
   class CTPRoI;
   class RoIBResult;
}

namespace HLT {

   class Chain;

   /**
      @class ILvl1ResultAccessTool
      this class provides the interface to the Lvl1 result access tool.
      Full description of the tool is given in Lvl1ResultAccessTool.

   */
   class ILvl1ResultAccessTool : public virtual IAlgTool
   {
   public:
      DeclareInterfaceID (ILvl1ResultAccessTool, 1, 0);

      virtual const std::vector<LVL1CTP::Lvl1Item*>& getDecisionItems() = 0;

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


   /**
      @class Lvl1ResultAccessTool
      This tool is used to handle all LVL1 results in a coherent way, taking the LVL1
      configuration into account to fill the raw bits with more meaningful data.

      Clients are the LVL1 converter class in the HLT Steering, namely the Lvl1Converter
      class; and the trigger decision tool (class TrigDecisionTool) which makes all
      trigger information available to the end-users.

      Lvl1ResultAccessTool needs to know the trigger configuration, it can however handle
      a changing configuration. Simply call the updateConfiguration(..) method each time
      the trigger configuration changes.

      For each new event, one has to explicitly tell @c to update the LVL1 result. This
      is done via the method updateResult(..). Afterwards, one can inspect the results
      via several getter methods, as getMuonRoIs() for
      the LVL1 Muon-type RoIs. The returned objects are simple classes which are defined
      for easy usage. See the classes description for more details.


      @author Till Eifert     <Till.Eifert@cern.ch>
      @author Nicolas Berger  <Nicolas.Berger@cern.ch>
   */
   class Lvl1ResultAccessTool : public extends<AthAlgTool, ILvl1ResultAccessTool>
   {
   public:

      Lvl1ResultAccessTool(const std::string& name, const std::string& type,
                           const IInterface* parent); //!< std Gaudi constructor

      virtual
      ~Lvl1ResultAccessTool(); //!< destructor -> delete Jet converter object

      virtual StatusCode initialize() override;

      // Accessors to the unpacked information:
      //_______________________________________

      /** @brief Get LVL1 items  ... for TrigDecision
       */
      virtual
      const std::vector<LVL1CTP::Lvl1Item*>& getDecisionItems() override  { return m_decisionItems; }


      // LVL1 RoIs and thresholds:
      //__________________________

      /** @brief get all LVL1 Muon-type RoI objects
       */
      virtual
      const std::vector< MuonRoI >&      getMuonRoIs()      const override { return m_muonRoIs; }

      /** @brief get all LVL1 EMTau-type RoI objects
       */
      virtual
      const std::vector< EMTauRoI >&     getEMTauRoIs()     const override { return m_emTauRoIs; }

      /** @brief get all LVL1 JetEnergy-type RoI objects
       */
      virtual
      const std::vector< JetEnergyRoI >& getJetEnergyRoIs() const override { return m_jetRoIs; }


      /** @brief Function to be called every time the trigger configuration changes,
       *         i.e. either in initialize() or in beginRun() of the client.
       *  @param useL1calo consider LVL1 Calorimeter (EMTau-type) RoIs ?
       *  @param useL1Muon consider LVL1 Muon RoIs ?
       *  @param useL1JetEnergy consider LVL1 JetEnergy RoIs ?
       */
      virtual
      StatusCode updateConfig(bool useL1Calo = true,
                              bool useL1Muon = true,
                              bool useL1JetEnergy = true) override;

      virtual std::vector< std::unique_ptr<LVL1CTP::Lvl1Item>> makeLvl1ItemConfig() const override;
      virtual std::vector< std::unique_ptr<LVL1CTP::Lvl1Item>> makeLvl1ItemConfig(const EventContext& context) const override;

      /** @brief update the LVL1 items settings from the (LVL1) trigger configuration.
       *         This method is called from within updateConfig(..), the reason to have it
       *         separate is that TrigDecisionTool only needs the LVL1 items and not the RoIs.
       */
      virtual
      StatusCode updateItemsConfigOnly() override;

      /** @brief checks if we have calibration items fired in this event
       * @return true if calibration items are present in this event
       */

      virtual bool isCalibrationEvent(const ROIB::RoIBResult& result) const override;


      /** @brief Extract LVL1 items from given RoIBResult and cache them in internally.
       *  @param result reference to RoIBResult object, holding all LVL1 RoIs and items
       *  @param lvl1ResultOut If non-null, create (in addition) a LVL1 container holding all Lvl1 results,
       *                       and return through this argument.
       *
       *  @return vector holding pointers to all LVL1 items
       */
      virtual
      std::vector< const LVL1CTP::Lvl1Item* >
      createL1Items(const ROIB::RoIBResult& result,
                    LVL1CTP::Lvl1Result const** lvl1ResultOut = nullptr) override;

      /// Alternate version with configuration passed in explicitly.
      /// Can be used when the tool is const.
      virtual
      std::vector< const LVL1CTP::Lvl1Item* >
      createL1Items(const std::vector< std::unique_ptr<LVL1CTP::Lvl1Item> >& lvl1ItemConfig,
                    const ROIB::RoIBResult& result,
                    LVL1CTP::Lvl1Result const** lvl1ResultOut = nullptr) const override;

      /** @brief Extract LVL1 Muon-type RoIs and cache them internally
       *  @param result reference to RoIBResult object, holding all LVL1 RoIs and items
       *
       *  @return reference to vector holding all Muon-RoI objects
       */
      virtual
      const std::vector< MuonRoI >&      createMuonThresholds(const ROIB::RoIBResult& result) override;

      /** @brief Extract LVL1 EMTau-type RoIs and cache them internally
       *  @param result reference to RoIBResult object, holding all LVL1 RoIs and items
       *
       *  @return reference to vector holding all EMTau-RoI objects
       */
      virtual
      const std::vector< EMTauRoI >&     createEMTauThresholds(const ROIB::RoIBResult& result, bool updateCaloRoIs=false) override;

      /** @brief Extract LVL1 JetEnergy-type RoIs and cache them internally
       *  @param result reference to RoIBResul object, holding all LVL1 RoIs and items
       *
       *  @return reference to vector holding all JetEnergy-RoI objects
       */
      virtual
      const std::vector< JetEnergyRoI >& createJetEnergyThresholds(const ROIB::RoIBResult& result, bool updateCaloRoIs=false) override;

      /** @brief Check if there was an overflow in the TOB transmission to CMX
       *  @param result reference to RoIBResul object
       *
       *  @return overflow bits for EM, Tau and Jet threshold
       */
      virtual
      std::bitset<3> lvl1EMTauJetOverflow(const ROIB::RoIBResult& result) override;

      /** @brief Extract all LVL1 items and all LVL1 items from given RoIBResult
       *  @param result reference to RoIBResult object, holding all LVL1 RoIs and items
       *  @param updateCaloRoIs if true, assume old Calo RoI words which need to be corrected
       */
      virtual
      StatusCode updateResult(const ROIB::RoIBResult& result,
                              bool updateCaloRoIs = false) override;

      /** @brief Extract all LVL1 items and all LVL1 items from given Lvl1Result
       *  @param result reference to Lvl1Result object, holding all LVL1 RoIs and items
       *  @param updateCaloRoIs if true, assume old Calo RoI words which need to be corrected
       */
      virtual
      StatusCode updateResult(const LVL1CTP::Lvl1Result& result) override;

   private:

      void clearDecisionItems(); //!< delete all LVL1 decisio items

      // L1 decoders
      LVL1::JEPRoIDecoder* m_jepDecoder { nullptr };
      LVL1::CPRoIDecoder* m_cpDecoder { nullptr };

      // Results cache
      std::vector< LVL1CTP::Lvl1Item* >     m_decisionItems;  //!< vector holding all LVL1 items for TrigDecision
      std::vector<const LVL1CTP::Lvl1Item*> m_itemsBPonly;    //!< vector holding all LVL1 items that were suppressed by prescales

      std::vector< MuonRoI> m_muonRoIs;     //!< cached LVL1 Muon-type RoI objects
      std::vector< EMTauRoI> m_emTauRoIs;   //!< cached LVL1 EMTau-type RoI objects
      std::vector< JetEnergyRoI> m_jetRoIs; //!< cached LVL1 JetEnergy-type RoI objects

      // Config data
      bool m_useL1Muon { false }, m_useL1Calo { false }, m_useL1JetEnergy { false }; //!< consider RoI types ?
      unsigned int m_muonCommissioningStep;            //!< change thresholds creation inclusivness depending on the comissioning stage 
      bool m_ignorePrescales; //!< Set to true to ignore prescales
      Gaudi::Property<bool> m_useNewConfig { this, "UseNewConfig", true, "When true, read the menu from detector store, when false use the LVL1ConfigSvc" };

      /// access to L1Prescales
      SG::ReadCondHandleKey<TrigConf::L1PrescalesSet> m_l1PrescaleSetInputKey{ this, "L1Prescales", "L1Prescales", "L1 prescales set"};

      std::vector< std::unique_ptr<LVL1CTP::Lvl1Item> > m_lvl1ItemConfig; //!< vector holding all configured LVL1 items
      std::vector< ConfigThreshold > m_muonCfg;           //!< vector holding all configured LVL1 muon thresholds
      std::vector< ConfigThreshold > m_emCfg;             //!< vector holding all configured LVL1 EM thresholds
      std::vector< ConfigThreshold > m_tauCfg;            //!< vector holding all configured LVL1 TAU thresholds
      std::vector< ConfigJetEThreshold > m_jetCfg;        //!< vector holding all configured LVL1 jet thresholds

      std::vector<TrigConf::TriggerThreshold*> m_emtauThresholds;
      std::vector<TrigConf::TriggerThreshold*> m_jetThresholds;

      bool addJetThreshold( HLT::JetEnergyRoI & roi, const ConfigJetEThreshold* threshold );
      bool addMetThreshold( HLT::JetEnergyRoI & roi, const ConfigJetEThreshold* threshold, bool isRestrictedRange );

      ServiceHandle<TrigConf::ILVL1ConfigSvc> m_lvl1ConfigSvc{ this, "LVL1ConfigSvc", "LVL1ConfigSvc", "LVL1 Config Service"}; //!< handle for the LVL1 configuration service
   };
} // end namespace


#endif
