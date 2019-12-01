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
  SG::ReadHandleKey<xAOD::MissingETContainer> m_metContainerKey { this, "METContainer", "MET_Reference_AntiKt4EMTopo", "Name of MET  container" };

  Gaudi::Property<std::string> m_metTotalKey { this, "metTotalKey", "FinalTrk", "Subkey for total MET" };

  SG::ReadHandleKey<xAOD::JetContainer> m_jetContainerKey {this,"JetContainerName","AntiKt4EMTopoJets","jet container name"};


  Gaudi::Property<std::vector<std::string>> m_calStrings {this, "METCaloKeys", {}, "MET calo components to plot" };


   Gaudi::Property<std::vector<std::string>> m_metKeys {this,"metKeys",{ }, "MET components to plot"};
   
   Gaudi::Property<bool> m_alltrigger {this,"alltrigger",false};
   Gaudi::Property<bool> m_dometcut {this,"dometcut",false};
   Gaudi::Property<bool> m_doBadJets {this,"DoBadJets",false};
   Gaudi::Property<double> m_metcut {this,"metcut", 80.};

  Gaudi::Property<bool> m_doJetCleaning{this, "DoJetCleaning", false, ""};
   ToolHandle<IJetSelector> m_jetCleaningTool{this, "JetCleaningTool", "", ""};
   // bool isGoodEvent(const EventContext& ctx ) const;
   bool isGoodEvent( const EventContext& ctx ) const;

};
#endif
