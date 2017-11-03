/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class TrackCollHandle_RefittedTracks    //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: September 2008                           //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1TrackSystems/TrackCollHandle_RefittedTracks.h"
#include "VP1TrackSystems/TrackHandle_TrkTrack.h"
// #include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Base/IVP1System.h"

//____________________________________________________________________
class TrackCollHandle_RefittedTracks::Imp {
public:
  QList<const Trk::Track*> fittedtracks;
};


//____________________________________________________________________
TrackCollHandle_RefittedTracks::TrackCollHandle_RefittedTracks(TrackSysCommonData *cd,
							       const QString& name, QList<const Trk::Track*> fittedtracks)
  : TrackCollHandle_TrkTrack(cd,name,TrackType::RefittedTrack), m_d(new Imp)
{
  m_d->fittedtracks = fittedtracks;
}

//____________________________________________________________________
TrackCollHandle_RefittedTracks::~TrackCollHandle_RefittedTracks()
{
  delete m_d;
}

//____________________________________________________________________
bool TrackCollHandle_RefittedTracks::load()
{
  hintNumberOfTracksInEvent(m_d->fittedtracks.count());
  int i(0);
  foreach (const Trk::Track * trk, m_d->fittedtracks) {
    if (trk) {
      messageDebug("ADDTRACK.");
      addTrackHandle(new TrackHandle_TrkTrack(this,trk));
    } else
      messageDebug("WARNING: Ignoring null Trk::Track pointer.");
    if (!(i++%100))
      systemBase()->updateGUI();
  }

  //Maybe we need to show measurements, etc.:
  //FIXME  updateVisibleAssociatedObjects(); ??

  return true;
}

//FIXME: These coll's should not have their states saved...
