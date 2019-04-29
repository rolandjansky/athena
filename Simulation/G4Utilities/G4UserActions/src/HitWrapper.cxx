/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <cmath>
#include <cstdlib>

#include "G4UserActions/HitWrapper.h"
#include "G4Step.hh"

#include "StoreGate/WriteHandle.h"

#include "MuonSimEvent/CSCSimHitCollection.h"
#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MuonSimEvent/RPCSimHitCollection.h"
#include "MuonSimEvent/TGCSimHitCollection.h"
/** for nSW */
#include "MuonSimEvent/sTGCSimHitCollection.h"
#include "MuonSimEvent/MMSimHitCollection.h"

#include "InDetSimEvent/TRTUncompressedHitCollection.h"
#include "InDetSimEvent/SiHitCollection.h"


namespace G4UA
{

  //---------------------------------------------------------------------------
  HitWrapper::HitWrapper(const Config& config)
    : AthMessaging(Gaudi::svcLocator()->service<IMessageSvc>("MessageSvc"),
                   "HitWrapper"),
      m_evtStore("StoreGateSvc/StoreGateSvc","HitWrapper"),
      m_detStore("StoreGateSvc/DetectorStore","HitWrapper"),
      m_config(config)
  {}

  //---------------------------------------------------------------------------
  void HitWrapper::EndOfEventAction(const G4Event*)
  {
    // @todo This syntax will not work at runtime in the master
    // branch. The HitWrapper should become a separate algorithm which
    // runs after G4AtlasAlg or ISF_Kernel.

    SG::WriteHandle<CSCSimHitCollection> csc("CSC_Hits");
    SG::WriteHandle<MDTSimHitCollection> mdt("MDT_Hits");
    SG::WriteHandle<TGCSimHitCollection> tgc("TGC_Hits");
    SG::WriteHandle<RPCSimHitCollection> rpc("RPC_Hits");
    /** for nSW */
    SG::WriteHandle<MMSimHitCollection> mmhits("MicromegasSensitiveDetector");
    SG::WriteHandle<sTGCSimHitCollection> stgc("sTGCSensitiveDetector");

    if (!csc.isValid() ) ATH_MSG_WARNING( " HitWrapper could not access csc hit collection" );
    else {
      //      std::cout << "Working on a collection of size " << cscC->size() << std::endl;
      //CSCSimHitCollection * csc = const_cast< CSCSimHitCollection * > (&(*cscC));
      for (CSCSimHitCollection::iterator hit=csc->begin();hit!=csc->end();++hit){
	//std::cout << "Wrapping CSC hit with time " << (*hit).globalTime() << std::endl;
	(*hit).setGlobalTime( fmod( (*hit).globalTime() , m_config.time ) );
      }
    }

    if ( ! mdt.isValid() ) ATH_MSG_WARNING( " HitWrapper could not access mdt hit collection" );
    else {
      //      std::cout << "Working on a collection of size " << mdtC->size() << std::endl;
      //MDTSimHitCollection * mdt = const_cast< MDTSimHitCollection * > (&(*mdtC));
      for (MDTSimHitCollection::iterator hit=mdt->begin();hit!=mdt->end();++hit){
	//std::cout << "Wrapping MDT hit with time " << (*hit).globalTime() << std::endl;
	(*hit).setGlobalTime( fmod( (*hit).globalTime() , m_config.time ) );
      }
    }

    if (! rpc.isValid() ) ATH_MSG_WARNING( " HitWrapper could not access rpc hit collection" );
    else {
      //      std::cout << "Working on a collection of size " << rpcC->size() << std::endl;
      //RPCSimHitCollection * rpc = const_cast< RPCSimHitCollection * > (&(*rpcC));
      for (RPCSimHitCollection::iterator hit=rpc->begin();hit!=rpc->end();++hit){
	//std::cout << "Wrapping RPC hit with time " << (*hit).globalTime() << std::endl;
	(*hit).setGlobalTime( fmod( (*hit).globalTime() , m_config.time ) );
      }
    }


    if (! tgc.isValid() ) ATH_MSG_WARNING( " HitWrapper could not access tgc hit collection" );
    else {
      //      std::cout << "Working on a collection of size " << tgcC->size() << std::endl;
      //TGCSimHitCollection * tgc = const_cast< TGCSimHitCollection * > (&(*tgcC));
      for (TGCSimHitCollection::iterator hit=tgc->begin();hit!=tgc->end();++hit){
	//std::cout << "Wrapping TGC hit with time " << (*hit).globalTime() << std::endl;
	(*hit).setGlobalTime( fmod( (*hit).globalTime() , m_config.time ) );
      }
    }


    /** for nSW */
    if (! mmhits.isValid() ) ATH_MSG_WARNING( " HitWrapper could not access Micromegas hit collection" );
    else {
      //      std::cout << "Working on a collection of size " << nswC->size() << std::endl;
      //GenericMuonSimHitCollection *mm = const_cast< GenericMuonSimHitCollection * > (&(*mmC));
      for (MMSimHitCollection::iterator hit=mmhits->begin();hit!=mmhits->end();++hit){
	//std::cout << "Wrapping GenericMuon hit with time " << (*hit).globalTime() << std::endl;
	(*hit).setGlobalTime( fmod( (*hit).globalTime() , m_config.time ) );
      }
    }

    if (! stgc.isValid() ) ATH_MSG_WARNING( " HitWrapper could not access sTGC hit collection" );
    else {
      //      std::cout << "Working on a collection of size " << nswC->size() << std::endl;
      //GenericMuonSimHitCollection *stgc = const_cast< GenericMuonSimHitCollection * > (&(*stgcC));
      for (sTGCSimHitCollection::iterator hit=stgc->begin();hit!=stgc->end();++hit){
	//std::cout << "Wrapping GenericMuon hit with time " << (*hit).globalTime() << std::endl;
	(*hit).setGlobalTime( fmod( (*hit).globalTime() , m_config.time ) );
      }
    }

    // Handle the inner detector next
    SG::WriteHandle<TRTUncompressedHitCollection> trt("TRTUncompressedHits");
    SG::WriteHandle<SiHitCollection> pix("PixelHits");
    SG::WriteHandle<SiHitCollection> sct("SCT_Hits");

    if (! trt.isValid() ) ATH_MSG_WARNING( " HitWrapper could not access trt hit collection" );
    else {
      //TRTUncompressedHitCollection * trt = const_cast< TRTUncompressedHitCollection * > (&*trtC);
      for (TRTUncompressedHitCollection::iterator hit=trt->begin();hit!=trt->end();++hit){
	//std::cout << "Wrapping TRT hit with time " << (*hit).globalTime() << std::endl;
	(*hit).SetGlobalTime( fmod( (*hit).GetGlobalTime() , m_config.time ) );
      }
    }

    if (! pix.isValid()) ATH_MSG_WARNING( " HitWrapper could not access pix hit collection" );
    else {
      //SiHitCollection * pix = const_cast<SiHitCollection *> (&*pixC);
      for (SiHitCollection::iterator hit=pix->begin();hit!=pix->end();++hit){
	//std::cout << "Wrapping Pix hit with time " << (*hit).meanTime() << std::endl;
	(*hit).setMeanTime( fmod( (*hit).meanTime() , m_config.time ) );
      }
    }

    if (! sct.isValid() ) ATH_MSG_WARNING( " HitWrapper could not access sct hit collection" );
    else {
      //SiHitCollection * sct = const_cast<SiHitCollection *> (&*sctC);
      for (SiHitCollection::iterator hit=sct->begin();hit!=sct->end();++hit){
	//std::cout << "Wrapping SCT hit with time " << (*hit).meanTime() << std::endl;
	(*hit).setMeanTime( fmod( (*hit).meanTime() , m_config.time ) );
      }
    }

  }

} // namespace G4UA
