
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"

#include "xAODBTagging/BTaggingContainer.h"

#include "../src/TriggerEDMSerialiserTool.h"
#include "../src/TriggerEDMDeserialiserAlg.h"


void testTrigEMContainer(  const EventContext &ctx );
void testTrigCompositeContainer(  const EventContext &ctx );

void testTrigEMContainerInsert(StoreGateSvc*);
void testTrigCompositeContainerInsert(StoreGateSvc*);

void testTrigEMContainerReadAndCheck(StoreGateSvc*);
void testTrigCompositeContainerReadAndCheck(StoreGateSvc*);

void testRoIDescriptorInsert(StoreGateSvc*);
void testRoIDescriptorReadAndCheck(StoreGateSvc*);


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
  VALUE( deser.sysInitialize() ) EXPECTED ( StatusCode::SUCCESS );

  // TODO simplify :-) ?
  auto runAlg = [&](TriggerEDMDeserialiserAlg& alg, const EventContext& ctx) {
    return alg.execute( ctx );
  };

  IProxyDict* xdict = &*deser.evtStore();
  xdict = deser.evtStore()->hiveProxyDict();
  EventContext ctx;
  ctx.setExtension( Atlas::ExtendedEventContext(xdict) );

  for ( int rep = 0; rep < 50 ; ++ rep ) {
    testTrigEMContainerInsert(pStore);
    testTrigCompositeContainerInsert(pStore);
    testRoIDescriptorInsert(pStore);

    auto hltres = new HLT::HLTResultMT();
    VALUE( ser->fill( *hltres, ctx ) ) EXPECTED ( StatusCode::SUCCESS );

    pStore->clearStore().ignore();
    // now objects are only in serialised form in HLTResultMT object

    VALUE( pStore->record( hltres, "HLTResultMT" ) ) EXPECTED ( StatusCode::SUCCESS );
    VALUE( runAlg( deser, ctx ) ) EXPECTED ( StatusCode::SUCCESS );

    testTrigEMContainerReadAndCheck(pStore);
    testTrigCompositeContainerReadAndCheck(pStore);
    testRoIDescriptorReadAndCheck(pStore);
    // see if we have ownership issues
    pStore->clearStore().ignore();
  }

  delete ser;
  std::cout <<"ok"<< std::endl;
  return 0;
}


