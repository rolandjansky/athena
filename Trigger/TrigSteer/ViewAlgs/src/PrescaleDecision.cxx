/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cstdlib>
#include "CxxUtils/make_unique.h"
#include "L1Decoder/TrigIdentifiers.h"
#include "./PrescaleDecision.h"

using namespace TrigConf;

PrescaleDecision::PrescaleDecision(const std::string& name, ISvcLocator* pSvcLocator) 
 : AthAlgorithm(name, pSvcLocator), 
   m_inputChainDecisions("MenuInput"), 
   m_outputChainDecisions("PrescaledChains"), 
   m_outputChainDecisionsAux("PrescaledChainsAux.")
{
  declareProperty("InputChainDecisions", m_inputChainDecisions, "Name of the decisions object which contains decisions of previous stages of the chains");
  declareProperty("OutputChainDecisions", m_outputChainDecisions, "Name of the decisions object which contains decisions for chains");
  declareProperty("OutputChainDecisionsAux", m_outputChainDecisionsAux, "Aux");
  declareProperty("Prescales", m_psValuesConf, "Prescales vector in the form \"HLT_XYZ 6\", \"HLT_ABC 5\"");
}

StatusCode PrescaleDecision::initialize() {  
  std::istringstream input;
  // decode format HLT_em25 = L2_em20_calo x 1
  for ( auto requirement : m_psValuesConf ) {
    input.clear();
    input.str(requirement);
    std::string chain;
    double psVal;
    input >> chain >> psVal;
    TrigConf::HLTHash chainHash = HLTUtils::string2hash(chain, "chain");
    if ( m_prescales.find(chainHash) != m_prescales.end()) {
      ATH_MSG_ERROR( "Configuring twice the PS for the chain: " << chain);
      return StatusCode::FAILURE;
    } 
    m_prescales[chainHash] = psVal;
    ATH_MSG_DEBUG( "Prescale " << psVal  << " configured for chain " <<  chain);
  }
  return StatusCode::SUCCESS;
}

StatusCode PrescaleDecision::execute() {

  if ( not m_inputChainDecisions.isValid() ) {
    ATH_MSG_ERROR("No decisions object prom previous stage");
    return StatusCode::FAILURE;
  }

  m_outputChainDecisions = CxxUtils::make_unique< xAOD::TrigCompositeContainer >();
  m_outputChainDecisionsAux = CxxUtils::make_unique< xAOD::TrigCompositeAuxContainer>();  
  m_outputChainDecisions->setStore(m_outputChainDecisionsAux.ptr());
  
  for ( const auto chain: *m_inputChainDecisions.cptr() ) {

    int id = 0;
    chain->getDetail("ID", id);
    auto toBePrescaled = m_prescales.find(HLTHash(id));

    xAOD::TrigComposite * chainDecision  = new xAOD::TrigComposite();
    m_outputChainDecisions->push_back(chainDecision);
    chainDecision->setDetail("ID", id);
    
    if ( toBePrescaled == m_prescales.end() ) {// no prescaling directive 
      chainDecision->setDetail("Decision", 1);
      ATH_MSG_DEBUG("Just passing chain " << HLTUtils::hash2string(HLTHash(id), "chain") << " no prescaling configured for it ");
    } else {
      // here tricky RNG has to be placed to assure randomness and reproducibility
      // and be reentrant
      // will come
      std::srand(id); // + event into time
      //      const int d = double(std::rand())/RAND_MAX < 1./toBePrescaled->second ? 1 : 0;      
      const int d = 1;
      chainDecision->setDetail("Decision", d);
      ATH_MSG_DEBUG("Chain " << HLTUtils::hash2string(HLTHash(id), "chain") << " PS decision " << d);
    }

  }
  return StatusCode::SUCCESS;
}

