/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class SimHitHandle_TrackRecord          //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2008                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1TrackSystems/SimHitHandle_TrackRecord.h"
#include "VP1Utils/VP1ParticleData.h"
#include "VP1Base/VP1Msg.h"

#include "GeneratorObjects/HepMcParticleLink.h"
#include "TrkSurfaces/PlaneSurface.h"

#include "TrkParameters/TrackParameters.h"

//____________________________________________________________________
SimHitHandle_TrackRecord::SimHitHandle_TrackRecord(const TrackRecord* tr)
  : SimHitHandleBase(),
    m_trkrecord(const_cast<TrackRecord*>(tr)),
    m_mom(m_trkrecord->GetMomentum().mag()),
    m_momdir( Amg::Hep3VectorToEigen( m_trkrecord->GetMomentum().unit() ) ),
    m_link( new HepMcParticleLink(m_trkrecord->GetBarCode()))//NB: We assume an event index of 0 here!!!
{
}

//____________________________________________________________________
SimHitHandle_TrackRecord::~SimHitHandle_TrackRecord()
{
  delete m_link;
}

//____________________________________________________________________
Trk::TrackParameters * SimHitHandle_TrackRecord::createTrackParameters() const
{
	Amg::Transform3D transf =  Amg::CLHEPTranslate3DToEigen(HepGeom::Translate3D(m_trkrecord->GetPosition().x(),m_trkrecord->GetPosition().y(),m_trkrecord->GetPosition().z()));
  //Surface:
  const Trk::PlaneSurface * surf
    = new Trk::PlaneSurface( &transf );
  //Fixme: surface and transform ever DELETED??

  //Fixme: Don't do this in every simhit!!:
  bool hasCharge;
  double charge;
  charge = VP1ParticleData::particleCharge(pdg(),hasCharge);
  if (!hasCharge) {
    VP1Msg::message("SimHitHandle_TrackRecord::createTrackParameters ERROR: Could not find particle charge (pdg="
		      +QString::number(pdg())+"). Assuming charge=+1.");
    charge = +1.0;
  } else {
    if (VP1Msg::verbose())
      VP1Msg::messageVerbose("Looked up particle charge for track record with pdg code "+VP1Msg::str(pdg())+": "+VP1Msg::str(charge));
  }
  return new Trk::AtaPlane(posStart(), Amg::Hep3VectorToEigen(m_trkrecord->GetMomentum()),charge,*surf);
}

