/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>
#include "CxxUtils/make_unique.h"
#include "L1Decoder/TrigIdentifiers.h"
#include "ViewAlgs/HypoDecision.h"
#include "./DecisionAlg.h"

using namespace TrigConf;


DecisionAlg::DecisionAlg(const std::string& name, ISvcLocator* pSvcLocator) 
//  : AthAlgorithm(name, pSvcLocator), 
  :AthFilterAlgorithm(name, pSvcLocator), 
    //    m_inputChainDecisions({"MenuInput"}), 
    m_outputChainDecisions("MenuOutput"), 
    m_outputChainDecisionsAux("MenuOutputAux."),

    //    m_inputPartialDecisions({"ProxyInput"}),    
    m_outputHypoDecisions("HypoOutput"), 
    m_outputHypoDecisionsAux("HypoOutputAux.") 
{
  declareProperty("InputChainDecisions", m_inputChainDecisions, "Name of the decisions object of previous stages of the chains");
  declareProperty("InputHypoDecisions", m_inputHypoDecisions, "Name of the decisions object which contains decisions of hypo algorithms");

  declareProperty("OutputChainDecisions", m_outputChainDecisions, "Name of the decisions object which contains decisions for chains");
  declareProperty("OutputChainDecisionsAux", m_outputChainDecisionsAux, "Aux");
  declareProperty("OutputHypoDecisions", m_outputHypoDecisions, "Name of the decisions object which are to be produced");
  declareProperty("OutputHypoDecisionsAux", m_outputHypoDecisionsAux, "Aux");
  declareProperty("Required", m_requiredConf, "List of required signatures, each specified by: chain_name = hypo_name x multiplicity, hypo_name x multiplicity" );
}

