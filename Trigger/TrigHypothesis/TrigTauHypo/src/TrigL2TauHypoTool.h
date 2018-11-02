/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGL2TAUHYPOTOOL_H
#define TRIGEGAMMAHYPO_TRIGL2TAUHYPOTOOL_H 1

#include "GaudiKernel/Property.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
//#include "xAODTrigTauJet/TrigElectronContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "ITrigL2TauCaloHypoTool.h"


/**
 * @class Implementation of the Egamma selection for CaloClusters
 * @brief 
 **/

class TrigL2TauHypoTool : public extends<AthAlgTool, ITrigL2TauCaloHypoTool> {
 public:
  TrigL2TauHypoTool( const std::string& type, 
			  const std::string& name, 
			  const IInterface* parent );

  virtual ~TrigL2TauHypoTool();

  virtual StatusCode initialize() override;

  virtual StatusCode decide( std::vector<ITrigL2TauCaloHypoTool::ClusterInfo>& input )  const override;

  virtual bool decide( const ITrigL2TauCaloHypoTool::ClusterInfo& i ) const override;

  /*struct TauInfo {
    TrigCompositeUtils::Decision* decision;
    //const xAOD::TrigElectron* electron;
    const xAOD::CaloCluster* cluster;
    const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
  };*/

  /**
   * @brief decides upon a collection of electrons
   **/
  //StatusCode decide( std::vector<ITrigL2TauCaloHypoTool::ClusterInfo>& input )  const override;
  //StatusCode decide( std::vector<TauInfo>& decisions )  const;

  /**
   * @brief Auxiluary method, single electron selection
   **/
  //bool decideOnSingleObject( const xAOD::CaloCluster* caloclus, size_t cutIndex ) const;

  /**
   * @brief actual implementation of decide, in case of inclusive selection ( one object cut )
   **/
  //StatusCode inclusiveSelection( std::vector<ClusterInfo>& input ) const;

  /**
   * @brief actual implementation of decide, in case of multiple objects selection ( independentone )
   **/
  //StatusCode multiplicitySelection( std::vector<ClusterInfo>& input ) const;

  /**
   * @brief stores decisions for all object passing multiple cuts
   * The passsingSelection inner vectors have to have size == input size
   **/
  //StatusCode markPassing( std::vector<TauInfo>& input, const std::set<size_t>& passing ) const;




  
 private:
  HLT::Identifier m_decisionId;
  Gaudi::Property<bool>  m_decisionPerCluster{ this, "DecisionPerCluster", true, "Is multiplicity requirement refering to taus ( false ) or RoIs/clusters with taus ( false ), relevant only in when multiplicity > 1" };

  Gaudi::Property<bool>  m_respectPreviousDecision{ this, "RespectPreviousDecision", false, "If false, ( do not even check ), the decision made for the cluster" };
  Gaudi::Property<bool>  m_acceptAll{ this, "AcceptAll", false, "Ignore selection" };
  //Gaudi::Property< std::vector<float> > m_trackPt{ this, "TrackPt",  { float( 5.0*CLHEP::GeV ) }, "Track pT requirement ( separate threshold for each electron )" };
  Gaudi::Property< std::vector<float> > m_lowerPtCut{ this,  "LowerPtCut", { float ( 5.0*CLHEP::GeV ) }, "Cluster Pt Preselction cut (separate threshold for each tau)"      }; //loose cut
  Gaudi::Property< std::vector<float> > m_clusterCone{ this,  "ClusterEnergySumCone", {0}, "Cluster cone size for energy sum"     }; //loose cut
  Gaudi::Property< std::vector<bool> > m_cellCut{ this,  "UseCellCut", {0}, "Decide to apply or not the Cell-based shower cut"};
  Gaudi::Property< std::vector<float> > m_cellCore{ this,  "CellVariableCore", {0}, "Cell Variable Core" };
  Gaudi::Property< std::vector<float> > m_cellOuter{ this,  "CellVariableOuter", {0}, "Cell Variable Outer" };
  Gaudi::Property< std::vector<float> > m_coreFractionCut{ this,  "CoreFractionCut", {0}, "Cell core fraction cut" };
  Gaudi::Property< std::vector<float> > m_hadRadiusCut{ this,  "HadRadiusCut", {0}, "Hadronic Radius Cut" };

  size_t m_multiplicity = 1;

  ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "", "Monitoring tool" };
}; 

#endif //> !TRIGTAUHYPO_TRIGL2TAUHYPOTOOL_H
