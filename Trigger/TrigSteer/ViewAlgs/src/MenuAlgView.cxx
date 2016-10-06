/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>
#include "CxxUtils/make_unique.h"
#include "L1Decoder/TrigIdentifiers.h"

#include "MenuAlgView.h"

using namespace TrigConf;

namespace AthViews {

MenuAlgView::MenuAlgView(const std::string& name, ISvcLocator* pSvcLocator) 
   :  AthViewAlgorithm(name, pSvcLocator),
//  : AthAlgorithm(name, pSvcLocator), 
    m_inputChainDecisions("MenuInput"), 
    m_outputChainDecisions("MenuOutput"), 
    m_outputChainDecisionsAux("MenuOutputAux."),

    m_inputProxyDecisions("ProxyInput"),    
    m_outputProxyDecisions("ProxyOutput"), 
    m_outputProxyDecisionsAux("ProxyOutputAux.") 
{
  declareProperty("InputChainDecisions", m_inputChainDecisions, "Name of the decisions object of previous stages of the chains");
  declareProperty("OutputChainDecisions", m_outputChainDecisions, "Name of the decisions object which contains decisions for chains");
  declareProperty("OutputChainDecisionsAux", m_outputChainDecisionsAux, "Aux");

  declareProperty("HypoDecisions", m_inputProxyDecisions, "Name of the decisions object which contains decisions of hypo algorithms");
  declareProperty("OutputDecisions", m_outputProxyDecisions, "Name of the decisions object which are to be produced");
  declareProperty("OutputDecisionsAux", m_outputProxyDecisionsAux, "Aux");
  declareProperty("Required", m_requiredConf, "List of required signatures, each specified by: chain_name = hypo_name x multiplicity ... (nonuniform chains not supported yet)" );
}

StatusCode MenuAlgView::initialize() { 
  //decode property
  std::istringstream input;
  // decode format HLT_em25 = L2_em20_calo x 1
  for ( auto requirement : m_requiredConf ) {
    input.clear();
    input.str(requirement);
    std::string chain;
    std::string eqSign;
    std::string hypo;
   std::string xChar;
    size_t multiplicity;
    input >> chain >> eqSign >> hypo >> xChar >> multiplicity;    
    if ( xChar != "x" or eqSign != "=" ) {
      ATH_MSG_WARNING("Ubable to decode " << requirement <<  " it shoudl be of form 'chain = hypo x N'"   );
      return StatusCode::FAILURE;
    }
    m_required.push_back({HLTUtils::string2hash(chain, "chain"), HLTUtils::string2hash(hypo), multiplicity});    
    ATH_MSG_DEBUG( "Will require " << hypo << " x " << multiplicity << " for the chain: " << chain);
  }
  return StatusCode::SUCCESS;
}





StatusCode MenuAlgView::execute() {
  // harvest all the hypo decisions
  if ( not m_inputProxyDecisions.isValid() ) {
    ATH_MSG_ERROR("No Proxy " <<m_inputProxyDecisions.name()<<" from previous hypos");
    return StatusCode::FAILURE;
  }

  std::map<HLTHash, size_t> hypoPassedObjectsMultiplicity;
  {
    for ( const auto roiComposite: *m_inputProxyDecisions.cptr() ) {
      std::vector<TriggerElementID> passedHypoIDs;
      roiComposite->getDetail("Passed", passedHypoIDs);
      for ( auto id: passedHypoIDs ) {
	hypoPassedObjectsMultiplicity[HLTHash(id)] += 1;
      } 
    }
  }


  if ( not m_inputChainDecisions.isValid() ) {
    ATH_MSG_ERROR("No chain decisions object prom previous stage, with name "<< m_inputChainDecisions.name());
    return StatusCode::FAILURE;
  }
  std::set<HLTHash> chainsSurvivingPreviousStage;
  {
    //neeed to filter out chains not requiring this stepAlgo (stored in m_required)!!
    std::vector<int> passedChainIDs;
    for ( const auto chainComposite : *m_inputChainDecisions.cptr() ) {
      int id = 0;
      chainComposite->getDetail("ID", id);
      int decision = 0;
      chainComposite->getDetail("Decision", decision);
      if ( decision != 0 ) {
	chainsSurvivingPreviousStage.insert(HLTHash(id));      
	ATH_MSG_DEBUG("Noticied that the chain " << HLTUtils::hash2string(HLTHash(id), "chain") << " passed at the previous stage");
      } else {
	ATH_MSG_VERBOSE("Noticied that the chain " << HLTUtils::hash2string(HLTHash(id), "chain") << " rejected at the previous stage ");
      }
    }
  }
  

  m_outputChainDecisions = CxxUtils::make_unique< xAOD::TrigCompositeContainer >();
  m_outputChainDecisionsAux = CxxUtils::make_unique< xAOD::TrigCompositeAuxContainer>();  
  m_outputChainDecisions->setStore(m_outputChainDecisionsAux.ptr());

  ATH_MSG_DEBUG("Checking menu, got " << m_required.size() 
		<< " chains to decide on, previous stage passed " << chainsSurvivingPreviousStage.size() 
		<< " chains, hypos passed " << hypoPassedObjectsMultiplicity.size()  );

  std::set<HLTHash> tesOfPassingChains;
  size_t passingChainsCount = 0;

  // loop over the requirements
  for ( const auto req: m_required ) {
    // check if the chain was not already rejected, if so simply ignore it in further consideration
    if ( chainsSurvivingPreviousStage.count(req.chain) != 1  ) {
      ATH_MSG_DEBUG("Chain rejected at the previous stage "<< HLTUtils::hash2string(req.chain, "chain"));
      continue;
    }
    
    // irrespecively of the decision we store the information
    xAOD::TrigComposite * chainDecision  = new xAOD::TrigComposite();
    m_outputChainDecisions->push_back(chainDecision);
    chainDecision->setDetail("ID", ChainID(req.chain));

    const int satisfied = hypoPassedObjectsMultiplicity[req.hypo] >= req.multiplicity ? 1 : 0;     // chain pass condition
    ATH_MSG_DEBUG("Chain ID:" << req.chain << " " << HLTUtils::hash2string(req.chain, "chain") << "  decision " << satisfied);
    chainDecision->setDetail("Decision", satisfied);
    tesOfPassingChains.insert(req.hypo);
    passingChainsCount += satisfied;
  }
  
  // iterate over hypo decisions and create decision object for those which are interested from the menu standpoint

  m_outputProxyDecisions = CxxUtils::make_unique< xAOD::TrigCompositeContainer >();
  m_outputProxyDecisionsAux = CxxUtils::make_unique< xAOD::TrigCompositeAuxContainer>();  
  m_outputProxyDecisions->setStore(m_outputProxyDecisionsAux.ptr());
    
  if ( passingChainsCount > 0) {
    ATH_MSG_DEBUG("Event passed this step");
    size_t nRoI=0;
    for ( const auto roiComposite: *m_inputProxyDecisions.cptr() ) {
      if ( hasTE( roiComposite,  tesOfPassingChains) ) {
	xAOD::TrigComposite * teDecision  = new xAOD::TrigComposite();
	m_outputProxyDecisions->push_back(teDecision);
	teDecision->setObjectLink("seed", ElementLink<xAOD::TrigCompositeContainer>(m_inputProxyDecisions.name(), nRoI) );
      }
      nRoI ++;
    }
  } else {
    ATH_MSG_DEBUG("Event rejected at this step");
  }

   
  return StatusCode::SUCCESS;
}


bool MenuAlgView::hasTE(const xAOD::TrigComposite* c, const std::set<HLTHash>& passing) const {
  std::vector<TriggerElementID> passedHypoIDs;
  c->getDetail("Passed", passedHypoIDs);
  for ( auto id: passedHypoIDs ) {
    if ( passing.count(HLTHash(id)) == 1 )
      return true;
  } 
  return false;
}


}
