/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFInfoTrack_p1
 *
 * @brief persistent partner for TrigMuonEFInfo
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @author Alexander Oh  <alexander.oh@cern.ch> - U. Manchester
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFINFOTRACK_P1_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFINFOTRACK_P1_H

// #include <stdint.h>
// #include <string>
#include "AthenaPoolUtilities/TPObjRef.h"

class TrigMuonEFInfoTrack_p1
{
	friend class TrigMuonEFInfoTrackCnv;

public:

	TrigMuonEFInfoTrack_p1() {}
	virtual ~TrigMuonEFInfoTrack_p1(){}

        //private:
	unsigned short int m_muonType;
	TPObjRef m_spectrometerTrack;
	TPObjRef m_extrapolatedTrack;
	TPObjRef m_combinedTrack;

};

#endif
