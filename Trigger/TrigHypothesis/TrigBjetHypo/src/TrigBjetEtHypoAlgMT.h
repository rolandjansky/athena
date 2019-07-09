/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGBJETHYPO_TRIGBJETETHYPOALGMT_H
#define TRIGBJETHYPO_TRIGBJETETHYPOALGMT_H 1

#include "TrigBjetHypoAlgBaseMT.h"

/**
 * @class Implements b-jet selection for the new HLT framework
 * @brief 
 **/

class TrigBjetEtHypoAlgMT : public TrigBjetHypoAlgBaseMT {
 public: 

  TrigBjetEtHypoAlgMT( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TrigBjetEtHypoAlgMT();

  virtual StatusCode  initialize();
  virtual StatusCode  execute( const EventContext& context ) const;

 protected:
  TrigBjetEtHypoAlgMT();

  virtual StatusCode retrieveJets( const EventContext&,
				   ElementLinkVector< xAOD::JetContainer >&,
				   const SG::ReadHandleKey< xAOD::JetContainer >&,
				   const TrigCompositeUtils::DecisionContainer* ) const;
  
  virtual StatusCode retrieveRoIs( const EventContext&,
                                   const TrigRoiDescriptorCollection*&,
                                   const SG::ReadHandleKey< TrigRoiDescriptorCollection >& ) const;
  
  virtual StatusCode setJetLink( const EventContext&,
				 const SG::ReadHandleKey< xAOD::JetContainer >&,
				 const unsigned int,
				 const TrigCompositeUtils::DecisionContainer*&,
				 std::vector< TrigCompositeUtils::Decision* >& ) const;
  
  virtual const TrigCompositeUtils::Decision* getPreviousDecision( const TrigCompositeUtils::DecisionContainer*,unsigned int ) const;
  
 protected:
  ToolHandleArray< TrigBjetEtHypoTool > m_hypoTools {this,"HypoTools",{},"Hypo Tools"};
  
  Gaudi::Property< std::string > m_roiLink {this,"RoILink","roi","RoI link to attach to the output decision"};
  
  SG::ReadHandleKey< xAOD::JetContainer > m_inputJetsKey {this,"Jets","Undefined","Input Jet Container Key"};
  SG::ReadHandleKey< TrigRoiDescriptorCollection > m_inputRoIKey {this,"RoIs","Undefined","Input RoIs that will be linked to the output decision"};
}; 

#endif 
