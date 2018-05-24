/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGL2PHOTONHYPOTOOL_H
#define TRIGEGAMMAHYPO_TRIGL2PHOTONHYPOTOOL_H 1

#include "GaudiKernel/Property.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigEgamma/TrigPhotonContainer.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"


/**
 * @class Implementation of the Egamma selection for CaloClusters
 * @brief 
 **/

class TrigL2PhotonHypoTool : virtual public ::AthAlgTool
{ 
 public: 
  TrigL2PhotonHypoTool( const std::string& type, 
			  const std::string& name, 
			  const IInterface* parent );

  virtual ~TrigL2PhotonHypoTool();
  virtual StatusCode initialize() override;



  struct PhotonInfo {
    TrigCompositeUtils::Decision* decision;
    const xAOD::TrigPhoton* photon;
    const xAOD::TrigEMCluster* cluster;
    const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
  };


  StatusCode decide( std::vector<PhotonInfo>& decisions )  const;

  bool decideOnSingleObject( const xAOD::TrigPhoton* photon, size_t cutIndex ) const;

  StatusCode inclusiveSelection( std::vector<PhotonInfo>& input ) const;

  StatusCode multiplicitySelection( std::vector<PhotonInfo>& input ) const;

  StatusCode markPassing( std::vector<PhotonInfo>& input, const std::set<size_t>& passing ) const;




  
 private:
  HLT::Identifier m_decisionId;

  Gaudi::Property<bool>  m_decisionPerCluster{ this, "DecisionPerCluster", true, "Is multiplicity requirement refering to photons ( false ) or RoIs ( false ), relevant only in when multiplicity > 1" };

  Gaudi::Property<bool>  m_respectPreviousDecision{ this, "RespectPreviousDecision", false, "If false, ( do not even check ), the decision made for the cluster" };
  Gaudi::Property<bool>  m_acceptAll{ this, "AcceptAll", false, "Ignore selection" };
  Gaudi::Property<bool>  m_etCutOnly{ this, "EtCutOnly", false, "" };
  Gaudi::Property< std::vector<double> >  m_etabin{ this,  "EtaBins", {}, "Eta bins" };

  Gaudi::Property< std::vector<double> >  m_detacluster{ this,  "dETACLUSTERthr", {0.2}, "Delta Eta to Cluster" };
  Gaudi::Property< std::vector<double> >  m_dphicluster{ this,  "dPHICLUSTERthr", {0.2}, "Delta Phi to Cluster" };
  Gaudi::Property< std::vector<double> >  m_F1thr{ this,  "F1thr", {}, "F1 Threshold" };
  Gaudi::Property< std::vector<std::vector<double>> > m_eTthr{this, "m_eTthr", {{}}, "et threshold"};
  Gaudi::Property< std::vector<std::vector<double>> >  m_eT2thr{this, "m_eT2thr", {{}}, "eT2 threshold"};
  Gaudi::Property< std::vector<std::vector<double>> >  m_hadeTthr{this, "m_hadeTthr", {{}}, "hadet threshold"};
  Gaudi::Property< std::vector<std::vector<double>> >  m_hadeT2thr{this, "m_hadeT2thr", {{}}, "hadet2 threshold"};
  Gaudi::Property< std::vector<std::vector<double>> >  m_carcorethr{this, "m_carcorethr", {{}}, "car core threshold"};
  Gaudi::Property< std::vector<std::vector<double>> >  m_caeratiothr{this, "m_caeratiothr", {{}}, " cera ratio threshold"};

  
  size_t m_multiplicity = 1;

  ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "", "Monitoring tool" };
}; 

#endif //> !TRIGEGAMMAHYPO_TRIGL2CALOHYPOTOOL_H
