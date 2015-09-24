/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/HitWrapper.h"
#include <iostream>
#include <cmath>
#include <cstdlib>


HitWrapper::HitWrapper(const std::string& type, const std::string& name, const IInterface* parent) : UserActionBase(type,name,parent),
												     m_time(25.) { 

  declareProperty("WrapTime",m_time);

}


#include "MuonSimEvent/CSCSimHitCollection.h"
#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MuonSimEvent/RPCSimHitCollection.h"
#include "MuonSimEvent/TGCSimHitCollection.h"
/** for nSW */
#include "MuonSimEvent/GenericMuonSimHitCollection.h"

#include "InDetSimEvent/TRTUncompressedHitCollection.h"
#include "InDetSimEvent/SiHitCollection.h"

void HitWrapper::EndOfEvent(const G4Event* /*anEvent*/)
{

  const DataHandle <CSCSimHitCollection> cscC;
  const DataHandle <MDTSimHitCollection> mdtC;
  const DataHandle <TGCSimHitCollection> tgcC;
  const DataHandle <RPCSimHitCollection> rpcC;
  /** for nSW */
  const DataHandle<GenericMuonSimHitCollection> mmC;
  const DataHandle<GenericMuonSimHitCollection> stgcC;
  
  if ( evtStore()->retrieve(cscC).isFailure() || !cscC) ATH_MSG_WARNING( " HitWrapper could not access csc hit collection" );
  else {
    //      std::cout << "Working on a collection of size " << cscC->size() << std::endl;
    CSCSimHitCollection * csc = const_cast< CSCSimHitCollection * > (&(*cscC));
    for (CSCSimHitCollection::iterator hit=csc->begin();hit!=csc->end();++hit){
      //        std::cout << "Wrapping CSC hit with time " << (*hit).globalTime() << std::endl;
      (*hit).setGlobalTime( fmod( (*hit).globalTime() , m_time ) );
    }
  }
 
  if ( evtStore()->retrieve(mdtC).isFailure() || !mdtC) ATH_MSG_WARNING( " HitWrapper could not access mdt hit collection" );
  else {
    //      std::cout << "Working on a collection of size " << mdtC->size() << std::endl;
    MDTSimHitCollection * mdt = const_cast< MDTSimHitCollection * > (&(*mdtC));
    for (MDTSimHitCollection::iterator hit=mdt->begin();hit!=mdt->end();++hit){
      //        std::cout << "Wrapping MDT hit with time " << (*hit).globalTime() << std::endl;
      (*hit).setGlobalTime( fmod( (*hit).globalTime() , m_time ) );
    }
  }

  if (evtStore()->retrieve(rpcC).isFailure() || !rpcC) ATH_MSG_WARNING( " HitWrapper could not access rpc hit collection" );
  else {
    //      std::cout << "Working on a collection of size " << rpcC->size() << std::endl;      
    RPCSimHitCollection * rpc = const_cast< RPCSimHitCollection * > (&(*rpcC));
    for (RPCSimHitCollection::iterator hit=rpc->begin();hit!=rpc->end();++hit){
      //        std::cout << "Wrapping RPC hit with time " << (*hit).globalTime() << std::endl;
      (*hit).setGlobalTime( fmod( (*hit).globalTime() , m_time ) );
    }
  }

  
  if (evtStore()->retrieve(tgcC).isFailure() || !tgcC) ATH_MSG_WARNING( " HitWrapper could not access tgc hit collection" );
  else {
    //      std::cout << "Working on a collection of size " << tgcC->size() << std::endl;      
    TGCSimHitCollection * tgc = const_cast< TGCSimHitCollection * > (&(*tgcC));
    for (TGCSimHitCollection::iterator hit=tgc->begin();hit!=tgc->end();++hit){
      //        std::cout << "Wrapping TGC hit with time " << (*hit).globalTime() << std::endl;
      (*hit).setGlobalTime( fmod( (*hit).globalTime() , m_time ) );
    }
  }
  

  /** for nSW */
  if (evtStore()->retrieve(mmC,"MicromegasSensitiveDetector").isFailure() || !mmC) ATH_MSG_WARNING( " HitWrapper could not access Micromegas hit collection" );
  else {
    //      std::cout << "Working on a collection of size " << nswC->size() << std::endl;      
    GenericMuonSimHitCollection *mm = const_cast< GenericMuonSimHitCollection * > (&(*mmC));
    for (GenericMuonSimHitCollection::iterator hit=mm->begin();hit!=mm->end();++hit){
      //        std::cout << "Wrapping GenericMuon hit with time " << (*hit).globalTime() << std::endl;
      (*hit).setGlobalTime( fmod( (*hit).globalTime() , m_time ) );
    }
  }
  
  if (evtStore()->retrieve(stgcC,"sTGCSensitiveDetector").isFailure() || !stgcC) ATH_MSG_WARNING( " HitWrapper could not access sTGC hit collection" );
  else {
    //      std::cout << "Working on a collection of size " << nswC->size() << std::endl;      
    GenericMuonSimHitCollection *stgc = const_cast< GenericMuonSimHitCollection * > (&(*stgcC));
    for (GenericMuonSimHitCollection::iterator hit=stgc->begin();hit!=stgc->end();++hit){
      //        std::cout << "Wrapping GenericMuon hit with time " << (*hit).globalTime() << std::endl;
      (*hit).setGlobalTime( fmod( (*hit).globalTime() , m_time ) );
    }
  }
  
  // Handle the inner detector next
  const DataHandle <TRTUncompressedHitCollection> trtC;
  const DataHandle <SiHitCollection> pixC;
  const DataHandle <SiHitCollection> sctC;
  
  if (evtStore()->retrieve(trtC).isFailure() || !trtC) ATH_MSG_WARNING( " HitWrapper could not access trt hit collection" );
  else {
    TRTUncompressedHitCollection * trt = const_cast< TRTUncompressedHitCollection * > (&*trtC);
    for (TRTUncompressedHitCollection::iterator hit=trt->begin();hit!=trt->end();++hit){
      //        std::cout << "Wrapping TRT hit with time " << (*hit).globalTime() << std::endl;
      (*hit).SetGlobalTime( fmod( (*hit).GetGlobalTime() , m_time ) );
    }
  }
  
  if (evtStore()->retrieve(pixC,"PixelHits").isFailure() || !pixC) ATH_MSG_WARNING( " HitWrapper could not access pix hit collection" );
  else {
    SiHitCollection * pix = const_cast<SiHitCollection *> (&*pixC);
    for (SiHitCollection::iterator hit=pix->begin();hit!=pix->end();++hit){
      //        std::cout << "Wrapping Pix hit with time " << (*hit).meanTime() << std::endl;
      (*hit).setMeanTime( fmod( (*hit).meanTime() , m_time ) );
    }
  }
  
  if (evtStore()->retrieve(sctC,"SCT_Hits").isFailure() || !sctC) ATH_MSG_WARNING( " HitWrapper could not access sct hit collection" );
  else {
    SiHitCollection * sct = const_cast<SiHitCollection *> (&*sctC);
    for (SiHitCollection::iterator hit=sct->begin();hit!=sct->end();++hit){
      //        std::cout << "Wrapping SCT hit with time " << (*hit).meanTime() << std::endl;
      (*hit).setMeanTime( fmod( (*hit).meanTime() , m_time ) );
    }
  }
  
}





StatusCode HitWrapper::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( IUserAction::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IUserAction*>(this);
    addRef();
  } else {
    // Interface is not directly available : try out a base class
    return UserActionBase::queryInterface(riid, ppvInterface);
  }
  return StatusCode::SUCCESS;
}
