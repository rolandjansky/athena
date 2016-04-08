/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_CLinkEv/ALFA_CLinkEvent.h"


// Default constructor needed by athenaroot/athenapool
ALFA_CLinkEvent::ALFA_CLinkEvent()
{
	memset(&m_DCSId,0,sizeof(DCSID));
}

// destructor
ALFA_CLinkEvent::~ALFA_CLinkEvent()
{

}

StatusCode ALFA_CLinkEvent::AddLink(eDVType eType, const void* pObj)
{
	switch(eType){
	case EDVT_RAWDATAEVCOLLECTION:
		m_RawDataEvColl=DataLink<ALFA_RawDataContainer>(*((ALFA_RawDataContainer*)pObj));
		break;
	case EDVT_DIGITCOLLECTION:
		m_DigitEvColl=DataLink<ALFA_DigitCollection>(*((ALFA_DigitCollection*)pObj));
		break;
	case EDVT_ODDIGITCOLLECTION:
		m_ODDigitEvColl=DataLink<ALFA_ODDigitCollection>(*((ALFA_ODDigitCollection*)pObj));
		break;
	case EDVT_LOCRECEVCOLLECTION:
		m_LocRecEvColl=DataLink<ALFA_LocRecEvCollection>(*((ALFA_LocRecEvCollection*)pObj));
		break;
	case EDVT_LOCRECODEVCOLLECTION:
		m_LocRecODEvColl=DataLink<ALFA_LocRecODEvCollection>(*((ALFA_LocRecODEvCollection*)pObj));
		break;
	case EDVT_LOCRECCORREVCOLLECTION:
		m_LocRecCorrEvColl=DataLink<ALFA_LocRecCorrEvCollection>(*((ALFA_LocRecCorrEvCollection*)pObj));
		break;
	case EDVT_LOCRECCORRODEVCOLLECTION:
		m_LocRecCorrODEvColl=DataLink<ALFA_LocRecCorrODEvCollection>(*((ALFA_LocRecCorrODEvCollection*)pObj));
		break;
// 	case EDVT_GLORECEVCOLLECTION:
// 		m_GloRecEvColl=DataLink<ALFA_GloRecEvCollection>(*((ALFA_GloRecEvCollection*)pObj));
// 		break;
	default:
		break;
	}

	return StatusCode::SUCCESS;
}

const void* ALFA_CLinkEvent::GetLinkedObject(eDVType eType) const
{
	const void* pObj=NULL;

	switch(eType){
	case EDVT_RAWDATAEVCOLLECTION:
		pObj=m_RawDataEvColl.getDataPtr();
		break;
	case EDVT_DIGITCOLLECTION:
		pObj=m_DigitEvColl.getDataPtr();
		break;
	case EDVT_ODDIGITCOLLECTION:
		pObj=m_ODDigitEvColl.getDataPtr();
		break;
	case EDVT_LOCRECEVCOLLECTION:
		pObj=m_LocRecEvColl.getDataPtr();
		break;
	case EDVT_LOCRECODEVCOLLECTION:
		pObj=m_LocRecODEvColl.getDataPtr();
		break;
	case EDVT_LOCRECCORREVCOLLECTION:
		pObj=m_LocRecCorrEvColl.getDataPtr();
		break;
	case EDVT_LOCRECCORRODEVCOLLECTION:
		pObj=m_LocRecCorrODEvColl.getDataPtr();
		break;
// 	case EDVT_GLORECEVCOLLECTION:
// 		pObj=m_GloRecEvColl.getDataPtr();
// 		break;
	default:
		break;
	}

	return pObj;
}

void ALFA_CLinkEvent::SetDCSFolderID(eDCSItem eItem, const unsigned long long ullID)
{
	switch(eItem){
	case EDCSI_BLM:
		m_DCSId.ullBlmID=ullID;
		break;
	case EDCSI_HVCHANNEL:
		m_DCSId.ullHVChannelID=ullID;
		break;
	case EDCSI_LOCALMONITORING:
		m_DCSId.ullLocalMonitoringID=ullID;
		break;
	case EDCSI_MOVEMENT:
		m_DCSId.ullMovementID=ullID;
		break;
	case EDCSI_RADMON:
		m_DCSId.ullRadMonID=ullID;
		break;
	case EDCSI_TRIGGERRATES:
		m_DCSId.ullTriggerRatesID=ullID;
		break;
	case EDCSI_FECONFIGURATION:
		m_DCSId.ullFEConfigurationID=ullID;
		break;
	case EDCSI_TRIGGERSETTINGS:
		m_DCSId.ullTriggerSettingsID=ullID;
		break;
	default:
		break;
	}
}

void ALFA_CLinkEvent::SetDCSFolderIDs(const DCSID* pDCSIds)
{
	if(pDCSIds!=NULL) memcpy(&m_DCSId,pDCSIds,sizeof(DCSID));
}

unsigned long long ALFA_CLinkEvent::GetDCSFolderID(eDCSItem eItem) const
{
	unsigned long long ullID=0;

	switch(eItem){
	case EDCSI_BLM:
		ullID=m_DCSId.ullBlmID;
		break;
	case EDCSI_HVCHANNEL:
		ullID=m_DCSId.ullHVChannelID;
		break;
	case EDCSI_LOCALMONITORING:
		ullID=m_DCSId.ullLocalMonitoringID;
		break;
	case EDCSI_MOVEMENT:
		ullID=m_DCSId.ullMovementID;
		break;
	case EDCSI_RADMON:
		ullID=m_DCSId.ullRadMonID;
		break;
	case EDCSI_TRIGGERRATES:
		ullID=m_DCSId.ullTriggerRatesID;
		break;
	case EDCSI_FECONFIGURATION:
		ullID=m_DCSId.ullFEConfigurationID;
		break;
	case EDCSI_TRIGGERSETTINGS:
		ullID=m_DCSId.ullTriggerSettingsID;
		break;
	default:
		break;
	}

	return ullID;
}

