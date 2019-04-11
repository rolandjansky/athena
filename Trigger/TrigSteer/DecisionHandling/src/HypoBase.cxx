/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "DecisionHandling/HypoBase.h"
#include "DecisionHandling/HLTIdentifier.h"

HypoBase::HypoBase( const std::string& name, ISvcLocator* pSvcLocator )
  : ::AthReentrantAlgorithm( name, pSvcLocator ) {}

HypoBase::~HypoBase() {}


const SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer>& HypoBase::decisionInput() const{
  return m_input;
}

const SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer>& HypoBase::decisionOutput() const{
  return m_output;
}

StatusCode HypoBase::sysInitialize() {
  CHECK( AthReentrantAlgorithm::sysInitialize() ); // initialise base class
  CHECK( m_input.initialize() );
  // TODO - remove this renounce once https://gitlab.cern.ch/gaudi/Gaudi/merge_requests/863 is in Athena,master
  // m_input should always be valid if hypo runs
  renounce(m_input);
  ATH_MSG_DEBUG("HypoBase::sysInitialize() Will consume decision: " << m_input.key() );
  CHECK( m_output.initialize() );
  ATH_MSG_DEBUG("HypoBase::sysInitialize() And produce decision: " << m_output.key() );
  return StatusCode::SUCCESS;
}

StatusCode HypoBase::printDebugInformation(SG::WriteHandle<TrigCompositeUtils::DecisionContainer>& outputHandle,
                                           MSG::Level lvl) const {
  if (msgLvl(lvl)) {
    msg() << lvl;
    msg() << "Exiting with " << outputHandle->size() <<" Decision objects" << endmsg;
    size_t count = 0;
    for (const TrigCompositeUtils::Decision* d : *outputHandle){
      TrigCompositeUtils::DecisionIDContainer objDecisions;      
      TrigCompositeUtils::decisionIDs( d, objDecisions );
      msg() << "Number of positive decisions for Decision object #" << count++ << ": " << objDecisions.size() << endmsg;
      for (const TrigCompositeUtils::DecisionID id : objDecisions ) {
        msg() << " --- Passes chain: " << HLT::Identifier( id ) << endmsg;
      }  
    }
  }
  return StatusCode::SUCCESS;
}

