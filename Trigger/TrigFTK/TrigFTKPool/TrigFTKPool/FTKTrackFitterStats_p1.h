/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKTRACKFITTERSTATS_P1_H
#define FTKTRACKFITTERSTATS_P1_H

class FTKTrackFitterStats_p1 {
private:
  friend class FTKTrackFitterStatsCnv_p1;

  unsigned int m_ncombs; // number of combinations
  unsigned int m_nfits; // number of fitted combinations
  unsigned int m_nfits_maj; // number of fitted combinations in majority roads
  unsigned int m_nfits_maj_pix; // number of fitted combinations in majority roads for pix missing
  unsigned int m_nfits_maj_SCT; // number of fitted combinations in majority roads for SCT missing
  unsigned int m_nfits_rec; // number of full fits attempted to recover
  unsigned int m_nfits_addrec; // additional fits performed because the recovery
  unsigned int m_nfits_bad; // # fits with a bad chi^2
  unsigned int m_nfits_rej; // # of fits rejected because the HW
  unsigned int m_nfits_badmaj; // # fits with a bad chi^2
  unsigned int m_nfits_rejmaj; // # of fits rejected because the HW

public:
  FTKTrackFitterStats_p1();

};
#endif // FTKTRACKFITTERSTATS_H
