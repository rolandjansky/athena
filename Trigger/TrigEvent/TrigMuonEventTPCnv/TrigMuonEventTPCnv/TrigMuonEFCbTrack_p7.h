/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFCbTrack_p7
 *
 * @brief persistent partner for TrigMuonEFCbTrack
 *
 * @author Ilija Vukotic <ilija@vukotic.me>
 * @author Mark Owen <markowen@cern.ch>
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFCBTRACK_P7_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFCBTRACK_P7_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include "DataModelAthenaPool/ElementLink_p3.h"

class TrigMuonEFCbTrack_p7 {
	friend class TrigMuonEFCbTrackCnv;
public:

	TrigMuonEFCbTrack_p7() : m_matchChi2(0.0), m_nIdSctHits(0), m_nIdPixelHits(0), m_nTrtHits(0), m_idTrackLink() {}
	virtual ~TrigMuonEFCbTrack_p7(){}

        //private:
	float m_matchChi2;
	unsigned short int m_nIdSctHits;
	unsigned short int m_nIdPixelHits;
	unsigned short int m_nTrtHits;

	TPObjRef m_TrigMuonEFTrack;
	ElementLinkInt_p3 m_idTrackLink;
};

#endif
