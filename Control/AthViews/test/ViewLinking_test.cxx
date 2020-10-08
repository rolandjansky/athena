/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/getMessageSvc.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include "TestTools/initGaudi.h"
#include "TestTools/expect.h"
#include "TestTools/expect_exception.h"
#include "AthViews/View.h"

struct TestClass {
  int value = 0;
};

CLASS_DEF( TestClass , 16530831 , 1 )
using namespace SG;
void testDataInView( StoreGateSvc* /*sg*/ , MsgStream& log ) {
  auto parentView = new View( "ParentView", -1 );

  auto t1 = std::make_unique<TestClass>();
  t1->value = 1;
  //  auto status1 = parentView->recordObject( t1, "test1" );
    
  {
    SG::WriteHandle<TestClass> wh( "test1" );
    wh.setProxyDict( parentView ).ignore();
    auto status = wh.record( std::move( t1 ) );
    VALUE( status.isSuccess() ) EXPECTED( true );
  }

  auto childView = new View( "ChildView", -1 );

  auto t2 = std::make_unique<TestClass>();
  t2->value = 2;
  {
    SG::WriteHandle<TestClass> wh( "test2" );
    wh.setProxyDict( childView ).ignore();
    auto status = wh.record( std::move( t2 ) );
    VALUE( status.isSuccess() ) EXPECTED( true ); 
  }
  //  auto status2 = childView->recordObject( t2, "test1" );
  // all prepared, will start testing if querries respond correctly
  {
    // rtivial test, we ask for a wrong object
    SG::ReadHandle<TestClass> rh( "test" );
    rh.setProxyDict( childView ).ignore();
    VALUE( rh.isValid() ) EXPECTED( false );
  }



  {
    SG::ReadHandle<TestClass> rh( "test2" );
    rh.setProxyDict( childView ).ignore();
    VALUE( rh.isValid() ) EXPECTED( true );
    VALUE( rh->value ) EXPECTED( 2 );
  }

  {
    SG::ReadHandle<TestClass> rh( "test1" );
    rh.setProxyDict( childView ).ignore();
    VALUE( rh.isValid() ) EXPECTED( false );
  }
  log << MSG::INFO << "Views that are not linked behave correctly " << endmsg;
  // link them and see if data object is accessible
  childView->linkParent( parentView );

  {
    SG::ReadHandle<TestClass> rh( "test2" );
    rh.setProxyDict( childView ).ignore();
    VALUE( rh.isValid() ) EXPECTED( true );
    VALUE( rh->value ) EXPECTED( 2 );
  }
  {
    SG::ReadHandle<TestClass> rh( "test1" );
    rh.setProxyDict( childView ).ignore();
    VALUE( rh.isValid() ) EXPECTED( true );
    VALUE( rh->value ) EXPECTED( 1 );
  }
  log << MSG::INFO << "Views that are linked behave correctly " << endmsg;

  // hide object from parent by adding one in the Child
  auto t3 = std::make_unique<TestClass>();
  t3->value = 3;

  {
    SG::WriteHandle<TestClass> wh( "test1" );
    wh.setProxyDict( childView ).ignore();
    auto status = wh.record( std::move( t3 ) );
    VALUE( status.isSuccess() ) EXPECTED( true );
  }

  {
    SG::ReadHandle<TestClass> rh( "test1" );
    rh.setProxyDict( childView ).ignore();
    VALUE( rh.isValid() ) EXPECTED( true );
    VALUE( rh->value ) EXPECTED ( 3 );
  }

  log << MSG::INFO << "Hiding works as expected " << endmsg;


}

void testFallThrough( StoreGateSvc* sg , MsgStream& log) {
  auto t = std::make_unique<TestClass>();
  SG::WriteHandle<TestClass> wh( "inStore" );
  wh.setProxyDict( sg ).ignore();
  auto status = wh.record( std::move( t ) );
  VALUE( status.isSuccess() ) EXPECTED( true ); 
    
  // the whole trick is that the read handle is pointed to the view,
  // but should read from the main store if the fall though
  // is enabled
  {
    auto opaqueView = new View( "OpaqueView", -1, false );
    SG::ReadHandle<TestClass> rh( "inStore" );
    rh.setProxyDict( opaqueView ).ignore();
    VALUE( rh.isValid() ) EXPECTED( false );
  }
  {
    auto transparentView = new View( "TransparentView", -1 );
    SG::ReadHandle<TestClass> rh( "inStore" );
    rh.setProxyDict( transparentView ).ignore();
    VALUE( rh.isValid() ) EXPECTED( true );
  }
  log << MSG::INFO << "Fall through works as expected " << endmsg;
}



int main() {
  using namespace std;

  MsgStream log(Athena::getMessageSvc(), "ViewLinking_test");

  //  MsgStream* msglog = &log;


  ISvcLocator* pSvcLoc;
  //if (!Athena_test::initGaudi("test.txt",  pSvcLoc)) {
  if (!Athena_test::initGaudi("",  pSvcLoc)) {
    log << MSG::ERROR << "Can not intit Gaudi" << endmsg;
    return -1;
  }
  assert(pSvcLoc);

  StoreGateSvc* pStore(0);

  if( pSvcLoc->service("StoreGateSvc", pStore, true).isSuccess() ) {
    log << MSG::INFO << "SG pointer: " << pStore << endmsg;
  } else {
    log << MSG::ERROR << "SG not available" << endmsg;
    return -1;
  }

  testDataInView( pStore, log );
  testFallThrough( pStore, log );
  
  return 0;
}
