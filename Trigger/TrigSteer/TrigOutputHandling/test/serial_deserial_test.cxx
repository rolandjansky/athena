
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include <iostream>

#include "TestTools/expect.h"
#include "TestTools/initGaudi.h"
#include "GaudiKernel/IToolSvc.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"
#include "../src/BareDataBucket.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"

#include "../src/TriggerEDMSerialiserTool.h"
#include "../src/TriggerEDMDeserialiserAlg.h"


void testTrigEMContiner(  const EventContext &ctx );
void testTrigCompositeContiner(  const EventContext &ctx );

void testTrigEMContinerInsert(StoreGateSvc*);
void testTrigCompositeContinerInsert(StoreGateSvc*);

void testTrigEMContinerReadAndCheck(StoreGateSvc*);
void testTrigCompositeContinerReadAndCheck(StoreGateSvc*);


int main() {
  using namespace std;
  ISvcLocator* pSvcLoc;
  if( !Athena_test::initGaudi("test.txt",  pSvcLoc) ) {
    cerr << "ERROR This test can not be run" << endl;
    return -1;
  }
  assert(pSvcLoc);
  MsgStream log(Athena::getMessageSvc(), "serial_deserial_test");
  
  StoreGateSvc* pStore = nullptr;
  VALUE( pSvcLoc->service("StoreGateSvc", pStore, true).isSuccess() ) EXPECTED ( true );

  IToolSvc * toolSvc = nullptr;
  VALUE( pSvcLoc->service("ToolSvc", toolSvc, true) ) EXPECTED ( StatusCode::SUCCESS );
  
  IAlgTool* algTool;
  VALUE( toolSvc->retrieveTool("TriggerEDMSerialiserTool/serialiser", algTool) ) EXPECTED( StatusCode::SUCCESS );
  TriggerEDMSerialiserTool* ser = dynamic_cast< TriggerEDMSerialiserTool*>(algTool);

  


  VALUE( ser == nullptr ) EXPECTED ( false );

  TriggerEDMDeserialiserAlg deser ("deserialiser", pSvcLoc);  deser.addRef();
  deser.sysInitialize();

  TriggerEDMDeserialiserAlg deser2 ("deserialiser2", pSvcLoc);  deser2.addRef();
  deser2.sysInitialize();


  // TDOD simplify :-) ?
  auto runAlg = [&](TriggerEDMDeserialiserAlg& alg) {
    IProxyDict* xdict = &*alg.evtStore();
    xdict = alg.evtStore()->hiveProxyDict();
    EventContext ctx;
    ctx.setExtension( Atlas::ExtendedEventContext(xdict) );
    Gaudi::Hive::setCurrentContext (ctx);  
    return alg.execute( ctx );
  };

  

  for ( int rep = 0; rep < 50 ; ++ rep ) {
    testTrigEMContinerInsert(pStore);
    testTrigCompositeContinerInsert(pStore);

    auto hltres = new HLT::HLTResultMT();
    VALUE( ser->fill( *hltres ) ) EXPECTED ( StatusCode::SUCCESS );  
    
    pStore->clearStore();
    // now objects are only in serialised form in HLTResultMT object
    
    VALUE( pStore->record( hltres, "HLTResultMT" ) ) EXPECTED ( StatusCode::SUCCESS );
    VALUE( runAlg( deser ) ) EXPECTED ( StatusCode::SUCCESS );
    VALUE( runAlg( deser2 ) ) EXPECTED ( StatusCode::SUCCESS );
    
    testTrigEMContinerReadAndCheck(pStore);
    testTrigCompositeContinerReadAndCheck(pStore);
    
    // see if we do nto have owneship issues
    pStore->clearStore();  
  }

  delete ser;
  std::cout <<"ok"<< std::endl;
  return 0;
}


