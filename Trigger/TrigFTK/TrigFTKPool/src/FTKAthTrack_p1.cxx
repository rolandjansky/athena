/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigFTKPool/FTKAthTrack_p1.h"


FTKAthTrack_p1::FTKAthTrack_p1() :
   m_bankID(-1), m_roadID(-1), m_sectorID(-1), m_trackID(-1),
   m_invpt(0), m_d0(0), m_phi(0), m_z0(0),
   m_ctheta(0), m_chi2(0), m_origchi2(0), m_nmissing(0), 
   m_typemask(0), m_bitmask(0), 
   m_HF_rejected(0),m_HW_rejected(0),m_HW_track(-1),
   m_eventindex(-1), m_barcode(-1), m_barcode_frac(0.), m_patternID(0)

{
}
