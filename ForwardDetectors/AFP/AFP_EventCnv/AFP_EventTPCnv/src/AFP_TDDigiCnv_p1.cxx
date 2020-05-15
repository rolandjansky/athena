/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_DigiEv/AFP_TDDigi.h"

#include "Identifier/Identifier.h"
#include "AFP_EventTPCnv/AFP_TDDigiCnv_p1.h"

void AFP_TDDigiCnv_p1::persToTrans(const AFP_TDDigi_p1* persObj, AFP_TDDigi* transObj, MsgStream &log)
const {
	log << MSG::DEBUG << "AFP_TDDigiCnv_p1::persToTrans called " << endmsg;


	transObj->m_fADC=persObj->m_fADC;                    
	transObj->m_fTDC=persObj->m_fTDC;                      

	transObj->m_nStationID=persObj->m_nStationID;
	transObj->m_nDetectorID=persObj->m_nDetectorID;
	transObj->m_nSensitiveElementID=persObj->m_nSensitiveElementID;
}

void AFP_TDDigiCnv_p1::transToPers(const AFP_TDDigi* transObj, AFP_TDDigi_p1* persObj, MsgStream &log)
const {
	log << MSG::DEBUG << "AFP_TDDigiCnv_p1::transToPers called " << endmsg;

        persObj->m_fADC=transObj->m_fADC;
        persObj->m_fTDC=transObj->m_fTDC;

	persObj->m_nStationID=transObj->m_nStationID;
	persObj->m_nDetectorID=transObj->m_nDetectorID;
	persObj->m_nSensitiveElementID=transObj->m_nSensitiveElementID;
 }
