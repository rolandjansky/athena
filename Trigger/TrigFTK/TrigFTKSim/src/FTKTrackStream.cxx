/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKTrackStream.h"

#include <iostream>
#include <iomanip>
using namespace std;

FTKTrackStream::FTKTrackStream() :
  m_run_number(0ul), m_event_number(0ul),
  m_ntracks(0), m_ntracks_pre_hw(0), m_ncombs(0),m_nfits(0), m_nfits_maj(0),
  m_nfits_maj_SCT(0),
  m_nfits_maj_pix(0),
  m_nfits_rec(0), m_nfits_addrec(0), m_nfits_bad(0), m_nfits_rej(0),
  m_nfits_badmaj(0), m_nfits_rejmaj(0),
  m_ntracksI(0), m_ntracksI_pre_hw(0), m_ncombsI(0), m_nfitsI(0), m_nfits_majI(0),
  m_nfits_majI_SCT(0),
  m_nfits_majI_pix(0),
  m_nfits_recI(0), m_nfits_addrecI(0), m_nfits_badI(0), m_nfits_rejI(0),
  m_nfits_badmajI(0), m_nfits_rejmajI(0), m_nconn(0), m_nextrapolatedTracks(0),
  m_ntracks_pattern(0), m_ntracks_hits(0)
{
  m_tracks = new TClonesArray("FTKTrack",1000);
  m_tracks_pre_hw = new TClonesArray("FTKTrack",1000);
  m_tracksI = new TClonesArray("FTKTrack",1000);
  m_tracksI_pre_hw = new TClonesArray("FTKTrack",1000);
  m_tracks_pattern = new TClonesArray("FTKTrack",1000);
  m_tracks_hits = new TClonesArray("FTKTrack",1000);
}


FTKTrackStream::~FTKTrackStream()
{
  m_tracks->Delete();
  delete m_tracks;
  m_tracks_pre_hw->Delete();
  delete m_tracks_pre_hw;
  m_tracksI->Delete();
  delete m_tracksI;
  m_tracksI_pre_hw->Delete();
  delete m_tracksI_pre_hw;
  m_tracks_pattern->Delete();
  delete m_tracks_pattern;
  m_tracks_hits->Delete();
  delete m_tracks_hits;
}


/** add a track in the final list */
void FTKTrackStream::addTrack(const FTKTrack &track)
{
  new ((*m_tracks)[m_ntracks++]) FTKTrack(track); 
  m_trackIdMap[std::make_pair(track.getTrackID(),track.getBankID())] = m_ntracks-1; 
}

/** add a track in the final list */
void FTKTrackStream::addTrack_pre_hw(const FTKTrack &track)
{
  new ((*m_tracks_pre_hw)[m_ntracks_pre_hw++]) FTKTrack(track); 
}

/** return a given track, identified by its position in 
    the list */
FTKTrack* FTKTrackStream::getTrack(int ipos) const
{
  if (ipos>=m_ntracks) return 0;
  else return (FTKTrack*)m_tracks->At(ipos);
}

/** return a given track, identified by its position in 
    the list */
FTKTrack* FTKTrackStream::getTrack_pre_hw(int ipos) const
{
  if (ipos>=m_ntracks_pre_hw) return 0;
  else return (FTKTrack*)m_tracks_pre_hw->At(ipos);
}

/** this method, passing a track ID and a bank ID, return the ID 
    of the corresponding track. If it doesn't exist return -1 */
int FTKTrackStream::findTrack(int trackid, int bankid)
{
  if( !m_trackIdMap.size() ) 
    buildTrackMap(); 
  
  std::map< std::pair<int,int>, int >::iterator it = m_trackIdMap.find( std::make_pair(trackid,bankid)); 
  if( it != m_trackIdMap.end() ) 
    return it->second; 
  else 
    return -1;
}

/** add a track in the final list */
void FTKTrackStream::addTrackI(const FTKTrack &track)
{
  new ((*m_tracksI)[m_ntracksI++]) FTKTrack(track); 
}

/** add a track in the final list */
void FTKTrackStream::addTrackI_pre_hw(const FTKTrack &track)
{
  new ((*m_tracksI_pre_hw)[m_ntracksI_pre_hw++]) FTKTrack(track); 
}

/** return a given track, identified by its position in 
    the list */
