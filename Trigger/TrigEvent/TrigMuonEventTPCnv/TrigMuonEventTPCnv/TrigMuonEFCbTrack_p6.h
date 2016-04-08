/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFCbTrack_p6
 *
 * @brief persistent partner for TrigMuonEFCbTrack
 *
 * @author Ilija Vukotic <ilija@vukotic.me>
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFCBTRACK_P6_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFCBTRACK_P6_H

#include "AthenaPoolUtilities/TPObjRef.h"

class TrigMuonEFCbTrack_p6 {
	friend class TrigMuonEFCbTrackCnv;
public:

	TrigMuonEFCbTrack_p6() : m_matchChi2(0.0), m_nIdSctHits(0), m_nIdPixelHits(0), m_nTrtHits(0) {}
	virtual ~TrigMuonEFCbTrack_p6(){}

        //private:
	float m_matchChi2;
	unsigned short int m_nIdSctHits;
	unsigned short int m_nIdPixelHits;
	unsigned short int m_nTrtHits;

	TPObjRef m_TrigMuonEFTrack;
};

#endif
