/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "iostream"
#include "vector"

#include "TestTools/initGaudi.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrigNavigation/Holder.h"
#include "TrigNavigation/Holder.icc"

#include "TrigNavigation/TriggerElement.h"

#include "TestTypes.h"
#include "TestUtils.h"

using namespace std;
using namespace HLTNavDetails;
// Hi-lock: (("REPORT_AND_STOP" (0 (quote hi-yellow) t)))

//const int OK=1;
//const int FAILED=0;

StoreGateSvc* pStore(0);



template<class HTYPE> 
StatusCode reg( HTYPE* full, const char* name, int idx, ITypeProxy* /*aux*/, typename HTYPE::base_type*& base_holder ) {
  BEGIN_TEST("Registration");
  IHolder* iholder = full->clone("", name, idx);
  if ( ! iholder ) REPORT_AND_STOP ("Holder can't create IHolder" );

  base_holder = dynamic_cast<typename HTYPE::base_type*>(iholder); // we do nto intend to do it but here it is to see if types realy fit
  if ( ! base_holder ) REPORT_AND_STOP ("Holder can't create base holder" );
  

  iholder->prepare(msglog, pStore,0, false);
  if ( iholder->syncWithSG() == false ) REPORT_AND_STOP( "can not sync wiht holder" );
  
  END_TEST;
}

StatusCode getUniqueKeyBeforeReg() {
  BEGIN_TEST( "use of unique key without sync to SG" );
  auto h = new HolderImp<TestBContainer, TestBContainer >();
  h->prepare(msglog, pStore,0);
  std::string key = h->getUniqueKey();
  REPORT_AND_CONTINUE("Got unique Key:" << key);
  END_TEST;
}

//*****************************************************************************
StatusCode creation() {
  BEGIN_TEST( "Creation of the holders Container <-> Container" );
  Holder<TestBContainer >* cc(0);

  if (  reg( new HolderImp<TestBContainer, TestBContainer >() , "creation0", 0, 0, cc).isFailure() ) REPORT_AND_STOP( "reg creation0" );

  REPORT_AND_CONTINUE( "Creation of the holders Object <-> Container" );
  Holder<TestA>* oc(0); 
  if ( reg(new HolderImp<TestA, TestAContainer >(), "creation1", 1, 0, oc).isFailure() ) REPORT_AND_STOP("reg creation1") ;


  //////////////////////////////////////////////////////////////////
  REPORT_AND_CONTINUE( "Creation of the holders Container <-> Container with details" );
  ITypeProxy* deco = HLT::TypeMaps::proxies()[ClassID_traits<TestAuxB>::ID()]->clone();
  Holder<TestBContainer >* cc_dec(0) ; 
  if ( reg(new HolderImp<TestBContainer, TestBContainer >(), "creation2", 2, deco, cc_dec).isFailure() ) REPORT_AND_STOP("reg creation2") ;


  REPORT_AND_CONTINUE( "Creation of the holders Object <-> Container" );
  Holder<TestA>* oc_dec(0); 
  if ( reg(new HolderImp<TestA, TestAContainer >(), "creation3", 3, deco, oc_dec).isFailure() ) REPORT_AND_STOP("reg creation3") ;

  Holder<TestBContainerView >* cc_view(0);  
  if (  reg( new HolderImp<TestBContainerView, TestBContainerView >() , "creation4", 0, 0, cc_view).isFailure() ) REPORT_AND_STOP( "reg creation4" );
  
  //  REPORT_AND_CONTINUE( "Creation of the holders ViewObject <-> ViewContainer" );
  //  Holder<TestB >* cc_view_obj(0);
  //  if (  reg( new HolderImp<TestB, TestBContainerView >() , "creation0", 0, 0, cc_view_obj).isFailure() ) REPORT_AND_STOP( "reg creation5" );
  END_TEST;
}

