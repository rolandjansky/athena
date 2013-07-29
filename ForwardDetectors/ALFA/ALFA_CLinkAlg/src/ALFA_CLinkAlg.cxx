/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_CLinkAlg/ALFA_CLinkAlg.h"
#include "AthenaKernel/errorcheck.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

ALFA_CLinkAlg::ALFA_CLinkAlg(const std::string& name, ISvcLocator* pSvcLocator)
	: AthAlgorithm(name, pSvcLocator), m_iovSvc( "IOVDbSvc", name )//, m_incidentSvc( "IncidentSvc", name )
{
	// data type using in the local reconstruction
	// for the simulation data the value is 0, for the real data the value is 1. Unset value is -1
	declareProperty("DataType", m_nDataType=1, "data type using in the local reconstruction");


	declareProperty("ProcessingMode", m_nProcessingMode=2, "Processing mode, 1=offline, 2=online");

}


ALFA_CLinkAlg::~ALFA_CLinkAlg()
{
}

StatusCode ALFA_CLinkAlg::initialize()
{
	ATH_MSG_DEBUG ("ALFA_CLinkAlg::initialize()");
	StatusCode sc=StatusCode::FAILURE;

	if((sc=m_iovSvc.retrieve())!=StatusCode::SUCCESS){
		return sc;
	}

	if (m_nDataType==1)
	{
		memset(&m_CurrentDCSId,0,sizeof(DCSID));

		//register IOV/COOL callbacks
		CHECK(AddCOOLFolderCallback(DCSCOLLNAME_BLM));
		CHECK(AddCOOLFolderCallback(DCSCOLLNAME_HVCHANNEL));
		CHECK(AddCOOLFolderCallback(DCSCOLLNAME_LOCALMONITORING));
		CHECK(AddCOOLFolderCallback(DCSCOLLNAME_MOVEMENT));
		CHECK(AddCOOLFolderCallback(DCSCOLLNAME_RADMON));
		CHECK(AddCOOLFolderCallback(DCSCOLLNAME_TRIGGERRATES));
		CHECK(AddCOOLFolderCallback(DCSCOLLNAME_FECONFIGURATION));
		CHECK(AddCOOLFolderCallback(DCSCOLLNAME_TRIGGERSETTINGS));
	}

	return sc;
}

StatusCode ALFA_CLinkAlg::execute()
{
	ATH_MSG_DEBUG ("ALFA_CLinkAlg::execute()");

	ALFA_CLinkEvent* pDataEvent=new ALFA_CLinkEvent();
	CHECK(LoadAllEventData(pDataEvent));
	if (m_nDataType==1) pDataEvent->SetDCSFolderIDs(&m_CurrentDCSId);
	CHECK(evtStore()->record(pDataEvent, "ALFA_CLinkEvent"));

	return StatusCode::SUCCESS;
}

StatusCode ALFA_CLinkAlg::finalize()
{
	ATH_MSG_DEBUG ("ALFA_CLinkAlg::finalize()");

	return StatusCode::SUCCESS;
}

