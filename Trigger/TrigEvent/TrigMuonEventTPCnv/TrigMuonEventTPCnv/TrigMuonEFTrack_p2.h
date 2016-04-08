/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFTrack_p2
 *
 * @brief persistent partner for TrigMuonEFTrack
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @author Sergio Grancagnolo  <Sergio.Grancagnolo@le.infn.it>  - U.Salento/INFN Le
 * @author Alexander Oh <alexander.oh@cern.ch>         - U. Manchester
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFTRACK_P2_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFTRACK_P2_H

#include <stdint.h>
#include <string>
#include "AthenaPoolUtilities/TPObjRef.h"

class TrigMuonEFTrack_p2
{
	friend class TrigMuonEFTrackCnv;

public:

	TrigMuonEFTrack_p2() {}
	virtual ~TrigMuonEFTrack_p2(){}

        //private:
	double m_charge;
	double m_d0;
	double m_z0;
	double m_chi2;
	double m_chi2prob;
	double m_posx;
	double m_posy;
	double m_posz;
	unsigned short int m_nMdtHitsPhi;
	unsigned short int m_nRpcHitsPhi;
	unsigned short int m_nTgcHitsPhi;
	unsigned short int m_nCscHitsPhi;
	unsigned short int m_nMdtHitsEta;
	unsigned short int m_nRpcHitsEta;
	unsigned short int m_nTgcHitsEta;
	unsigned short int m_nCscHitsEta;
	TPObjRef m_P4IPtCotThPhiM;
};

#endif
