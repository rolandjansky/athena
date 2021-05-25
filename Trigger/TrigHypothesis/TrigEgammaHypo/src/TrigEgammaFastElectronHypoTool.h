/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TrigEgammaFastElectronHypoTool_H
#define TRIGEGAMMAHYPO_TrigEgammaFastElectronHypoTool_H 1

#include "Gaudi/Property.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"


/**
 * @class Implementation of the Egamma selection for CaloClusters
 * @brief 
 **/

class TrigEgammaFastElectronHypoTool : virtual public ::AthAlgTool
{ 
 public: 
  TrigEgammaFastElectronHypoTool( const std::string& type, 
                                     const std::string& name, 
                                     const IInterface* parent );

  virtual StatusCode initialize() override;


  struct ElectronInfo {
    TrigCompositeUtils::Decision* decision;
    const xAOD::TrigElectron* electron;
    const xAOD::TrigEMCluster* cluster;
    const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
  };

  /**
   * @brief decides upon a collection of electrons
   **/
  StatusCode decide( std::vector<ElectronInfo>& decisions )  const;


 private:

  bool decide( const xAOD::TrigElectron* electron ) const;

  HLT::Identifier m_decisionId;
  Gaudi::Property<bool>  m_acceptAll{ this, "AcceptAll", false, "Ignore selection" };
  Gaudi::Property<bool>  m_doLRT{ this, "DoLRT", false, "apply d0 cut for LRT triggers" };
  Gaudi::Property<float> m_trackPt{ this, "TrackPt",  5.0*Gaudi::Units::GeV, "Track pT requirement ( separate threshold for each electron )" };
  Gaudi::Property<float> m_caloTrackDEta{ this,  "CaloTrackdETA", 0, "Delta Eta between the track and cluster"      }; //loose cut
  Gaudi::Property<float> m_caloTrackDPhi{ this,  "CaloTrackdPHI", 0, "Delta Phi between track and cluster"     }; //loose cut
  Gaudi::Property<float> m_caloTrackdEoverPLow{ this,  "CaloTrackdEoverPLow", 0, "Min E over Pt cut "};
  Gaudi::Property<float> m_caloTrackdEoverPHigh{ this,  "CaloTrackdEoverPHigh", 0, "Max E over pT cut" };
  Gaudi::Property<float> m_trtRatio{ this,  "TRTRatio", 0, "TRT HT ratio" };
  Gaudi::Property<float> m_d0{ this,  "d0Cut", 0, "d0 cut" };
  

  ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "", "Monitoring tool" };
}; 

#endif //> !TRIGEGAMMAHYPO_TRIGEGAMMAFASTCALOHYPOTOOL_H