StatusCode ALFA_CLinkAlg::LoadAllEventData(ALFA_CLinkEvent* pDataEvent)
{
	StatusCode sc;

	//RawDataContainer
	if (m_nDataType==1)
	{
		const ALFA_RawDataContainer* pAuxRawDataColl=NULL;
		sc = evtStore()->retrieve(pAuxRawDataColl, EVCOLLNAME_RAWDATA);
		if(sc.isFailure() || !pAuxRawDataColl) {
			msg(MSG::WARNING) << "Container '"<<EVCOLLNAME_RAWDATA<<"' not found" << endreq;
			//return StatusCode::FAILURE;
		}
		else CHECK(pDataEvent->AddLink(EDVT_RAWDATAEVCOLLECTION, pAuxRawDataColl));
	}

	//DigitCollection
	const ALFA_DigitCollection* pAuxDigitColl;
	sc = evtStore()->retrieve(pAuxDigitColl, EVCOLLNAME_DIGIT);
	if(sc.isFailure() || !pAuxDigitColl) {
		msg(MSG::WARNING) << "Container '"<<EVCOLLNAME_DIGIT<<"' not found" << endreq;
		//return StatusCode::FAILURE;
	}
	else CHECK(pDataEvent->AddLink(EDVT_DIGITCOLLECTION, pAuxDigitColl));

	//ODDigitCollection
	const ALFA_ODDigitCollection* pAuxODDigitColl;
	sc = evtStore()->retrieve(pAuxODDigitColl, EVCOLLNAME_ODDIGIT);
	if(sc.isFailure() || !pAuxODDigitColl) {
		msg(MSG::WARNING) << "Container '"<<EVCOLLNAME_ODDIGIT<<"' not found" << endreq;
		//return StatusCode::FAILURE;
	}
	else CHECK(pDataEvent->AddLink(EDVT_ODDIGITCOLLECTION, pAuxODDigitColl));

	//LocRecEvCollection
	const ALFA_LocRecEvCollection* pAuxLocRecEvColl;
	sc = evtStore()->retrieve(pAuxLocRecEvColl, EVCOLLNAME_LOCREC);
	if(sc.isFailure() || !pAuxLocRecEvColl) {
		msg(MSG::WARNING) << "Container '"<<EVCOLLNAME_LOCREC<<"' not found" << endreq;
		//return StatusCode::FAILURE;
	}
	else CHECK(pDataEvent->AddLink(EDVT_LOCRECEVCOLLECTION, pAuxLocRecEvColl));

	//LocRecODEvCollection
	const ALFA_LocRecODEvCollection* pAuxLocRecODEvColl;
	sc = evtStore()->retrieve(pAuxLocRecODEvColl, EVCOLLNAME_LOCRECOD);
	if(sc.isFailure() || !pAuxLocRecODEvColl) {
		msg(MSG::WARNING) << "Container '"<<EVCOLLNAME_LOCRECOD<<"' not found" << endreq;
		//return StatusCode::FAILURE;
	}
	else CHECK(pDataEvent->AddLink(EDVT_LOCRECODEVCOLLECTION, pAuxLocRecODEvColl));

	//LocRecCorrEvCollection
	const ALFA_LocRecCorrEvCollection* pAuxLocRecCorrEvColl;
	sc = evtStore()->retrieve(pAuxLocRecCorrEvColl, EVCOLLNAME_LOCRECCORR);
	if(sc.isFailure() || !pAuxLocRecCorrEvColl) {
		msg(MSG::WARNING) << "Container '"<<EVCOLLNAME_LOCRECCORR<<"' not found" << endreq;
		//return StatusCode::FAILURE;
	}
	else CHECK(pDataEvent->AddLink(EDVT_LOCRECCORREVCOLLECTION, pAuxLocRecCorrEvColl));

	//LocRecCorrODEvCollection
	const ALFA_LocRecCorrODEvCollection* pAuxLocRecCorrODEvColl;
	sc = evtStore()->retrieve(pAuxLocRecCorrODEvColl, EVCOLLNAME_LOCRECCORROD);
	if(sc.isFailure() || !pAuxLocRecCorrODEvColl) {
		msg(MSG::WARNING) << "Container '"<<EVCOLLNAME_LOCRECCORROD<<"' not found" << endreq;
		//return StatusCode::FAILURE;
	}
	else CHECK(pDataEvent->AddLink(EDVT_LOCRECCORRODEVCOLLECTION, pAuxLocRecCorrODEvColl));

	//GloRecEvCollection
	//const ALFA_GloRecEvCollection* pAuxGloRecEvColl;
	//sc = evtStore()->retrieve(pAuxGloRecEvColl, EVCOLLNAME_GLOREC);
	//if(sc.isFailure() || !pAuxGloRecEvColl) {
	//	msg(MSG::WARNING) << "Container '"<<EVCOLLNAME_GLOREC<<"' not found" << endreq;
	//	return StatusCode::FAILURE;
	//}
	//else CHECK(pDataEvent->AddLink(EDVT_GLORECEVCOLLECTION, pAuxGloRecEvColl));

	return StatusCode::SUCCESS;

}

