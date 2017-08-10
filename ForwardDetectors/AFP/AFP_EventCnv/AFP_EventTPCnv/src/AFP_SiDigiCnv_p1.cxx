/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#define private public
//#define protected public
#include "AFP_DigiEv/AFP_SiDigi.h"
//#undef private
//#undef protected

#include "Identifier/Identifier.h"
#include "AFP_EventTPCnv/AFP_SiDigiCnv_p1.h"

void AFP_SiDigiCnv_p1::persToTrans(const AFP_SiDigi_p1* persObj, AFP_SiDigi* transObj, MsgStream &log)
{
	log << MSG::DEBUG << "AFP_SiDigiCnv_p1::persToTrans called " << endmsg;


	transObj->m_fADC=persObj->m_fADC;                    
	transObj->m_fTDC=persObj->m_fTDC;                      

	transObj->m_nStationID=persObj->m_nStationID;
	transObj->m_nDetectorID=persObj->m_nDetectorID;
	transObj->m_nPixelRow=persObj->m_nPixelRow;
	transObj->m_nPixelCol=persObj->m_nPixelCol;
}

void AFP_SiDigiCnv_p1::transToPers(const AFP_SiDigi* transObj, AFP_SiDigi_p1* persObj, MsgStream &log)
{
	log << MSG::DEBUG << "AFP_SiDigiCnv_p1::transToPers called " << endmsg;

      persObj->m_fADC=transObj->m_fADC;
      persObj->m_fTDC=transObj->m_fTDC;

	persObj->m_nStationID=transObj->m_nStationID;
	persObj->m_nDetectorID=transObj->m_nDetectorID;	
	persObj->m_nPixelRow=transObj->m_nPixelRow;
	persObj->m_nPixelCol=transObj->m_nPixelCol;
 }
