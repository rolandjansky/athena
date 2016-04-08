/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFCbTrack_p2
 *
 * @brief persistent partner for TrigMuonEFCbTrack
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @author Sergio Grancagnolo  <Sergio.Grancagnolo@le.infn.it>  - U.Salento/INFN Le
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFCBTRACK_P2_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFCBTRACK_P2_H

#include <stdint.h>
#include <string>
#include "AthenaPoolUtilities/TPObjRef.h"

class TrigMuonEFCbTrack_p2
{
	friend class TrigMuonEFCbTrackCnv;

public:

	TrigMuonEFCbTrack_p2() {}
	virtual ~TrigMuonEFCbTrack_p2(){}

        //private:
	double m_matchChi2;
	unsigned short int m_nIdSctHits;
	unsigned short int m_nIdPixelHits;
	unsigned short int m_nTrtHits;

	TPObjRef m_TrigMuonEFTrack;
	TPObjRef m_P4IPtCotThPhiM;
};

#endif
