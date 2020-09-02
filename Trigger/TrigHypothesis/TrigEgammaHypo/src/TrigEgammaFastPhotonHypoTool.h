/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGEGAMMAFASTPHOTONHYPOTOOL_H
#define TRIGEGAMMAHYPO_TRIGEGAMMAFASTPHOTONHYPOTOOL_H 1

#include "Gaudi/Property.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigEgamma/TrigPhotonContainer.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"


/**
 * @class Implementation of the Egamma selection for CaloClusters
 * @brief 
 **/

class TrigEgammaFastPhotonHypoTool : virtual public ::AthAlgTool
{ 
 public: 
  TrigEgammaFastPhotonHypoTool( const std::string& type, 
			  const std::string& name, 
			  const IInterface* parent );

  virtual ~TrigEgammaFastPhotonHypoTool();
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
  Gaudi::Property<bool>  m_acceptAll{ this, "AcceptAll", false, "Ignore selection" };
  Gaudi::Property<bool>  m_etCutOnly{ this, "EtCutOnly", true, "" };
  Gaudi::Property< std::vector<double> >  m_etabin{ this,  "EtaBins", {0, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47}, "Eta bins" };

  Gaudi::Property< std::vector<double> >  m_detacluster{ this,  "dETACLUSTERthr", {0.1}, "Delta Eta to Cluster" };
  Gaudi::Property< std::vector<double> >  m_dphicluster{ this,  "dPHICLUSTERthr", {0.1}, "Delta Phi to Cluster" };
  Gaudi::Property< std::vector<double> >  m_F1thr{ this,  "F1thr", {0.005}, "F1 Threshold" };
  Gaudi::Property< std::vector<std::vector<double>> > m_eTthr{this, "ETthr", {{0.*CLHEP::GeV}}, "et threshold"};
  Gaudi::Property< std::vector<std::vector<double>> >  m_eT2thr{this, "ET2thr", {{90.0*CLHEP::GeV}}, "eT2 threshold"};
  Gaudi::Property< std::vector<std::vector<double>> >  m_hadeTthr{this, "HADETthr", {{0.}}, "hadet threshold"};
  Gaudi::Property< std::vector<std::vector<double>> >  m_hadeT2thr{this, "HADET2thr", {{0.}}, "hadet2 threshold"};
  Gaudi::Property< std::vector<std::vector<double>> >  m_carcorethr{this, "CARCOREthr", {{0.}}, "car core threshold"};
  Gaudi::Property< std::vector<std::vector<double>> >  m_caeratiothr{this, "CAERATIOthr", {{0.}}, " cera ratio threshold"};

  
  size_t m_multiplicity = 1;

  ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "", "Monitoring tool" };
}; 

#endif //> !TRIGEGAMMAHYPO_TRIGL2CALOHYPOTOOL_H
