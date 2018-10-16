/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class TrackHandle_xAODTrackParticle         //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2008                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1TrackSystems/TrackHandle_xAODTrackParticle.h"
#include "VP1TrackSystems/TrackCollHandleBase.h"
#include "VP1Base/VP1Msg.h"
#include "TrkParameters/TrackParameters.h"
#include "VP1TrackSystems/VP1TrackSanity.h"
#include "TrkTrackSummary/TrackSummary.h"

//____________________________________________________________________
class TrackHandle_xAODTrackParticle::Imp {
public:

  const Trk::Track * createTrack(DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces)
  {
    if (!trackStateOnSurfaces) {
      VP1Msg::messageDebug("TrackHandle_xAODTrackParticle WARNING: Could not create track due to null TSOS vector");
      return 0;
    }
    if (trackStateOnSurfaces->size()==0) {
      VP1Msg::messageDebug("TrackHandle_xAODTrackParticle WARNING: Could not create track due to empty TSOS vector");
      delete trackStateOnSurfaces;
      return 0;
    }
    Trk::TrackInfo ti(Trk::TrackInfo::Unknown,theclass->extrapolationParticleHypothesis());
    const Trk::Track * trk = new Trk::Track(ti,trackStateOnSurfaces/*track assumes ownership*/,0/*fitquality*/);
    if (VP1Msg::verbose())
      VP1Msg::messageVerbose("TrackHandle_xAODTrackParticle created track with "
 			     +QString::number(trackStateOnSurfaces->size())+" parameters");
    return trk;
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////////

  TrackHandle_xAODTrackParticle * theclass;
  const xAOD::TrackParticle * trackparticle;
  const Trk::Track* trkTrack;
  bool trkTrackInit;

};

//____________________________________________________________________
TrackHandle_xAODTrackParticle::TrackHandle_xAODTrackParticle(TrackCollHandleBase* ch, const xAOD::TrackParticle *tp)
  : TrackHandleBase(ch), m_d(new Imp)
{
  m_d->theclass = this;
  m_d->trackparticle = tp;
  m_d->trkTrack = 0;
  m_d->trkTrackInit = false;

}

//____________________________________________________________________
TrackHandle_xAODTrackParticle::~TrackHandle_xAODTrackParticle()
{
  delete m_d;
}

//____________________________________________________________________
QStringList TrackHandle_xAODTrackParticle::clicked() const
{
  QStringList l;
  l << "xAODTrackParticle:";
  l << TrackHandleBase::baseInfo();
  return l;
}

//____________________________________________________________________
Amg::Vector3D TrackHandle_xAODTrackParticle::momentum() const
{
  const Trk::Perigee& p = m_d->trackparticle->perigeeParameters();
  if (!common()->trackSanityHelper()->isSafe(&p))
    return Amg::Vector3D(0,0,0);//fixme: warn?
  return p.momentum();
}

//____________________________________________________________________
double TrackHandle_xAODTrackParticle::calculateCharge() const
{
  // const Trk::MeasuredPerigee* p = m_d->trackparticle->measuredPerigee();
  // if (!common()->trackSanityHelper()->isSafe(p))
  //   return unknown();//fixme: warn
  return m_d->trackparticle->charge();
}

//____________________________________________________________________
const Trk::Track * TrackHandle_xAODTrackParticle::provide_pathInfoTrkTrack() const
{
  if (m_d->trkTrackInit)
    return m_d->trkTrack;
  m_d->trkTrackInit = true;
  std::vector<const Trk::TrackParameters*>  trackpars;
  DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>;

  const Trk::Perigee* peri = &(m_d->trackparticle->perigeeParameters());
  // const Trk::ParametersBase* base = static_cast<const Trk::TrackParameters*>(peri);
  
  // if (!trackpars.empty()) {
  trackpars.push_back(peri);
    // bool needresorting = trackpars.at(0)!=m_d->trackparticle->perigee();//Needed since TrackParticles are (at the moment)
    //                                                                   //created with the first parameter put last
    // if (needresorting) {
    //   const Trk::ParametersT<Trk::Charged>* p = dynamic_cast<const Trk::ParametersT<Trk::Charged>* >(trackpars.at(trackpars.size()-1));
    //   if (p)
    //     trackStateOnSurfaces->push_back(new Trk::TrackStateOnSurface(0,p->clone(),0,0));
    // }
    // unsigned limit(needresorting?trackpars.size()-1:trackpars.size());

    //NB: We only ever created this handle if charge()!=0.0:
    for (unsigned i = 0; i < trackpars.size(); ++i) {
      const Trk::TrackParameters* p = (trackpars.at(i));
      if (!p)
        continue;
      if (!common()->trackSanityHelper()->isSafe(p))
        continue;
      trackStateOnSurfaces->push_back(new Trk::TrackStateOnSurface(0,p->clone(),0,0));
    }
  // }

  m_d->trkTrack = m_d->createTrack(trackStateOnSurfaces);
  return m_d->trkTrack;
}

//____________________________________________________________________
void TrackHandle_xAODTrackParticle::ensureTouchedMuonChambersInitialised() const
{
}

//____________________________________________________________________
unsigned TrackHandle_xAODTrackParticle::getNPixelHits() const
{ 
  uint8_t numberOfPixelHits = 0; 
  if (m_d->trackparticle && m_d->trackparticle->summaryValue(numberOfPixelHits,xAOD::numberOfPixelHits))
    return numberOfPixelHits; 
  return 0;
}

//____________________________________________________________________
unsigned TrackHandle_xAODTrackParticle::getNSCTHits() const
{ 
  uint8_t numberOfSCTHits = 0;   
  if (m_d->trackparticle && m_d->trackparticle->summaryValue(numberOfSCTHits,xAOD::numberOfSCTHits))
    return numberOfSCTHits; 
  return 0;
}

//____________________________________________________________________
unsigned TrackHandle_xAODTrackParticle::getNTRTHits() const
{ 
  uint8_t numberOfTRTHits = 0;   
    if (m_d->trackparticle && m_d->trackparticle->summaryValue(numberOfTRTHits,xAOD::numberOfTRTHits)) 
      return numberOfTRTHits; 
    return 0;
}

//____________________________________________________________________
unsigned TrackHandle_xAODTrackParticle::getNMuonPhiHits() const
{ 
  return 0;
  // return (m_d->trackparticle->trackSummary()) ? m_d->trackparticle->trackSummary()->get(Trk::numberOfRpcPhiHits) + m_d->trackparticle->trackSummary()->get(Trk::numberOfTgcPhiHits) + m_d->trackparticle->trackSummary()->get(Trk::numberOfCscPhiHits) : 0;
}
//____________________________________________________________________
unsigned TrackHandle_xAODTrackParticle::getNMDTHits() const
{ 
  return 0;
  // return (m_d->trackparticle->trackSummary()) ? m_d->trackparticle->trackSummary()->get(Trk::numberOfMdtHits) : 0; 
}
//____________________________________________________________________
unsigned TrackHandle_xAODTrackParticle::getNRPCHits() const
{ 
  return 0;
  // return (m_d->trackparticle->trackSummary()) ? m_d->trackparticle->trackSummary()->get(Trk::numberOfRpcEtaHits) + m_d->trackparticle->trackSummary()->get(Trk::numberOfRpcPhiHits) : 0; 
}//____________________________________________________________________
unsigned TrackHandle_xAODTrackParticle::getNTGCHits() const
{ 
  return 0;
  // return (m_d->trackparticle->trackSummary()) ? m_d->trackparticle->trackSummary()->get(Trk::numberOfTgcEtaHits) + m_d->trackparticle->trackSummary()->get(Trk::numberOfTgcPhiHits) : 0; 
}
//____________________________________________________________________
unsigned TrackHandle_xAODTrackParticle::getNCSCHits() const
{ 
  return 0;
  // return (m_d->trackparticle->trackSummary()) ? m_d->trackparticle->trackSummary()->get(Trk::numberOfCscEtaHits) + m_d->trackparticle->trackSummary()->get(Trk::numberOfCscPhiHits) : 0; 
}
