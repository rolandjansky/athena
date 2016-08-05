/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKPool/FTKTrackFitterStats_p1.h"

FTKTrackFitterStats_p1::FTKTrackFitterStats_p1() :
  m_ncombs(0), m_nfits(0), m_nfits_maj(0), m_nfits_maj_SCT(0), m_nfits_maj_pix(0), m_nfits_rec(0), m_nfits_addrec(0),
  m_nfits_bad(0), m_nfits_rej(0), m_nfits_badmaj(0), m_nfits_rejmaj(0)
{}
