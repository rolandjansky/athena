/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFTrack_p6
 *
 * @brief persistent partner for TrigMuonEFTrack
 *
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFTRACK_P6_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFTRACK_P6_H

#include <stdint.h>
#include "AthenaPoolUtilities/TPObjRef.h"

class TrigMuonEFTrack_p6
{
	friend class TrigMuonEFTrackCnv;

public:

	TrigMuonEFTrack_p6(){}
	virtual ~TrigMuonEFTrack_p6(){}

        //private:
    
    // float m_charge;
    // float m_d0;
    // float m_z0;
    // float m_chi2;
    // float m_chi2prob;
    // float m_posx;
    // float m_posy;
    // float m_posz;
    // this array stores all the floats listed above in the order they are listed here
    float m_allTheFloats[8];
    
    // unsigned short int m_nRpcHitsPhi;
    // unsigned short int m_nTgcHitsPhi;
    // unsigned short int m_nCscHitsPhi;
    // unsigned short int m_nRpcHitsEta;
    // unsigned short int m_nTgcHitsEta;
    // unsigned short int m_nCscHitsEta;
    // unsigned short int m_nMdtHits;
    // this array stores all the unsigned shorts listed above in the order they are listed here
    unsigned short int m_allTheInts[7];
    
	TPObjRef m_P4IPtCotThPhiM;
};

#endif
