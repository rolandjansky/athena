/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CTP_CTPSIMULATION_H
#define TRIGT1CTP_CTPSIMULATION_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "Gaudi/Property.h"
#include "StoreGate/DataHandle.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

// monitoring from HLT
#include "GaudiKernel/ITHistSvc.h"


// data default location
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"

// input/output data classes
#include "TrigT1Interfaces/MuCTPICTP.h"
#include "TrigT1Interfaces/EmTauCTP.h"
#include "TrigT1Interfaces/JetCTP.h"
#include "TrigT1Interfaces/EnergyCTP.h"
#include "TrigT1Interfaces/FrontPanelCTP.h"
#include "xAODTrigger/JetRoIContainer.h"
#include "xAODTrigger/EnergySumRoI.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigT1Result/CTP_RDO.h"
#include "TrigT1Interfaces/CTPSLink.h"

// new configuration data
#include "TrigConfData/L1Menu.h"


// internal
#include "./ItemMap.h"
#include "./ThresholdMap.h"
#include "./ResultBuilder.h"

#include "GaudiKernel/LockedHandle.h"
#include "AthenaKernel/RNGWrapper.h"

#include "TH1.h"
#include "TH2.h"

#include <map>
#include <string>
#include <mutex>

namespace LVL1 {
   class CPRoIDecoder;
   class JEPRoIDecoder;
}

namespace LVL1CTP {

   class CTPSimulation : public ::AthReentrantAlgorithm {

   public:

      CTPSimulation( const std::string& name, ISvcLocator* pSvcLocator );
      ~CTPSimulation();

      virtual StatusCode initialize() override;

      virtual StatusCode start() override;

      virtual StatusCode execute( const EventContext& context ) const override;

      virtual StatusCode finalize() override;


   private:

      struct ConfigSource {
      ConfigSource(const TrigConf::CTPConfig* ctpConfig, const TrigConf::L1Menu* l1menu) : m_ctpConfig(ctpConfig), m_l1menu(l1menu) {}
         const TrigConf::CTPConfig* m_ctpConfig {nullptr}; // run 2
         const TrigConf::L1Menu * m_l1menu { nullptr }; // run 3
         const TrigConf::CTPConfig* ctpConfig() const { return m_ctpConfig; }
         const TrigConf::L1Menu* l1menu() const { return m_l1menu; }
      };


      // histogramming related 
      StatusCode bookHists() const;
      StatusCode setHistLabels() const;
      StatusCode createMultiplicityHist(const std::string & type, unsigned int maxMult = 10 ) const;
      StatusCode setMultiplicityHistLabels(const ConfigSource & cfgSrc, const std::string & type, TrigConf::L1DataDef::TriggerType tt ) const;
      StatusCode hbook(const std::string & path, std::unique_ptr<TH1> hist) const;
      StatusCode hbook(const std::string & path, std::unique_ptr<TH2> hist) const;
      StatusCode storeMetadata() const;
      LockedHandle<TH1> & get1DHist(const std::string & histName) const;
      LockedHandle<TH2> & get2DHist(const std::string & histName) const;      
      std::string getBaseHistPath() const;

      // execution related
      StatusCode fillInputHistograms(const EventContext& context) const;

      StatusCode extractMultiplicities(std::map<std::string, unsigned int> & thrMultiMap, const EventContext& context) const;

      StatusCode simulateItems(const std::map<std::string, unsigned int> & thrMultiMap, const EventContext& context) const;

      // private member functions
      unsigned int calculateMultiplicity    ( const TrigConf::TriggerThreshold * confThr, const EventContext& context ) const;
      unsigned int calculateJetMultiplicity ( const TrigConf::TriggerThreshold * confThr, const EventContext& context ) const;
      unsigned int calculateEMMultiplicity  ( const TrigConf::TriggerThreshold * confThr, const EventContext& context ) const;
      unsigned int calculateTauMultiplicity ( const TrigConf::TriggerThreshold * confThr, const EventContext& context ) const;
      unsigned int calculateMETMultiplicity ( const TrigConf::TriggerThreshold * confThr, const EventContext& context ) const;
      unsigned int calculateMuonMultiplicity( const TrigConf::TriggerThreshold * confThr, const EventContext& context ) const;
      unsigned int calculateTopoMultiplicity( const TrigConf::TriggerThreshold * confThr, const EventContext& context ) const;

      unsigned int calculateMultiplicity    ( const TrigConf::L1Threshold & confThr, const TrigConf::L1Menu * l1menu, const EventContext& context ) const;
      unsigned int calculateJetMultiplicity ( const TrigConf::L1Threshold & confThr, const TrigConf::L1Menu * l1menu, const EventContext& context ) const;
      unsigned int calculateEMMultiplicity  ( const TrigConf::L1Threshold & confThr, const TrigConf::L1Menu * l1menu, const EventContext& context ) const;
      unsigned int calculateTauMultiplicity ( const TrigConf::L1Threshold & confThr, const TrigConf::L1Menu * l1menu, const EventContext& context ) const;
      unsigned int calculateMETMultiplicity ( const TrigConf::L1Threshold & confThr, const TrigConf::L1Menu * l1menu, const EventContext& context ) const;
      unsigned int calculateMuonMultiplicity( const TrigConf::L1Threshold & confThr, const TrigConf::L1Menu * l1menu, const EventContext& context ) const;
      unsigned int calculateTopoMultiplicity( const TrigConf::L1Threshold & confThr, const TrigConf::L1Menu * l1menu, const EventContext& context ) const;

      // member variables
      mutable std::once_flag m_onceflag;

