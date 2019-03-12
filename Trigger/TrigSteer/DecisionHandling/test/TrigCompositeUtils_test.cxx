/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/WriteHandleKey.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/EventContext.h"
#include "SGTools/TestStore.h"
#include "TestTools/initGaudi.h"
#include "TestTools/expect.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "CxxUtils/checker_macros.h"

int main ATLAS_NOT_THREAD_SAFE () {

  using namespace TrigCompositeUtils;
  xAOD::TrigComposite::s_throwOnCopyError = true;

  errorcheck::ReportMessage::hideFunctionNames (true);

  // initialize Gaudi, SG
  ISvcLocator* pSvcLoc;
  Athena_test::initGaudi(pSvcLoc); 
  StoreGateSvc* pSG(0);
  assert( pSvcLoc->service("StoreGateSvc", pSG, true).isSuccess() );

  // Create a context
  IProxyDict* xdict = &*pSG;
  xdict = pSG->hiveProxyDict();
  EventContext ctx(0,0);
  ctx.setExtension( Atlas::ExtendedEventContext(xdict) );
  Gaudi::Hive::setCurrentContext (ctx);
  std::cout << "Context: " << ctx << std::endl;


  // check current context
  const EventContext& ctx1 = Gaudi::Hive::currentContext();
  std::cout << "Current context: " << ctx1 << std::endl;

  // create handle and record tests

  /* calling createAndStore is equivalent to:
  SG::WriteHandle<DecisionContainer> wh1( whk1 );
  auto data = std::make_unique<DecisionContainer>() ;
  auto aux = std::make_unique<DecisionAuxContainer>() ;
  data->setStore( aux.get() );
  assert( wh1.record( std::move( data ), std::move( aux )  ).isSuccess());
  DecisionContainer* dc =wh1.ptr()
  */

  // Test to cause and error in createAndStore by recording the same key twice
  SG::WriteHandleKey<DecisionContainer> whkT ( "arbitrary" );
  std::cout << "initialize SG::WriteHandleKey<DecisionContainer> whkT" << std::endl;
  assert( whkT.initialize().isSuccess() );
  std::cout << "call createAndStore( whkT ) with default context, twice" << std::endl;
  SG::WriteHandle<DecisionContainer> whT1 = createAndStore( whkT );
  SG::WriteHandle<DecisionContainer> whT2 = createAndStore( whkT );
  // There will be WARNING ERROR and FATAL messages from the above line
  std::cout << "handle name \"" << whT1.name() << "\" isPresent " << whT1.isPresent()<< " isValid " << whT1.isValid() << std::endl;
  std::cout << "handle name \"" << whT2.name() << "\" isPresent " << whT2.isPresent()<< " isValid " << whT2.isValid() << std::endl;
  assert( whT1.isValid() ); // it worked
  assert( ! whT2.isValid() ); // it failed
  std::cout << std::endl;

  SG::WriteHandleKey<DecisionContainer> whk1 ( "myDecisions1" );
  std::cout << "initialize SG::WriteHandleKey<DecisionContainer> whk1" << std::endl;
  assert( whk1.initialize().isSuccess() );
  std::cout << "call createAndStore( whk1 ) with default context" << std::endl;
  SG::WriteHandle<DecisionContainer> wh1 = createAndStore( whk1 );
  std::cout << "handle name " << wh1.name() << " store " << wh1.store() << std::endl;
  VALUE( wh1.store() ) EXPECTED ( "StoreGateSvc_Impl" );
  assert( wh1.isValid() );

  SG::WriteHandleKey<DecisionContainer> whk2 ( "myDecisions2" );
  std::cout << "initialize SG::WriteHandleKey<DecisionContainer> whk2" << std::endl;
  assert( whk2.initialize().isSuccess() );
  std::cout << "call createAndStore( whk2, ctx )" << std::endl;
  SG::WriteHandle<DecisionContainer> wh2 = createAndStore( whk2, ctx );
  DecisionContainer* dc2 = wh2.ptr();  
  std::cout << "handle name " << wh2.name() << " store " << wh2.store() << " container size " << dc2->size() << std::endl;
  VALUE( wh2.name() ) EXPECTED ( "myDecisions2" );
  VALUE( wh2.store() ) EXPECTED ( "StoreGateSvc_Impl" );
  VALUE( dc2->size() ) EXPECTED ( 0 );
  assert( wh2.isValid() );

  DecisionContainer* dc = dc2;

  std::cout << "testing insert decisions" << std::endl;
  // try insertions
  auto d1 = newDecisionIn( dc, "d1" );
  addDecisionID( 1, d1 );
  addDecisionID( 2, d1 );

  auto d2 = newDecisionIn( dc );
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
    bool yn = isAnyIDPassing( d1, cids );
    std::cout <<  " either 3 or 7 contained in d1 " << ( yn ? "YES" : "NO" ) << std::endl;
    VALUE( yn ) EXPECTED ( false );
  }

  {
    bool yn = isAnyIDPassing( d2, cids );
    std::cout <<  " either 3 or 7 contained in d2 " << ( yn ? "YES" : "NO" ) << std::endl;
    VALUE( yn ) EXPECTED ( true );
  }
  
  // double inserts

  auto d3 = newDecisionIn( dc );
  addDecisionID( 95, d3 );
  addDecisionID( 99, d3 );
  addDecisionID( 99, d3 );
  auto storedDecisions  = d3->getDetail<std::vector<DecisionID>>( "decisions" );
  VALUE ( storedDecisions.size() ) EXPECTED ( 2 );
  VALUE ( ( DecisionID )storedDecisions.front() ) EXPECTED ( 95 );
  VALUE ( ( DecisionID )storedDecisions.back() ) EXPECTED ( 99 );

  // Make an EL with an invalid (zero) hash key and try to add it to a Decision
  auto data1 = std::make_unique<DecisionContainer>() ;
  auto aux1 = std::make_unique<DecisionAuxContainer>() ;
  data1->setStore( aux1.get() );
  data1->push_back(d1);
  std::cout << "Call setObjectLink with a bad ElementLink that has no recorded collection. Expect cerr output to follow." << std::endl;
  ElementLink<DecisionContainer> badEL1( *data1, 0 );
  VALUE ( d3->setObjectLink( "bad", badEL1 ) ) EXPECTED ( false );
  // this fails, and cerr output also expected

  // Make an EL that is invalid, e.g. no container, and try to add it to a Decision
  std::cout << "Call setObjectLink with a bad ElementLink that has no corresponding collection. Expect cerr output to follow." << std::endl; 
  ElementLink<DecisionContainer> badEL2( "CollectionKeyThatDoesNotExist", 0 );
  VALUE ( d3->setObjectLink( "bad", badEL2 ) ) EXPECTED ( false );
  // this fails, and cerr output also expected

  // Make, store and retrieve a "seed" link within a collection
  ElementLink<DecisionContainer> el( *dc, 1 );
  assert( d3->addObjectCollectionLink( "seed", el ) );
  ElementLinkVector<DecisionContainer > resElVector = d3->objectCollectionLinks<DecisionContainer>("seed");
  assert( resElVector.size() == 1 );
  ElementLink<DecisionContainer> resEl = resElVector.front();
  VALUE( resEl.index() ) EXPECTED ( el.index() );
  VALUE( resEl.key() ) EXPECTED ( el.key() );

  // Make a new decision, with name and context
  std::cout << std::endl;
  std::cout << "New decision d3b with name & context " << std::endl;
  auto d3b = newDecisionIn( dc, "d3b", ctx );
  // Make, store and retrieve a "testLink" link, with context
  ElementLink<DecisionContainer> el2( *dc, 1, ctx );
  assert ( d3b->setObjectLink( "testlink", el2 ) );
  std::cout << "d3b: " << *d3b << std::endl;
  std::cout << "el2 " << el2.key() << " " << el2.index() << std::endl;
  ElementLink<DecisionContainer > resEl2 = d3b->objectLink<DecisionContainer>("testlink");
  const Decision* res2 = d3b->object<Decision>("testlink");
  VALUE( resEl2.index() ) EXPECTED ( el2.index() );
  VALUE( resEl2.key() ) EXPECTED ( el2.key() );
  VALUE( *resEl2.cptr() ) EXPECTED ( res2 );

  // check copyLinks (copies all)
  auto d4 = newDecisionIn( dc );
  VALUE ( copyLinks(d3, d4) ) EXPECTED ( true );
  std::cout << "d3: " << *d3 << std::endl;
  std::cout << "d4: " << *d4 << std::endl;
  std::cout << "el: key " << el.key() << " index " << el.index() << std::endl;
  ElementLinkVector<DecisionContainer > resElCopiedVector = d4->objectCollectionLinks<DecisionContainer>("seed");
  ElementLink<DecisionContainer> resElCopied = resElCopiedVector.front();
  VALUE ( resElCopied.index() ) EXPECTED ( el.index() );
  VALUE ( resElCopied.key() ) EXPECTED ( el.key() );

  // check new decision with automatically propagated links from previous decision 
  std::cout << "create d5 " << std::endl;
  auto d5 = newDecisionIn ( dc, "d5" );
  std::cout << "set link " << std::endl;
  assert( d5->setObjectLink( "feature", el ) );
  std::cout << "d5: " << *d5 << std::endl;
  std::cout << "create d6 " << std::endl;
  auto d6 = newDecisionIn ( dc, d5, "d6" );
  std::cout << "d6: " << *d6 << std::endl;
  std::cout << "get d5 feature link " << std::endl;
  ElementLink<DecisionContainer > d5feature = d5->objectLink<DecisionContainer>("feature");
  std::cout << "get d6 feature link " << std::endl;
  // We have to get this recursivly
  LinkInfo<DecisionContainer> linkInfo = findLink<DecisionContainer>(d6, "feature");
  VALUE ( linkInfo.isValid() ) EXPECTED ( true );
  ElementLink<DecisionContainer > d6feature = linkInfo.link;
  std::cout << "compare feature links " << std::endl;
  VALUE ( d5feature.index() ) EXPECTED ( d6feature.index() );
  VALUE ( d5feature.key() ) EXPECTED ( d6feature.key() );
  std::cout << "get self and seed links " << std::endl;
  auto d5self = d5->objectLink<DecisionContainer>("self");
  auto d6seedVector = getLinkToPrevious(d6);
  auto d6seed = d6seedVector.front();
  VALUE ( d5self.index() ) EXPECTED ( d6seed.index() );
  VALUE ( d5self.key()   ) EXPECTED ( d6seed.key()   );

  return 0;
  
}
