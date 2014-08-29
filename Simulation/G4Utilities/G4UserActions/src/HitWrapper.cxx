/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/HitWrapper.h"
#include <iostream>
#include <cmath>
#include <cstdlib>

static HitWrapper hwa("HitWrapper");

void HitWrapper::BeginOfEventAction(const G4Event* /*anEvent*/){;}
void HitWrapper::BeginOfRunAction(const G4Run* /*aRun*/){;}
void HitWrapper::EndOfRunAction(const G4Run* /*aRun*/){;}
void HitWrapper::SteppingAction(const G4Step* /*aStep*/){;}

void HitWrapper::parseProperties()
{
  if(theProperties.find("WrapTime")==theProperties.end()){
    if (log().level()<=MSG::DEBUG) log()<<MSG::DEBUG<<"HitWrapper: no WrapTime specified, setting to default (=" << m_time << "ns)"<<endreq;
    theProperties["WrapTime"]="25.0";
  } else {
    m_time = std::strtod(theProperties["WrapTime"].c_str(),0);
    if (m_time<0 || std::isinf(m_time) || std::isnan(m_time)){  // GIGO
      log()<<MSG::INFO << "HitWrapper: Setting the time to " << m_time << " is not allowed.  Resetting to 25 ns." << endreq; 
      m_time = 25.;
    } else log()<<MSG::INFO << "HitWrapper: Setting the time to " << m_time << " ns as requested." << endreq;
  }
  m_init=true;
}

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"
#include "MuonSimEvent/CSCSimHitCollection.h"
#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MuonSimEvent/RPCSimHitCollection.h"
#include "MuonSimEvent/TGCSimHitCollection.h"
/** for nSW */
#include "MuonSimEvent/GenericMuonSimHitCollection.h"

#include "InDetSimEvent/TRTUncompressedHitCollection.h"
#include "InDetSimEvent/SiHitCollection.h"

