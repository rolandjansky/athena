/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTObjects/HTTTrackStream.h"

#include <iostream>
#include <iomanip>
using namespace std;

HTTTrackStream::HTTTrackStream() :
  m_run_number(0ul), m_event_number(0ul),
  m_ntracks(0), m_ncombs(0), m_nfits(0), m_nfits_maj(0),
  m_nfits_maj_SCT(0),
  m_nfits_maj_pix(0),
  m_nfits_rec(0), m_nfits_addrec(0), m_nfits_bad(0), m_nfits_rej(0),
  m_nfits_badmaj(0), m_nfits_rejmaj(0),
  m_ntracksI(0), m_ncombsI(0), m_nfitsI(0), m_nfits_majI(0),
  m_nfits_majI_SCT(0),
  m_nfits_majI_pix(0),
  m_nfits_recI(0), m_nfits_addrecI(0), m_nfits_badI(0), m_nfits_rejI(0),
  m_nfits_badmajI(0), m_nfits_rejmajI(0), m_nconn(0), m_nextrapolatedTracks(0)
{
  m_tracks = new TClonesArray("HTTTrack", 1000);
  m_tracksI = new TClonesArray("HTTTrack", 1000);
}

HTTTrackStream & HTTTrackStream::operator = (const HTTTrackStream &in)
{
  if (this != &in) {
    m_run_number = in.m_run_number;
    m_event_number = in.m_event_number;
    m_ntracks = in.m_ntracks;
    m_ncombs = in.m_ncombs;
    m_nfits = in.m_nfits;
    m_nfits_maj = in.m_nfits_maj;
    m_nfits_maj_SCT = in.m_nfits_maj_SCT;
    m_nfits_maj_pix = in.m_nfits_maj_pix;
    m_nfits_rec = in.m_nfits_rec;
    m_nfits_addrec = in.m_nfits_addrec;
    m_nfits_bad = in.m_nfits_bad;
    m_nfits_rej = in.m_nfits_rej;
    m_nfits_badmaj = in.m_nfits_badmaj;
    m_nfits_rejmaj = in.m_nfits_rejmaj;
    m_ntracksI = in.m_ntracksI;
    m_ncombsI = in.m_ncombsI;
    m_nfitsI = in.m_nfitsI;
    m_nfits_majI = in.m_nfits_majI;
    m_nfits_majI_SCT = in.m_nfits_majI_SCT;
    m_nfits_majI_pix = in.m_nfits_majI_pix;
    m_nfits_recI = in.m_nfits_recI;
    m_nfits_addrecI = in.m_nfits_addrecI;
    m_nfits_badI = in.m_nfits_badI;
    m_nfits_rejI = in.m_nfits_rejI;
    m_nfits_badmajI = in.m_nfits_badmajI;
    m_nfits_rejmajI = in.m_nfits_rejmajI;
    m_nconn = in.m_nconn;
    m_nextrapolatedTracks = in.m_nextrapolatedTracks;
    m_tracks = new TClonesArray("HTTTrack", 1000);
    m_tracksI = new TClonesArray("HTTTrack", 1000);
    for (int i = 0; i < in.m_ntracks; i++) {
      HTTTrack* trackP = in.getTrack(i);
      const HTTTrack track = *trackP;
      addTrack(track);
    }
    for (int i = 0; i < in.m_ntracksI; i++) {
      HTTTrack* trackP = in.getTrackI(i);
      const HTTTrack track = *trackP;
      addTrackI(track);
    }   
  }

  return *this;
}

HTTTrackStream::~HTTTrackStream()
{
  m_tracks->Delete();
  delete m_tracks;
  m_tracksI->Delete();
  delete m_tracksI;
}