StatusCode ALFA_CLinkAlg::AddCOOLFolderCallback(const string& Folder)
{
	StatusCode sc=StatusCode::FAILURE;

	const DataHandle<CondAttrListCollection> DataPtr;
	sc=detStore()->regFcn(&ALFA_CLinkAlg::COOLUpdate, this, DataPtr, Folder, true);
	if(sc!=StatusCode::SUCCESS){
		msg(MSG::ERROR) << "Cannot register COOL callback for folder '"<<Folder<<"'" << endreq;
	}

	return sc;
}

//StatusCode ALFA_CLinkAlg::COOLUpdate(IOVSVC_CALLBACK_ARGS_P(/*I*/, keys))
StatusCode ALFA_CLinkAlg::COOLUpdate(IOVSVC_CALLBACK_ARGS_K(keys))
{
	const EventInfo* pEvInfo;
	CHECK(evtStore()->retrieve(pEvInfo));

	list<string>::const_iterator iter;

	for(iter=keys.begin();iter!=keys.end();iter++){
		if((*iter)==DCSCOLLNAME_BLM){
			msg(MSG::DEBUG) << " IOV/COOL Notification '"<<DCSCOLLNAME_BLM<<"'" << endreq;
			m_CurrentDCSId.ullBlmID=CalcDCSId(EDCSI_BLM);
		}
		else if((*iter)==DCSCOLLNAME_HVCHANNEL){
			msg(MSG::DEBUG) << " IOV/COOL Notification '"<<DCSCOLLNAME_HVCHANNEL<<"'" << endreq;
			m_CurrentDCSId.ullHVChannelID=CalcDCSId(EDCSI_HVCHANNEL);
		}
		else if((*iter)==DCSCOLLNAME_LOCALMONITORING){
			msg(MSG::DEBUG) << " IOV/COOL Notification '"<<DCSCOLLNAME_LOCALMONITORING<<"'" << endreq;
			m_CurrentDCSId.ullLocalMonitoringID=CalcDCSId(EDCSI_LOCALMONITORING);
		}
		else if((*iter)==DCSCOLLNAME_MOVEMENT){
			msg(MSG::DEBUG) << " IOV/COOL Notification '"<<DCSCOLLNAME_MOVEMENT<<"'" << endreq;
			m_CurrentDCSId.ullMovementID=CalcDCSId(EDCSI_MOVEMENT);
		}
		else if((*iter)==DCSCOLLNAME_RADMON){
			msg(MSG::DEBUG) << " IOV/COOL Notification '"<<DCSCOLLNAME_RADMON<<"'" << endreq;
			m_CurrentDCSId.ullRadMonID=CalcDCSId(EDCSI_RADMON);
		}
		else if((*iter)==DCSCOLLNAME_TRIGGERRATES){
			msg(MSG::DEBUG) << " IOV/COOL Notification '"<<DCSCOLLNAME_TRIGGERRATES<<"'" << endreq;
			m_CurrentDCSId.ullTriggerRatesID=CalcDCSId(EDCSI_TRIGGERRATES);
		}
		else if((*iter)==DCSCOLLNAME_FECONFIGURATION){
			msg(MSG::DEBUG) << " IOV/COOL Notification '"<<DCSCOLLNAME_FECONFIGURATION<<"'" << endreq;
			m_CurrentDCSId.ullFEConfigurationID=CalcDCSId(EDCSI_FECONFIGURATION);
		}
		else if((*iter)==DCSCOLLNAME_TRIGGERSETTINGS){
			msg(MSG::DEBUG) << " IOV/COOL Notification '"<<DCSCOLLNAME_TRIGGERSETTINGS<<"'" << endreq;
			m_CurrentDCSId.ullTriggerSettingsID=CalcDCSId(EDCSI_TRIGGERSETTINGS);
		}
	}

	return StatusCode::SUCCESS;
}

