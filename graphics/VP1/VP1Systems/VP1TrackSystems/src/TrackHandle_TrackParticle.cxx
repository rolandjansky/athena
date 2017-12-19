/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class TrackHandle_TrackParticle         //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2008                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1TrackSystems/TrackHandle_TrackParticle.h"
#include "VP1TrackSystems/TrackCollHandleBase.h"
#include "VP1Base/VP1Msg.h"
#include "TrkParameters/TrackParameters.h"
#include "Particle/TrackParticle.h"
#include "VP1TrackSystems/VP1TrackSanity.h"
#include "TrkTrackSummary/TrackSummary.h"

//____________________________________________________________________
class TrackHandle_TrackParticle::Imp {
public:

  const Trk::Track * createTrack(DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces)
  {
    if (!trackStateOnSurfaces) {
      VP1Msg::messageDebug("TrackHandle_TrackParticle WARNING: Could not create track due to null TSOS vector");
      return 0;
    }
    if (trackStateOnSurfaces->size()==0) {
      VP1Msg::messageDebug("TrackHandle_TrackParticle WARNING: Could not create track due to empty TSOS vector");
      delete trackStateOnSurfaces;
      return 0;
    }
    Trk::TrackInfo ti(Trk::TrackInfo::Unknown,theclass->extrapolationParticleHypothesis());
    const Trk::Track * trk = new Trk::Track(ti,trackStateOnSurfaces/*track assumes ownership*/,0/*fitquality*/);
    if (VP1Msg::verbose())
      VP1Msg::messageVerbose("TrackHandle_TrackParticle created track with "
 			     +QString::number(trackStateOnSurfaces->size())+" parameters");
    return trk;
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////////

  TrackHandle_TrackParticle * theclass;
  const Rec::TrackParticle * trackparticle;
  const Trk::Track* trkTrack;
  bool trkTrackInit;

};

//____________________________________________________________________
TrackHandle_TrackParticle::TrackHandle_TrackParticle(TrackCollHandleBase* ch, const Rec::TrackParticle *tp)
  : TrackHandleBase(ch), m_d(new Imp)
{
  m_d->theclass = this;
  m_d->trackparticle = tp;
  m_d->trkTrack = 0;
  m_d->trkTrackInit = false;

}

//____________________________________________________________________
TrackHandle_TrackParticle::~TrackHandle_TrackParticle()
{
  delete m_d;
}

//____________________________________________________________________
QStringList TrackHandle_TrackParticle::clicked() const
{
  QStringList l;
  l << "TrackParticle:";
  l << TrackHandleBase::baseInfo();
  return l;
}

//____________________________________________________________________
Amg::Vector3D TrackHandle_TrackParticle::momentum() const
{
  const Trk::Perigee* p = m_d->trackparticle->perigee();
  if (!common()->trackSanityHelper()->isSafe(p))
    return Amg::Vector3D(0,0,0);//fixme: warn?
  return p->momentum();
}

//____________________________________________________________________
double TrackHandle_TrackParticle::calculateCharge() const
{
  const Trk::Perigee* p = m_d->trackparticle->perigee();
  if (!common()->trackSanityHelper()->isSafe(p))
    return unknown();//fixme: warn
  return p->charge();
}

//____________________________________________________________________
const Trk::Track * TrackHandle_TrackParticle::provide_pathInfoTrkTrack() const
{
  if (m_d->trkTrackInit)
    return m_d->trkTrack;
  m_d->trkTrackInit = true;
  const std::vector<const Trk::TrackParameters*>&  trackpars = m_d->trackparticle->trackParameters();
  DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>;

  if (!trackpars.empty()) {
    bool needresorting = trackpars.at(0)!=m_d->trackparticle->perigee();//Needed since TrackParticles are (at the moment)
                                                                      //created with the first parameter put last
    if (needresorting) {
      const Trk::TrackParameters* p = trackpars.at(trackpars.size()-1);
      if (p)
        trackStateOnSurfaces->push_back(new Trk::TrackStateOnSurface(0,p->clone(),0,0));
    }
    unsigned limit(needresorting?trackpars.size()-1:trackpars.size());

    //NB: We only ever created this handle if charge()!=0.0:
    for (unsigned i = 0; i < limit; ++i) {
      const Trk::TrackParameters* p = trackpars.at(i);
      if (!p)
        continue;
      if (!common()->trackSanityHelper()->isSafe(p))
        continue;
      trackStateOnSurfaces->push_back(new Trk::TrackStateOnSurface(0,p->clone(),0,0));
    }
  }

  m_d->trkTrack = m_d->createTrack(trackStateOnSurfaces);
  return m_d->trkTrack;
}

//____________________________________________________________________
void TrackHandle_TrackParticle::ensureTouchedMuonChambersInitialised() const
{
}

//____________________________________________________________________
unsigned TrackHandle_TrackParticle::getNPixelHits() const
{ 
  return (m_d->trackparticle->trackSummary()) ? m_d->trackparticle->trackSummary()->get(Trk::numberOfPixelHits) : 0; 
}
//____________________________________________________________________
unsigned TrackHandle_TrackParticle::getNSCTHits() const
{ 
  return (m_d->trackparticle->trackSummary()) ? m_d->trackparticle->trackSummary()->get(Trk::numberOfSCTHits) : 0; 
}
//____________________________________________________________________
unsigned TrackHandle_TrackParticle::getNTRTHits() const
{ 
  return (m_d->trackparticle->trackSummary()) ? m_d->trackparticle->trackSummary()->get(Trk::numberOfTRTHits) : 0; 
}
//____________________________________________________________________
unsigned TrackHandle_TrackParticle::getNMuonPhiHits() const
{ 
  return (m_d->trackparticle->trackSummary()) ? m_d->trackparticle->trackSummary()->get(Trk::numberOfRpcPhiHits) + m_d->trackparticle->trackSummary()->get(Trk::numberOfTgcPhiHits) + m_d->trackparticle->trackSummary()->get(Trk::numberOfCscPhiHits) : 0;
}
//____________________________________________________________________
unsigned TrackHandle_TrackParticle::getNMDTHits() const
{ 
  return (m_d->trackparticle->trackSummary()) ? m_d->trackparticle->trackSummary()->get(Trk::numberOfMdtHits) : 0; 
}
//____________________________________________________________________
unsigned TrackHandle_TrackParticle::getNRPCHits() const
{ 
  return (m_d->trackparticle->trackSummary()) ? m_d->trackparticle->trackSummary()->get(Trk::numberOfRpcEtaHits) + m_d->trackparticle->trackSummary()->get(Trk::numberOfRpcPhiHits) : 0; 
}//____________________________________________________________________
unsigned TrackHandle_TrackParticle::getNTGCHits() const
{ 
  return (m_d->trackparticle->trackSummary()) ? m_d->trackparticle->trackSummary()->get(Trk::numberOfTgcEtaHits) + m_d->trackparticle->trackSummary()->get(Trk::numberOfTgcPhiHits) : 0; 
}
//____________________________________________________________________
unsigned TrackHandle_TrackParticle::getNCSCHits() const
{ 
  return (m_d->trackparticle->trackSummary()) ? m_d->trackparticle->trackSummary()->get(Trk::numberOfCscEtaHits) + m_d->trackparticle->trackSummary()->get(Trk::numberOfCscPhiHits) : 0; 
}
