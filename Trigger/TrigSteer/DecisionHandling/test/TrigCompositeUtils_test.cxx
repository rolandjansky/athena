/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "TestTools/expect.h"
#include "DecisionHandling/TrigCompositeUtils.h"

#include "xAODTrigger/TrigCompositeAuxContainer.h"

int main() {
  using namespace TrigCompositeUtils;

  auto dc = std::make_unique<DecisionContainer>();
  auto decisionAux = std::make_unique<DecisionAuxContainer>();
  dc->setStore( decisionAux.get() );  


  // try insertions
  auto d1 = newDecisionIn( dc.get() );
  addDecisionID( 1, d1 );
  addDecisionID( 2, d1 );

  auto d2 = newDecisionIn( dc.get() );
  addDecisionID( 1, d2 );
  addDecisionID( 3, d2 );

  DecisionIDContainer ids;
  decisionIDs( d1, ids );

  for ( auto id: ids )
    std::cout << id << " from d1\n"; 

  decisionIDs( d2, ids );
  for ( auto id: ids )
    std::cout << id << " from d1 and d2\n"; 

  ids.clear();
  ids.insert( 3 );
  ids.insert( 7 );
  const DecisionIDContainer& cids = ids;
  {
    bool yn = passingIDs( d1, cids );
    std::cout <<  " either 3 or 7 contained in d1 " << ( yn ? "YES" : "NO" ) << std::endl;
    VALUE( yn ) EXPECTED ( false );
  }

  {
    bool yn = passingIDs( d2, cids );
    std::cout <<  " either 3 or 7 contained in d2 " << ( yn ? "YES" : "NO" ) << std::endl;
    VALUE( yn ) EXPECTED ( true );
  }
  
  // double inserts

  auto d3 = newDecisionIn( dc.get() );
  addDecisionID( 95, d3 );
  addDecisionID( 99, d3 );
  addDecisionID( 99, d3 );
  auto storedDecisions  = d3->getDetail<std::vector<int>>( "decisions" );
  VALUE( storedDecisions.size() ) EXPECTED ( 2 );
  VALUE( ( int )storedDecisions.front() ) EXPECTED ( 95 );
  VALUE( ( int )storedDecisions.back() ) EXPECTED ( 99 );



  return 0;
  
}
