/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauPerformNtuple/TrigTauTrackCollection.h"
ClassImp(TrigTauTrackCollection) 

TrigTauTrackCollection::TrigTauTrackCollection::TrigTauTrackCollection()
  : TObject(), m_roiWord(0), m_label("")
{
  tracks.reserve(50);
}

TrigTauTrackCollection::TrigTauTrackCollection( const TrigTauTrackCollection & other)
  : TObject(other)
  , m_roiWord( other.m_roiWord)
  , m_label(other.m_label)
{
  for(std::vector<TrigTauTrack*>::const_iterator itr = other.tracks.begin(); itr != other.tracks.end(); ++itr)
    tracks.push_back(new TrigTauTrack(**itr));
}

TrigTauTrackCollection::~TrigTauTrackCollection() {reset();}
  
void TrigTauTrackCollection::reset()
{
  for(std::vector<TrigTauTrack*>::iterator itr = tracks.begin(); itr != tracks.end(); ++itr)
    delete *itr;
  tracks.erase(tracks.begin(), tracks.end() );
  m_roiWord = 0;
  m_label = "";
}
