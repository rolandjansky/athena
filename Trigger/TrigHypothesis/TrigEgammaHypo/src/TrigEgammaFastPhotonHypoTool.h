/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TrigEgammaFastPhotonHypoTool_H
#define TRIGEGAMMAHYPO_TrigEgammaFastPhotonHypoTool_H 1

#include "Gaudi/Property.h"
#include "GaudiKernel/SystemOfUnits.h"
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

    virtual StatusCode initialize() override;

    struct PhotonInfo {
      TrigCompositeUtils::Decision* decision;
      const xAOD::TrigPhoton* photon;
      const xAOD::TrigEMCluster* cluster;
      const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
    };


    StatusCode decide( std::vector<PhotonInfo>& decisions )  const;


  private:

    bool decide( const xAOD::TrigPhoton* photon ) const;

    int findCutIndex( float eta ) const;

    HLT::Identifier m_decisionId;

    Gaudi::Property<bool>                  m_acceptAll{ this, "AcceptAll", false, "Ignore selection" };
    Gaudi::Property< std::vector<float> >  m_etabin{ this,  "EtaBins", {0, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47}, "Eta bins" };
    Gaudi::Property< std::vector<float> >  m_F1thr{ this,  "F1thr", {0.005}, "F1 Threshold" };
    Gaudi::Property< std::vector<float> >  m_eTthr{this, "ETthr", {0.*Gaudi::Units::GeV}, "et threshold"};
    Gaudi::Property< std::vector<float> >  m_eT2thr{this, "ET2thr", {90.0*Gaudi::Units::GeV}, "eT2 threshold"};
    Gaudi::Property< std::vector<float> >  m_hadeTthr{this, "HADETthr", {0.}, "hadet threshold"};
    Gaudi::Property< std::vector<float> >  m_hadeT2thr{this, "HADET2thr", {0.}, "hadet2 threshold"};
    Gaudi::Property< std::vector<float> >  m_carcorethr{this, "CARCOREthr", {0.}, "car core threshold"};
    Gaudi::Property< std::vector<float> >  m_caeratiothr{this, "CAERATIOthr", {0.}, " cera ratio threshold"};

    ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "", "Monitoring tool" };
}; 

#endif //> !TRIGEGAMMAHYPO_TRIGL2CALOHYPOTOOL_H