//*****************************************************************************
StatusCode add_operation(bool wihtAux) {
  BEGIN_TEST ( "Simple test of Container <-> Container holder (add)" );
  ITypeProxy* deco(0);
  if (wihtAux)
    deco = HLT::TypeMaps::proxies()[ClassID_traits<TestAuxB>::ID()]->clone();

  Holder<TestBContainer>* cch(0);
  if ( reg( new HolderImp<TestBContainer, TestBContainer>(), "TestB", 11, deco, cch).isFailure() ) REPORT_AND_STOP("It should have failed before");

  TestBContainer* dav0 = new TestBContainer;;
  dav0->push_back(new TestB(6));
  REPORT_AND_CONTINUE( "adding simple vec" );
  HLT::TriggerElement::ObjectIndex single = cch->add(dav0, false);

  //  BContainer dav(SG::OWN_ELEMENTS);
  TestBContainer* dav = new TestBContainer;
  dav->push_back(new TestB(1));
  dav->push_back(new TestB(2));

  REPORT_AND_CONTINUE( "Added another ownership" );
  pStore->record(dav, "dav1").ignore();
  HLT::TriggerElement::ObjectIndex two =  cch->add(dav, true);

  TestBContainer* dav2 = new TestBContainer;
  dav2->push_back(new TestB(3));
  dav2->push_back(new TestB(4));  

  REPORT_AND_CONTINUE( "Added another ownership -with the hint" );
  pStore->record(dav2, "dav2").ignore();
  HLT::TriggerElement::ObjectIndex three =  cch->add(dav2, true, "dav2");

  REPORT_AND_CONTINUE( "Adding a VIEW container" );
  TestBContainer* dav3 = new TestBContainer;
  dav3->clear(SG::VIEW_ELEMENTS);
  dav3->push_back(new TestB(7));
  dav3->push_back(new TestB(7));

  /*HLT::TriggerElement::ObjectIndex four = */ cch->add(dav3, false, "dav3");

  // retieve back the information
  {
    const TestBContainer* getv(0);
    cch->get(getv, single);
    if ( (*getv)[0]->value() != 6 ) 
      REPORT_AND_STOP( "Content missing or reordered -single feature in the container)");
  }
  {
    const TestBContainer* getv(0);
    cch->get(getv, two);
    if ( getv->size() != 2 or (*getv)[0]->value() != 1 or (*getv)[1]->value() != 2 ) 
      REPORT_AND_STOP( "Content missing or reordered -single feature in the container)");
  }

  {
    const TestBContainer* getv(0);
    cch->get(getv, three);
    if ( getv->size() != 2 or (*getv)[0]->value() != 3 or (*getv)[1]->value() != 4 ) 
      REPORT_AND_STOP( "Content missing or reordered - single feature in the container)");
  }
  REPORT_AND_CONTINUE( "END Simple test of Container <-> Container holder -add-" );
  ///////////////////////////////////////////////////////////////////////////// 

  REPORT_AND_CONTINUE( "END Simple test of Object <-> Container holder -add-" );
  Holder<TestA>* och(0); 
  if ( reg(new HolderImp<TestA, TestAContainer >(), "TestA", 12, deco, och).isFailure() ) REPORT_AND_STOP("reg TestA") ;

  TestA* an = new TestA(235);
  HLT::TriggerElement::ObjectIndex coord = och->add(an, true); 
  //  HLT::TriggerElement::ObjectIndex ancoord = coord;
  const TestA* aback(0);
  if ( och->get(aback, coord) == false ) 
    REPORT_AND_STOP ("Content missing of single feature" );
  
  if ( aback == 0 ) REPORT_AND_STOP ("Content is none" );

  if ( aback->value() != 235 ) REPORT_AND_STOP ( "Content is different" );

  END_TEST;
}

//*****************************************************************************
StatusCode contains_operations() {
  BEGIN_TEST("Operations");
  Holder<TestBContainer>* cch(0);
  if ( reg( new HolderImp<TestBContainer, TestBContainer>(), "TestB", 11, 0, cch).isFailure() ) REPORT_AND_STOP("It should have failed before");
  TestB* b0 = new TestB(67);
  TestBContainer * t = new TestBContainer();
  t->push_back( new TestB(9) );
  t->push_back(b0);
  t->push_back( new TestB(8) );
  if ( not cch->add(t, false, "whatever").valid() )
    REPORT_AND_STOP("adding failed");
  HLT::TriggerElement::ObjectIndex testidx;
  if ( not cch->contains(t, testidx) ) 
    REPORT_AND_STOP("Holder should have this pointer");

  TestBContainer * t2 = new TestBContainer();
  t2->push_back( new TestB());
  if ( cch->contains(t2, testidx) ) 
    REPORT_AND_STOP("Holder should not have this pointer");

  END_TEST;
}

