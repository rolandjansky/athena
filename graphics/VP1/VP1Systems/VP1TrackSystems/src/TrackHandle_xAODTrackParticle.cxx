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
      return nullptr;
    }
    if (trackStateOnSurfaces->size()==0) {
      VP1Msg::messageDebug("TrackHandle_xAODTrackParticle WARNING: Could not create track due to empty TSOS vector");
      delete trackStateOnSurfaces;
      return nullptr;
    }
    Trk::TrackInfo ti(Trk::TrackInfo::Unknown,theclass->extrapolationParticleHypothesis());
    std::unique_ptr<DataVector<const Trk::TrackStateOnSurface>>  sink(trackStateOnSurfaces);
    const Trk::Track* trk =
      new Trk::Track(ti,
                     std::move(*sink),
                     nullptr /*fitquality*/);
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
  m_d->trkTrack = nullptr;
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

  trackpars.push_back(peri);


    //NB: We only ever created this handle if charge()!=0.0:
    for (unsigned i = 0; i < trackpars.size(); ++i) {
      const Trk::TrackParameters* p = (trackpars.at(i));
      if (!p)
        continue;
      if (!common()->trackSanityHelper()->isSafe(p))
        continue;
      trackStateOnSurfaces->push_back(new Trk::TrackStateOnSurface(nullptr,p->uniqueClone(),nullptr,nullptr));
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
}
//____________________________________________________________________
unsigned TrackHandle_xAODTrackParticle::getNMDTHits() const
{
  return 0;
}
//____________________________________________________________________
unsigned TrackHandle_xAODTrackParticle::getNRPCHits() const
{
  return 0;
}//____________________________________________________________________
unsigned TrackHandle_xAODTrackParticle::getNTGCHits() const
{
  return 0;
}
//____________________________________________________________________
unsigned TrackHandle_xAODTrackParticle::getNCSCHits() const
{
  return 0;
}
