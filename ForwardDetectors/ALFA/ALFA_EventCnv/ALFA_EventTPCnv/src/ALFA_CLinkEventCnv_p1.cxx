/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "ALFA_CLinkEv/ALFA_CLinkEvent.h"
#undef private
#undef protected

#include "ALFA_EventTPCnv/ALFA_CLinkEvent_p1.h"
#include "ALFA_EventTPCnv/ALFA_CLinkEventCnv_p1.h"

void ALFA_CLinkEventCnv_p1::persToTrans(const ALFA_CLinkEvent_p1* persObj, ALFA_CLinkEvent* transObj, MsgStream &log)
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_CLinkEventCnv_p1");
	//LogStream<<MSG::INFO<<"MARK01-ALFA_CLinkEventCnv_p1::persToTrans"<<endreq;

	transObj->m_DCSId=persObj->m_DCSId;
	m_DLRawDataEvCollCnv.persToTrans(&persObj->m_RawDataEvColl,&transObj->m_RawDataEvColl,log);
	m_DLDigitEvCollCnv.persToTrans(&persObj->m_DigitEvColl,&transObj->m_DigitEvColl,log);
	m_DLODDigitEvCollCnv.persToTrans(&persObj->m_ODDigitEvColl,&transObj->m_ODDigitEvColl,log);
	m_DLLocRecEvCollCnv.persToTrans(&persObj->m_LocRecEvColl,&transObj->m_LocRecEvColl,log);
	m_DLLocRecODEvCollCnv.persToTrans(&persObj->m_LocRecODEvColl,&transObj->m_LocRecODEvColl,log);
	m_DLLocRecCorrEvCollCnv.persToTrans(&persObj->m_LocRecCorrEvColl,&transObj->m_LocRecCorrEvColl,log);
	m_DLLocRecCorrODEvCollCnv.persToTrans(&persObj->m_LocRecCorrODEvColl,&transObj->m_LocRecCorrODEvColl,log);
// 	m_DLGloRecEvCollCnv.persToTrans(&persObj->m_GloRecEvColl,&transObj->m_GloRecEvColl,log);

	//LogStream<<MSG::INFO<<"MARK02-ALFA_CLinkEventCnv_p1::persToTrans"<<endreq;
}
 
void ALFA_CLinkEventCnv_p1::transToPers(const ALFA_CLinkEvent* transObj, ALFA_CLinkEvent_p1* persObj,MsgStream &log)
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_CLinkEventCnv_p1");
	//LogStream<<MSG::INFO<<"MARK01-ALFA_CLinkEventCnv_p1::transToPers"<<endreq;

	persObj->m_DCSId=transObj->m_DCSId;
	m_DLRawDataEvCollCnv.transToPers(&transObj->m_RawDataEvColl,&persObj->m_RawDataEvColl,log);
	m_DLDigitEvCollCnv.transToPers(&transObj->m_DigitEvColl,&persObj->m_DigitEvColl,log);
	m_DLODDigitEvCollCnv.transToPers(&transObj->m_ODDigitEvColl,&persObj->m_ODDigitEvColl,log);
	m_DLLocRecEvCollCnv.transToPers(&transObj->m_LocRecEvColl,&persObj->m_LocRecEvColl,log);
	m_DLLocRecODEvCollCnv.transToPers(&transObj->m_LocRecODEvColl,&persObj->m_LocRecODEvColl,log);
	m_DLLocRecCorrEvCollCnv.transToPers(&transObj->m_LocRecCorrEvColl,&persObj->m_LocRecCorrEvColl,log);
	m_DLLocRecCorrODEvCollCnv.transToPers(&transObj->m_LocRecCorrODEvColl,&persObj->m_LocRecCorrODEvColl,log);
// 	m_DLGloRecEvCollCnv.transToPers(&transObj->m_GloRecEvColl,&persObj->m_GloRecEvColl,log);

	//LogStream<<MSG::INFO<<"MARK02-ALFA_CLinkEventCnv_p1::transToPers"<<endreq;
}
