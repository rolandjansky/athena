/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//#include <iostream> // removeme
#include <algorithm>
#include <functional>
//#include "boost/mem_fn.hpp"
//#include "boost/bind.hpp"
#include "boost/foreach.hpp"
#include "boost/regex.hpp"

#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "TrigConfInterfaces/ITrigConfigSvc.h"
#include "TrigConfHLTData/HLTUtils.h"

#include "OptimalExecutionOrderStrategy.h"

using namespace HLT;

// various utils
std::set<std::string> getL1ThresholdsOfChain(const HLT::SteeringChain* chain, TrigConf::ITrigConfigSvc* config) {
  // find the configuration of item seeding it
  std::vector<std::string> tesout = TrigConf::HLTUtils::implicitChainTEs(*(chain->getConfigChain()), *(config->sequenceList()) );
  std::set<std::string> tesoutset(tesout.begin(), tesout.end());
  std::set<std::string> tes = TrigConf::HLTUtils::inputTEs(tesoutset, *(config->sequenceList()) );
  return tes;
}


bool Bag::insert(const HLT::SteeringChain* chain) {
  if ( matching.find(chain) != matching.end() ) {
    content.push_back(chain);
    return true;
  }
  return false;
}

void Bag::flush(std::vector<HLT::SteeringChain*>& chains) {
  BOOST_FOREACH(const HLT::SteeringChain* ch, content) {
    chains.push_back(const_cast<HLT::SteeringChain*>(ch));
  }
  content.clear();
}




class BagForAll : public Bag {
public:
  BagForAll() {}
  virtual bool prepare(const HLT::SteeringChain* ) { return true; }
  virtual bool insert(const HLT::SteeringChain* chain) { 
    content.push_back(chain);
    return true;
  }

};


class BagForL1Threshold : public Bag {
public:
  BagForL1Threshold(const std::string& thname, TrigConf::ITrigConfigSvc* config) : m_th(thname), m_config(config) {}
  bool prepare(const HLT::SteeringChain* chain) {      
    std::set<std::string> thresholds = getL1ThresholdsOfChain(chain, m_config);
    if ( thresholds.find(m_th) != thresholds.end() ) {
      matching.insert(chain);
      //std::cout << "in this bag we like: " << chain->getChainName() << std::endl;
      return true;
    }
    return false;
  }
private:
  std::string m_th;
  TrigConf::ITrigConfigSvc* m_config;
};

class BagForChain : public Bag {
public:
  BagForChain(const std::string& name) : m_chname(name){}
  bool prepare(const HLT::SteeringChain* chain) {  
    //if (chain->getChainName() == m_chname ) {
    boost::cmatch what;
    if ( boost::regex_match(chain->getChainName().c_str(), what, m_chname) ) {
      matching.insert(chain);
      return true;
    }
    return false;
  }  
private:
  boost::regex m_chname;
};



/////////////////////////////////////////////////////////////////////////////
// strategy tool implementation

OptimalExecutionOrderStrategy::OptimalExecutionOrderStrategy(const std::string& name, const std::string& type,
						     const IInterface* parent)
  : AthAlgTool(name, type, parent),
    m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name)
{
  declareProperty( "order", m_orderSpecification, "Order by which chains should be sorted. th:J5 means L1 threshold of J5." );
  declareInterface<HLT::IExecutionOrderStrategy>( this ); 
}


StatusCode OptimalExecutionOrderStrategy::initialize() {
  m_orderingBags.reserve(m_orderSpecification.size());
  BOOST_FOREACH(const std::string& selector, m_orderSpecification) {
    std::string type  = selector.substr(0, selector.find(':'));
    std::string value = selector.substr(selector.find(':')+1);
    if ( type == "th" ) { // starts from the th:
      m_orderingBags.push_back(new BagForL1Threshold(value, &*m_configSvc));
    } else if ( type == "name" ) {
      m_orderingBags.push_back(new BagForChain(value));
    } 
  }

  // very important at the end there needs to be bag which matches everything, the default one is like this
  m_orderingBags.push_back(new BagForAll);
  ATH_MSG_DEBUG("Configured " << m_orderingBags.size() << " ordering categories");

  if (m_configSvc.retrieve().isFailure()) {
    ATH_MSG_FATAL(  "failed to retrieve config service: " << m_configSvc );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;  
}

StatusCode OptimalExecutionOrderStrategy::finalize() {
  BOOST_FOREACH(Bag* bag, m_orderingBags) {
    delete bag;
  }
  m_orderingBags.clear();
  return StatusCode::SUCCESS;
}


StatusCode OptimalExecutionOrderStrategy::prepare(std::vector<HLT::SteeringChain*>& configured_chains) {
  BOOST_FOREACH(HLT::SteeringChain* chain, configured_chains) { 
    bool inBag=false;    
    unsigned int bagcnt = 0;
    BOOST_FOREACH(Bag* bag, m_orderingBags) {
      
      if ( bag->prepare(chain) ) {
	ATH_MSG_DEBUG("Chain "<< chain->getChainName() << " fits priority bag " << bagcnt );
	inBag=true;
	break;
      }      
      bagcnt++;
    }
    if ( inBag == false ) {
      ATH_MSG_ERROR("While ordering chains we have lost: "  << chain->getChainName() << " ! This is fatally bad! Switch to DefaultExecutionOrderStrategy asap."   );            
      return StatusCode::FAILURE;
    }    
  }
  

  return StatusCode::SUCCESS;
}


void OptimalExecutionOrderStrategy::apply(std::vector<HLT::SteeringChain*>& active_chains) const {


  BOOST_FOREACH(HLT::SteeringChain* chain, active_chains) {
    ATH_MSG_VERBOSE("order before: " << chain->getChainName());
  }


  // loop over the chain and drop it in the first Bag it fits into
  // when this is succefull the loop can be stopped
  BOOST_FOREACH(HLT::SteeringChain* chain, active_chains) { 
    BOOST_FOREACH(Bag* bag, m_orderingBags) {
      if ( bag->insert(chain) ) {
	break;
      }      
    }
  }

  // security very cheap checks so we do them each time
  size_t before = active_chains.size();
  active_chains.clear();
  active_chains.reserve(before);

  BOOST_FOREACH(Bag* bag, m_orderingBags) {
    bag->flush(active_chains);
  }


  BOOST_FOREACH(HLT::SteeringChain* chain, active_chains) {
    ATH_MSG_VERBOSE("order after: " << chain->getChainName());
  }

}



