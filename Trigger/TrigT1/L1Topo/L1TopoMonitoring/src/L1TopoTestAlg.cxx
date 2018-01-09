/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoTestAlg.h"
#include "L1TopoRDO/L1TopoRDOCollection.h"
#include "L1TopoRDO/Helpers.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1Result/RecRoIBResult.h"
#include "TrigConfInterfaces/IL1TopoConfigSvc.h"
#include "L1TopoConfig/L1TopoMenu.h" 

L1TopoTestAlg::L1TopoTestAlg(const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthAlgorithm(name,pSvcLocator),
  m_l1topoConfigSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name)
{}

StatusCode L1TopoTestAlg::initialize()
{
  ATH_MSG_INFO ("initialize");
  CHECK( m_l1topoConfigSvc.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode L1TopoTestAlg::execute()
{
  ATH_MSG_INFO ("execute");

  // Retrieve and print the L1Topo RDOs from the DAQ RODs
  const DataHandle<L1TopoRDOCollection> rdos = 0;
  StatusCode sc = StatusCode::SUCCESS;
  sc = evtStore()->retrieve(rdos);
  if (sc.isFailure() or 0 == rdos) {
    ATH_MSG_WARNING ( "Could not retrieve L1Topo DAQ RDO collection from StoreGate" );
  }
  else {
    // loop over and print RDOs
    for (const L1TopoRDO* rdo : *rdos){
      ATH_MSG_VERBOSE( *rdo );
      const std::vector<uint32_t> &cDataWords = rdo->getDataWords();
      if ( cDataWords.size() == 0 ) {
	ATH_MSG_WARNING ( "L1TopoRDO DAQ is empty" );
      }
      else{
	//L1Topo::dosomething(*rdo)
      }
    }
  }

  // Retrieve and print the L1Topo RDOs from the ROI RODs
  const ROIB::RoIBResult* roibresult = 0;
  CHECK (evtStore()->retrieve(roibresult) );
  const std::vector< ROIB::L1TopoResult > l1TopoResults = roibresult->l1TopoResult();
  ATH_MSG_INFO( "Number of L1Topo ROI RODs found: " << l1TopoResults.size() );

  // make a temporary L1TopoRDOCollection for the ROI RDOs. 
  // This could also be saved to StoreGate for recording to ESD  for example.
  L1TopoRDOCollection rois;
  for (auto & r : l1TopoResults){
    ATH_MSG_VERBOSE( r.dump() );
    rois.push_back(new L1TopoRDO(r.rdo()));
  }
  // get trigger and overflow bits from all the RDOs 
  // into a pair of 128-bit bitsets, 
  // ordered with the same numbering scheme as the L1Topo configuration uses.
  //auto triggerBits = L1Topo::getDecisionAndOverflowBits(rois); 
  auto triggerBits = L1Topo::getDecisionAndOverflowBits(*rdos); 

  // fake a bit set as no test files have any
  ATH_MSG_WARNING ("Faking trigger bits 87,120,123,124!");
  triggerBits.first.set(87);
  triggerBits.first.set(120);
  triggerBits.first.set(123);
  triggerBits.first.set(124);
  ATH_MSG_WARNING ("Faking overflow bit 87");
  triggerBits.second.set(87);

  ATH_MSG_INFO( "Trigger  bits from L1Topo ROI RDOs 0b" << triggerBits.first );
  ATH_MSG_INFO( "Overflow bits from L1Topo ROI RDOs 0b" << triggerBits.second );


  if (triggerBits.first.all()){
    ATH_MSG_INFO( "L1Topo has ALL trigger bits set" );
  }
  else if (triggerBits.first.any()){
    ATH_MSG_INFO( "L1Topo has " << triggerBits.first.count() << " trigger bits set" );
  }
  else { // triggerBits.first.none()
    ATH_MSG_INFO( "L1Topo has NO trigger bits set" );
  }


  // Get L1Topo menu
  const TXC::L1TopoMenu *topoMenu = m_l1topoConfigSvc->menu();
  if(topoMenu == nullptr) {
    ATH_MSG_FATAL("No L1Topo menu from " << m_l1topoConfigSvc->name());
    return StatusCode::FAILURE;
  }
  const std::vector<TXC::TriggerLine> & topoTriggers = topoMenu->getL1TopoConfigOutputList().getTriggerLines();
  ATH_MSG_DEBUG( "Number of trigger lines configured " << topoTriggers.size() );
  for(const TXC::TriggerLine tl : topoTriggers) {
    ATH_MSG_VERBOSE( tl.name() << " " << tl.counter() );
  }

  for (unsigned int i=0; i<triggerBits.first.size(); ++i){
    if (triggerBits.first.test(i)){
      if (i<topoTriggers.size()){
	ATH_MSG_INFO( "algorithm " << i << " " << topoTriggers.at(i) << " triggered" );
      }
      else {
	ATH_MSG_INFO( "algorithm " << i << " triggered" );
	ATH_MSG_WARNING( "L1TopoConfig has no line defined for " << i );
      }
    }
  }

  return StatusCode::SUCCESS;

}

StatusCode L1TopoTestAlg::finalize()
{
  ATH_MSG_INFO ("finialize");
  return StatusCode::SUCCESS;
}