void HitWrapper::EndOfEventAction(const G4Event* /*anEvent*/)
{
  if (!m_init) parseProperties();

  static StoreGateSvc * m_storeGate = 0;
  if (!m_storeGate){
    ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
    StatusCode status = svcLocator->service("StoreGateSvc", m_storeGate);
    if (status.isFailure()){
      log()<<MSG::WARNING<< "HitWrapper::EndOfEventAction could not access StoreGateSvc"<<endreq;
    }
  }

  if (m_storeGate){
    const DataHandle <CSCSimHitCollection> cscC;
    const DataHandle <MDTSimHitCollection> mdtC;
    const DataHandle <TGCSimHitCollection> tgcC;
    const DataHandle <RPCSimHitCollection> rpcC;
    /** for nSW */
   const DataHandle<GenericMuonSimHitCollection> mmC;
   const DataHandle<GenericMuonSimHitCollection> stgcC;

    StatusCode sc = m_storeGate->retrieve(cscC);
    if (sc.isFailure() || !cscC) log() << MSG::WARNING << " HitWrapper could not access csc hit collection" << endreq;
    else {
//      std::cout << "Working on a collection of size " << cscC->size() << std::endl;
      CSCSimHitCollection * csc = const_cast< CSCSimHitCollection * > (&(*cscC));
      for (CSCSimHitCollection::iterator hit=csc->begin();hit!=csc->end();++hit){
//        std::cout << "Wrapping CSC hit with time " << (*hit).globalTime() << std::endl;
        (*hit).setGlobalTime( fmod( (*hit).globalTime() , m_time ) );
      }
    }

    sc = m_storeGate->retrieve(mdtC);
    if (sc.isFailure() || !mdtC) log() << MSG::WARNING << " HitWrapper could not access mdt hit collection" << endreq;
    else {
//      std::cout << "Working on a collection of size " << mdtC->size() << std::endl;
      MDTSimHitCollection * mdt = const_cast< MDTSimHitCollection * > (&(*mdtC));
      for (MDTSimHitCollection::iterator hit=mdt->begin();hit!=mdt->end();++hit){
//        std::cout << "Wrapping MDT hit with time " << (*hit).globalTime() << std::endl;
        (*hit).setGlobalTime( fmod( (*hit).globalTime() , m_time ) );
      }
    }

    sc = m_storeGate->retrieve(rpcC);
    if (sc.isFailure() || !rpcC) log() << MSG::WARNING << " HitWrapper could not access rpc hit collection" << endreq;
    else {
//      std::cout << "Working on a collection of size " << rpcC->size() << std::endl;      
      RPCSimHitCollection * rpc = const_cast< RPCSimHitCollection * > (&(*rpcC));
      for (RPCSimHitCollection::iterator hit=rpc->begin();hit!=rpc->end();++hit){
//        std::cout << "Wrapping RPC hit with time " << (*hit).globalTime() << std::endl;
        (*hit).setGlobalTime( fmod( (*hit).globalTime() , m_time ) );
      }
    }

    sc = m_storeGate->retrieve(tgcC);
    if (sc.isFailure() || !tgcC) log() << MSG::WARNING << " HitWrapper could not access tgc hit collection" << endreq;
    else {
//      std::cout << "Working on a collection of size " << tgcC->size() << std::endl;      
      TGCSimHitCollection * tgc = const_cast< TGCSimHitCollection * > (&(*tgcC));
      for (TGCSimHitCollection::iterator hit=tgc->begin();hit!=tgc->end();++hit){
//        std::cout << "Wrapping TGC hit with time " << (*hit).globalTime() << std::endl;
        (*hit).setGlobalTime( fmod( (*hit).globalTime() , m_time ) );
      }
    }


/** for nSW */
    sc = m_storeGate->retrieve(mmC,"MicromegasSensitiveDetector");
    if (sc.isFailure() || !mmC) log() << MSG::WARNING << " HitWrapper could not access Micromegas hit collection" << endreq;
    else {
//      std::cout << "Working on a collection of size " << nswC->size() << std::endl;      
      GenericMuonSimHitCollection *mm = const_cast< GenericMuonSimHitCollection * > (&(*mmC));
      for (GenericMuonSimHitCollection::iterator hit=mm->begin();hit!=mm->end();++hit){
//        std::cout << "Wrapping GenericMuon hit with time " << (*hit).globalTime() << std::endl;
        (*hit).setGlobalTime( fmod( (*hit).globalTime() , m_time ) );
      }
    }

 
   sc = m_storeGate->retrieve(stgcC,"sTGCSensitiveDetector");
    if (sc.isFailure() || !stgcC) log() << MSG::WARNING << " HitWrapper could not access sTGC hit collection" << endreq;
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

    sc = m_storeGate->retrieve(trtC);
    if (sc.isFailure() || !trtC) log() << MSG::WARNING << " HitWrapper could not access trt hit collection" << endreq;
    else {
      TRTUncompressedHitCollection * trt = const_cast< TRTUncompressedHitCollection * > (&*trtC);
      for (TRTUncompressedHitCollection::iterator hit=trt->begin();hit!=trt->end();++hit){
//        std::cout << "Wrapping TRT hit with time " << (*hit).globalTime() << std::endl;
        (*hit).SetGlobalTime( fmod( (*hit).GetGlobalTime() , m_time ) );
      }
    }

    sc = m_storeGate->retrieve(pixC,"PixelHits");
    if (sc.isFailure() || !pixC) log() << MSG::WARNING << " HitWrapper could not access pix hit collection" << endreq;
    else {
      SiHitCollection * pix = const_cast<SiHitCollection *> (&*pixC);
      for (SiHitCollection::iterator hit=pix->begin();hit!=pix->end();++hit){
//        std::cout << "Wrapping Pix hit with time " << (*hit).meanTime() << std::endl;
        (*hit).setMeanTime( fmod( (*hit).meanTime() , m_time ) );
      }
    }

    sc = m_storeGate->retrieve(sctC,"SCT_Hits");
    if (sc.isFailure() || !sctC) log() << MSG::WARNING << " HitWrapper could not access sct hit collection" << endreq;
    else {
      SiHitCollection * sct = const_cast<SiHitCollection *> (&*sctC);
      for (SiHitCollection::iterator hit=sct->begin();hit!=sct->end();++hit){
//        std::cout << "Wrapping SCT hit with time " << (*hit).meanTime() << std::endl;
        (*hit).setMeanTime( fmod( (*hit).meanTime() , m_time ) );
      }
    }

  }
}

