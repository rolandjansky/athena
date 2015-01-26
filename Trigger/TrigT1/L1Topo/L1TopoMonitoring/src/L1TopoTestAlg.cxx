/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoTestAlg.h"
#include "L1TopoRDO/L1TopoRDOCollection.h"
#include "L1TopoRDO/Helpers.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1Result/RecRoIBResult.h"

L1TopoTestAlg::L1TopoTestAlg(const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthAlgorithm(name,pSvcLocator),m_blah(0)
{}

StatusCode L1TopoTestAlg::initialize()
{
  ATH_MSG_INFO ("initialize");
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
    for (auto & rdo : *rdos){
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
  L1TopoRDOCollection rois;
  for (auto & r : l1TopoResults){
    ATH_MSG_VERBOSE( r.dump() );
    rois.push_back(new L1TopoRDO(r.rdo()));
  }
  auto triggerBits = L1Topo::getDecisionAndOverflowBits(rois); 
  // not implemented: auto triggerBits = L1Topo::getDecisionAndOverflowBits(l1TopoResults); 
  ATH_MSG_INFO( "Trigger  bits from L1Topo ROI RDOs 0b" << triggerBits.first );
  ATH_MSG_INFO( "Overflow bits from L1Topo ROI RDOs 0b" << triggerBits.first );

  /* 
     const ROIB::RecRoIBResult* recroibresult = 0;
     CHECK (evtStore()->retrieve(recroibresult) );
  */

  return StatusCode::SUCCESS;

}

StatusCode L1TopoTestAlg::finalize()
{
  ATH_MSG_INFO ("finialize");
  return StatusCode::SUCCESS;
}

