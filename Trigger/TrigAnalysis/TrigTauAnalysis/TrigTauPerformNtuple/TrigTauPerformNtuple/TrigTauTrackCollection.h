/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUTRACKCOLLECTION_H
#define TRIGTAUTRACKCOLLECTION_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauTrackCollection
/// Package : TauTrigPerformance
/// Author  : Olya Igonkina (University of Oregon)
/// Created : June 2008
///
/// DESCRIPTION:
///           contains tracks associated with RoI object
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <vector>
#include "TObject.h"
#include "TrigTauPerformNtuple/TrigTauTrack.h"

class TrigTauTrackCollection : public TObject
{
 public :
  TrigTauTrackCollection();
  TrigTauTrackCollection( const TrigTauTrackCollection & other);
  virtual ~TrigTauTrackCollection();

  virtual void reset();

  unsigned int              nTracks() const {return tracks.size();} ;
  const  TrigTauTrack&      track(unsigned int i ) const { return *tracks[i]; };
  TrigTauTrack&             track(unsigned int i )  { return *tracks[i]; };
  std::vector<TrigTauTrack*> tracks;

  int                       m_roiWord;
  std::string               m_label;

 private:
  ClassDef(TrigTauTrackCollection, 2)  ;
};

#endif
