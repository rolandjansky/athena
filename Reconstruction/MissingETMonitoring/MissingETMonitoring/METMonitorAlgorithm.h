/*
  # Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef METMONITORALGORITHM_H 
#define METMONITORALGORITHM_H


#include "AthenaMonitoring/AthMonitorAlgorithm.h" 
#include "AthenaMonitoring/Monitored.h" 
#include "xAODJet/JetContainer.h" 

#include "StoreGate/ReadHandleKey.h" 
#include "xAODMissingET/MissingET.h" 
#include "xAODMissingET/MissingETContainer.h" 
#include "xAODMissingET/MissingETComposition.h" 

#include "TRandom3.h" 
// JVT 
#include "JetInterface/IJetUpdateJvt.h"
// Jet cleaning 
#include "JetInterface/IJetSelector.h"
class METMonitoringAlg : public AthMonitorAlgorithm {
public:     
  METMonitoringAlg( const std::string& name, ISvcLocator* pSvcLocator );     
  virtual ~METMonitoringAlg();    
  StatusCode initialize();    
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

 private:
  SG::ReadHandleKey<xAOD::MissingETContainer> m_metTrackKey { this, "METTrack_AOD_Name", "MET_Track", "Name of MET Track container" };
  SG::ReadHandleKey<xAOD::MissingETContainer> m_metAntiKtEMTopoKey { this, "METAntiKtEMTopo_AOD_Name", "MET_Reference_AntiKt4EMTopo", "Name of MET AntiKt EM Topo container" };
  //  SG::ReadHandleKey<xAOD::MissingETContainer> m_metAntiKtEMTopoKey ;

SG::ReadHandleKey<xAOD::MissingETContainer> m_metCaloContainerKey{ this, "METCalo_AOD_Name", "MET_Calo", "Name of MET Calo container" };

//   SG::ReadHandleKey<xAOD::JetContainer> m_jetContainerKey;
   Gaudi::Property<std::string> m_jetContainerName {this,"JetContainerName","AntiKt4EMTopoJets","jet container name"};


   const std::vector<std::string> m_calStrings {       "PEMB", "EMB", "PEME", "EME", "TILE", "HEC", "FCAL" };

   //      std::vector<std::string> m_metKeys;   
   std::string   m_metFinKey = "MET_RefFinal";   
   std::string    m_metCalKey = "MET_LocHadTopo";   
   std::string    m_metRegKey = "";   
   std::string    m_jetColKey = "AntiKt4EMTopoJets";
   std::string    m_eleColKey = "Electrons";    
   std::string    m_muoColKey = "Muons";

   //   Gaudi::Property<std::vector<std::string>> m_metKeys {this,"metKeys",{"MET_RefEle", "MET_RefGamma" }};
   //      Gaudi::Property<std::vector<std::string>> m_metKeys {this,"metKeys",{"MET_RefEle", "MET_RefGamma" }, "explain this property"};
   Gaudi::Property<std::vector<std::string>> m_metKeys {this,"metKeys",{ }, "explain this property"};
   
   Gaudi::Property<bool> m_alltrigger {this,"alltrigger",false};
   Gaudi::Property<bool> m_dometcut {this,"dometcut",false};
   Gaudi::Property<bool> m_doBadJets {this,"DoBadJets",false};
   Gaudi::Property<double> m_metcut {this,"metcut",80.};
   //   Gaudi::Property<std::vector<float>> m_cellEnergyUpperLimitsHG{this,         "CellEnergyUpperLimitsHG", {}, "Energy upper limits of the HG cell-time histograms"};
   //   Gaudi::Property<std::vector<std::string>> m_metKeys{this,         "CellEnergyUpperLimitsHG", {}, "Energy upper limits of the HG cell-time histograms"};

   //    Gaudi::Property< std::string > m_metKeys {this,"metKeys","MET_RefFinal"};
   Gaudi::Property<bool> m_doJetCleaning{this, "DoJetCleaning", false, ""};
   ToolHandle<IJetSelector> m_jetCleaningTool{this, "JetCleaningTool", "", ""};
   // bool isGoodEvent(const EventContext& ctx ) const;
   bool isGoodEvent( ) const;

};
#endif
