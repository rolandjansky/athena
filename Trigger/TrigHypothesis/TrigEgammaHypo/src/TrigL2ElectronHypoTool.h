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


  struct Input {
    TrigCompositeUtils::Decision* decision;
    const xAOD::TrigElectron* electron;
    const xAOD::TrigEMCluster* cluster;
    const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
  };

  /**
   * @brief decides upon a collection of electrons
   **/
  StatusCode decide( std::vector<Input>& decisions )  const;

  /**
   * @brief Auxiluary method, single electron selection
   **/
  bool decideOnSingleObject( const xAOD::TrigElectron* electron, size_t cutIndex ) const;

  /**
   * @brief actual implementation of decide, in case of inclusive selection (one object cut)
   **/
  StatusCode inclusiveSelection( std::vector<Input>& input ) const;

  /**
   * @brief actual implementation of decide, in case of multiple objects selection (independentone)
   **/
  StatusCode multiplicitySelection( std::vector<Input>& input ) const;

  /**
   * @brief stores decisions for all object passing multiple cuts
   * The passsingSelection inner vectors have to have size == input size
   **/
  StatusCode markPassing( std::vector<Input>& input, const std::set<size_t>& passing ) const;




  
 private:
  HLT::Identifier m_decisionId;
  Gaudi::Property<bool>  m_decisionPerCluster{ this, "DecisionPerCluster", true, "Is multiplicity requirement refering to electrons (false) or RoIs/clusters with electrons (false), relevant only in when multiplicity > 1" };

  Gaudi::Property<bool>  m_respectPreviousDecision{ this, "RespectPreviousDecision", false, "If false, (do not even check), the decision made for the cluster" };
  Gaudi::Property<bool>  m_acceptAll{ this, "AcceptAll", false, "Ignore selection" };
  Gaudi::Property< std::vector<float> > m_trackPt{ this, "TrackPt",  { float(5.0*CLHEP::GeV) }, "Track pT requirement (separate threshold for each electron)" };
  Gaudi::Property< std::vector<float> > m_caloTrackDEta{ this,  "CaloTrackdETA", {0}, "Delta Eta between the track and cluster"      }; //loose cut
  Gaudi::Property< std::vector<float> > m_caloTrackDPhi{ this,  "CaloTrackdPHI", {0}, "Delta Phi between track and cluster"     }; //loose cut
  Gaudi::Property< std::vector<float> > m_caloTrackdEoverPLow{ this,  "CaloTrackdEoverPLow", {0}, "Min E over Pt cut "};
  Gaudi::Property< std::vector<float> > m_caloTrackdEoverPHigh{ this,  "CaloTrackdEoverPHigh", {0}, "Max E over pT cut" };
  Gaudi::Property< std::vector<float> > m_trtRatio{ this,  "TRTRatio", {0}, "TRT HT ratio" };

  
  size_t m_multiplicity = 1;

  ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "", "Monitoring tool" };
}; 

inline const InterfaceID& TrigL2ElectronHypoTool::interfaceID() 
{ 
   return IID_TrigL2ElectronHypoTool; 
}


#endif //> !TRIGEGAMMAHYPO_TRIGL2CALOHYPOTOOL_H
