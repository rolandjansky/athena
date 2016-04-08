/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFInfo_p3
 *
 * @brief persistent partner for TrigMuonEFInfo
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @author Sergio Grancagnolo  <Sergio.Grancagnolo@le.infn.it> - U.Salento/INFN Le
 * @author Alexander Oh  <alexander.oh@cern.ch>        - U. Manchester
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFINFO_P3_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFINFO_P3_H

#include <stdint.h>
#include <string>
#include "AthenaPoolUtilities/TPObjRef.h"

class TrigMuonEFInfo_p3
{
	friend class TrigMuonEFInfoCnv;

public:

	TrigMuonEFInfo_p3() {}
	virtual ~TrigMuonEFInfo_p3(){}

        //private:
	unsigned short int m_roi;
	unsigned short int m_nSegments;
	unsigned short int m_nMdtHits;
	unsigned short int m_nRpcHits;
	unsigned short int m_nTgcHits;
	unsigned short int m_nCscHits;
	double m_etaPreviousLevel;
	double m_phiPreviousLevel;

	TPObjRef m_spectrometerTrack;
	TPObjRef m_extrapolatedTrack;
	TPObjRef m_combinedTrack;
	TPObjRef m_trackContainer;

};

#endif