StatusCode DecisionAlg::initialize() { 
  //decode property
  std::istringstream input;
  // decode format HLT_em25 = L2_em20_calo x 1, L2_em15_calo x 1
  for ( auto requirement : m_requiredConf ) {
    input.clear();
    input.str(requirement);
    std::string chain;
    std::string eqSign;
    input >> chain >> eqSign;
    ChainID chID = HLTUtils::string2hash(chain);
    if ( eqSign != "=" ) {
      ATH_MSG_ERROR("Unable to decode " << requirement <<  " expected \"=\" after chain name"   );
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Decoded chain" << chain);
    
    while (true) {
      std::string hypo;
      std::string xChar;
      size_t multiplicity;
      input >> hypo >> xChar >> multiplicity;    
      if ( xChar != "x"  ) {
	ATH_MSG_ERROR("Unable to decode " << requirement <<  " expected \"x\" after hypo name, decoded hypo " << hypo << " mult " << multiplicity   );
	return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG( "Will require " << hypo << " x " << multiplicity << " for the chain: " << chain);
      m_required[chID].push_back({HLTUtils::string2hash(hypo), multiplicity});    
      if ( input.peek() == ',' ) {
	ATH_MSG_DEBUG("Extracing more requirements");
	input.get();
      } else {
	ATH_MSG_DEBUG("No more requirements");
	break;
      }

    }
  }

  ATH_CHECK ( m_inputHypoDecisions.initialize() );
  ATH_CHECK ( m_inputChainDecisions.initialize() );

  return StatusCode::SUCCESS;
}





StatusCode DecisionAlg::execute() {

  this->setFilterPassed(false);
  // harvest all the hypo decisions

  auto inputHypoDecisions = m_inputHypoDecisions.makeHandles();
  std::map<HLTHash, size_t> hypoPassedObjectsMultiplicity;
  {
    // there is a three level nesting here,
    // several input collections from several hypos
    // in each hypo collection there is a decision per RoI
    // in each RoI there is a decision ffrom several Hypo tools

    for ( auto fromOneHypoAlg:  inputHypoDecisions) {
      if ( not fromOneHypoAlg.isValid()  ) 
	return StatusCode::FAILURE;
      ATH_MSG_VERBOSE("Hypo decisions collection available");
      for ( auto forOneRoI:  *fromOneHypoAlg ) {
	ATH_MSG_VERBOSE("Hypo decisions collection available for RoI");

	if ( not HypoDecision::containsDecision(*forOneRoI) ) {
	  ATH_MSG_WARNING("Hypo information not delivered");
	  continue;
	}
	HypoDecision decisions(*forOneRoI);

	for ( auto id: decisions.passed() ) {
	  hypoPassedObjectsMultiplicity[HLTHash(id)] += 1;
	  ATH_MSG_DEBUG("Hypo " << HLTUtils::hash2string(id) << " passed in " << hypoPassedObjectsMultiplicity[HLTHash(id)] << " RoIs in this event ");
	} 
      }
    }
  }


  std::set<HLTHash> chainsSurvivingPreviousStage;
  {

    // for ( const auto chain: *m_inputChainDecisions.cptr() ) {
    //   int id = 0;
    //   int decision = 0;
    //   chain->getDetail("ID", id);
    //   chain->getDetail("Decision", decision);
    //   ATH_MSG_VERBOSE("Input chain " << HLTUtils::hash2string(HLTHash(id), "chain") << " decision ="<<decision);
    // }

    //neeed to filter out chains not requiring this decisionAlgo (stored in m_required)!!
    std::vector<int> passedChainIDs;
    
    auto inputChainDecisions = m_inputChainDecisions.makeHandles();
    for ( auto fromOnePredecesor: inputChainDecisions ) {

      for ( const auto chainComposite : *fromOnePredecesor ) {
	int id = 0;
	chainComposite->getDetail("ID", id);
	if ( m_required.find(HLTHash(id)) == m_required.end() ){
	  ATH_MSG_VERBOSE("Noticied that the chain " << HLTUtils::hash2string(HLTHash(id), "chain") << " is not required in this decision");
	  continue; // we are not deciding on this chain
	}
	int decision = 0;
	chainComposite->getDetail("Decision", decision);
	if ( decision != 0 ) {
	  chainsSurvivingPreviousStage.insert(HLTHash(id));      
	  ATH_MSG_VERBOSE("Noticied that the chain " << HLTUtils::hash2string(HLTHash(id), "chain") << " passed at the previous stage");
	} else {
	  ATH_MSG_VERBOSE("Noticied that the chain " << HLTUtils::hash2string(HLTHash(id), "chain") << " rejected at the previous stage ");
	}
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
    if ( chainsSurvivingPreviousStage.count(req.first) != 1  ) {
      ATH_MSG_DEBUG("Chain rejected at the previous stage "<< HLTUtils::hash2string(req.first, "chain"));
      continue;
    }
    
    // irrespecively of the decision we store the information for chains 
    // which riched taht step
    xAOD::TrigComposite * chainDecision  = new xAOD::TrigComposite();
    m_outputChainDecisions->push_back(chainDecision);
    chainDecision->setDetail("ID", ChainID(req.first));

    bool satisfied = true;
    for ( auto partialReq : req.second ) {
      if (  hypoPassedObjectsMultiplicity[partialReq.hypo] < partialReq.multiplicity ) 
	satisfied = false;
    }
    ATH_MSG_DEBUG("Chain ID:" << req.first << " " << HLTUtils::hash2string(req.first, "chain") << "  decision " << satisfied);
    chainDecision->setDetail("Decision", satisfied);
    if ( satisfied ) {
      for ( auto partialReq : req.second ) {
	tesOfPassingChains.insert(partialReq.hypo);
      }
      passingChainsCount += satisfied;
    }
  }
  
  // iterate over hypo decisions and create decision object for those which are interested from the menu standpoint

  m_outputHypoDecisions = CxxUtils::make_unique< xAOD::TrigCompositeContainer >();
  m_outputHypoDecisionsAux = CxxUtils::make_unique< xAOD::TrigCompositeAuxContainer>();  
  m_outputHypoDecisions->setStore(m_outputHypoDecisionsAux.ptr());
    
  if ( passingChainsCount <= 0) {
    ATH_MSG_DEBUG("DecisionAlgEvent rejected in this stage");
    return StatusCode::SUCCESS;
  }


  for ( auto fromOneHypoAlg:  inputHypoDecisions) {
    size_t nRoI=0;
    for ( const auto roi: *fromOneHypoAlg ) {
      if ( hasTE( roi,  tesOfPassingChains) ) {
	xAOD::TrigComposite * teDecision  = new xAOD::TrigComposite();
	m_outputHypoDecisions->push_back(teDecision);
	teDecision->setObjectLink("seed", ElementLink<xAOD::TrigCompositeContainer>(fromOneHypoAlg.name(), nRoI) );
      }
      nRoI ++;
    }
  }
  ATH_MSG_DEBUG("DecisionAlgEvent accepted a this stage");
  setFilterPassed(true);
  
  return StatusCode::SUCCESS;
}


bool DecisionAlg::hasTE(const xAOD::TrigComposite* c, const std::set<HLTHash>& passing) const {
  std::vector<TriggerElementID> passedHypoIDs;
  c->getDetail("Passed", passedHypoIDs);
  for ( auto id: passedHypoIDs ) {
    if ( passing.count(HLTHash(id)) == 1 )
      return true;
  } 
  return false;
}
