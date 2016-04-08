/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFTrack_p3
 *
 * @brief persistent partner for TrigMuonEFTrack
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @author Sergio Grancagnolo  <Sergio.Grancagnolo@le.infn.it>  - U.Salento/INFN Le
 * @author Alexander Oh <alexander.oh@cern.ch>         - U. Manchester
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFTRACK_P4_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFTRACK_P4_H

#include <stdint.h>
#include <string>
#include "AthenaPoolUtilities/TPObjRef.h"

class TrigMuonEFTrack_p4
{
	friend class TrigMuonEFTrackCnv;

public:

	TrigMuonEFTrack_p4() {}
	virtual ~TrigMuonEFTrack_p4(){}

        //private:
	float m_charge;
	float m_d0;
	float m_z0;
	float m_chi2;
	float m_chi2prob;
	float m_posx;
	float m_posy;
	float m_posz;
	unsigned short int m_nRpcHitsPhi;
	unsigned short int m_nTgcHitsPhi;
	unsigned short int m_nCscHitsPhi;
	unsigned short int m_nRpcHitsEta;
	unsigned short int m_nTgcHitsEta;
	unsigned short int m_nCscHitsEta;
	TPObjRef m_P4IPtCotThPhiM;
};

#endif
