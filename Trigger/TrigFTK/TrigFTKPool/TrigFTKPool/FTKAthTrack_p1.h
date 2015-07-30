/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGFTKSIM_FTKATHTRACK_P1_H
#define TRIGFTKSIM_FTKATHTRACK_P1_H

#include <vector>

class FTKAthTrack_p1 {
private:
  int m_bankID; // Bank ID of the road related to this track
  int m_roadID; // Road identifier
  int m_patternID; // TODO add documentation
  int m_sectorID; // Sector identifier
  int m_trackID; // Unique track ID in this bank

  float m_invpt; // 1/(2*Pt), aka half of 1/pt
  float m_d0; // impact paramter
  float m_phi; // phi of the track
  float m_z0; // z0
  float m_ctheta; // cot(theta)
  float m_chi2; // chi2 of the track
  float m_origchi2; // In the case of majority recovery, this is the chi2 of 
                    // the full fit. In all other cases m_chi2 == m_origchi2

  int m_nmissing; // number of missing coordinates
  unsigned int m_typemask; // set on in bits related to the step recovery were used, ex.: 0 no recovery, 01, rec 1st step, 11, recovery in the 1st and the 2nd stage
  unsigned int m_bitmask;
  std::vector<float> m_coord; // coordinates used in the fit

  int m_HF_rejected; /* 0 if the track is accepted, >0 if rejected by hit filter
			if negative, belongs to a HF rejected road
		     */
  int m_HW_rejected; /* The flag has 3 digit: [Q][RW][HW]:
		      Q : 1 if has a bad chi2, 0 if not
		      RW: is the RW level rejected the road, 0 if not rejected
		      HW: 1 if rejected by HW, 0 if not */
  int m_HW_track; // ID of the track excluded this track

  signed long m_eventindex; // matched particle event index
  signed long m_barcode; // matched geant particle barcode
  float m_barcode_frac; // largest "matching fraction" with any "good"
                        // geant particle, corresponding to the
                        // particle with m_barcode

public:
  FTKAthTrack_p1();

  friend class FTKAthTrackCnv_p1;
};

#endif // TRIGFTKSIM_FTKATHTRACK_P1_H