unsigned long long ALFA_CLinkAlg::CalcDCSId(eDCSItem eItem)
{
	unsigned long long ullID;
	string Folder, Foldername, Tag;
	IOVRange Range;
	bool bRetrieved;
	unsigned long long ullBytesRead;
	float fReadTime;

	switch(eItem)
	{
	case EDCSI_BLM:
		Folder=DCSCOLLNAME_BLM;
		break;
	case EDCSI_HVCHANNEL:
		Folder=DCSCOLLNAME_HVCHANNEL;
		break;
	case EDCSI_LOCALMONITORING:
		Folder=DCSCOLLNAME_LOCALMONITORING;
		break;
	case EDCSI_MOVEMENT:
		Folder=DCSCOLLNAME_MOVEMENT;
		break;
	case EDCSI_RADMON:
		Folder=DCSCOLLNAME_RADMON;
		break;
	case EDCSI_TRIGGERRATES:
		Folder=DCSCOLLNAME_TRIGGERRATES;
		break;
	case EDCSI_FECONFIGURATION:
		Folder=DCSCOLLNAME_FECONFIGURATION;
		break;
	case EDCSI_TRIGGERSETTINGS:
		Folder=DCSCOLLNAME_TRIGGERSETTINGS;
		break;
	default:
		break;
	}

	const CondAttrListCollection* pAttrListCol=NULL;
	CHECK(detStore()->retrieve(pAttrListCol,Folder));
	if(!m_iovSvc->getKeyInfo(Folder,Foldername,Tag,Range,bRetrieved,ullBytesRead,fReadTime)) {
		msg(MSG::ERROR)<<"Couldn't get IOV data about folder: "<<Folder<<endreq;
		return 0;
	}

	// Construct the ID:
	IOVTime time=Range.start();
	if(time.isRunEvent()){
		ullID=static_cast<unsigned long long>(((time.run()&0xffff)<<16)|(time.event()&0xffff));
	}
	else if(time.isTimestamp()){
		//ullID=static_cast<unsigned long long>(time.timestamp()&0xffffffff);
		ullID=static_cast<unsigned long long>(time.timestamp());
	}
	else{
		ullID=0;
	}

	return ullID;
}

StatusCode ALFA_CLinkAlg::CalcAllDCSIds(PDCSID pDCSIds)
{
	bool bRes=true;

	if(pDCSIds!=NULL){
		memset(pDCSIds,0,sizeof(DCSID));
		bRes&=(pDCSIds->ullBlmID=CalcDCSId(EDCSI_BLM))>0;
		bRes&=(pDCSIds->ullHVChannelID=CalcDCSId(EDCSI_HVCHANNEL))>0;
		bRes&=(pDCSIds->ullLocalMonitoringID=CalcDCSId(EDCSI_LOCALMONITORING))>0;
		bRes&=(pDCSIds->ullMovementID=CalcDCSId(EDCSI_MOVEMENT))>0;
		bRes&=(pDCSIds->ullRadMonID=CalcDCSId(EDCSI_RADMON))>0;
		bRes&=(pDCSIds->ullTriggerRatesID=CalcDCSId(EDCSI_TRIGGERRATES))>0;
		bRes&=(pDCSIds->ullFEConfigurationID=CalcDCSId(EDCSI_FECONFIGURATION))>0;
		bRes&=(pDCSIds->ullTriggerSettingsID=CalcDCSId(EDCSI_TRIGGERSETTINGS))>0;
	}
	else bRes=false;

	return bRes? StatusCode::SUCCESS:StatusCode::FAILURE;
}
