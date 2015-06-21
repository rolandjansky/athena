/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FCALFastSimDedicatedSD.h"
#include "LArSimEvent/LArHit.h"
#include "LArSimEvent/LArHitContainer.h"
#include "LArReadoutGeometry/FCALDetectorManager.h"
#include "LArReadoutGeometry/FCALModule.h"
#include "LArG4Code/EnergySpot.h"
#include "LArG4SD/LArG4SD.h"
#include "LArSimEvent/LArHitContainer.h"
#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/LArVHitMerger.h"
#include "LArG4Code/LArVHitMergerFactory.h"
#include "GeoModelKernel/GeoTubs.h"

using HepGeom::Transform3D;
using HepGeom::Point3D;

// Constructor:
FCALFastSimDedicatedSD::FCALFastSimDedicatedSD(const std::string& type, const std::string& name, const IInterface *parent)
  : SensitiveDetectorBase(type,name,parent)
  , m_hitMerger(0)
  , m_fcalManager(nullptr)
{
  m_noVolumes=true;
  declareInterface<IFastSimDedicatedSD>(this);
}

StatusCode FCALFastSimDedicatedSD::initialize()
{
  ATH_MSG_VERBOSE( name() << "::initialize()");
  CHECK( detStore()->retrieve(m_fcalManager) );
  return SensitiveDetectorBase::initialize();
}

StatusCode FCALFastSimDedicatedSD::retrieveLArHitMerger()
{
  // At the beginning of first event initialize m_hitMerger pointer
  // 1. Get Hit Merger factory from DS
  // 2. Query Hit Merger factory for Hit Merger pointer
  const DataHandle<LArVHitMergerFactory> _factory;
  CHECK( detStore()->retrieve(_factory,"LArHitMergerFactory") );

  m_hitMerger = _factory->getHitMerger();

  if(m_hitMerger==0){
    ATH_MSG_ERROR("0 pointer to the Hit Merger");
    return StatusCode::FAILURE;
  }
  m_hitMerger->BeginOfEvent(); //FIXME lazy init hack
  return StatusCode::SUCCESS;
}

// End each event (do hit merger here)
StatusCode FCALFastSimDedicatedSD::EndOfAthenaEvent() {
  if(m_hitMerger) m_hitMerger->EndOfEvent();
  return StatusCode::SUCCESS;
}

StatusCode
FCALFastSimDedicatedSD::queryInterface(const InterfaceID& riid, void** ppvIf) {
  if ( riid == IFastSimDedicatedSD::interfaceID() ) {
    *ppvIf = (IFastSimDedicatedSD*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return SensitiveDetectorBase::queryInterface( riid, ppvIf );
}

// ProcessHitsMethod
void FCALFastSimDedicatedSD::ProcessSpot(const EnergySpot  & spot){
  // Fill the identifier.

  static Transform3D xfNeg[3] = {
    m_fcalManager->getFCAL(FCALModule::Module(1),FCALModule::Endcap(0))->getAbsoluteTransform().inverse(),
    m_fcalManager->getFCAL(FCALModule::Module(2),FCALModule::Endcap(0))->getAbsoluteTransform().inverse(),
    m_fcalManager->getFCAL(FCALModule::Module(3),FCALModule::Endcap(0))->getAbsoluteTransform().inverse()};

  static Transform3D xfPos[3] = {
    m_fcalManager->getFCAL(FCALModule::Module(1),FCALModule::Endcap(1))->getAbsoluteTransform().inverse(),
    m_fcalManager->getFCAL(FCALModule::Module(2),FCALModule::Endcap(1))->getAbsoluteTransform().inverse(),
    m_fcalManager->getFCAL(FCALModule::Module(3),FCALModule::Endcap(1))->getAbsoluteTransform().inverse()};

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

  //FIXME temporary hack for lazy initialization for LArHitMerger
  if (!m_hitMerger && retrieveLArHitMerger().isFailure())
    {
      ATH_MSG_FATAL("Could not retrieve the LArHitMerger! Will crash now.");
      throw;
    }
  // call process with dummy first argument
  m_hitMerger->process(0,id, spot.GetTime(), spot.GetEnergy());
  return;


}
