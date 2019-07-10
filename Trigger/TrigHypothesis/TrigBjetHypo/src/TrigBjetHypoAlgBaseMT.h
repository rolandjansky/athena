/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGBJETHYPO_TRIGBJETHYPOALGBASEMT_H
#define TRIGBJETHYPO_TRIGBJETHYPOALGBASEMT_H 1

#include "DecisionHandling/HypoBase.h"

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthLinks/ElementLinkVector.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

#include "TrigBjetEtHypoTool.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

/**
 * @class Implements b-jet selection for the new HLT framework
 * @brief 
 **/

class TrigBjetHypoAlgBaseMT : public ::HypoBase {
 public: 

  TrigBjetHypoAlgBaseMT( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TrigBjetHypoAlgBaseMT();

  virtual StatusCode initialize() = 0;
  virtual StatusCode execute( const EventContext& context ) const = 0;

 protected:
  TrigBjetHypoAlgBaseMT();

  StatusCode retrievePreviousDecisionContainer( const EventContext&,
						const TrigCompositeUtils::DecisionContainer*& ) const;

  StatusCode retrieveJetsFromStoreGate( const EventContext&,
					ElementLinkVector< xAOD::JetContainer >&,
					const SG::ReadHandleKey< xAOD::JetContainer >&,
					const TrigCompositeUtils::DecisionContainer* ) const;

  StatusCode retrieveJetsFromEventView( const EventContext&, 
					ElementLinkVector< xAOD::JetContainer >&,
					const SG::ReadHandleKey< xAOD::JetContainer >&,
					const TrigCompositeUtils::DecisionContainer* ) const;
    
}; 

#endif //> !TRIGBJETHYPO_TRIGBJETETHYPOALG_H
