/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class SimHitHandle_SiHit                //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1TrackSystems/SimHitHandle_SiHit.h"
#include "VP1Base/VP1Msg.h"
#include "VP1Utils/VP1DetInfo.h"
#include "VP1Utils/VP1ParticleData.h"

#include "InDetSimEvent/SiHit.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "VP1TrackSystems/VP1TrackSanity.h"

//____________________________________________________________________
class SimHitHandle_SiHit::Imp {
public:
  Imp( const SiHit * h ) : thehit(h),detelem(0) {}
  const SiHit * thehit;
  mutable const InDetDD::SiDetectorElement * detelem;
  bool ensureDetElemInit() const;
  Amg::Vector3D localToGlobal(const HepGeom::Point3D<double>&) const;
};


//____________________________________________________________________
SimHitHandle_SiHit::SimHitHandle_SiHit(const SiHit * h)
  : SimHitHandleBase(), m_d(new Imp(h))
{
  if (!h)
    VP1Msg::message("SimHitHandle_SiHit constructor ERROR: Received null hit pointer");
}

//____________________________________________________________________
SimHitHandle_SiHit::~SimHitHandle_SiHit()
{
  delete m_d;
}

//____________________________________________________________________
bool SimHitHandle_SiHit::Imp::ensureDetElemInit() const
{
  if (detelem)
    return true;
  int Barrel = thehit->getBarrelEndcap();
  //Not needed anymore and spoils stuff for slhc studies:  if (Barrel== 1) Barrel = -2;//Found this hack in GeoSiHit.

  if (thehit->isPixel()) {
    Identifier id = VP1DetInfo::pixelIDHelper()->wafer_id( Barrel,
							   thehit->getLayerDisk(),
							   thehit->getPhiModule(),
							   thehit->getEtaModule() );
    //fixme: id is_valid ?
    detelem = VP1DetInfo::pixelDetMgr()->getDetectorElement(id);
  } else {
    Identifier id = VP1DetInfo::sctIDHelper()->wafer_id( Barrel,
							 thehit->getLayerDisk(),
							 thehit->getPhiModule(),
							 thehit->getEtaModule(),
							 thehit->getSide() );
    //fixme: id is_valid ?
    detelem = VP1DetInfo::sctDetMgr()->getDetectorElement(id);
  }
  //Fixme : Handle case where detelem can not be found gracefully. And check pointers from VP1DetInfo!!
  if (!detelem)
    VP1Msg::messageDebug("SimHitHandle_SiHit ERROR: Could not get detector element for hit!");
  return detelem!=0;
}

//____________________________________________________________________
//Trk::GlobalMomentum SimHitHandle_SiHit::momentumDirection() const
Amg::Vector3D SimHitHandle_SiHit::momentumDirection() const
{
  if (VP1Msg::verbose()&&m_d->thehit->localEndPosition()==m_d->thehit->localStartPosition())
    VP1Msg::messageVerbose("SimHitHandle_SiHit::momentumDirection() ERROR: posStart()==posEnd()");
  return (posEnd()-posStart()).unit();
}

//____________________________________________________________________
Amg::Vector3D SimHitHandle_SiHit::Imp::localToGlobal( const HepGeom::Point3D<double>& local ) const
{
  if (!ensureDetElemInit())
    return Amg::Vector3D(0,0,0);
  return Amg::Vector3D(detelem->transformHit() * local);
}

//____________________________________________________________________
Amg::Vector3D SimHitHandle_SiHit::posStart() const
{
  return m_d->localToGlobal(m_d->thehit->localStartPosition());
}

//____________________________________________________________________
Amg::Vector3D SimHitHandle_SiHit::posEnd() const
{
  return m_d->localToGlobal(m_d->thehit->localEndPosition());
}

//____________________________________________________________________
double SimHitHandle_SiHit::hitTime() const
{
  return m_d->thehit->meanTime();
}

//____________________________________________________________________
const HepMcParticleLink& SimHitHandle_SiHit::particleLink() const
{
  return m_d->thehit->particleLink();
}

//____________________________________________________________________
Trk::TrackParameters * SimHitHandle_SiHit::createTrackParameters() const
{
  //Find charge and magnitude of momentum:
  double c;
  if ( !hasCharge() ) {
    bool ok;
    c = VP1ParticleData::particleCharge(pdg(),ok);
    if (!ok) {
      VP1Msg::message("SimHitHandle_SiHit::createTrackParameters ERROR: Could not find particle charge (pdg="
		      +QString::number(pdg())+"). Assuming charge=+1.");
      c = +1.0;
    } else {
      if (VP1Msg::verbose())
	VP1Msg::messageVerbose("Looked up particle charge for silicon simhit with pdg code "+VP1Msg::str(pdg())+": "+VP1Msg::str(c));
    }
    const_cast<SimHitHandle_SiHit*>(this)->setCharge(c);
  } else {
    c = charge();
  }

  double mom = momentum();
  if (mom<=0) {
    VP1Msg::message("SimHitHandle_SiHit::createTrackParameters ERROR: Unknown momentum. Using 1 GeV");
    mom = 1*CLHEP::GeV;
  }

  ////We could in principle get a surface like this:
  //   if (!m_d->ensureDetElemInit()) {
  //     VP1Msg::messageDebug("SimHitHandle_SiHit WARNING: Could not get detector element!");
  //     return 0;
  //   }
  //   const Trk::PlaneSurface * surf
  //     = dynamic_cast<const Trk::PlaneSurface *>( &(m_d->detelem->surface()));
  //   if (!surf) {
  //     VP1Msg::message("SimHitHandle_SiHit::createTrackParameters ERROR: could not get Trk::PlaneSurface");
  //     return 0;
  //   }
  ////And then proceed to construct a new AtaPlane parameter with that
  ////surface. However, that gives some problems, so instead we simply
  ////create a perigee:

  const Amg::Vector3D globpos = posStart();

//  const Trk::GlobalMomentum u(momentumDirection());
  const Amg::Vector3D u(momentumDirection());

  return new Trk::Perigee(0, 0, u.phi(), u.theta(), c/mom, globpos);
}
