/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class SimHitHandle_ForwardHit           //
//                                                            //
//  Author: Thomas Keck (t.keck@cern.ch)                      //
//  Initial version: July 2013                                //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1TrackSystems/SimHitHandle_ForwardHit.h"
#include "VP1Base/VP1Msg.h"
#include "VP1Utils/VP1DetInfo.h"
#include "VP1Utils/VP1ParticleData.h"
#include "VP1TrackSystems/VP1TrackSanity.h"

#include "ForwardRegion_SimEv/SimulationHit.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
// #include "TrkSurfaces/PlaneSurface.h"
// #include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/Surface.h"

#include "TrkParameters/TrackParameters.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"


//____________________________________________________________________
class SimHitHandle_ForwardHit::Imp {
public:
  Imp( const SimulationHit * h ) : thehit(h){}
  const SimulationHit * thehit;
};


//____________________________________________________________________
SimHitHandle_ForwardHit::SimHitHandle_ForwardHit(const SimulationHit * h)
  : SimHitHandleBase(), m_d(new Imp(h))
{
  if (!h)
    VP1Msg::message("SimHitHandle_ForwardHit constructor ERROR: Received null hit pointer");

  m_link =  HepMcParticleLink(m_d->thehit->trackID());
}

//____________________________________________________________________
SimHitHandle_ForwardHit::~SimHitHandle_ForwardHit()
{
  delete m_d;
}

//Trk::GlobalMomentum SimHitHandle_ForwardHit::momentumDirection() const
Amg::Vector3D SimHitHandle_ForwardHit::momentumDirection() const
{
  return Amg::Hep3VectorToEigen(m_d->thehit->pre().direction);
}

double SimHitHandle_ForwardHit::actualMomentum() const
{
  return m_d->thehit->pre().momentum.mag();
}
//____________________________________________________________________
Amg::Vector3D SimHitHandle_ForwardHit::posStart() const
{
  return Amg::Hep3VectorToEigen(m_d->thehit->pre().position);
}

//____________________________________________________________________
Amg::Vector3D SimHitHandle_ForwardHit::posEnd() const
{
  return Amg::Hep3VectorToEigen(m_d->thehit->post().position);
}

//____________________________________________________________________
double SimHitHandle_ForwardHit::hitTime() const
{
  return m_d->thehit->pre().time;
}

//____________________________________________________________________
const HepMcParticleLink& SimHitHandle_ForwardHit::particleLink() const
{
   return m_link;
}

//____________________________________________________________________
int SimHitHandle_ForwardHit::actualPDGCodeFromSimHit() const
{
  return m_d->thehit->particleEncoding();
}

//____________________________________________________________________
Trk::TrackParameters * SimHitHandle_ForwardHit::createTrackParameters() const
{

	//  const Trk::StraightLineSurface * surf = new Trk::StraightLineSurface(new HepGeom::Translate3D(0,0,0));
	Amg::Transform3D transf = Amg::CLHEPTranslate3DToEigen( HepGeom::Translate3D(0,0,0) );
    const Trk::StraightLineSurface * surf = new Trk::StraightLineSurface( &transf );


  if (!surf) {
    VP1Msg::message("SimHitHandle_ForwardHit::createTrackParameters ERROR: could not get Trk::StraightLineSurface");
    return 0;
  }

  double c;
  if ( !hasCharge() ) {
    bool ok;
    c = VP1ParticleData::particleCharge(m_d->thehit->particleEncoding(),ok);
    if (!ok) {
      VP1Msg::message("SimHitHandle_ForwardHit::createTrackParameters ERROR: Could not find particle charge (pdg="
		      +QString::number(m_d->thehit->particleEncoding())+"). Assuming charge=+1.");
      c = +1.0;
    } else {
      if (VP1Msg::verbose())
	VP1Msg::messageVerbose("Looked up particle charge for trt simhit with pdg code "+VP1Msg::str(m_d->thehit->particleEncoding())+": "+VP1Msg::str(c));
    }
    const_cast<SimHitHandle_ForwardHit*>(this)->setCharge(c);
  } else {
    c = charge();
  }
  return new Trk::AtaStraightLine(posStart(),momentumDirection(),c,*surf);
  //Fixme: Use constructor with local coords instead?:
}
