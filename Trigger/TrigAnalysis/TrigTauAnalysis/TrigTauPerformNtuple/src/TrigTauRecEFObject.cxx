/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauPerformNtuple/TrigTauRecEFObject.h"

#include "TrigTauPerformNtuple/TrigTauTrackCollection.h"

ClassImp(TrigTauRecEFObject)  

TrigTauRecEFObject::TrigTauRecEFObject()
{
  reset();
}

TrigTauRecEFObject::TrigTauRecEFObject(const TrigTauRecEFObject & other) :
  TrigTauRecObject(other),
  m_roiID(other.m_roiID),
  m_roiWord(other.m_roiWord),
  m_tags(other.m_tags),
  r_EFIDtrackCollection(other.r_EFIDtrackCollection)
{
  TrigTauTrackCollection* otherTC = static_cast<TrigTauTrackCollection*>(
      other.r_EFIDtrackCollection.GetObject());
  TrigTauTrackCollection* thisTC = static_cast<TrigTauTrackCollection*>(
      r_EFIDtrackCollection.GetObject());
  
  for( unsigned int i = 0; i < otherTC->nTracks(); ++i)
  {
    for( unsigned int j=0; j < other.nTracks(); ++j)
        if( &otherTC->track( i) == &other.track( j))
            r_tracks.Add( &thisTC->track(i));
    for( unsigned int j=0; j < other.nLooseTracks(); ++j)
        if( &otherTC->track( i) == &other.looseTrack( j))
            r_looseTracks.Add( &thisTC->track(i));
    for( unsigned int j=0; j < other.nSeedCaloTracks(); ++j)
        if( &otherTC->track( i) == &other.seedCaloTrack( j))
            r_seedCaloTracks.Add( &thisTC->track(i));
    for( unsigned int j=0; j < other.nSeedTrkTracks(); ++j)
        if( &otherTC->track( i) == &other.seedTrkTrack( j))
            r_seedTrkTracks.Add( &thisTC->track(i));
    for( unsigned int j=0; j < other.nSeedCaloWideTracks(); ++j)
        if( &otherTC->track( i) == &other.seedCaloWideTrack( j))
            r_seedCaloWideTracks.Add( &thisTC->track(i));
    for( unsigned int j=0; j < other.nSeedTrkWideTracks(); ++j)
        if( &otherTC->track( i) == &other.seedTrkWideTrack( j))
            r_seedTrkWideTracks.Add( &thisTC->track(i));
    for( unsigned int j=0; j < other.nOtherTracks(); ++j)
        if( &otherTC->track( i) == &other.otherTrack( j))
            r_otherTracks.Add( &thisTC->track(i));
  }
}

TrigTauRecEFObject::~TrigTauRecEFObject()
{
}

void TrigTauRecEFObject::reset()
{
  TrigTauRecObject::reset();
  m_roiID = 0;
  m_roiWord = 0;
  m_tags.reset();
  r_EFIDtrackCollection = 0;

  r_tracks.Clear();
  r_looseTracks.Clear();
  r_seedCaloTracks.Clear();
  r_seedTrkTracks.Clear();
  r_seedCaloWideTracks.Clear();
  r_seedTrkWideTracks.Clear();
  r_otherTracks.Clear();

  return ;
}

