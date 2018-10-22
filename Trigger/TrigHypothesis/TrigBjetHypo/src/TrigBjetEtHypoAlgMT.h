/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGBJETHYPO_TRIGBJETETHYPOALGMT_H
#define TRIGBJETHYPO_TRIGBJETETHYPOALGMT_H 1

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "DecisionHandling/TrigCompositeUtils.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

#include "TrigBjetEtHypoTool.h"
#include "DecisionHandling/HypoBase.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"

/**
 * @class Implements b-jet selection for the new HLT framework
 * @brief 
 **/

class TrigBjetEtHypoAlgMT : public ::HypoBase { 
 public: 

  TrigBjetEtHypoAlgMT( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrigBjetEtHypoAlgMT(); 

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute_r( const EventContext& context ) const override;
  virtual StatusCode  finalize() override;
 
 private: 
  TrigBjetEtHypoAlgMT();
  StatusCode shortListJets( const xAOD::JetContainer*,
			    std::unique_ptr< xAOD::JetContainer >&,
			    std::unique_ptr< TrigRoiDescriptorCollection >&,
			    const Amg::Vector3D* primaryVertex = nullptr ) const; // TMP

 private:
  ToolHandleArray< TrigBjetEtHypoTool > m_hypoTools {this,"HypoTools",{},"Hypo Tools"};

 private:
  // This part is taken from Jet Splitter
  Gaudi::Property< bool > m_imposeZconstraint {this,"ImposeZconstraint",false,"Impose Constraint on PV z, thus selecting Jets pointing to PV"};
  Gaudi::Property< float > m_etaHalfWidth {this,"EtaHalfWidth",0.4,"Eta Half Width"};
  Gaudi::Property< float > m_phiHalfWidth {this,"PhiHalfWidth",0.4,"Phi Half Width"};
  Gaudi::Property< float > m_zHalfWidth {this,"ZHalfWidth",10.0,"Z Half Width in mm"};
  Gaudi::Property< float > m_minJetEt {this,"JetMinEt",15000,"Minimum of Output Jet Et in GeV"};
  Gaudi::Property< float > m_maxJetEta {this,"JetMaxEta",3.2,"Maximum eta acceptance of output Jet"};
  //=========== Handles
  SG::ReadHandleKey< xAOD::JetContainer > m_inputJetsKey {this,"Jets","Jets","Input Jet Container Key"};
  SG::WriteHandleKey< xAOD::JetContainer > m_outputJetsKey {this,"OutputJets","SplitJets","Output Jet Container Key"};
  SG::WriteHandleKey< TrigRoiDescriptorCollection > m_outputRoiKey {this,"OutputRoi","SplitJet","Output RoI Container Key -- Same as OutputJets"};

}; 

#endif //> !TRIGBJETHYPO_TRIGBJETETHYPOALG_H
