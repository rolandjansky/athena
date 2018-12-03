/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetTrackTruthMapCnv.h"
#include "RootConversions/TConverterRegistry.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMap_old_cnv.h"
#include "TROOT.h"
#include <iostream>

//constructor
TrigInDetTrackTruthMapCnv::TrigInDetTrackTruthMapCnv( ISvcLocator* svcloc ) :
  TrigInDetTrackTruthMapCnvBase( svcloc ) {
}

//destructor
TrigInDetTrackTruthMapCnv::~TrigInDetTrackTruthMapCnv() {}


//create persistent
TrigInDetTrackTruthMap_PERS* TrigInDetTrackTruthMapCnv::createPersistent(TrigInDetTrackTruthMap* transObj) {
    MsgStream mlog(messageService(), "TrigInDetTrackTruthMapConverter" );

    mlog << MSG::DEBUG << "TrigInDetTrackTruthMapCnv::createPersistent" << endreq;

    TrigInDetTrackTruthMap_PERS* persObj = m_trigInDetTrackTruthMapCnv_tlp2.createPersistent( transObj, mlog );

    return persObj;
}

//create transient
TrigInDetTrackTruthMap* TrigInDetTrackTruthMapCnv::createTransient() {
  
  MsgStream mlog(messageService(), "TrigInDetTrackTruthMapConverter" );
  mlog << MSG::DEBUG << "TrigInDetTrackTruthMapCnv::createTransient " << endreq;
  
  static pool::Guid tlp1_guid("08892FEB-5706-4938-9226-F45C0AA662E7");
  static pool::Guid tlp2_guid("02074F47-F290-4A48-B503-4DCAB4181B3D");
  static pool::Guid tlp3_guid("81BFA8A7-89A8-4072-B6E2-7124300CA9EB");
  static pool::Guid p0_guid("41581666-F06D-44AE-93B9-D7E912A27AA1");
  
  TrigInDetTrackTruthMap  *transObj = 0;
  
  
  if( compareClassGuid(tlp3_guid) ) {

    mlog << MSG::DEBUG << "TrigInDetTrackTruthMapCnv::reading tlp3 persistent object" << endreq;
    poolReadObject< TrigInDetTrackTruthMap_tlp3 >( m_trigInDetTrackTruthMapCnv_tlp3 );
    transObj = m_trigInDetTrackTruthMapCnv_tlp3.createTransient( mlog );

  }
  else if( compareClassGuid(tlp2_guid) ) {
    
    mlog << MSG::DEBUG << "TrigInDetTrackTruthMapCnv::reading tlp2 persistent object" << endreq;
    poolReadObject< TrigInDetTrackTruthMap_tlp2 >( m_trigInDetTrackTruthMapCnv_tlp2 );
    transObj = m_trigInDetTrackTruthMapCnv_tlp2.createTransient( mlog );
    
  }
  else if( compareClassGuid(tlp1_guid) ) {    
    mlog << MSG::DEBUG << "TrigInDetTrackTruthMapCnv::reading tlp1 persistent object" << endreq;
    TrigInDetTrackTruthMapCnv_tlp1  tlp1_Converter;
    poolReadObject< TrigInDetTrackTruthMap_tlp1 >(tlp1_Converter);
    transObj = tlp1_Converter.createTransient( mlog );
  }
  else if( compareClassGuid(p0_guid) ) {
    
    mlog << MSG::DEBUG << "TrigInDetTrackTruthMapCnv::reading p0 persistent object" << endreq;
    // old version from before TP separation, just return it
    transObj = this->poolReadObject<TrigInDetTrackTruthMap>();
  }  
  else {
    throw std::runtime_error("Unsupported persistent version of TrigInDetTrackTruthMap");
  }
  
  return transObj;
}


StatusCode TrigInDetTrackTruthMapCnv::initialize()
{
  static bool did_rootcnv = false;
  if (!did_rootcnv) {
    did_rootcnv = true;
    gROOT->GetClass("TrigInDetTrackTruthMap_old");
    static TrigInDetTrackTruthMap_old_cnv cnv;
    TConverterRegistry::Instance()->AddConverter (&cnv);
  }
  return TrigInDetTrackTruthMapCnvBase::initialize();
}
