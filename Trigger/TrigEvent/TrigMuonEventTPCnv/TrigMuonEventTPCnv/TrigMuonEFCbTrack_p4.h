/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFCbTrack_p4
 *
 * @brief persistent partner for TrigMuonEFCbTrack
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @author Sergio Grancagnolo  <Sergio.Grancagnolo@le.infn.it>  - U.Salento/INFN Le
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFCBTRACK_P4_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFCBTRACK_P4_H

#include <stdint.h>
#include <string>
#include "AthenaPoolUtilities/TPObjRef.h"

class TrigMuonEFCbTrack_p4
{
	friend class TrigMuonEFCbTrackCnv;

public:

	TrigMuonEFCbTrack_p4() {}
	virtual ~TrigMuonEFCbTrack_p4(){}

        //private:
	float m_matchChi2;
	unsigned short int m_nIdSctHits;
	unsigned short int m_nIdPixelHits;
	unsigned short int m_nTrtHits;

	TPObjRef m_TrigMuonEFTrack;
	TPObjRef m_P4IPtCotThPhiM;
};

#endif