void testTrigEMContinerInsert(StoreGateSvc* pStore) {
  
  // place test data
  auto em = new xAOD::TrigEMClusterContainer();
  auto emAux = new xAOD::TrigEMClusterAuxContainer();
  em->setStore( emAux );
  SG::AuxElement::Accessor< int >   viewIndex( "viewIndex" );
  SG::AuxElement::Accessor< float > testFloat( "testFloat" );
  SG::AuxElement::Accessor< float > testSmallFloat( "testSmallFloat" );

  
  auto fill = [&]( double x) {
      auto cluster = new xAOD::TrigEMCluster();
      em->push_back( cluster );
      cluster->setEnergy(0.0);
      cluster->setEt(0.0);
      cluster->setRawEnergy(0.0);
      cluster->setRawEt(x - 0.2);
      cluster->setE277(0);
      cluster->setEmaxs1(0);
      cluster->setE2tsts1(0);
      cluster->setEhad1(-999);
      cluster->setWeta2(-999);
      cluster->setFracs1(-999);
      cluster->setE233(-999);
      cluster->setE237(-999);
      cluster->setWstot(-999);
      cluster->setEta1(-999);
      cluster->setNCells( x );
      cluster->setRawEta(-999);
      cluster->setRawPhi(-999);
      viewIndex( *cluster ) = x+1; // for sake of change add 1    
      testFloat( *cluster ) = float(x)+0.1; // for sake of change add 0.1          
      testSmallFloat( *cluster ) = float(x);
    };


  VALUE( em->setOption ("testSmallFloat", SG::AuxDataOption ("nbits", 13)) ) NOT_EXPECTED( 0 );
  VALUE( em->setOption ("testSmallFloat", SG::AuxDataOption ("signed", 0))) NOT_EXPECTED( 0 );
  VALUE( em->setOption ("testSmallFloat", SG::AuxDataOption ("nmantissa", 13)) ) NOT_EXPECTED( 0 );


  
  const size_t count = 30;
  for ( size_t i = 0; i < count; i++ ) {
    fill(i);
  }
  
  VALUE( pStore->record( em, "EMClusters" ) ) EXPECTED ( StatusCode::SUCCESS );
  VALUE( pStore->record( emAux, "EMClustersAux." ) ) EXPECTED ( StatusCode::SUCCESS );
}

void testTrigEMContinerReadAndCheck(StoreGateSvc* pStore) {

  const xAOD::TrigEMClusterContainer *emback = nullptr;
  VALUE( pStore->retrieve( emback, "DESERIALISED_EMClusters") ) EXPECTED ( StatusCode::SUCCESS );

  VALUE( emback ) NOT_EXPECTED ( nullptr );
  

  SG::AuxElement::ConstAccessor< int > viewIndexReader( "viewIndex" );  
  SG::AuxElement::ConstAccessor< float > testFloatReader( "testFloat" );  
  SG::AuxElement::ConstAccessor< float > testSmallFloatReader( "testSmallFloat" );  

  VALUE ( emback->size() ) EXPECTED ( 30 ); // as many fills were made

  for ( size_t i = 0;  i < emback->size(); i++ ) {
    const xAOD::TrigEMCluster* cl = emback->at( i );
    VALUE( cl ) NOT_EXPECTED ( nullptr );
    VALUE( cl->nCells() ) EXPECTED( i ); // test regular Aux content
    int vi = viewIndexReader( *cl );
    float fl = testFloatReader( *cl );
    float smallFl = testSmallFloatReader( *cl );
    float rawEt = cl->rawEt();

    std::cout << vi << " " << fl << " " << rawEt << " ";
    VALUE( vi ) EXPECTED ( i + 1 );
    VALUE( fl ) EXPECTED ( 0.1 + i );    
    VALUE( (smallFl - i) < 0.1 ) EXPECTED ( true );  // precission is poor because we decided to reduce space for this variable
    VALUE( rawEt ) EXPECTED ( i - 0.2 );    
  }
  std::cout << std::endl;
}

void testTrigCompositeContinerInsert(StoreGateSvc* pStore) {

  auto m = new xAOD::TrigCompositeContainer();
  auto mAux = new xAOD::TrigCompositeAuxContainer();
  m->setStore( mAux );
  m->push_back( new xAOD::TrigComposite() );
  m->push_back( new xAOD::TrigComposite() );
  {
    m->at(0)->setDetail<float>("detail1", 0.1);
    m->at(0)->setDetail<int>("detail2", 5);
    m->at(1)->setDetail<float>("detail1", 1.7);
    m->at(1)->setDetail<int>("detail2", 7);
  }
  
  VALUE( pStore->record( m, "EMClustersDecisions" ) ) EXPECTED ( StatusCode::SUCCESS );
  VALUE( pStore->record( mAux, "EMClustersDecisionsAux." ) ) EXPECTED ( StatusCode::SUCCESS );
}

void testTrigCompositeContinerReadAndCheck(StoreGateSvc* pStore) {

  const xAOD::TrigCompositeContainer *decisions = nullptr;
  VALUE( pStore->retrieve( decisions, "DESERIALISED_EMClustersDecisions") ) EXPECTED ( StatusCode::SUCCESS );
  VALUE ( decisions ) NOT_EXPECTED ( nullptr );

  
  VALUE( decisions->size() ) EXPECTED( 2 );
  VALUE( decisions->at(0)->getDetail<float>("detail1") ) EXPECTED( 0.1 );
  VALUE( decisions->at(1)->getDetail<float>("detail1") ) EXPECTED( 1.7 );
  VALUE( decisions->at(0)->getDetail<int>("detail2") ) EXPECTED( 5 );
  VALUE( decisions->at(1)->getDetail<int>("detail2") ) EXPECTED( 7 );
}