      // Needed services and tools
      ServiceHandle<ITHistSvc> m_histSvc { this, "THistSvc", "THistSvc/THistSvc", "Histogramming svc" };
      ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc { this, "TrigConfigSvc", "TrigConf::TrigConfigSvc/TrigConfigSvc", "Trigger configuration service" };
      ServiceHandle<StoreGateSvc> m_detStore { this, "DetectorStore", "StoreGateSvc/DetectorStore", "Detector store to get the menu" };
      ToolHandle<LVL1CTP::ResultBuilder> m_resultBuilder { this, "ResultBuilder", "LVL1CTP__ResultBuilder/ResultBuilder", "Builds the CTP result" };

      // random engine for calculating prescales
      ATHRNG::RNGWrapper m_RNGEngines;

      // thread safe histogram handlers
      mutable std::map<std::string, LockedHandle<TH1>> m_hist1D  ATLAS_THREAD_SAFE;
      mutable std::map<std::string, LockedHandle<TH2>> m_hist2D  ATLAS_THREAD_SAFE;

      // inputs
      // new L1Topo 
      SG::ReadHandleKey<LVL1::FrontPanelCTP> m_iKeyTopo{ this, "TopoInput", LVL1::DEFAULT_L1TopoCTPLocation, "Input from topo" };
      // legacy L1Topo 
      SG::ReadHandleKey<LVL1::FrontPanelCTP> m_iKeyLegacyTopo{ this, "LegacyTopoInput", LVL1::DEFAULT_L1TopoLegacyCTPLocation, "Input from legacy topo" };
      // MUCTPI
      SG::ReadHandleKey<LVL1::MuCTPICTP> m_iKeyMuctpi{ this, "MuctpiInput", LVL1MUCTPI::DEFAULT_MuonCTPLocation, "Input from Muctpi" };
      // CTPIN
      SG::ReadHandleKey<LVL1::EmTauCTP>  m_iKeyCtpinEM{  this, "CtpinEMInput",  LVL1::TrigT1CaloDefs::EmTauCTPLocation, "Input from CTPIN em and tau (legacy)" };
      SG::ReadHandleKey<LVL1::JetCTP>    m_iKeyCtpinJet{ this, "CtpinJetInput", LVL1::TrigT1CaloDefs::JetCTPLocation, "Input from CTPIN jet (legacy)" };
      SG::ReadHandleKey<LVL1::EnergyCTP> m_iKeyCtpinXE{  this, "CtpinXEInput",  LVL1::TrigT1CaloDefs::EnergyCTPLocation, "Input from CTPIN energy (te,xe,xs - legacy)" };
      // new FEX object collections (temporary until moved to L1Topo simulation)
      // jFEX
      SG::ReadHandleKey< xAOD::JetRoIContainer >  m_iKeyJFexJets {  this, "jFexJetInput",  "jRoundJets", "Input list of jFEX jets" };
      SG::ReadHandleKey< xAOD::JetRoIContainer >  m_iKeyJFexLJets{  this, "jFexLJetInput",  "jRoundLargeRJets", "Input list of jFEX large-R jets" };
      // gFEX
      SG::ReadHandleKey< xAOD::JetRoIContainer >  m_iKeyGFexJets    {  this, "gFexJetInput", "gL1Jets", "Input list of gFEX jets" };
      SG::ReadHandleKey< xAOD::EnergySumRoI >     m_iKeyGFexMETPufit{  this, "gFexMETPufitInput", "gXEPUFIT_MET", "Input list of gFEX MET Pufit" };
      SG::ReadHandleKey< xAOD::EnergySumRoI >     m_iKeyGFexMETRho  {  this, "gFexMETRhoInput", "gXERHO_MET", "Input list of gFEX MET Rho" };
      SG::ReadHandleKey< xAOD::EnergySumRoI >     m_iKeyGFexMETJwoJ {  this, "gFexMETJwoJInput", "gXEJWOJ_MET", "Input list of gFEX MET JwoJ" };
      // eFEX
      SG::ReadHandleKey< xAOD::TrigEMClusterContainer >  m_iKeyEFexCluster{  this, "eFexClusterInput", "SClusterCl", "Input list of eFEX cluster" };
      SG::ReadHandleKey< xAOD::EmTauRoIContainer >       m_iKeyEFexTau    {  this, "eFexTauInput", "SClusterTau", "Input list of eFEX tau" };

      // outputs
      SG::WriteHandleKey<CTP_RDO>  m_oKeyRDO  {this, "RDOOutput", LVL1CTP::DEFAULT_RDOOutputLocation, "Output of CTP RDO object (sim)"};
      SG::WriteHandleKey<CTPSLink> m_oKeySLink{this, "ROIOutput", LVL1CTP::DEFAULT_CTPSLinkLocation, "Output of CTP SLink object (sim)"};


      // properties
      Gaudi::Property<bool> m_isData { this, "IsData", false, "emulate CTP as part of MC or rerun on data" };
      Gaudi::Property<std::string>  m_histPath { this, "HistPath",  "/EXPERT/L1", "Booking path for the histogram" };
      Gaudi::Property<bool> m_useNewConfig { this, "UseNewConfig", false, "When true, read the menu from detector store, when false use the L1ConfigSvc" };
      Gaudi::Property<bool> m_forceBunchGroupPattern { this, "ForceBunchGroupPattern", true, "When true, ignore the bunchgroups and use the provided BunchGroupPattern" };
      Gaudi::Property<unsigned int> m_bunchGroupPattern { this, "BunchGroupPattern", 0x0003, "Bunchgroup pattern applied at every event, useful for simulation. Bit x corresponds to bunchgroup x" };

      // to decode the L1 Run-2 hardware ROIs from data
      LVL1::CPRoIDecoder * m_decoder { nullptr };
      LVL1::JEPRoIDecoder * m_jetDecoder { nullptr };
   };

}

#endif
