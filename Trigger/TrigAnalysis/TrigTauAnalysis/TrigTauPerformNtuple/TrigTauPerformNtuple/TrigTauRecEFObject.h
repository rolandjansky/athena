/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAURECEFOBJECT_H
#define TRIGTAURECEFOBJECT_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauRecEFObject.h
/// Package : TrigTauPerformNtuple
/// Author  : Olya Igonkina (University of Oregon)
/// Created : April 2006
///
/// DESCRIPTION:
///           copy of tauObject to be dumped in root ntuple
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "TRef.h"
#include "TrigTauPerformNtuple/TrigTauRecObject.h"
#include "TrigTauPerformNtuple/TrigTauFlags.h"

class TrigTauRecEFObject : public TrigTauRecObject
{
public :
  
  TrigTauRecEFObject();
  TrigTauRecEFObject(const TrigTauRecEFObject & other);
  
  virtual ~TrigTauRecEFObject();
  void reset();
  
  int m_roiID;
  long int m_roiWord;
  TrigTauFlags m_tags;
  
  // special treatment of EF tracks - keep only pointers here.
  TRef                     r_EFIDtrackCollection;  
  
  unsigned int nTracks() const
        { return r_tracks.GetEntriesFast(); };
  const TrigTauTrack& track( unsigned int i) const
        { return *((TrigTauTrack*)r_tracks.At(i)); };
  TrigTauTrack& track( unsigned int i)  
        { return *((TrigTauTrack*)r_tracks.At(i)); };
  void resetTracks()
        { r_tracks.Clear(); };
  TRefArray r_tracks;
  
  unsigned int nLooseTracks() const
        { return r_looseTracks.GetEntriesFast(); };
  const TrigTauTrack& looseTrack(unsigned int i ) const 
        { return *((TrigTauTrack*)r_looseTracks.At(i)); };
  TrigTauTrack&           looseTrack(unsigned int i )  
        { return *((TrigTauTrack*)r_looseTracks.At(i)); };
  TRefArray r_looseTracks;
  
  unsigned int nSeedCaloTracks() const
        { return r_seedCaloTracks.GetEntriesFast(); };
  const TrigTauTrack& seedCaloTrack(unsigned int i ) const 
        { return *((TrigTauTrack*)r_seedCaloTracks.At(i)); };
  TrigTauTrack&           seedCaloTrack(unsigned int i )  
        { return *((TrigTauTrack*)r_seedCaloTracks.At(i)); };
  TRefArray r_seedCaloTracks;
  
  unsigned int nSeedTrkTracks() const
        { return r_seedTrkTracks.GetEntriesFast(); };
  const TrigTauTrack& seedTrkTrack(unsigned int i ) const 
        { return *((TrigTauTrack*)r_seedTrkTracks.At(i)); };
  TrigTauTrack&           seedTrkTrack(unsigned int i )  
        { return *((TrigTauTrack*)r_seedTrkTracks.At(i)); };
  TRefArray r_seedTrkTracks;
  
  unsigned int nSeedCaloWideTracks() const
        { return r_seedCaloWideTracks.GetEntriesFast(); };
  const TrigTauTrack& seedCaloWideTrack(unsigned int i ) const 
        { return *((TrigTauTrack*)r_seedCaloWideTracks.At(i)); };
  TrigTauTrack&           seedCaloWideTrack(unsigned int i )  
        { return *((TrigTauTrack*)r_seedCaloWideTracks.At(i)); };
  TRefArray r_seedCaloWideTracks;
  
  unsigned int nSeedTrkWideTracks() const
        { return r_seedTrkWideTracks.GetEntriesFast(); };
  const TrigTauTrack& seedTrkWideTrack(unsigned int i ) const 
        { return *((TrigTauTrack*)r_seedTrkWideTracks.At(i)); };
  TrigTauTrack&           seedTrkWideTrack(unsigned int i )  
        { return *((TrigTauTrack*)r_seedTrkWideTracks.At(i)); };
  TRefArray r_seedTrkWideTracks;
  
  unsigned int nOtherTracks() const
        { return r_otherTracks.GetEntriesFast(); };
  const TrigTauTrack& otherTrack(unsigned int i ) const 
        { return *((TrigTauTrack*)r_otherTracks.At(i)); };
  TrigTauTrack&           otherTrack(unsigned int i )  
        { return *((TrigTauTrack*)r_otherTracks.At(i)); };
  TRefArray r_otherTracks;
  
  
private:
  
  ClassDef(TrigTauRecEFObject, 6)  ;
};

#endif //TRIGTAURECOBJECT_H
