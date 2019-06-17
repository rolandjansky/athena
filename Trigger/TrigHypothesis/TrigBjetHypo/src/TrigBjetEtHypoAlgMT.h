/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute( const EventContext& context ) const override;

 private: 
  TrigBjetEtHypoAlgMT();

  StatusCode retrieveJetsFromStoreGate( const EventContext& context,const xAOD::JetContainer*& ) const;
  StatusCode retrieveJetsFromEventView( const EventContext& context,
					const xAOD::JetContainer*&,
					SG::ReadHandle< TrigCompositeUtils::DecisionContainer >& ) const;

 private:
  ToolHandleArray< TrigBjetEtHypoTool > m_hypoTools {this,"HypoTools",{},"Hypo Tools"};

 private:
  Gaudi::Property< bool > m_readFromView {this,"ReadFromView",false,"Use configuration that supports Event View"};
  Gaudi::Property< std::string > m_roiLink {this,"RoILink","roi","RoI link to attach to the output decision"};
  Gaudi::Property< std::string > m_jetLink {this,"JetLink","jets","Jets link to attach to the output decision"};

  SG::ReadHandleKey< xAOD::JetContainer > m_inputJetsKey {this,"Jets","Undefined","Input Jet Container Key"};
  SG::ReadHandleKey< TrigRoiDescriptorCollection > m_inputRoIKey {this,"RoIs","Undefined","Input RoIs that will be linked to the output decision"};
}; 

#endif //> !TRIGBJETHYPO_TRIGBJETETHYPOALG_H