void testTrigEMContainerInsert(StoreGateSvc* pStore) {

  // place test data
  auto em = new xAOD::TrigEMClusterContainer();
  auto emAux = new xAOD::TrigEMClusterAuxContainer();
  em->setStore( emAux );
  SG::AuxElement::Accessor< int >   viewIndex( "viewIndex" );
  SG::AuxElement::Accessor< float > testFloat( "testFloat" );
  SG::AuxElement::Accessor< float > testSmallFloat( "testSmallFloat" );
  SG::AuxElement::Accessor< ElementLink<xAOD::BTaggingContainer> > testTypedElementLink( "testTypedElementLink" );


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
      testTypedElementLink( *cluster ) = ElementLink<xAOD::BTaggingContainer>(123, 456);
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

void testTrigEMContainerReadAndCheck(StoreGateSvc* pStore) {

  const xAOD::TrigEMClusterContainer *emback = nullptr;
  VALUE( pStore->retrieve( emback, "DESERIALISED_EMClusters") ) EXPECTED ( StatusCode::SUCCESS );

  VALUE( emback ) NOT_EXPECTED ( nullptr );


  SG::AuxElement::ConstAccessor< int > viewIndexReader( "viewIndex" );
  SG::AuxElement::ConstAccessor< float > testFloatReader( "testFloat" );
  SG::AuxElement::ConstAccessor< float > testSmallFloatReader( "testSmallFloat" );
  SG::AuxElement::ConstAccessor< ElementLink<xAOD::BTaggingContainer> > testTypedElementLinkReader( "testTypedElementLink" );

  VALUE ( emback->size() ) EXPECTED ( 30 ); // as many fills were made

  for ( size_t i = 0;  i < emback->size(); i++ ) {
    const xAOD::TrigEMCluster* cl = emback->at( i );
    VALUE( cl ) NOT_EXPECTED ( nullptr );
    VALUE( cl->nCells() ) EXPECTED( i ); // test regular Aux content
    int vi = viewIndexReader( *cl );
    float fl = testFloatReader( *cl );
    float smallFl = testSmallFloatReader( *cl );
    float rawEt = cl->rawEt();
    ElementLink<xAOD::BTaggingContainer> el = testTypedElementLinkReader( *cl );

    std::cout << vi << " " << fl << " " << rawEt << " ";
    VALUE( vi ) EXPECTED ( i + 1 );
    VALUE( fl ) EXPECTED ( 0.1 + i );
    VALUE( (smallFl - i) < 0.1 ) EXPECTED ( true );  // precision is poor because we decided to reduce space for this variable
    VALUE( rawEt ) EXPECTED ( i - 0.2 );
    VALUE( el ) EXPECTED ( ElementLink<xAOD::BTaggingContainer>(123, 456) );
  }
  std::cout << std::endl;
}

void testTrigCompositeContainerInsert(StoreGateSvc* pStore) {

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

void testTrigCompositeContainerReadAndCheck(StoreGateSvc* pStore) {

  const xAOD::TrigCompositeContainer *decisions = nullptr;
  VALUE( pStore->retrieve( decisions, "DESERIALISED_EMClustersDecisions") ) EXPECTED ( StatusCode::SUCCESS );
  VALUE ( decisions ) NOT_EXPECTED ( nullptr );


  VALUE( decisions->size() ) EXPECTED( 2 );
  VALUE( decisions->at(0)->getDetail<float>("detail1") ) EXPECTED( 0.1 );
  VALUE( decisions->at(1)->getDetail<float>("detail1") ) EXPECTED( 1.7 );
  VALUE( decisions->at(0)->getDetail<int>("detail2") ) EXPECTED( 5 );
  VALUE( decisions->at(1)->getDetail<int>("detail2") ) EXPECTED( 7 );
}


void testRoIDescriptorInsert(StoreGateSvc* pStore) {

  TrigRoiDescriptorCollection *rc = new TrigRoiDescriptorCollection();
  rc->push_back(new TrigRoiDescriptor( 3.5, -0.1, 0.2, // eta,
				       0.1, -0.1, 0.1));

  rc->push_back(new TrigRoiDescriptor( -1.5, -0.3, 0.3, // eta,
				       -0.4, -0.23, 0.197));


  VALUE( pStore->record( rc, "RoIDescriptors" ) ) EXPECTED ( StatusCode::SUCCESS );


}

void testRoIDescriptorReadAndCheck(StoreGateSvc* pStore) {

  const TrigRoiDescriptorCollection *rc = nullptr;
  VALUE( pStore->retrieve( rc, "DESERIALISED_RoIDescriptors") ) EXPECTED ( StatusCode::SUCCESS );
  VALUE( rc ) NOT_EXPECTED( nullptr );
  VALUE( rc->size() ) EXPECTED( 2 );
  VALUE( rc->at(0)->eta() ) EXPECTED( 3.5 );
  VALUE( rc->at(0)->phi() ) EXPECTED( 0.1 );
  VALUE( rc->at(0)->etaMinus() ) EXPECTED( -0.1 );
  VALUE( rc->at(0)->etaPlus() ) EXPECTED( 0.2 );
  VALUE( rc->at(0)->phiMinus() ) EXPECTED( -0.1 );
  VALUE( rc->at(0)->phiPlus() ) EXPECTED( 0.1 );

  VALUE( rc->at(1)->eta() ) EXPECTED( -1.5 );
  VALUE( rc->at(1)->phi() ) EXPECTED( -0.4 );
  VALUE( rc->at(1)->etaMinus() ) EXPECTED( -0.3 );
  VALUE( rc->at(1)->etaPlus() ) EXPECTED( 0.3 );
  VALUE( rc->at(1)->phiMinus() ) EXPECTED( -0.23 );
  VALUE( rc->at(1)->phiPlus() ) EXPECTED( 0.197 );




}
