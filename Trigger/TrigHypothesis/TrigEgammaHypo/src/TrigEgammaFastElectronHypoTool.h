/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGEGAMMAFASTELECTRONHYPOTOOL_H
#define TRIGEGAMMAHYPO_TRIGEGAMMAFASTELECTRONHYPOTOOL_H 1

#include "Gaudi/Property.h"
#include "CLHEP/Units/SystemOfUnits.h"
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

  virtual ~TrigEgammaFastElectronHypoTool();
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

  /**
   * @brief Auxiluary method, single electron selection
   **/
  bool decideOnSingleObject( const xAOD::TrigElectron* electron, size_t cutIndex ) const;

  /**
   * @brief actual implementation of decide, in case of inclusive selection ( one object cut )
   **/
  StatusCode inclusiveSelection( std::vector<ElectronInfo>& input ) const;

  /**
   * @brief actual implementation of decide, in case of multiple objects selection ( independentone )
   **/
  StatusCode multiplicitySelection( std::vector<ElectronInfo>& input ) const;

  /**
   * @brief stores decisions for all object passing multiple cuts
   * The passsingSelection inner vectors have to have size == input size
   **/
  StatusCode markPassing( std::vector<ElectronInfo>& input, const std::set<size_t>& passing ) const;




  
 private:
  HLT::Identifier m_decisionId;
  Gaudi::Property<bool>  m_decisionPerCluster{ this, "DecisionPerCluster", true, "Is multiplicity requirement refering to electrons ( false ) or RoIs/clusters with electrons ( false ), relevant only in when multiplicity > 1" };
  Gaudi::Property<bool>  m_acceptAll{ this, "AcceptAll", false, "Ignore selection" };
  Gaudi::Property< std::vector<float> > m_trackPt{ this, "TrackPt",  { float( 5.0*CLHEP::GeV ) }, "Track pT requirement ( separate threshold for each electron )" };
  Gaudi::Property< std::vector<float> > m_caloTrackDEta{ this,  "CaloTrackdETA", {0}, "Delta Eta between the track and cluster"      }; //loose cut
  Gaudi::Property< std::vector<float> > m_caloTrackDPhi{ this,  "CaloTrackdPHI", {0}, "Delta Phi between track and cluster"     }; //loose cut
  Gaudi::Property< std::vector<float> > m_caloTrackdEoverPLow{ this,  "CaloTrackdEoverPLow", {0}, "Min E over Pt cut "};
  Gaudi::Property< std::vector<float> > m_caloTrackdEoverPHigh{ this,  "CaloTrackdEoverPHigh", {0}, "Max E over pT cut" };
  Gaudi::Property< std::vector<float> > m_trtRatio{ this,  "TRTRatio", {0}, "TRT HT ratio" };

  
  size_t m_multiplicity = 1;

  ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "", "Monitoring tool" };
}; 

#endif //> !TRIGEGAMMAHYPO_TRIGEGAMMAFASTCALOHYPOTOOL_H
