/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//
//
#ifndef ANALYSISTRIGGERALGS_ROIBRESULTTOXAOD_H
#define ANALYSISTRIGGERALGS_ROIBRESULTTOXAOD_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

// Tool/service include(s):
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigT1Interfaces/ITrigT1MuonRecRoiTool.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloToolInterfaces/IL1CPMTools.h"
#include "TrigT1CaloToolInterfaces/IL1JEMJetTools.h"

// EDM include(s):
#include "TrigT1Result/RoIBResult.h"
#include "xAODTrigL1Calo/CPMTowerContainer.h"
#include "xAODTrigL1Calo/JetElementContainer.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "xAODTrigger/EnergySumRoI.h"
#include "xAODTrigger/JetEtRoI.h"
#include "xAODTrigger/JetRoIContainer.h"

/**
 *  @short RoIB result to xAOD converter
 *
 *  The RoIBResultToxAOD algorithm builds the xAOD analysis objects
 *  from the LVL1 @c ROIB::RoIBResult object. 
 *
 * @author Tadashi Maeno <Tadashi.Maeno@cern.ch>
 * @author Attila Kraznahorkay Jr. <Attila.Krasznahorkay@cern.ch>
 * @author Alan Watson <Alan.Watson@cern.ch>
 * @author Wolfgang Ehrenfeld <Wolfgang.Menges@desy.de>
 */
class RoIBResultToxAOD : public AthAlgorithm {

public:
   /// Algorithm constructor
   RoIBResultToxAOD( const std::string& name, ISvcLocator* svcLoc );

   /// @name Function(s) implementing the @c Algorithm interface
   /// @{

   /// Declare that the algorithm is clonable
   virtual bool isClonable() const override { return true; }

   /// Function initialising the algorithm
   virtual StatusCode initialize() override;

   /// Function executing the algorithm
   virtual StatusCode execute() override;

   /// @}

private:
   /// Create the EmTau RoI objects
   StatusCode createEmTauRoI( const ROIB::RoIBResult& roib,
                              const EventContext& ctx );
   /// Create the JetEnergy RoI object
   StatusCode createJetEnergyRoI( const ROIB::RoIBResult& roib,
                                  const EventContext& ctx );
   /// Create the Muon RoI objects
   StatusCode createMuonRoI( const ROIB::RoIBResult& roib,
                             const EventContext& ctx ) const;

   /// @name Services
   /// @{

   /// The trigger configuration service
   ServiceHandle< TrigConf::ILVL1ConfigSvc > m_configSvc {
      this, "LVL1ConfigSvc", "TrigConf::LVL1ConfigSvc/LVL1ConfigSvc",
      "Service providing the LVL1 trigger configuration" };

   /// The RPC RoI reconstruction tool
   ToolHandle<LVL1::ITrigT1MuonRecRoiTool> m_recRPCRoiTool { this, "RecRpcRoiTool", "LVL1::TrigT1RPCRecRoiTool/TrigT1RPCRecRoiTool"};
   /// The TGC RoI reconstruction service
   ToolHandle< LVL1::ITrigT1MuonRecRoiTool > m_recTGCRoiTool {
      this, "RecTgcRoiTool", "LVL1::TrigT1TGCRecRoiTool/TrigT1TGCRecRoiTool",
      "TGC RoI reconstruction service" };

   /// @}

   /// @name Tools
   /// @{

   /// Tool for calculation of EmTau trigger sums per RoI
   ToolHandle< LVL1::IL1CPMTools > m_emTauTool {
      this, "L1CPMTools", "LVL1::L1CPMTools/L1CPMTools",
      "Tool for calculation of EmTau trigger sums per RoI" };
   /// Tool for calculation of Jet cluster sums per RoI
   ToolHandle< LVL1::IL1JEMJetTools > m_jetTool {
      this, "L1JEMJetTools", "LVL1::L1JEMJetTools/L1JEMJetTools",
      "Tool for calculation of Jet cluster sums per RoI" };

   /// @}

   /// @name Read handle keys
   /// @{

   /// Read key for the ROIB::RoIBResult object
   SG::ReadHandleKey< ROIB::RoIBResult > m_roibResultKey {
      this, "RoIBResultInputKey", "RoIBResult",
      "Key for the ROIB::RoIBResult input object" };

