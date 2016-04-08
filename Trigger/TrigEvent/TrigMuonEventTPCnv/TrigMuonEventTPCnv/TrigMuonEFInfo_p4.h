/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFInfo_p4
 *
 * @brief persistent partner for TrigMuonEFInfo
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @author Sergio Grancagnolo  <Sergio.Grancagnolo@le.infn.it> - U.Salento/INFN Le
 * @author Alexander Oh  <alexander.oh@cern.ch>        - U. Manchester
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFINFO_P4_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFINFO_P4_H

#include <stdint.h>
#include <string>
#include "AthenaPoolUtilities/TPObjRef.h"

class TrigMuonEFInfo_p4
{
	friend class TrigMuonEFInfoCnv;

public:

	TrigMuonEFInfo_p4() :
          m_roi(0), m_nSegments(0), m_nMdtHits(0),
          m_nRpcHits(0), m_nTgcHits(0), m_nCscHits(0),
          m_etaPreviousLevel(0.0), m_phiPreviousLevel(0.0)
          {}
	virtual ~TrigMuonEFInfo_p4(){}

        //private:
	unsigned short int m_roi;
	unsigned short int m_nSegments;
	unsigned short int m_nMdtHits;
	unsigned short int m_nRpcHits;
	unsigned short int m_nTgcHits;
	unsigned short int m_nCscHits;
	float m_etaPreviousLevel;
	float m_phiPreviousLevel;

	TPObjRef m_spectrometerTrack;
	TPObjRef m_extrapolatedTrack;
	TPObjRef m_combinedTrack;
	TPObjRef m_trackContainer;

};

#endif
