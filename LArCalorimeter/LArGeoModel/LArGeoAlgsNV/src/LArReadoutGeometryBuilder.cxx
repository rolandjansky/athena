/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArReadoutGeometryBuilder.h"

#include "LArReadoutGeometry/HECDetectorManager.h"
#include "LArReadoutGeometry/HECDetDescr.h"
#include "LArReadoutGeometry/HECLongBlock.h"
#include "LArReadoutGeometry/HECDetectorRegion.h"

#include "LArReadoutGeometry/FCALDetectorManager.h"
#include "LArReadoutGeometry/FCALModule.h"
#include "LArReadoutGeometry/FCAL_ChannelMap.h"

#include "LArReadoutGeometry/EMECDetectorManager.h"
#include "LArReadoutGeometry/EMECDetectorRegion.h"
#include "LArReadoutGeometry/EMECDetDescr.h"

#include "LArReadoutGeometry/EMBDetectorManager.h"
#include "LArReadoutGeometry/EMBDetectorRegion.h"
#include "LArReadoutGeometry/EMBDetDescr.h"

#include "LArHV/LArHVManager.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/CellBinning.h"
#include "GeoModelUtilities/StoredPhysVol.h"
#include "GeoModelUtilities/StoredAlignX.h"

std::tuple<EMBDetectorManager*
	   ,EMECDetectorManager*
	   ,HECDetectorManager*
	   ,FCALDetectorManager*> buildLArReadoutGeometry(StoreGateSvc* detStore
							  , const LArHVManager* hvManager
							  , IMessageSvc* msgSvc
							  , int testbeam
							  , double projectivityDisplacement)
{
  MsgStream log(msgSvc, "buildLArReadoutGeometry");

  EMBDetectorManager*  embDetectorManager{nullptr};
  EMECDetectorManager* emecDetectorManager{nullptr};
  HECDetectorManager*  hecDetectorManager{nullptr};
  FCALDetectorManager* fcalDetectorManager{nullptr};

  { // FCAL
    fcalDetectorManager = new FCALDetectorManager(&(hvManager->getFCALHVManager()));
    std::string fcalModName[2][3]={{"FCAL1_NEG","FCAL2_NEG","FCAL3_NEG"},
				   {"FCAL1_POS","FCAL2_POS","FCAL3_POS"}};
    for (int e=0;e<2;e++) {
      for (int m=0;m<3;m++) {
	StoredPhysVol *vol;
	if(detStore->contains<StoredPhysVol>(fcalModName[e][m])){
	  if (detStore->retrieve(vol,fcalModName[e][m])==StatusCode::SUCCESS) {
	    FCALModule *module=new FCALModule(vol->getPhysVol(),FCALModule::Module(m+1),FCALModule::Endcap(e),projectivityDisplacement);
	    fcalDetectorManager->addModule(module);
	  }
	}
	else {
	  log << MSG::DEBUG << " No Stored PV for " << fcalModName[e][m] << " in Detector Store" << endmsg;
	}
      }
    }
  }

  { // HEC
    hecDetectorManager = new  HECDetectorManager(&(hvManager->getHECHVManager()));
    for (unsigned int s=0;s<4;s++) {
      for (unsigned int r=0;r<2;r++) {
	unsigned int nPhi    = r==0? 2:1;
       	for (unsigned int endcap=0;endcap<2;endcap++) {
	  StoredPhysVol *vol;
	  std::string nameTag= endcap==0 ? (s<2 ? "HEC1_NEG":"HEC2_NEG") : (s<2 ? "HEC1_POS":"HEC2_POS");
	  if(detStore->contains<StoredPhysVol>(nameTag)){
	    if (StatusCode::SUCCESS==detStore->retrieve(vol, nameTag)) {
	      unsigned int width = 32;
	      double startPhi =  endcap==0?  M_PI : 0;
	      double endPhi   =  endcap==0? -M_PI : 2*M_PI ;

	      CellBinning  phiBinning(startPhi, endPhi, width*nPhi);
	      HECDetDescr *d = new HECDetDescr(hecDetectorManager,s,r,phiBinning);

	      HECDetectorRegion::DetectorSide side = (HECDetectorRegion::DetectorSide) endcap;
	      HECDetectorRegion *region = new HECDetectorRegion(vol->getPhysVol(),d,side,projectivityDisplacement);
	      hecDetectorManager->addDetectorRegion(region);
	    }
	  }
	  else
	    log << MSG::DEBUG << " No Stored PV for " << nameTag
		<< " in Detector Store" << endmsg;
	}
      }
    }
  }

  { // EMEC
    emecDetectorManager  = new EMECDetectorManager(&(hvManager->getEMECHVManager(EMECHVModule::INNER))
						   ,&(hvManager->getEMECHVManager(EMECHVModule::OUTER))
						   ,&(hvManager->getEMECPresamplerHVManager()));

    // Here is a table of min and max eta for different sampling layers, radial part (i/o) and region.

    for (int e=0;e<2;e++) {

      double startPhi =  e==0?  M_PI-2*M_PI/768/2 : -2*M_PI/768/2;
      double endPhi   =  e==0? -M_PI-2*M_PI/768/2 : 2*M_PI-2*M_PI/768/2 ;


      StoredPhysVol *sPhys;

      if(detStore->contains<StoredPhysVol>(e==0 ? "EMEC_OUTER_WHEEL_NEG" : "EMEC_OUTER_WHEEL_POS")) {
	if (detStore->retrieve(sPhys,e==0 ? "EMEC_OUTER_WHEEL_NEG" : "EMEC_OUTER_WHEEL_POS")==StatusCode::SUCCESS) {
	  GeoFullPhysVol *emecEnvelope=(GeoFullPhysVol *) sPhys->getPhysVol();

	  {
	    CellBinning phiBinning(startPhi,endPhi,64);
	    EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,1,0,0,phiBinning);
	    EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::DetectorSide(e),projectivityDisplacement);
	    emecDetectorManager->addDetectorRegion(region);
	  }

	  // Outer Wheel Sampling 1 Region 1:
	  {
	    CellBinning phiBinning(startPhi,endPhi,64);
	    EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,1,1,0,phiBinning);
	    EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::DetectorSide(e),projectivityDisplacement);
	    emecDetectorManager->addDetectorRegion(region);
	  }
	  // Outer Wheel Sampling 1 Region 2:
	  {
	    CellBinning phiBinning(startPhi,endPhi,64);
	    EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,1,2,0,phiBinning);
	    EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::DetectorSide(e),projectivityDisplacement);
	    emecDetectorManager->addDetectorRegion(region);
	  }
	  // Outer Wheel Sampling 1 Region 3:
	  {
	    CellBinning phiBinning(startPhi,endPhi,64);
	    EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,1,3,0,phiBinning);
	    EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::DetectorSide(e),projectivityDisplacement);
	    emecDetectorManager->addDetectorRegion(region);
	  }
	  // Outer Wheel Sampling 1 Region 4:
	  {
	    CellBinning phiBinning(startPhi,endPhi,64);
	    EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,1,4,0,phiBinning);
	    EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::DetectorSide(e),projectivityDisplacement);
	    emecDetectorManager->addDetectorRegion(region);
	  }
	  // Outer Wheel Sampling 1 Region 5:
	  {
	    CellBinning phiBinning(startPhi,endPhi,64);
	    EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,1,5,0,phiBinning);
	    EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::DetectorSide(e),projectivityDisplacement);
	    emecDetectorManager->addDetectorRegion(region);
	  }
	  // Outer Wheel Sampling 2 Region 0:
	  {
	    CellBinning phiBinning(startPhi,endPhi,256);
	    EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,2,0,0,phiBinning);
	    EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::DetectorSide(e),projectivityDisplacement);
	    emecDetectorManager->addDetectorRegion(region);
	  }
	  // Outer Wheel Sampling 2 Region 1:
	  {
	    CellBinning phiBinning(startPhi,endPhi,256);
	    EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,2,1,0,phiBinning);
	    EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::DetectorSide(e),projectivityDisplacement);
	    emecDetectorManager->addDetectorRegion(region);
	  }
	  // Outer Wheel Sampling 3 Region 0:
	  {
	    CellBinning phiBinning(startPhi,endPhi,256);
	    EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,3,0,0,phiBinning);
	    EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::DetectorSide(e),projectivityDisplacement);
	    emecDetectorManager->addDetectorRegion(region);
	  }
	}
      }
      else {
	log << MSG::DEBUG << " No Stored PV for " << (e==0 ? "EMEC_OUTER_WHEEL_NEG" : "EMEC_OUTER_WHEEL_POS")
	    << " in Detector Store" << endmsg;
      }

      if(detStore->contains<StoredPhysVol>(e==0 ? "EMEC_INNER_WHEEL_NEG" : "EMEC_INNER_WHEEL_POS")) {
	if (detStore->retrieve(sPhys,e==0 ? "EMEC_INNER_WHEEL_NEG" : "EMEC_INNER_WHEEL_POS")==StatusCode::SUCCESS) {

	  GeoFullPhysVol *emecEnvelope=(GeoFullPhysVol *) sPhys->getPhysVol();

	  // Inner Wheel Sampling 1 Region 0:
	  {
	    CellBinning phiBinning(startPhi,endPhi,64);
	    EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,1,0,1,phiBinning);
	    EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::DetectorSide(e),projectivityDisplacement);
	    emecDetectorManager->addDetectorRegion(region);
	  }
	  // Inner Wheel Sampling 2 Region 0:

	  {
	    CellBinning phiBinning(startPhi,endPhi,64);
	    EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,2,0,1,phiBinning);
	    EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::DetectorSide(e),projectivityDisplacement);
	    emecDetectorManager->addDetectorRegion(region);
	  }

	}
      }
      else {
      	log << MSG::DEBUG << " No Stored PV for " << (e==0 ? "EMEC_INNER_WHEEL_NEG" : "EMEC_INNER_WHEEL_POS")
	    << " in Detector Store" << endmsg;
      }

      StoredPhysVol *sPresamplerEnvelope;

      if(detStore->contains<StoredPhysVol>(e==0 ? "PRESAMPLER_EC_NEG":"PRESAMPLER_EC_POS" )) {
	if (StatusCode::SUCCESS==detStore->retrieve(sPresamplerEnvelope, e==0 ? "PRESAMPLER_EC_NEG":"PRESAMPLER_EC_POS" )) {
	  GeoFullPhysVol * PresamplerEnvelope=sPresamplerEnvelope->getPhysVol();
	  CellBinning presamplerPhiBinning(startPhi,endPhi,64);
	  EMECDetDescr *presamplerDetDescr = new EMECDetDescr(emecDetectorManager,0,0,0,presamplerPhiBinning);
	  EMECDetectorRegion *presamplerRegion = new EMECDetectorRegion(PresamplerEnvelope,presamplerDetDescr,EMECDetectorRegion::DetectorSide(e),projectivityDisplacement);
	  emecDetectorManager->addDetectorRegion(presamplerRegion);
	}
      }
      else {
	log << MSG::DEBUG << " No Stored PV for " << (e==0 ? "PRESAMPLER_EC_NEG":"PRESAMPLER_EC_POS")
	    << " in Detector Store" << endmsg;
      }
    }
  }

  { // EMB
    embDetectorManager  = new EMBDetectorManager(hvManager->getEMBHVManager(),hvManager->getEMBPresamplerHVManager());
    int firstEndcap= testbeam==0 ? 0:1;
    int endEndcap=2;

    for (int e= firstEndcap ;e<endEndcap;e++) {

      double startPhi =  e==0?  M_PI-2*M_PI/1024/2 : -2*M_PI/1024/2;
      double endPhi   =  e==0? -M_PI-2*M_PI/1024/2 : 2*M_PI-2*M_PI/1024/2 ;
      int    NDIV     = 1;
      if (testbeam==1) {
	startPhi=0*M_PI/180;
	endPhi  =22.5*M_PI/180;
	NDIV=16;
      }

      // Sampling layer 3 region 0:
      {
	StoredPhysVol *sPhys;
	if(detStore->contains<StoredPhysVol>(e==0 ? "EMB_NEG" : "EMB_POS")) {
	  if (detStore->retrieve(sPhys,e==0 ? "EMB_NEG" : "EMB_POS")==StatusCode::SUCCESS) {
	    CellBinning phiBinning(startPhi,endPhi,256/NDIV);
	    EMBDetDescr *detDescr = new EMBDetDescr(embDetectorManager,3,0,phiBinning);
	    GeoFullPhysVol *embEnvelope=(GeoFullPhysVol *) sPhys->getPhysVol();
	    EMBDetectorRegion *region = new EMBDetectorRegion(embEnvelope,detDescr,EMBDetectorRegion::DetectorSide(e));
	    embDetectorManager->addDetectorRegion(region);
	  }
	}
	else {
	    log << MSG::DEBUG << " No Stored PV for " << (e==0 ? "EMB_NEG" : "EMB_POS" )<< " in Detector Store" << endmsg;
	}
      }
      // Sampling layer 2 region 0:
      {
	StoredPhysVol *sPhys;
	if(detStore->contains<StoredPhysVol>(e==0 ? "EMB_NEG" : "EMB_POS")) {
	  if (detStore->retrieve(sPhys,e==0 ? "EMB_NEG" : "EMB_POS")==StatusCode::SUCCESS) {
	    CellBinning phiBinning(startPhi,endPhi,256/NDIV);
	    EMBDetDescr *detDescr = new EMBDetDescr(embDetectorManager,2,0,phiBinning);
	    GeoFullPhysVol *embEnvelope=(GeoFullPhysVol *) sPhys->getPhysVol();
	    EMBDetectorRegion *region = new EMBDetectorRegion(embEnvelope,detDescr,EMBDetectorRegion::DetectorSide(e));
	    embDetectorManager->addDetectorRegion(region);
	  }
	}
	else {
	  log << MSG::DEBUG << " No Stored PV for " << (e==0 ? "EMB_NEG" : "EMB_POS" )<< " in Detector Store" << endmsg;
	}
      }
      // Sampling layer 2 region 1:
      {
	StoredPhysVol *sPhys;
	if(detStore->contains<StoredPhysVol>(e==0 ? "EMB_NEG" : "EMB_POS")) {
	  if (detStore->retrieve(sPhys,e==0 ? "EMB_NEG" : "EMB_POS")==StatusCode::SUCCESS) {
	    CellBinning phiBinning(startPhi,endPhi,256/NDIV);
	    EMBDetDescr *detDescr = new EMBDetDescr(embDetectorManager,2,1,phiBinning);
	    GeoFullPhysVol *embEnvelope=(GeoFullPhysVol *) sPhys->getPhysVol();
	    EMBDetectorRegion *region = new EMBDetectorRegion(embEnvelope,detDescr,EMBDetectorRegion::DetectorSide(e));
	    embDetectorManager->addDetectorRegion(region);
	  }
	}
	else {
	  log << MSG::DEBUG << " No Stored PV for " << (e==0 ? "EMB_NEG" : "EMB_POS") << " in Detector Store" << endmsg;
	}
      }
      // Sampling layer 1 region 0:
      {
	StoredPhysVol *sPhys;
	if(detStore->contains<StoredPhysVol>(e==0 ? "EMB_NEG" : "EMB_POS")) {
	  if (detStore->retrieve(sPhys,e==0 ? "EMB_NEG" : "EMB_POS")==StatusCode::SUCCESS) {
	    CellBinning phiBinning(startPhi,endPhi,64/NDIV);
	    EMBDetDescr *detDescr = new EMBDetDescr(embDetectorManager,1,0,phiBinning);
	    GeoFullPhysVol *embEnvelope=(GeoFullPhysVol *) sPhys->getPhysVol();
	    EMBDetectorRegion *region = new EMBDetectorRegion(embEnvelope,detDescr,EMBDetectorRegion::DetectorSide(e));
	    embDetectorManager->addDetectorRegion(region);
	  }
	}
	else {
	  log << MSG::DEBUG << " No Stored PV for " << (e==0 ? "EMB_NEG" : "EMB_POS") << " in Detector Store" << endmsg;
	}
      }
      // Sampling layer 1 region 0:
      {
	StoredPhysVol *sPhys;
	if(detStore->contains<StoredPhysVol>(e==0 ? "EMB_NEG" : "EMB_POS")) {
	  if (detStore->retrieve(sPhys,e==0 ? "EMB_NEG" : "EMB_POS")==StatusCode::SUCCESS) {
	    CellBinning phiBinning(startPhi,endPhi,256/NDIV);
	    EMBDetDescr *detDescr = new EMBDetDescr(embDetectorManager,1,1,phiBinning);
	    GeoFullPhysVol *embEnvelope=(GeoFullPhysVol *) sPhys->getPhysVol();
	    EMBDetectorRegion *region = new EMBDetectorRegion(embEnvelope,detDescr,EMBDetectorRegion::DetectorSide(e));
	    embDetectorManager->addDetectorRegion(region);
	  }
	}
	else {
	  log << MSG::DEBUG << " No Stored PV for " << (e==0 ? "EMB_NEG" : "EMB_POS") << " in Detector Store" << endmsg;
	}
      }
      // Sampling layer 0 region 0:
      {
	StoredPhysVol *sPhys;
	if(detStore->contains<StoredPhysVol>(e==0 ? "EMB_NEG" : "EMB_POS")) {
	  if (detStore->retrieve(sPhys,e==0 ? "EMB_NEG" : "EMB_POS")==StatusCode::SUCCESS) {
	    CellBinning phiBinning(startPhi,endPhi,64/NDIV);
	    EMBDetDescr *detDescr = new EMBDetDescr(embDetectorManager,0,0,phiBinning);
	    GeoFullPhysVol *embEnvelope=(GeoFullPhysVol *) sPhys->getPhysVol();
	    EMBDetectorRegion *region = new EMBDetectorRegion(embEnvelope,detDescr,EMBDetectorRegion::DetectorSide(e));
	    embDetectorManager->addDetectorRegion(region);
	  }
	}
	else {
	  log << MSG::DEBUG << " No Stored PV for " << (e==0 ? "EMB_NEG" : "EMB_POS") << " in Detector Store" << endmsg;
	}
      }
    }
  }

  bool allOk{true};
  StatusCode sc = detStore->record(embDetectorManager,  embDetectorManager->getName());
  if(sc.isFailure()) {
    log << MSG::ERROR << "Unable to record embDetectorManager" << endmsg;
    allOk = false;
  }

  sc = detStore->record(emecDetectorManager, emecDetectorManager->getName());
  if(sc.isFailure()) {
    log << MSG::ERROR << "Unable to record emecDetectorManager" << endmsg;
    allOk = false;
  }

  sc = detStore->record(hecDetectorManager,  hecDetectorManager->getName());
  if(sc.isFailure()) {
    log << MSG::ERROR << "Unable to record hecDetectorManager" << endmsg;
    allOk = false;
  }

  sc = detStore->record(fcalDetectorManager, fcalDetectorManager->getName());
  if(sc.isFailure()) {
    log << MSG::ERROR << "Unable to record fcalDetectorManager" << endmsg;
    allOk = false;
  }

  if(!allOk) {
    delete embDetectorManager;
    delete emecDetectorManager;
    delete hecDetectorManager;
    delete fcalDetectorManager;

    embDetectorManager=nullptr;
    emecDetectorManager=nullptr;
    hecDetectorManager=nullptr;
    fcalDetectorManager=nullptr;
  }

  return std::make_tuple(embDetectorManager,emecDetectorManager,hecDetectorManager,fcalDetectorManager);
}