   /// Read key for the xAOD::CPMTowerContainer object
   SG::ReadHandleKey< xAOD::CPMTowerContainer > m_cpmTowerKey {
      this, "CPMTowerLocation", LVL1::TrigT1CaloDefs::CPMTowerLocation,
      "Key for the xAOD::CPMTowerContainer input object" };
   /// Read key for the xAOD::JetElementContainer object
   SG::ReadHandleKey< xAOD::JetElementContainer > m_jetElementKey {
      this, "JetElementLocation", LVL1::TrigT1CaloDefs::JetElementLocation,
      "Key for the xAOD::JetElementContainer input object" };

   /// @}

   /// @name Write handle keys
   /// @{

   /// Write key for the xAOD::MuonRoIContainer object
   SG::WriteHandleKey< xAOD::MuonRoIContainer > m_muonRoIKey {
      this, "xAODKeyMuon", "LVL1MuonRoIs",
      "Key for the xAOD::MuonRoIContainer output object" };
   /// Write key for the xAOD::EmTauRoIContainer object
   SG::WriteHandleKey< xAOD::EmTauRoIContainer > m_emtauRoIKey {
      this, "xAODKeyEmTau", "LVL1EmTauRoIs",
      "Key for the xAOD::EmTauRoIContainer output object" };
   /// Write key for the xAOD::EnergySumRoI object
   SG::WriteHandleKey< xAOD::EnergySumRoI > m_energysumRoIKey {
      this, "xAODKeyEsum", "LVL1EnergySumRoI",
      "Key for the xAOD::EnergySumRoI output object" };
   /// Write key for the xAOD::JetEtRoI object
   SG::WriteHandleKey< xAOD::JetEtRoI > m_jetetRoIKey {
      this, "xAODKeyJetEt", "LVL1JetEtRoI",
      "Key for the xAOD::JetEtRoI output object" };
   /// Write key for the xAOD::JetRoIContainer object
   SG::WriteHandleKey< xAOD::JetRoIContainer > m_jetRoIKey {
      this, "xAODKeyJet", "LVL1JetRoIs",
      "Key for the xAOD::JetRoIContainer output object" };

   /// @}

   /// @name Other properties
   /// @{
   /// Use new-style menu
   Gaudi::Property<bool> m_useNewConfig { this, "UseNewConfig", true, "When true, read the menu from detector store, when false use the L1ConfigSvc" };

   /// Use inputs from the Calo system
   Gaudi::Property< bool > m_doCalo {
      this, "DoCalo", true, "Use inputs from the Calo system" };
   /// Use inputs from the Muon system
   Gaudi::Property< bool > m_doMuon {
      this, "DoMuon", true, "Use inputs from the Muon system" };

   /// List of LVL1 items for e/gamma trigger type
   Gaudi::Property< std::vector< std::string > > m_egammaItem {
      this, "EGammaItem", {}, "List of LVL1 items for e/gamma trigger type" };
   /// List of LVL1 items for tau trigger type
   Gaudi::Property< std::vector< std::string > > m_tauhadItem {
      this, "TauHadItem", {}, "List of LVL1 items for tau trigger type" };
   /// List of LVL1 items for jet trigger type
   Gaudi::Property< std::vector< std::string > > m_jetItem {
      this, "JetItem", {}, "List of LVL1 items for jet trigger type" };
   /// List of LVL1 items for energy sum trigger type
   Gaudi::Property< std::vector< std::string > > m_esumItem {
      this, "ESumItem", {}, "List of LVL1 items for energy sum trigger type" };
   /// List of LVL1 items for high pt muon trigger type
   Gaudi::Property< std::vector< std::string > > m_highmuItem {
      this, "HighMuItem", {},
      "List of LVL1 items for high pt muon trigger type" };
   /// List of LVL1 items for low pt muon trigger type
   Gaudi::Property< std::vector< std::string > > m_lowmuItem {
      this, "LowMuItem", {},
      "List of LVL1 items for low pt muon trigger type" };

   /// @}

}; // class RoIBResultToxAOD

#endif // ANALYSISTRIGGERALGS_ROIBRESULTTOXAOD_H
