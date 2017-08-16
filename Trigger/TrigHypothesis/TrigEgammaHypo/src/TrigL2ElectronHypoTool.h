/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGL2ELECTRONHYPOTOOL_H
#define TRIGEGAMMAHYPO_TRIGL2ELECTRONHYPOTOOL_H 1

#include "GaudiKernel/Property.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"


static const InterfaceID IID_TrigL2ElectronHypoTool("TrigL2ElectronHypoTool", 1, 0);

/**
 * @class Implementation of the Egamma selection for CaloClusters
 * @brief 
 **/


using namespace TrigCompositeUtils;

class TrigL2ElectronHypoTool
  : virtual public ::AthAlgTool
{ 
 public: 
  TrigL2ElectronHypoTool( const std::string& type, 
			  const std::string& name, 
			  const IInterface* parent );

  virtual ~TrigL2ElectronHypoTool();
  StatusCode initialize() override;
  StatusCode finalize() override;

  static const InterfaceID& interfaceID();

  TrigCompositeUtils::DecisionID decisionId() const {
    return m_id.numeric();
  }

  struct Input {
    Decision* decision;
    const xAOD::TrigElectron* electron;
    const xAOD::TrigEMCluster* cluster;
    const DecisionIDContainer previousDecisionIDs;
  };

  /**
   * @brief decides upon a collection of electrons
   **/
  StatusCode decide( std::vector<Input>& decisions )  const;
  std::pair<bool,int> decideOnSingleObject( const xAOD::TrigElectron* electron ) const;
  

  
 private:
  HLT::Identifier m_id;
  Gaudi::Property<int>   m_multiplicity{ this, "Multiplicity", 1, "Multiplicity requirement" };
  Gaudi::Property<bool>  m_roisMultiplicity{ this, "RoIsMultiplicity", true, "Is multiplicity requirement refering to electrons (false) or RoIs with electrons (false)" };

  Gaudi::Property<bool>  m_acceptAll{ this, "AcceptAll", false, "Ignore selection" };
  Gaudi::Property<float> m_trackPtthr{ this, "TrackPt",  5.0*CLHEP::GeV, "Track pT requirement" };
  Gaudi::Property<float> m_calotrackdeta{ this,  "CaloTrackdETA", 0, "Delta Eta between the track and cluster"      }; //loose cut
  Gaudi::Property<float> m_calotrackdphi{ this,  "CaloTrackdPHI", 0, "Delta Phi between track and cluster"     }; //loose cut
  Gaudi::Property<float> m_calotrackdeoverp_low{ this,  "CaloTrackdEoverPLow", 0, "Min E over Pt cut "};
  Gaudi::Property<float> m_calotrackdeoverp_high{ this,  "CaloTrackdEoverPHigh", 0, "Max E over pT cut" };
  Gaudi::Property<float> m_trtratio{ this,  "TRTRatio", 0, "TRT HT ratio" };

  ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "GenericMonitoringTool/MOnTool", "Monitoring tool"};
}; 

inline const InterfaceID& TrigL2ElectronHypoTool::interfaceID() 
{ 
   return IID_TrigL2ElectronHypoTool; 
}


#endif //> !TRIGEGAMMAHYPO_TRIGL2CALOHYPOTOOL_H
