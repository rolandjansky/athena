/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FCALFastSimDedicatedSD.h"
#include "LArSimEvent/LArHit.h"
#include "LArSimEvent/LArHitContainer.h"
#include "LArReadoutGeometry/FCALDetectorManager.h"
#include "LArReadoutGeometry/FCALModule.h"
#include "LArG4Code/EnergySpot.h"
#include "GeoModelKernel/GeoTubs.h"
#include "StoreGate/StoreGateSvc.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Geometry/Point3D.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"

using HepGeom::Transform3D;
using HepGeom::Point3D;

// Constructor:
FCALFastSimDedicatedSD::FCALFastSimDedicatedSD(StoreGateSvc* detStore)
  : IFastSimDedicatedSD("FCALFastSimDedicatedSD", detStore) 
  , m_fcalManager(nullptr)
{
  G4cout << GetName() << "::initialize()" << G4endl;
  if ( detStore->retrieve(m_fcalManager).isFailure() ){
    throw std::runtime_error("Could not retrieve FCAL manager");
  }
}

// ProcessHitsMethod
void FCALFastSimDedicatedSD::ProcessSpot(const EnergySpot  & spot){
  // Fill the identifier.

  static Transform3D xfNeg[3] = {
    Amg::EigenTransformToCLHEP(m_fcalManager->getFCAL(FCALModule::Module(1),FCALModule::Endcap(0))->getAbsoluteTransform().inverse()),
    Amg::EigenTransformToCLHEP(m_fcalManager->getFCAL(FCALModule::Module(2),FCALModule::Endcap(0))->getAbsoluteTransform().inverse()),
    Amg::EigenTransformToCLHEP(m_fcalManager->getFCAL(FCALModule::Module(3),FCALModule::Endcap(0))->getAbsoluteTransform().inverse())};

  static Transform3D xfPos[3] = {
    Amg::EigenTransformToCLHEP(m_fcalManager->getFCAL(FCALModule::Module(1),FCALModule::Endcap(1))->getAbsoluteTransform().inverse()),
    Amg::EigenTransformToCLHEP(m_fcalManager->getFCAL(FCALModule::Module(2),FCALModule::Endcap(1))->getAbsoluteTransform().inverse()),
    Amg::EigenTransformToCLHEP(m_fcalManager->getFCAL(FCALModule::Module(3),FCALModule::Endcap(1))->getAbsoluteTransform().inverse())};

  static const GeoTubs * fcalTubs[3] = {
    (const GeoTubs *) m_fcalManager->getFCAL(FCALModule::Module(1),FCALModule::Endcap(0))->getMaterialGeom()->getLogVol()->getShape(),
    (const GeoTubs *) m_fcalManager->getFCAL(FCALModule::Module(2),FCALModule::Endcap(0))->getMaterialGeom()->getLogVol()->getShape(),
    (const GeoTubs *) m_fcalManager->getFCAL(FCALModule::Module(3),FCALModule::Endcap(0))->getMaterialGeom()->getLogVol()->getShape()};


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

  const FCAL_ChannelMap *cmap = m_fcalManager->getChannelMap();
  int sampling=module;
  int etaIndex=0, phiIndex=0;
  bool ok = cmap->getTileID(sampling,
                            localPosition.x(),
                            localPosition.y(),
                            etaIndex,
                            phiIndex);


  // zSide is negative if z<0.
  int zSide = iAmPositive ? 2:-2;
  //static LArG4Identifier id;
  m_larID.clear();
  if (ok){
    m_larID << 4          // LArCalorimeter
            << 3          // LArFCAL
            << zSide      // EndCap
            << sampling   // FCal Module #
            << etaIndex
            << phiIndex;
  } else {
    return;
  }

  // call process to add this to the collection 
  SimpleHit(m_larID, spot.GetTime(), spot.GetEnergy());
  return;
}
