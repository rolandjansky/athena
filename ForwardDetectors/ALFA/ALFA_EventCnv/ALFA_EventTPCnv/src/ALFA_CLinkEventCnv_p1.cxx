/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_CLinkEv/ALFA_CLinkEvent.h"
#include "ALFA_EventTPCnv/ALFA_CLinkEvent_p1.h"
#include "ALFA_EventTPCnv/ALFA_CLinkEventCnv_p1.h"

void ALFA_CLinkEventCnv_p1::persToTrans(const ALFA_CLinkEvent_p1* persObj, ALFA_CLinkEvent* transObj, MsgStream &log)
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_CLinkEventCnv_p1");
	//LogStream<<MSG::INFO<<"MARK01-ALFA_CLinkEventCnv_p1::persToTrans"<<endreq;

        transObj->SetDCSFolderIDs (&persObj->m_DCSId);

	DataLink<ALFA_RawDataContainer> rawDataEvCollLink;
	m_DLRawDataEvCollCnv.persToTrans(&persObj->m_RawDataEvColl,&rawDataEvCollLink,log);
        transObj->setRawDataEvCollLink (rawDataEvCollLink);

	DataLink<ALFA_DigitCollection> digitEvCollLink;
	m_DLDigitEvCollCnv.persToTrans(&persObj->m_DigitEvColl,&digitEvCollLink,log);
        transObj->setDigitEvCollLink (digitEvCollLink);

        DataLink<ALFA_ODDigitCollection> ODDigitEvCollLink;
	m_DLODDigitEvCollCnv.persToTrans(&persObj->m_ODDigitEvColl,&ODDigitEvCollLink,log);
        transObj->setODDigitEvCollLink (ODDigitEvCollLink);
        

	DataLink<ALFA_LocRecEvCollection> locRecEvCollLink;
	m_DLLocRecEvCollCnv.persToTrans(&persObj->m_LocRecEvColl,&locRecEvCollLink,log);
        transObj->setLocRecEvCollLink (locRecEvCollLink);

	DataLink<ALFA_LocRecODEvCollection> locRecODEvCollLink;
	m_DLLocRecODEvCollCnv.persToTrans(&persObj->m_LocRecODEvColl,&locRecODEvCollLink,log);
        transObj->setLocRecODEvCollLink (locRecODEvCollLink);

	DataLink<ALFA_LocRecCorrEvCollection> locRecCorrEvCollLink;
	m_DLLocRecCorrEvCollCnv.persToTrans(&persObj->m_LocRecCorrEvColl,&locRecCorrEvCollLink,log);
        transObj->setLocRecCorrEvCollLink (locRecCorrEvCollLink);

	DataLink<ALFA_LocRecCorrODEvCollection> locRecCorrODEvCollLink;
	m_DLLocRecCorrODEvCollCnv.persToTrans(&persObj->m_LocRecCorrODEvColl,&locRecCorrODEvCollLink,log);
        transObj->setLocRecCorrODEvCollLink (locRecCorrODEvCollLink);

// 	m_DLGloRecEvCollCnv.persToTrans(&persObj->m_GloRecEvColl,&transObj->m_GloRecEvColl,log);

	//LogStream<<MSG::INFO<<"MARK02-ALFA_CLinkEventCnv_p1::persToTrans"<<endreq;
}
 
void ALFA_CLinkEventCnv_p1::transToPers(const ALFA_CLinkEvent* transObj, ALFA_CLinkEvent_p1* persObj,MsgStream &log)
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_CLinkEventCnv_p1");
	//LogStream<<MSG::INFO<<"MARK01-ALFA_CLinkEventCnv_p1::transToPers"<<endreq;

        persObj->m_DCSId=transObj->DCSId();

	m_DLRawDataEvCollCnv.transToPers(&transObj->rawDataEvCollLink(),&persObj->m_RawDataEvColl,log);
	m_DLDigitEvCollCnv.transToPers(&transObj->digitEvCollLink(),&persObj->m_DigitEvColl,log);
	m_DLODDigitEvCollCnv.transToPers(&transObj->ODDigitEvCollLink(),&persObj->m_ODDigitEvColl,log);
	m_DLLocRecEvCollCnv.transToPers(&transObj->locRecEvCollLink(),&persObj->m_LocRecEvColl,log);
	m_DLLocRecODEvCollCnv.transToPers(&transObj->locRecODEvCollLink(),&persObj->m_LocRecODEvColl,log);
	m_DLLocRecCorrEvCollCnv.transToPers(&transObj->locRecCorrEvCollLink(),&persObj->m_LocRecCorrEvColl,log);
	m_DLLocRecCorrODEvCollCnv.transToPers(&transObj->locRecCorrODEvCollLink(),&persObj->m_LocRecCorrODEvColl,log);
// 	m_DLGloRecEvCollCnv.transToPers(&transObj->m_GloRecEvColl,&persObj->m_GloRecEvColl,log);

	//LogStream<<MSG::INFO<<"MARK02-ALFA_CLinkEventCnv_p1::transToPers"<<endreq;
}