//*****************************************************************************
StatusCode serialization() {
  BEGIN_TEST("serialization");
  Holder<TestBContainer>* cch(0);
  if ( reg( new HolderImp<TestBContainer, TestBContainer>(), "blu", 2, 0, cch).isFailure() ) REPORT_AND_STOP("It should have failed before");
  IHolder * realH = cch;
  std::vector<uint32_t>  blob;
  
  CLID c;
  uint16_t idx;
  string label;

  realH->serialize(blob);

  REPORT_AND_CONTINUE( "blob size " << blob.size() );
  std::vector<uint32_t>::const_iterator it = blob.begin();


  IHolder::enquireSerialized(it,blob.end(), c, label, idx);
  *msglog << MSG::DEBUG  << "INFO clid  : " << c << endreq;  
  *msglog << MSG::DEBUG  << "INFO STidx : " << idx << endreq;  
  *msglog << MSG::DEBUG  << "INFO label : " << label << endreq;  
  if ( label != "blu" || idx != 2 || c != ClassID_traits<TestBContainer>::ID() ) 
    REPORT_AND_STOP ( "serialization or inquire failed" );
  
  END_TEST;
}


//*****************************************************************************
StatusCode externalCollection() {
  BEGIN_TEST("externalCollection");

  TestBContainer* dav = new TestBContainer;
  dav->push_back(new TestB(1));
  dav->push_back(new TestB(2));
  
  if ( pStore->record(dav, "HLT_external").isFailure() )
    REPORT_AND_STOP("Failed to record in SG");
  
  Holder<TestBContainer> *base = new HolderImp<TestBContainer, TestBContainer >();
  Holder<TestBContainer> *h =   dynamic_cast<Holder<TestBContainer>*>(base->clone("", "external", 77));
  h->prepare(msglog, pStore,0, false);  

  END_TEST;
}


//*****************************************************************************
int main() {


   ISvcLocator* pSvcLoc;
   if (!Athena_test::initGaudi("test.txt",  pSvcLoc)) {
     cerr << "ERROR This test can not be run" << endl;
     return 0;
   }
   assert(pSvcLoc);
   MsgStream log(Athena::getMessageSvc(), "Holder_test");
   msglog = &log;

  if( pSvcLoc->service("StoreGateSvc", pStore, true).isSuccess() ) {
    *msglog << MSG::DEBUG << "SG pointer: " << pStore << endreq;
  } else {
    ABORT( "ERROR no SG available" );
  }

  if ( creation().isFailure() ) 
    ABORT("Holders creation failed");

  *msglog << MSG::DEBUG << pStore->dump();
  *msglog << endreq;




  pStore->clearStore();
  if ( add_operation(false).isFailure() )
    ABORT("Failed add_operation w/o Aux");

  pStore->clearStore();
  *msglog << MSG::DEBUG << "Holders with decorations "  << endreq;
  if ( add_operation(true).isFailure() )
    ABORT("Failed add_operation with Aux");

  pStore->clearStore();
  *msglog << MSG::DEBUG << "Contains and co. methods "  << endreq;
  if ( contains_operations().isFailure() )
    ABORT("Failed contains_operations");


  *msglog << MSG::DEBUG << "Serialization "  << endreq;
  if ( serialization().isFailure() )
    ABORT("Failed serialization");

  *msglog << MSG::DEBUG << "Unique key "  << endreq;
  if ( getUniqueKeyBeforeReg().isFailure() ) 
    ABORT("UniqueKey failed");


  if ( externalCollection().isFailure() ) 
    ABORT("Sync to an exteranl collection failed");
  REPORT_AND_CONTINUE( "END all went fine" );
  return 0;
}