FTKTrack* FTKTrackStream::getTrackI(int ipos) const
{
  if (ipos>=m_ntracksI) return 0;
  else return (FTKTrack*)m_tracksI->At(ipos);
}

/** return a given track, identified by its position in 
    the list */
FTKTrack* FTKTrackStream::getTrackI_pre_hw(int ipos) const
{
  if (ipos>=m_ntracksI_pre_hw) return 0;
  else return (FTKTrack*)m_tracksI_pre_hw->At(ipos);
}

/** add a track in the final list */
void FTKTrackStream::addTrack_pattern(const FTKTrack &track)
{
  new ((*m_tracks_pattern)[m_ntracks_pattern++]) FTKTrack(track); 
}

/** add a track in the final list */
void FTKTrackStream::addTrack_hits(const FTKTrack &track)
{
  new ((*m_tracks_hits)[m_ntracks_hits++]) FTKTrack(track); 
}

/** return a given track, identified by its position in 
    the list */
FTKTrack* FTKTrackStream::getTrack_pattern(int ipos) const
{
  if (ipos>=m_ntracks_pattern) return 0;
  else return (FTKTrack*)m_tracks_pattern->At(ipos);
}

/** return a given track, identified by its position in 
    the list */
FTKTrack* FTKTrackStream::getTrack_hits(int ipos) const
{
  if (ipos>=m_ntracks_hits) return 0;
  else return (FTKTrack*)m_tracks_hits->At(ipos);
}

/** reset the list of the tracks and the statistical informatino
    collected for this event */
void FTKTrackStream::clear()
{
  m_run_number = 0ul;
  m_event_number = 0ul;

  m_tracks->Delete();
  m_ntracks = 0;
  m_tracks_pre_hw->Delete();
  m_ntracks_pre_hw = 0;
  m_tracksI->Delete();
  m_ntracksI = 0;
  m_tracksI_pre_hw->Delete();
  m_ntracksI_pre_hw = 0;
  m_tracks_pattern->Delete();
  m_ntracks_pattern = 0;
  m_tracks_hits->Delete();
  m_ntracks_hits = 0;

  m_ncombs = 0;
  m_nfits = 0;
  m_nconn = 0;
  m_nextrapolatedTracks = 0;
  m_nfits_maj = 0;
  m_nfits_maj_pix = 0;
  m_nfits_maj_SCT = 0;
  m_nfits_bad = 0;
  m_nfits_badmaj = 0;
  m_nfits_rec = 0;
  m_nfits_addrec = 0;
  m_nfits_rej = 0;
  m_nfits_rejmaj = 0;

  m_ncombsI = 0;
  m_nfitsI = 0;
  m_nfits_majI = 0;
  m_nfits_majI_pix = 0;
  m_nfits_majI_SCT = 0;
  m_nfits_badI = 0;
  m_nfits_badmajI = 0;
  m_nfits_recI = 0;
  m_nfits_addrecI = 0;
  m_nfits_rejI = 0;
  m_nfits_rejmajI = 0;

  m_trackIdMap.clear();
}


/** print a debug message summaryzing the information for the tracks
    found in one event for a given bank. 
    The level variable is used to choose the debug level of the output. 
    The level is related to the value assigned in the HWRejected().
*/

int FTKTrackStream::Print(int level, ostream &out)
{
  int printed(0);
  out << "*** Run/Event " << m_run_number << "/" << m_event_number << " ***" << endl;
  out << "*** Tracks (lvl=" << level << "): ***" << endl;
  out << "*** mntracks: " << m_ntracks << endl;
   for (int it=0;it<m_ntracks;++it) {
      FTKTrack *tmpt = getTrack(it);
      if (tmpt->getHWRejected()<=level) {
	printed += 1;
	out << (*tmpt) << endl;
      }
   }
   if (printed==0) out << " [ NO TRACKS ] " << endl;
   out << endl;

   return printed;
}

void FTKTrackStream::buildTrackMap() { 
  m_trackIdMap.clear(); 
  for (int itrack =0;itrack!=m_ntracks;++itrack) { // loop over the tracks 
    FTKTrack *cur_track = getTrack(itrack); 
    
    if (!cur_track) continue; // a null pointer is possible  
    m_trackIdMap[std::make_pair(cur_track->getTrackID(),cur_track->getBankID())] = itrack; 
  } 
  
}