// copy constructor
HTTTrackStream::HTTTrackStream(const HTTTrackStream &in) {
  m_run_number = in.m_run_number;
  m_event_number = in.m_event_number;
  m_ntracks = in.m_ntracks;
  m_ncombs = in.m_ncombs;
  m_nfits = in.m_nfits;
  m_nfits_maj = in.m_nfits_maj;
  m_nfits_maj_SCT = in.m_nfits_maj_SCT;
  m_nfits_maj_pix = in.m_nfits_maj_pix;
  m_nfits_rec = in.m_nfits_rec;
  m_nfits_addrec = in.m_nfits_addrec;
  m_nfits_bad = in.m_nfits_bad;
  m_nfits_rej = in.m_nfits_rej;
  m_nfits_badmaj = in.m_nfits_badmaj;
  m_nfits_rejmaj = in.m_nfits_rejmaj;
  m_ntracksI = in.m_ntracksI;
  m_ncombsI = in.m_ncombsI;
  m_nfitsI = in.m_nfitsI;
  m_nfits_majI = in.m_nfits_majI;
  m_nfits_majI_SCT = in.m_nfits_majI_SCT;
  m_nfits_majI_pix = in.m_nfits_majI_pix;
  m_nfits_recI = in.m_nfits_recI;
  m_nfits_addrecI = in.m_nfits_addrecI;
  m_nfits_badI = in.m_nfits_badI;
  m_nfits_rejI = in.m_nfits_rejI;
  m_nfits_badmajI = in.m_nfits_badmajI;
  m_nfits_rejmajI = in.m_nfits_rejmajI;
  m_nconn = in.m_nconn;
  m_nextrapolatedTracks = in.m_nextrapolatedTracks;
  m_tracks = new TClonesArray("HTTTrack", 1000);
  m_tracksI = new TClonesArray("HTTTrack", 1000);
  for (int i = 0; i < in.m_ntracks; i++) {
    HTTTrack* trackP = in.getTrack(i);
    const HTTTrack track = *trackP;
    addTrack(track);
  }
  for (int i = 0; i < in.m_ntracksI; i++) {
    HTTTrack* trackP = in.getTrackI(i);
    const HTTTrack track = *trackP;
    addTrackI(track);
  }
}


/** add a track in the final list */
void HTTTrackStream::addTrack(const HTTTrack& track)
{
  new ((*m_tracks)[m_ntracks++]) HTTTrack(track);
  m_trackIdMap[std::make_pair(track.getTrackID(), track.getBankID())] = m_ntracks - 1;
}


/** return a given track, identified by its position in
    the list */
HTTTrack* HTTTrackStream::getTrack(int ipos) const
{
  if (ipos >= m_ntracks) return 0;
  else return (HTTTrack*)m_tracks->At(ipos);
}

/** this method, passing a track ID and a bank ID, return the ID
    of the corresponding track. If it doesn't exist return -1 */
int HTTTrackStream::findTrack(int trackid, int bankid)
{
  if (!m_trackIdMap.size())
    buildTrackMap();

  std::map< std::pair<int, int>, int >::iterator it = m_trackIdMap.find(std::make_pair(trackid, bankid));
  if (it != m_trackIdMap.end())
    return it->second;
  else
    return -1;
}


/** add a track in the final list */
void HTTTrackStream::addTrackI(const HTTTrack& track)
{
  new ((*m_tracksI)[m_ntracksI++]) HTTTrack(track);
}


/** return a given track, identified by its position in
    the list */
HTTTrack* HTTTrackStream::getTrackI(int ipos) const
{
  if (ipos >= m_ntracksI) return 0;
  else return (HTTTrack*)m_tracksI->At(ipos);
}

/** reset the list of the tracks and the statistical informatino
    collected for this event */
void HTTTrackStream::clear()
{
  m_run_number = 0ul;
  m_event_number = 0ul;

  m_tracks->Delete();
  m_ntracks = 0;
  m_tracksI->Delete();
  m_ntracksI = 0;

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
*/

int HTTTrackStream::Print(int level, ostream& out)
{
  int printed(0);
  out << "*** Run/Event " << m_run_number << "/" << m_event_number << " ***" << endl;
  out << "*** Tracks (lvl=" << level << "): ***" << endl;
  out << "*** mntracks: " << m_ntracks << endl;
  // for (int it=0;it<m_ntracks;++it) {
  //    HTTTrack *tmpt = getTrack(it);
  // }
  if (printed == 0) out << " [ NO TRACKS ] " << endl;
  out << endl;

  return printed;
}

void HTTTrackStream::buildTrackMap() {
  m_trackIdMap.clear();
  for (int itrack = 0;itrack != m_ntracks;++itrack) { // loop over the tracks
    HTTTrack* cur_track = getTrack(itrack);

    if (!cur_track) continue; // a null pointer is possible
    m_trackIdMap[std::make_pair(cur_track->getTrackID(), cur_track->getBankID())] = itrack;
  }

}
