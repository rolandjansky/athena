/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4FastSimulation/FCALFastSimDedicatedSD.h"
#include "LArSimEvent/LArHit.h"
#include "LArSimEvent/LArHitContainer.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/StoreGate.h"
#include "LArReadoutGeometry/FCALDetectorManager.h"
#include "LArReadoutGeometry/FCALModule.h"
#include "LArG4Code/EnergySpot.h"
#include "LArG4SD/LArG4SD.h"
#include "LArSimEvent/LArHitContainer.h"
#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/LArVHitMerger.h"
#include "GeoModelKernel/GeoTubs.h"


using HepGeom::Transform3D;
using HepGeom::Point3D;


class FCALFastSimDedicatedSD::Clockwork {

public:

  const FCALDetectorManager * fcalManager;
};



// Constructor:
FCALFastSimDedicatedSD::FCALFastSimDedicatedSD():
  IFastSimDedicatedSD("FCALFastSimDedicated"), c(new Clockwork())
{

  c->fcalManager=NULL;
}

// Destructor:
FCALFastSimDedicatedSD::~FCALFastSimDedicatedSD() {
  delete c;
}


// ProcessHitsMethod
void FCALFastSimDedicatedSD::ProcessSpot(const EnergySpot  & spot){
 if (!c->fcalManager) {
   StoreGateSvc *detStore = StoreGate::pointer("DetectorStore");
     if (detStore->retrieve(c->fcalManager)!= StatusCode::SUCCESS) {
      throw std::runtime_error("Error, cannot get a FCAL Manager in FCALFastSimDedicatedSD");
    }
  }

  if (!c->fcalManager) {
    throw std::runtime_error("Error, cannot get a FCAL Manager in FCALFastSimDedicatedSD");
  }

  // Fill the identifier.


  
  static Transform3D xfNeg[3] = {
    c->fcalManager->getFCAL(FCALModule::Module(1),FCALModule::Endcap(0))->getAbsoluteTransform().inverse(),
    c->fcalManager->getFCAL(FCALModule::Module(2),FCALModule::Endcap(0))->getAbsoluteTransform().inverse(),
    c->fcalManager->getFCAL(FCALModule::Module(3),FCALModule::Endcap(0))->getAbsoluteTransform().inverse()};

  static Transform3D xfPos[3] = {
    c->fcalManager->getFCAL(FCALModule::Module(1),FCALModule::Endcap(1))->getAbsoluteTransform().inverse(),
    c->fcalManager->getFCAL(FCALModule::Module(2),FCALModule::Endcap(1))->getAbsoluteTransform().inverse(),
    c->fcalManager->getFCAL(FCALModule::Module(3),FCALModule::Endcap(1))->getAbsoluteTransform().inverse()};
	
  static const GeoTubs * fcalTubs[3] = {
    (const GeoTubs *) c->fcalManager->getFCAL(FCALModule::Module(1),FCALModule::Endcap(0))->getMaterialGeom()->getLogVol()->getShape(),
    (const GeoTubs *) c->fcalManager->getFCAL(FCALModule::Module(2),FCALModule::Endcap(0))->getMaterialGeom()->getLogVol()->getShape(),
    (const GeoTubs *) c->fcalManager->getFCAL(FCALModule::Module(3),FCALModule::Endcap(0))->getMaterialGeom()->getLogVol()->getShape()};


  // Fill the identifier.
  Point3D<double> globalPosition=spot.GetPosition();
  
  // Figure out if it is in the positive endcap or the netative endcap:
  bool iAmPositive=globalPosition.z()>0.;
  int  module=1;
  Point3D<double> localPosition= iAmPositive ? (xfPos[0]*globalPosition) : (xfNeg[0]*globalPosition);
  if (fabs(localPosition.z()) > fcalTubs[0]->getZHalfLength()) {
    module=2;
    localPosition= iAmPositive ? (xfPos[1]*globalPosition) : (xfNeg[1]*globalPosition);
    if (fabs(localPosition.z()) > fcalTubs[1]->getZHalfLength()) {
      module=3;
      localPosition= iAmPositive ? (xfPos[2]*globalPosition) : (xfNeg[2]*globalPosition);
      if (fabs(localPosition.z()) > fcalTubs[2]->getZHalfLength()) {
	module=0;
      }
    }
  }

  if (!module) {
//    std::cout << "Warning in FCALFastSimDedicatedSD: Cannot locate module" << std::endl;
    return;
  }

  const FCAL_ChannelMap *cmap = c->fcalManager->getChannelMap();
  int sampling=module;
  int etaIndex=0, phiIndex=0;
  bool ok = cmap->getTileID(sampling,
			    localPosition.x(),
			    localPosition.y(),
			    etaIndex,
			    phiIndex);
 
       
  // zSide is negative if z<0.
  int zSide = iAmPositive ? 2:-2;
  static LArG4Identifier id;
  id.clear();
  if (ok){
    id << 4          // LArCalorimeter
       << 3          // LArFCAL
       << zSide      // EndCap
       << sampling   // FCal Module # 
       << etaIndex
       << phiIndex;  
  } else {
    return;
  }

  // call process with dummy first argument
  m_hitMerger->process(0,id, spot.GetTime(), spot.GetEnergy());
  return;
 
  
}


