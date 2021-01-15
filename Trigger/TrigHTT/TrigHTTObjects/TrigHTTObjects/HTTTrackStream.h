/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTTRACKSTREAM_H
#define HTTTRACKSTREAM_H

#include "HTTTrack.h"

#include <TClonesArray.h>

#include <iostream>
#include <utility>

class HTTTrackStream {

public:
  HTTTrackStream();
  virtual ~HTTTrackStream();

  const unsigned long& runNumber() const { return m_run_number; }
  const unsigned long& eventNumber() const { return m_event_number; }
  void setRunNumber(const unsigned long& val) { m_run_number = val; }
  void setEventNumber(const unsigned long& val) { m_event_number = val; }

  void addTrack(const HTTTrack&);
  HTTTrack* getTrack(int) const;
  int findTrack(int trackid, int bankid);
  int getNTracks() const { return m_ntracks; }
  void buildTrackMap(); 

  void addTrackI(const HTTTrack&);
  HTTTrack* getTrackI(int) const;
  int getNTracksI() const { return m_ntracksI; }

  int getNConn() const {return m_nconn;}
  int getNExtrapolatedTracks() const {return m_nextrapolatedTracks;}

  void addNCombs(int v) { m_ncombs += v; }
  void addNFits(int v) { m_nfits += v; }
  void addNFitsMajority(int v) { m_nfits_maj += v; }
  void addNFitsMajority_pix(int v) { m_nfits_maj_pix += v; }
  void addNFitsMajority_SCT(int v) { m_nfits_maj_SCT += v; }
  void addNFitsRecovery(int v) { m_nfits_rec += v; }
  void addNAddFitsRecovery(int v) { m_nfits_addrec += v; }
  void addNFitsBad(int v) { m_nfits_bad += v; }
  void addNFitsHWRejected(int v) { m_nfits_rej += v; }
  void addNFitsBadMajority(int v) { m_nfits_badmaj += v; }
  void addNFitsHWRejectedMajority(int v) { m_nfits_rejmaj += v; }
  void addNConnections(int v) {m_nconn += v;}
  void addNExtrapolatedTracks(int v) {m_nextrapolatedTracks += v;}

  int getNCombs() const { return m_ncombs; }
  int getNFits() const { return m_nfits; }
  int getNFitsMajority() const { return m_nfits_maj; }
  int getNFitsMajority_pix() const { return m_nfits_maj_pix; }
  int getNFitsMajority_SCT() const { return m_nfits_maj_SCT; }
  int getNFitsRecovery() const { return m_nfits_rec; }
  int getNAddFitsRecovery() const { return m_nfits_addrec; }
  int getNFitsBad() const { return m_nfits_bad; }
  int getNFitsHWRejected() const { return m_nfits_rej; }
  int getNFitsBadMajority() const { return m_nfits_badmaj; }
  int getNFitsHWRejectedMajority() const { return m_nfits_rejmaj; }
  int getNConnections() const {return m_nconn;}

  void addNCombsI(int v) { m_ncombsI += v; }
  void addNFitsI(int v) { m_nfitsI += v; }
  void addNFitsMajorityI(int v) { m_nfits_majI += v; }
  void addNFitsMajorityI_pix(int v) { m_nfits_majI_pix += v; }
  void addNFitsMajorityI_SCT(int v) { m_nfits_majI_SCT += v; }
  void addNFitsRecoveryI(int v) { m_nfits_recI += v; }
  void addNAddFitsRecoveryI(int v) { m_nfits_addrecI += v; }
  void addNFitsBadI(int v) { m_nfits_badI += v; }
  void addNFitsHWRejectedI(int v) { m_nfits_rejI += v; }
  void addNFitsBadMajorityI(int v) { m_nfits_badmajI += v; }
  void addNFitsHWRejectedMajorityI(int v) { m_nfits_rejmajI += v; }

  int getNCombsI() const { return m_ncombsI; }
  int getNFitsI() const { return m_nfitsI; }
  int getNFitsMajorityI() const { return m_nfits_majI; }
  int getNFitsMajorityI_pix() const { return m_nfits_majI_pix; }
  int getNFitsMajorityI_SCT() const { return m_nfits_majI_SCT; }
  int getNFitsRecoveryI() const { return m_nfits_recI; }
  int getNAddFitsRecoveryI() const { return m_nfits_addrecI; }
  int getNFitsBadI() const { return m_nfits_badI; }
  int getNFitsHWRejectedI() const { return m_nfits_rejI; }
  int getNFitsBadMajorityI() const { return m_nfits_badmajI; }
  int getNFitsHWRejectedMajorityI() const { return m_nfits_rejmajI; }

  void clear();

  int Print(int level=0,std::ostream &out=std::cout);

private:
  unsigned long m_run_number; // run number
  unsigned long m_event_number; // event number

  int m_ntracks; // number of stored tracks
  TClonesArray *m_tracks; //-> list of stored tracks

  int m_ncombs; // number of combinations
  int m_nfits; // number of fitted combinations
  int m_nfits_maj; // number of fitted combinations in majority roads
  int m_nfits_maj_SCT; // number of fitted combinations in majority roads, missing hit in SCT layer
  int m_nfits_maj_pix; // number of fitted combinations in majority roads, missing hit in pixel layer
  int m_nfits_rec; // number of full fits attempted to recover
  int m_nfits_addrec; // additional fits performed because the recovery
  int m_nfits_bad; // # fits with a bad chi^2
  int m_nfits_rej; // # of fits rejected because the HW
  int m_nfits_badmaj; // # fits with a bad chi^2
  int m_nfits_rejmaj; // # of fits rejected because the HW
  /* PS: m_ntracks, if only good tracks are saved, has to be equal
     to m_nfits-m_nfits_bad-m_nfits_rej */
  
  int m_ntracksI; // number of stored tracks in the intermediate step

  TClonesArray *m_tracksI; //-> list of stored tracks in the intermedieate step


  // duplicate the counters for the 2nd stage
  int m_ncombsI;
  int m_nfitsI;
  int m_nfits_majI;
  int m_nfits_majI_SCT;
  int m_nfits_majI_pix;
  int m_nfits_recI;
  int m_nfits_addrecI;
  int m_nfits_badI;
  int m_nfits_rejI;
  int m_nfits_badmajI;
  int m_nfits_rejmajI;
  int m_nconn;
  int m_nextrapolatedTracks;

  std::map< std::pair<int,int>, int > m_trackIdMap; //! Not saved on disk.  
  // Used to optimize the findTrack function. 

  ClassDef(HTTTrackStream,2)
};


#endif // HTTTRACKSTREAM_H
