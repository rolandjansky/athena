/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKTRACKFITTERSTATS_H
#define FTKTRACKFITTERSTATS_H

class FTKTrackFitterStats {
private:
  unsigned int m_ncombs; // number of combinations
  unsigned int m_nfits; // number of fitted combinations
  unsigned int m_nfits_maj; // number of fitted combinations in majority roads
  unsigned int m_nfits_rec; // number of full fits attempted to recover
  unsigned int m_nfits_addrec; // additional fits performed because the recovery
  unsigned int m_nfits_bad; // # fits with a bad chi^2
  unsigned int m_nfits_rej; // # of fits rejected because the HW
  unsigned int m_nfits_badmaj; // # fits with a bad chi^2
  unsigned int m_nfits_rejmaj; // # of fits rejected because the HW

public:
  FTKTrackFitterStats();

  void setNCombs(int v) { m_ncombs = v; }
  void setNFits(int v) { m_nfits = v; }
  void setNFitsMajority(int v) { m_nfits_maj = v; }
  void setNFitsRecovery(int v) { m_nfits_rec = v; }
  void setNAddFitsRecovery(int v) { m_nfits_addrec = v; }
  void setNFitsBad(int v) { m_nfits_bad = v; }
  void setNFitsHWRejected(int v) { m_nfits_rej = v; }
  void setNFitsBadMajority(int v) { m_nfits_badmaj = v; }
  void setNFitsHWRejectedMajority(int v) { m_nfits_rejmaj = v; }

  void addNCombs(int v) { m_ncombs += v; }
  void addNFits(int v) { m_nfits += v; }
  void addNFitsMajority(int v) { m_nfits_maj += v; }
  void addNFitsRecovery(int v) { m_nfits_rec += v; }
  void addNAddFitsRecovery(int v) { m_nfits_addrec += v; }
  void addNFitsBad(int v) { m_nfits_bad += v; }
  void addNFitsHWRejected(int v) { m_nfits_rej += v; }
  void addNFitsBadMajority(int v) { m_nfits_badmaj += v; }
  void addNFitsHWRejectedMajority(int v) { m_nfits_rejmaj += v; }

  unsigned int getNCombs() const { return m_ncombs; }
  unsigned int getNFits() const { return m_nfits; }
  unsigned int getNFitsMajority() const { return m_nfits_maj; }
  unsigned int getNFitsRecovery() const { return m_nfits_rec; }
  unsigned int getNAddFitsRecovery() const { return m_nfits_addrec; }
  unsigned int getNFitsBad() const { return m_nfits_bad; }
  unsigned int getNFitsHWRejected() const { return m_nfits_rej; }
  unsigned int getNFitsBadMajority() const { return m_nfits_badmaj; }
  unsigned int getNFitsHWRejectedMajority() const { return m_nfits_rejmaj; }

};

#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(FTKTrackFitterStats,764785,1)

#endif // FTKTRACKFITTERSTATS_H
