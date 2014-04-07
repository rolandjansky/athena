/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class SimHitHandle_TRTHit               //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1TrackSystems/SimHitHandle_TRTHit.h"
#include "VP1Utils/VP1DetInfo.h"
#include "VP1Utils/VP1ParticleData.h"
#include "VP1Base/VP1Msg.h"

#include "InDetSimEvent/TRTUncompressedHit.h"
#include "InDetReadoutGeometry/TRT_BaseElement.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/Surface.h"
#include "TrkParameters/TrackParameters.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"

#include <bitset>

//____________________________________________________________________
class SimHitHandle_TRTHit::Imp {
public:
  Imp(const TRTUncompressedHit*h) : thehit(h), detelem(0),strawID(0) {}
  const TRTUncompressedHit* thehit;
  mutable const InDetDD::TRT_BaseElement * detelem;
  mutable size_t strawID;
  bool ensureDetElemInit() const;
};


//____________________________________________________________________
SimHitHandle_TRTHit::SimHitHandle_TRTHit(const TRTUncompressedHit* h)
  : SimHitHandleBase(), d(new Imp(h))
{
  if (!h)
    VP1Msg::message("SimHitHandle_TRTHit constructor ERROR: Received null hit pointer");
}

//____________________________________________________________________
SimHitHandle_TRTHit::~SimHitHandle_TRTHit()
{
  delete d;
}


//____________________________________________________________________
//Trk::GlobalMomentum SimHitHandle_TRTHit::momentumDirection() const
Amg::Vector3D SimHitHandle_TRTHit::momentumDirection() const
{
  if (VP1Msg::verbose()&&posStart()==posEnd())
    VP1Msg::messageVerbose("SimHitHandle_SiHit::momentumDirection() ERROR: posStart()==posEnd()");
  Amg::Vector3D u((posEnd()-posStart()).unit());
//  return Trk::GlobalMomentum(u.x(),u.y(),u.z());;
  return Amg::Vector3D(u.x(),u.y(),u.z());;
}

//____________________________________________________________________
double SimHitHandle_TRTHit::actualMomentum() const
{
  bool ok;
  double mass = VP1ParticleData::particleMass(d->thehit->GetParticleEncoding(),ok);
  if (!ok) {
    VP1Msg::message("SimHitHandle_TRTHit constructor WARNING: Could not determine mass for pdg code "
		    +QString::number(pdg())+". Assuming charged pion mass.");
    mass = 139.57019*CLHEP::MeV;//Charged pion mass
  }
  const double ekin = d->thehit->GetKineticEnergy()*CLHEP::MeV;
  const double gamma = 1.0 + ekin/mass;
  double betasq = 1.0-1.0/(gamma*gamma);
  if (betasq<=0.0)
    return 0.0;
  return sqrt(betasq)*(mass+ekin);
}

//____________________________________________________________________
bool SimHitHandle_TRTHit::Imp::ensureDetElemInit() const
{
  if (detelem)
    return true;
  const InDetDD::TRT_DetectorManager * mgr = VP1DetInfo::trtDetMgr();
  if (!mgr) {
    VP1Msg::messageDebug("SimHitHandle_TRTHit ERROR: Could not get TRT detector manager.");
    return false;
  }
  size_t hitID = thehit->GetHitID(), moduleID(0), trtID(0),sectorID(0);
  static const size_t mask  = 0x1F;
  static const size_t shift = 5;
  std::bitset<32> idBits(hitID);
  if (!idBits[21]/*IE: BARREL*/ ) {
    size_t layerID(0), ringID(0);
    strawID = hitID & mask;
    hitID >>= shift;
    layerID = hitID & mask;
    hitID >>= shift;
    moduleID = hitID & mask;
    hitID >>= shift;
    ringID = hitID & mask;
    trtID = hitID >> shift;
    detelem = mgr->getBarrelElement(trtID,ringID,moduleID,layerID);//fixme: handle case
  }
  else /*ENDCAP*/ {
    size_t wheelID(0), planeID(0);
    strawID = hitID & mask;
    hitID >>= shift;
    planeID = hitID & mask;
    hitID >>= shift;
    sectorID = hitID & mask;
    hitID >>= shift;
    wheelID = hitID & mask;
    trtID = hitID >> shift;
    bool isPositive = (trtID==2);
    detelem = mgr->getEndcapElement(isPositive,wheelID,planeID,sectorID);
  }
  if (detelem) {
    return true;
  } else {
    VP1Msg::messageDebug("SimHitHandle_TRTHit WARNING: Could not find detector element.");
    return false;
  }
}

//____________________________________________________________________
Amg::Vector3D SimHitHandle_TRTHit::posStart() const
{
  if (!d->ensureDetElemInit())
    return Amg::Vector3D(0,0,0);
  return Amg::CLHEPTransformToEigen(d->detelem->getAbsoluteTransform(d->strawID)) * Amg::Vector3D(d->thehit->GetPreStepX(),d->thehit->GetPreStepY(),d->thehit->GetPreStepZ());
}

//____________________________________________________________________
Amg::Vector3D SimHitHandle_TRTHit::posEnd() const
{
  if (!d->ensureDetElemInit())
    return Amg::Vector3D(0,0,0);
  return Amg::CLHEPTransformToEigen(d->detelem->getAbsoluteTransform(d->strawID)) * Amg::Vector3D(d->thehit->GetPostStepX(),d->thehit->GetPostStepY(),d->thehit->GetPostStepZ());
}

//____________________________________________________________________
double SimHitHandle_TRTHit::hitTime() const
{
  return d->thehit->GetGlobalTime();
}

//____________________________________________________________________
const HepMcParticleLink& SimHitHandle_TRTHit::particleLink() const
{
  return d->thehit->particleLink();
}

//____________________________________________________________________
int SimHitHandle_TRTHit::actualPDGCodeFromSimHit() const
{
  return d->thehit->GetParticleEncoding();
}

//____________________________________________________________________
Trk::TrackParameters * SimHitHandle_TRTHit::createTrackParameters() const
{
  if (!d->ensureDetElemInit())
    return 0;
  const Trk::StraightLineSurface * surf
    = dynamic_cast<const Trk::StraightLineSurface *>( &(d->detelem->strawSurface(d->strawID)));
  if (!surf) {
    VP1Msg::message("SimHitHandle_TRTHit::createTrackParameters ERROR: could not get Trk::StraightLineSurface");
    return 0;
  }

  double c;
  if ( !hasCharge() ) {
    bool ok;
    c = VP1ParticleData::particleCharge(d->thehit->GetParticleEncoding(),ok);
    if (!ok) {
      VP1Msg::message("SimHitHandle_TRTHit::createTrackParameters ERROR: Could not find particle charge (pdg="
		      +QString::number(d->thehit->GetParticleEncoding())+"). Assuming charge=+1.");
      c = +1.0;
    } else {
      if (VP1Msg::verbose())
	VP1Msg::messageVerbose("Looked up particle charge for trt simhit with pdg code "+VP1Msg::str(d->thehit->GetParticleEncoding())+": "+VP1Msg::str(c));
    }
    const_cast<SimHitHandle_TRTHit*>(this)->setCharge(c);
  } else {
    c = charge();
  }
  return new Trk::AtaStraightLine(0.5*(posStart()+posEnd()),momentum()*momentumDirection(),c,*surf);
  //Fixme: Use constructor with local coords instead?:
}
