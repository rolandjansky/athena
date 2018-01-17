/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/errorcheck.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "xAODForward/ALFAData.h"
#include "xAODForward/ALFADataContainer.h"
#include "xAODForward/ALFADataAuxContainer.h"
#include "ALFA_CLinkAlg/ALFA_CLinkAlg.h"

using namespace std;

ALFA_CLinkAlg::ALFA_CLinkAlg(const std::string& name, ISvcLocator* pSvcLocator)
	: AthAlgorithm(name, pSvcLocator), m_iovSvc( "IOVDbSvc", name )//, m_incidentSvc( "IncidentSvc", name )
{
	// data type using in the local reconstruction
	// for the simulation data the value is 0, for the real data the value is 1. Unset value is -1
	declareProperty("DataType", m_nDataType=1, "data type using in the local reconstruction");
	declareProperty("ProcessingMode", m_nProcessingMode=2, "Processing mode, 1=offline, 2=online");

	m_nMaxTrackCnt=0;
	memset(&m_CurrentDCSId,0,sizeof(DCSID));

}


ALFA_CLinkAlg::~ALFA_CLinkAlg()
{

}

StatusCode ALFA_CLinkAlg::initialize()
{
	ATH_MSG_DEBUG ("ALFA_CLinkAlg::initialize()");
	StatusCode sc=StatusCode::FAILURE;

	if((sc=m_iovSvc.retrieve())!=StatusCode::SUCCESS)
	{
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

	StatusCode sc=StatusCode::SUCCESS;

	ALFA_CLinkEvent* pDataEvent=new ALFA_CLinkEvent();

	if(pDataEvent)
	{
		sc=LoadAllEventData(pDataEvent);
		if(sc!=StatusCode::SUCCESS)
		{
			delete pDataEvent;
			sc=StatusCode::FAILURE;
		}
		else
		{
			if (m_nDataType==1) pDataEvent->SetDCSFolderIDs(&m_CurrentDCSId);
			sc=evtStore()->record(pDataEvent, "ALFA_CLinkEvent");

			if(sc!=StatusCode::SUCCESS)
			{
				delete pDataEvent;
				sc=StatusCode::FAILURE;
			}
			else
			{
				sc=GenerateXAOD();
			}
		}
	}
	else
	{
		sc=StatusCode::FAILURE;
	}

	return sc;
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
		if(sc.isFailure() || !pAuxRawDataColl)
		{
			msg(MSG::WARNING) << "Container '"<<EVCOLLNAME_RAWDATA<<"' not found" << endmsg;
			//return StatusCode::FAILURE;
		}
		else CHECK(pDataEvent->AddLink(EDVT_RAWDATAEVCOLLECTION, pAuxRawDataColl));
	}

	//DigitCollection
	const ALFA_DigitCollection* pAuxDigitColl;
	sc = evtStore()->retrieve(pAuxDigitColl, EVCOLLNAME_DIGIT);
	if(sc.isFailure() || !pAuxDigitColl) {
		msg(MSG::WARNING) << "Container '"<<EVCOLLNAME_DIGIT<<"' not found" << endmsg;
		//return StatusCode::FAILURE;
	}
	else CHECK(pDataEvent->AddLink(EDVT_DIGITCOLLECTION, pAuxDigitColl));

	//ODDigitCollection
	const ALFA_ODDigitCollection* pAuxODDigitColl;
	sc = evtStore()->retrieve(pAuxODDigitColl, EVCOLLNAME_ODDIGIT);
	if(sc.isFailure() || !pAuxODDigitColl) {
		msg(MSG::WARNING) << "Container '"<<EVCOLLNAME_ODDIGIT<<"' not found" << endmsg;
		//return StatusCode::FAILURE;
	}
	else CHECK(pDataEvent->AddLink(EDVT_ODDIGITCOLLECTION, pAuxODDigitColl));

	//LocRecEvCollection
	const ALFA_LocRecEvCollection* pAuxLocRecEvColl;
	sc = evtStore()->retrieve(pAuxLocRecEvColl, EVCOLLNAME_LOCREC);
	if(sc.isFailure() || !pAuxLocRecEvColl) {
		msg(MSG::WARNING) << "Container '"<<EVCOLLNAME_LOCREC<<"' not found" << endmsg;
		//return StatusCode::FAILURE;
	}
	else CHECK(pDataEvent->AddLink(EDVT_LOCRECEVCOLLECTION, pAuxLocRecEvColl));

	//LocRecODEvCollection
	const ALFA_LocRecODEvCollection* pAuxLocRecODEvColl;
	sc = evtStore()->retrieve(pAuxLocRecODEvColl, EVCOLLNAME_LOCRECOD);
	if(sc.isFailure() || !pAuxLocRecODEvColl) {
		msg(MSG::WARNING) << "Container '"<<EVCOLLNAME_LOCRECOD<<"' not found" << endmsg;
		//return StatusCode::FAILURE;
	}
	else CHECK(pDataEvent->AddLink(EDVT_LOCRECODEVCOLLECTION, pAuxLocRecODEvColl));

	//LocRecCorrEvCollection
	const ALFA_LocRecCorrEvCollection* pAuxLocRecCorrEvColl;
	sc = evtStore()->retrieve(pAuxLocRecCorrEvColl, EVCOLLNAME_LOCRECCORR);
	if(sc.isFailure() || !pAuxLocRecCorrEvColl) {
		msg(MSG::WARNING) << "Container '"<<EVCOLLNAME_LOCRECCORR<<"' not found" << endmsg;
		//return StatusCode::FAILURE;
	}
	else CHECK(pDataEvent->AddLink(EDVT_LOCRECCORREVCOLLECTION, pAuxLocRecCorrEvColl));

	//LocRecCorrODEvCollection
	const ALFA_LocRecCorrODEvCollection* pAuxLocRecCorrODEvColl;
	sc = evtStore()->retrieve(pAuxLocRecCorrODEvColl, EVCOLLNAME_LOCRECCORROD);
	if(sc.isFailure() || !pAuxLocRecCorrODEvColl) {
		msg(MSG::WARNING) << "Container '"<<EVCOLLNAME_LOCRECCORROD<<"' not found" << endmsg;
		//return StatusCode::FAILURE;
	}
	else CHECK(pDataEvent->AddLink(EDVT_LOCRECCORRODEVCOLLECTION, pAuxLocRecCorrODEvColl));

	//GloRecEvCollection
	//const ALFA_GloRecEvCollection* pAuxGloRecEvColl;
	//sc = evtStore()->retrieve(pAuxGloRecEvColl, EVCOLLNAME_GLOREC);
	//if(sc.isFailure() || !pAuxGloRecEvColl) {
	//	msg(MSG::WARNING) << "Container '"<<EVCOLLNAME_GLOREC<<"' not found" << endmsg;
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
		msg(MSG::ERROR) << "Cannot register COOL callback for folder '"<<Folder<<"'" << endmsg;
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
			msg(MSG::DEBUG) << " IOV/COOL Notification '"<<DCSCOLLNAME_BLM<<"'" << endmsg;
			m_CurrentDCSId.ullBlmID=CalcDCSId(EDCSI_BLM);
		}
		else if((*iter)==DCSCOLLNAME_HVCHANNEL){
			msg(MSG::DEBUG) << " IOV/COOL Notification '"<<DCSCOLLNAME_HVCHANNEL<<"'" << endmsg;
			m_CurrentDCSId.ullHVChannelID=CalcDCSId(EDCSI_HVCHANNEL);
		}
		else if((*iter)==DCSCOLLNAME_LOCALMONITORING){
			msg(MSG::DEBUG) << " IOV/COOL Notification '"<<DCSCOLLNAME_LOCALMONITORING<<"'" << endmsg;
			m_CurrentDCSId.ullLocalMonitoringID=CalcDCSId(EDCSI_LOCALMONITORING);
		}
		else if((*iter)==DCSCOLLNAME_MOVEMENT){
			msg(MSG::DEBUG) << " IOV/COOL Notification '"<<DCSCOLLNAME_MOVEMENT<<"'" << endmsg;
			m_CurrentDCSId.ullMovementID=CalcDCSId(EDCSI_MOVEMENT);
		}
		else if((*iter)==DCSCOLLNAME_RADMON){
			msg(MSG::DEBUG) << " IOV/COOL Notification '"<<DCSCOLLNAME_RADMON<<"'" << endmsg;
			m_CurrentDCSId.ullRadMonID=CalcDCSId(EDCSI_RADMON);
		}
		else if((*iter)==DCSCOLLNAME_TRIGGERRATES){
			msg(MSG::DEBUG) << " IOV/COOL Notification '"<<DCSCOLLNAME_TRIGGERRATES<<"'" << endmsg;
			m_CurrentDCSId.ullTriggerRatesID=CalcDCSId(EDCSI_TRIGGERRATES);
		}
		else if((*iter)==DCSCOLLNAME_FECONFIGURATION){
			msg(MSG::DEBUG) << " IOV/COOL Notification '"<<DCSCOLLNAME_FECONFIGURATION<<"'" << endmsg;
			m_CurrentDCSId.ullFEConfigurationID=CalcDCSId(EDCSI_FECONFIGURATION);
		}
		else if((*iter)==DCSCOLLNAME_TRIGGERSETTINGS){
			msg(MSG::DEBUG) << " IOV/COOL Notification '"<<DCSCOLLNAME_TRIGGERSETTINGS<<"'" << endmsg;
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
	CHECK(detStore()->retrieve(pAttrListCol,Folder), 0);
	if(!m_iovSvc->getKeyInfo(Folder,Foldername,Tag,Range,bRetrieved,ullBytesRead,fReadTime)) {
		msg(MSG::ERROR)<<"Couldn't get IOV data about folder: "<<Folder<<endmsg;
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

StatusCode ALFA_CLinkAlg::GenerateXAOD()
{
	StatusCode sc=StatusCode::SUCCESS;

	xAOD::ALFADataContainer* pxAODContainer=new xAOD::ALFADataContainer();
	CHECK(evtStore()->record(pxAODContainer,EVCOLLNAME_XAODALFADATACONTAINER));
	xAOD::ALFADataAuxContainer* pxAODAuxContainer=new xAOD::ALFADataAuxContainer();
	CHECK(evtStore()->record(pxAODAuxContainer,EVCOLLNAME_XAODALFADATAAUXCONTAINER));
	pxAODContainer->setStore(pxAODAuxContainer);

	CHECK(FillXAOD_TrackingData(pxAODContainer));
	CHECK(FillXAOD_HeaderData(pxAODContainer));

	return sc;
}

StatusCode ALFA_CLinkAlg::FillXAOD_TrackingData(xAOD::ALFADataContainer* pxAODContainer)
{
	StatusCode sc=StatusCode::SUCCESS, sc2=StatusCode::SUCCESS;

	unsigned int i;
	int nPotID, nSideID, nODSign;
	int arrTrackCntPerRPot[RPOTSCNT];
	vector<int> vecFiberSel;

	//LocRecEvCollection & LocRecODEvCollection
	const ALFA_LocRecEvCollection* pLocRecEvColl;
	const ALFA_LocRecODEvCollection* pLocRecODEvColl;
	ALFA_LocRecEvCollection::const_iterator iterLocRec;
	ALFA_LocRecODEvCollection::const_iterator iterLocRecOD;
	sc=evtStore()->retrieve(pLocRecEvColl, EVCOLLNAME_LOCREC);
	sc2=evtStore()->retrieve(pLocRecODEvColl, EVCOLLNAME_LOCRECOD);

	if(!sc.isFailure() && !sc2.isFailure() && pLocRecEvColl!=NULL && pLocRecODEvColl!=NULL)
	{
		m_nMaxTrackCnt=1;
		// resolve max track count from LocRecEvCollection
		memset(&arrTrackCntPerRPot[0],0,sizeof(arrTrackCntPerRPot));
		for(iterLocRec=pLocRecEvColl->begin();iterLocRec!=pLocRecEvColl->end();iterLocRec++)
		{
			nPotID=(*iterLocRec)->getPotNum();
			arrTrackCntPerRPot[nPotID]++;
		}
		for(iterLocRecOD=pLocRecODEvColl->begin();iterLocRecOD!=pLocRecODEvColl->end();iterLocRecOD++)
		{
			nPotID=(*iterLocRecOD)->getPotNum();
			arrTrackCntPerRPot[nPotID]++;
		}
		for(i=0;i<RPOTSCNT;i++){
			if(arrTrackCntPerRPot[i]>m_nMaxTrackCnt) m_nMaxTrackCnt=arrTrackCntPerRPot[i];
		}

		memset(&arrTrackCntPerRPot[0],0,sizeof(arrTrackCntPerRPot));
		ClearXAODTrackingData(m_nMaxTrackCnt,ERC_LOCUNCORRECTED);

		//fill data - LocRecEvCollection
		vecFiberSel.clear();
		for(iterLocRec=pLocRecEvColl->begin();iterLocRec!=pLocRecEvColl->end();iterLocRec++)
		{
			nPotID=(*iterLocRec)->getPotNum();

			(m_vecDetectorPartID)[nPotID*m_nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=1;
			(m_vecXDetCS)[nPotID*m_nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRec)->getXposition();
			(m_vecYDetCS)[nPotID*m_nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRec)->getYposition();

			(m_vecOverU)[nPotID*m_nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRec)->getOverU();
			(m_vecOverV)[nPotID*m_nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRec)->getOverV();
			(m_vecNumU)[nPotID*m_nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRec)->getNumU();
			(m_vecNumV)[nPotID*m_nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRec)->getNumV();

			vecFiberSel=(*iterLocRec)->getFibSel();
			for(i=0;i<vecFiberSel.size();i++)
			{
				(m_vecMDFibSel)[nPotID*m_nMaxTrackCnt*MDLAYERSCNT*MDPLATESCNT+arrTrackCntPerRPot[nPotID]*MDLAYERSCNT*MDPLATESCNT+i]=vecFiberSel[i];
			}

			arrTrackCntPerRPot[nPotID]++;
		}

		//fill data - LocRecODEvCollection
		vecFiberSel.clear();
		for(iterLocRecOD=pLocRecODEvColl->begin();iterLocRecOD!=pLocRecODEvColl->end();iterLocRecOD++)
		{
			nPotID=(*iterLocRecOD)->getPotNum();
			nSideID=(*iterLocRecOD)->getSide();

			nODSign=(nSideID==0)? -1:1;
			(m_vecDetectorPartID)[nPotID*m_nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(nSideID==0)? 3:2;
			(m_vecXDetCS)[nPotID*m_nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=nODSign*22.0;
			(m_vecYDetCS)[nPotID*m_nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecOD)->getYposition();
			(m_vecOverY)[nPotID*m_nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecOD)->getOverY();
			(m_vecNumY)[nPotID*m_nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecOD)->getNumY();

			vecFiberSel=(*iterLocRecOD)->getFibSel();
			for(i=0;i<vecFiberSel.size();i++)
			{
				(m_vecODFibSel)[nPotID*m_nMaxTrackCnt*ODPLATESCNT+arrTrackCntPerRPot[nPotID]*ODPLATESCNT+i]=vecFiberSel[i];
			}

			arrTrackCntPerRPot[nPotID]++;
		}

	}
	else
	{
		msg(MSG::WARNING) << "Cannot find '"<< EVCOLLNAME_LOCREC <<"' or '"<<EVCOLLNAME_LOCRECOD<<"' collection"<<endmsg;
		//return StatusCode::FAILURE;
	}

	//LocRecCorrEvCollection && LocRecCorrODEvCollection
	const ALFA_LocRecCorrEvCollection* pLocRecCorrEvColl;
	const ALFA_LocRecCorrODEvCollection* pLocRecCorrODEvColl;
	ALFA_LocRecCorrEvCollection::const_iterator iterLocRecCorr;
	ALFA_LocRecCorrODEvCollection::const_iterator iterLocRecCorrOD;
	sc=evtStore()->retrieve(pLocRecCorrEvColl, EVCOLLNAME_LOCRECCORR);
	sc2=evtStore()->retrieve(pLocRecCorrODEvColl, EVCOLLNAME_LOCRECCORROD);

	if(!sc.isFailure() && !sc2.isFailure() && pLocRecCorrEvColl!=NULL && pLocRecCorrODEvColl!=NULL)
	{
		memset(&arrTrackCntPerRPot[0],0,sizeof(arrTrackCntPerRPot));
		ClearXAODTrackingData(m_nMaxTrackCnt,ERC_LOCCORRECTED);

		//fill data - LocRecCorrEvCollection - ONLY DetCS for now (TODO rest)
		for(iterLocRecCorr=pLocRecCorrEvColl->begin();iterLocRecCorr!=pLocRecCorrEvColl->end();iterLocRecCorr++)
		{
			nPotID=(*iterLocRecCorr)->getPotNum();

			(m_vecXLhcCS)[nPotID*m_nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecCorr)->getXpositionLHC();
			(m_vecYLhcCS)[nPotID*m_nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecCorr)->getYpositionLHC();
			(m_vecZLhcCS)[nPotID*m_nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecCorr)->getZpositionLHC();

			(m_vecXRPotCS)[nPotID*m_nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecCorr)->getXpositionPot();
			(m_vecYRPotCS)[nPotID*m_nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecCorr)->getYpositionPot();

			(m_vecXStatCS)[nPotID*m_nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecCorr)->getXpositionStat();
			(m_vecYStatCS)[nPotID*m_nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecCorr)->getYpositionStat();

			(m_vecXBeamCS)[nPotID*m_nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecCorr)->getXpositionBeam();
			(m_vecYBeamCS)[nPotID*m_nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecCorr)->getYpositionBeam();

			arrTrackCntPerRPot[nPotID]++;
		}

		//fill data - LocRecCorrODEvCollection - ONLY DetCS for now (TODO rest)
		for(iterLocRecCorrOD=pLocRecCorrODEvColl->begin();iterLocRecCorrOD!=pLocRecCorrODEvColl->end();iterLocRecCorrOD++)
		{
			nPotID=(*iterLocRecCorrOD)->getPotNum();
			nSideID=(*iterLocRecCorrOD)->getSide();

			nODSign=(nSideID==0)? -1:1;
			(m_vecXLhcCS)[nPotID*m_nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=nODSign*22.0;
			(m_vecYLhcCS)[nPotID*m_nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecCorrOD)->getYpositionLHC();
			(m_vecZLhcCS)[nPotID*m_nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecCorrOD)->getZpositionLHC();

			(m_vecXRPotCS)[nPotID*m_nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=nODSign*22.0;
			(m_vecYRPotCS)[nPotID*m_nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecCorrOD)->getYpositionPot();

			(m_vecXStatCS)[nPotID*m_nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=nODSign*22.0;
			(m_vecYStatCS)[nPotID*m_nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecCorrOD)->getYpositionStat();

			(m_vecXBeamCS)[nPotID*m_nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=nODSign*22.0;
			(m_vecYBeamCS)[nPotID*m_nMaxTrackCnt+arrTrackCntPerRPot[nPotID]]=(*iterLocRecCorrOD)->getYpositionBeam();

			arrTrackCntPerRPot[nPotID]++;
		}
	}
	else
	{
		msg(MSG::WARNING) << "Cannot find '"<< EVCOLLNAME_LOCRECCORR <<"' or '"<<EVCOLLNAME_LOCRECCORROD<<"' collection"<<endmsg;
		//return StatusCode::FAILURE;
	}

	xAOD::ALFAData* pData=new xAOD::ALFAData();
	pxAODContainer->push_back(pData);

	//LocRecEvCollection & LocRecODEvCollection
	pData->setXDetCS(m_vecXDetCS);
	pData->setYDetCS(m_vecYDetCS);
	pData->setDetectorPartID(m_vecDetectorPartID);
	pData->setMaxTrackCnt(m_nMaxTrackCnt);
	pData->setOverU(m_vecOverU);
	pData->setOverV(m_vecOverV);
	pData->setOverY(m_vecOverY);
	pData->setNumU(m_vecNumU);
	pData->setNumV(m_vecNumV);
	pData->setNumY(m_vecNumY);
	pData->setMDFibSel(m_vecMDFibSel);
	pData->setODFibSel(m_vecODFibSel);

	//LocRecCorrEvCollection & LocRecCorrODEvCollection
	pData->setXLhcCS(m_vecXLhcCS);
	pData->setYLhcCS(m_vecYLhcCS);
	pData->setZLhcCS(m_vecZLhcCS);
	pData->setXRPotCS(m_vecXRPotCS);
	pData->setYRPotCS(m_vecYRPotCS);
	pData->setXStatCS(m_vecXStatCS);
	pData->setYStatCS(m_vecYStatCS);
	pData->setXBeamCS(m_vecXBeamCS);
	pData->setYBeamCS(m_vecYBeamCS);

	return StatusCode::SUCCESS;
}

StatusCode ALFA_CLinkAlg::FillXAOD_HeaderData(xAOD::ALFADataContainer* pxAODContainer)
{
	StatusCode sc=StatusCode::SUCCESS;

	unsigned int i;
	int nPotID, nPlateID, nFiberID, nSideID;
	ClearXAODHeaderData();

	if (m_nDataType==1)
	{
		//DCS IDs
		/*
		*m_pullDCSBlmID=DataEvent.GetDCSFolderID(EDCSI_BLM);
		*m_pullDCSHVChannelID=DataEvent.GetDCSFolderID(EDCSI_HVCHANNEL);
		*m_pullDCSLocalMonitoringID=DataEvent.GetDCSFolderID(EDCSI_LOCALMONITORING);
		*m_pullDCSMovementID=DataEvent.GetDCSFolderID(EDCSI_MOVEMENT);
		*m_pullDCSRadMonID=DataEvent.GetDCSFolderID(EDCSI_RADMON);
		*m_pullDCSTriggerRatesID=DataEvent.GetDCSFolderID(EDCSI_TRIGGERRATES);
		*m_pullDCSFEConfigurationID=DataEvent.GetDCSFolderID(EDCSI_FECONFIGURATION);
		*m_pullDCSTriggerSettingsID=DataEvent.GetDCSFolderID(EDCSI_TRIGGERSETTINGS);*/

		//RawDataContainer
		const ALFA_RawDataContainer* pRawDataColl;
		ALFA_RawDataContainer::const_iterator iterRawData;
		sc=evtStore()->retrieve(pRawDataColl, EVCOLLNAME_RAWDATA);
		if(!sc.isFailure() && pRawDataColl!=NULL)
		{
			//m_nTimeStamp=pRawDataColl->GetTimeStamp();
			//m_nTimeStamp_ns=pRawDataColl->GetTimeStampns();
			//m_nBCId=pRawDataColl->GetBCId();

			vector<bool> vecRPPattern;
			for(iterRawData=pRawDataColl->begin();iterRawData!=pRawDataColl->end();iterRawData++)
			{
				nPotID=(*iterRawData)->GetMBId_POT();
				(m_vecScaler)[nPotID-1]=(*iterRawData)->Get_scaler_POT();

				vecRPPattern=(*iterRawData)->Get_pattern_POT();
				for(i=0;i<vecRPPattern.size();i++){
					if(i<RPOTSCNT*TRIGPATCNT) (m_vecTrigPat)[(nPotID-1)*TRIGPATCNT+i]=vecRPPattern[vecRPPattern.size()-(i+1)];
				}
			}
		}
		else
		{
			msg(MSG::WARNING) << "Cannot find '"<< EVCOLLNAME_RAWDATA <<"' collection"<<endmsg;
			//return StatusCode::FAILURE;
		}
	}

	//DigitCollection
	const ALFA_DigitCollection* pDigitColl;
	ALFA_DigitCollection::const_iterator iterDigit;
	sc=evtStore()->retrieve(pDigitColl, EVCOLLNAME_DIGIT);
	if(!sc.isFailure() && pDigitColl!=NULL)
	{
		for(iterDigit=pDigitColl->begin();iterDigit!=pDigitColl->end();iterDigit++)
		{
			nPotID=(*iterDigit)->getStation(); //in range 0-7
			nPlateID=(*iterDigit)->getPlate(); //indexed from 0
			nFiberID=(*iterDigit)->getFiber(); //indexed from 0

			if(nPotID<RPOTSCNT && nPlateID<(MDLAYERSCNT*MDPLATESCNT) && nFiberID<MDFIBERSCNT)
			{
				(m_vecMDFiberHits)[(nPotID*MDLAYERSCNT*MDPLATESCNT*MDFIBERSCNT)+(nPlateID*MDFIBERSCNT)+nFiberID]=1;
				(m_vecMDMultiplicity)[(nPotID*MDLAYERSCNT*MDPLATESCNT)+nPlateID]++;
			}
			else
			{
				msg(MSG::ERROR) << "Index exceed array size for [RPotID, nPlateID, nFiberID]= ["<<nPotID<<", "<<nPlateID<<", "<<nFiberID<<"]"<<endmsg;
				//return StatusCode::FAILURE;
			}
		}
	}
	else{
		msg(MSG::WARNING) << "Cannot find '"<< EVCOLLNAME_DIGIT <<"' collection"<<endmsg;
		//return StatusCode::FAILURE;
	}

	//ODDigitCollection
	const ALFA_ODDigitCollection* pODDigitColl;
	ALFA_ODDigitCollection::const_iterator iterODDigit;
	sc=evtStore()->retrieve(pODDigitColl, EVCOLLNAME_ODDIGIT);
	if(!sc.isFailure() && pODDigitColl!=NULL)
	{
		for(iterODDigit=pODDigitColl->begin();iterODDigit!=pODDigitColl->end();iterODDigit++)
		{
			nPotID=(*iterODDigit)->getStation(); //in range 0-7
			nPlateID=(*iterODDigit)->getPlate(); //indexed from 0
			nSideID=(*iterODDigit)->getSide();   //indexed from 0
			nFiberID=(*iterODDigit)->getFiber(); //indexed from 0

			if(nPotID<RPOTSCNT && nPlateID<(ODPLATESCNT) && nFiberID<ODLAYERSCNT*ODFIBERSCNT)
			{
				if(nSideID==0){ //right side
					(m_vecODFiberHitsNeg)[(nPotID*ODPLATESCNT*ODLAYERSCNT*ODFIBERSCNT)+(nPlateID*ODLAYERSCNT*ODFIBERSCNT)+nFiberID]=1;
					(m_vecODMultiplicityNeg)[(nPotID*ODPLATESCNT)+nPlateID]++;
				}
				else{ //left side
					(m_vecODFiberHitsPos)[(nPotID*ODPLATESCNT*ODLAYERSCNT*ODFIBERSCNT)+(nPlateID*ODLAYERSCNT*ODFIBERSCNT)+nFiberID]=1;
					(m_vecODMultiplicityPos)[(nPotID*ODPLATESCNT)+nPlateID]++;
				}
			}
			else
			{
				msg(MSG::ERROR) << "Index exceed array size for [RPotID, nPlateID, nFiberID, nSideID]= ["<<nPotID<<", "<<nPlateID<<", "<<nFiberID<<", "<<nSideID<<"]"<<endmsg;
				//return StatusCode::FAILURE;
			}
		}
	}
	else
	{
		msg(MSG::WARNING) << "Cannot find '"<< EVCOLLNAME_ODDIGIT <<"' collection"<<endmsg;
		//return StatusCode::FAILURE;
	}

	xAOD::ALFAData* pData=new xAOD::ALFAData();
	pxAODContainer->push_back(pData);

	//RawDataContainer
	pData->setScaler(m_vecScaler);
	//pData->setBCId(m_nBCId);
	//pData->setTimeStamp(m_nTimeStamp);
	//pData->setTimeStamp_ns(m_nTimeStamp_ns);
	pData->setTrigPat(m_vecTrigPat);

	//DigitCollection
	pData->setMDFiberHits(m_vecMDFiberHits);
	pData->setMDMultiplicity(m_vecMDMultiplicity);

	//ODDigitCollection
	pData->setODFiberHitsPos(m_vecODFiberHitsPos);
	pData->setODFiberHitsNeg(m_vecODFiberHitsNeg);
	pData->setODMultiplicityPos(m_vecODMultiplicityPos);
	pData->setODMultiplicityNeg(m_vecODMultiplicityNeg);

	return StatusCode::SUCCESS;
}

void ALFA_CLinkAlg::ClearXAODTrackingData(const int nMaxTrackCnt, eRecType eType)
{
	if(eType==ERC_LOCUNCORRECTED)
	{
		//LocRecEvCollection & LocRecEvODCollection
		m_vecXDetCS.resize(RPOTSCNT*nMaxTrackCnt);
		fill_n(m_vecXDetCS.begin(),m_vecXDetCS.size(),-9999);
		m_vecYDetCS.resize(RPOTSCNT*nMaxTrackCnt);
		fill_n(m_vecYDetCS.begin(),m_vecYDetCS.size(),-9999);
		m_vecDetectorPartID.resize(RPOTSCNT*nMaxTrackCnt);
		fill_n(m_vecDetectorPartID.begin(),m_vecDetectorPartID.size(),0);
		m_vecOverU.resize(RPOTSCNT*nMaxTrackCnt);
		fill_n(m_vecOverU.begin(),m_vecOverU.size(),-9999);
		m_vecOverV.resize(RPOTSCNT*nMaxTrackCnt);
		fill_n(m_vecOverV.begin(),m_vecOverV.size(),-9999);
		m_vecOverY.resize(RPOTSCNT*nMaxTrackCnt);
		fill_n(m_vecOverY.begin(),m_vecOverY.size(),-9999);
		m_vecNumU.resize(RPOTSCNT*nMaxTrackCnt);
		fill_n(m_vecNumU.begin(),m_vecNumU.size(),-9999);
		m_vecNumV.resize(RPOTSCNT*nMaxTrackCnt);
		fill_n(m_vecNumV.begin(),m_vecNumV.size(),-9999);
		m_vecNumY.resize(RPOTSCNT*nMaxTrackCnt);
		fill_n(m_vecNumY.begin(),m_vecNumY.size(),-9999);
		m_vecMDFibSel.resize(RPOTSCNT*nMaxTrackCnt*MDLAYERSCNT*MDPLATESCNT);
		fill_n(m_vecMDFibSel.begin(),m_vecMDFibSel.size(),-9999);
		m_vecODFibSel.resize(RPOTSCNT*nMaxTrackCnt*ODPLATESCNT);
		fill_n(m_vecODFibSel.begin(),m_vecODFibSel.size(),-9999);
	}
	else if(eType==ERC_LOCCORRECTED)
	{
		m_vecXLhcCS.resize(RPOTSCNT*nMaxTrackCnt);
		fill_n(m_vecXLhcCS.begin(),m_vecXLhcCS.size(),-9999);
		m_vecYLhcCS.resize(RPOTSCNT*nMaxTrackCnt);
		fill_n(m_vecYLhcCS.begin(),m_vecYLhcCS.size(),-9999);
		m_vecZLhcCS.resize(RPOTSCNT*nMaxTrackCnt);
		fill_n(m_vecZLhcCS.begin(),m_vecZLhcCS.size(),-9999);
		m_vecXRPotCS.resize(RPOTSCNT*nMaxTrackCnt);
		fill_n(m_vecXRPotCS.begin(),m_vecXRPotCS.size(),-9999);
		m_vecYRPotCS.resize(RPOTSCNT*nMaxTrackCnt);
		fill_n(m_vecYRPotCS.begin(),m_vecYRPotCS.size(),-9999);
		m_vecXStatCS.resize(RPOTSCNT*nMaxTrackCnt);
		fill_n(m_vecXStatCS.begin(),m_vecXStatCS.size(),-9999);
		m_vecYStatCS.resize(RPOTSCNT*nMaxTrackCnt);
		fill_n(m_vecYStatCS.begin(),m_vecYStatCS.size(),-9999);
		m_vecXBeamCS.resize(RPOTSCNT*nMaxTrackCnt);
		fill_n(m_vecXBeamCS.begin(),m_vecXBeamCS.size(),-9999);
		m_vecYBeamCS.resize(RPOTSCNT*nMaxTrackCnt);
		fill_n(m_vecYBeamCS.begin(),m_vecYBeamCS.size(),-9999);
	}
}

void ALFA_CLinkAlg::ClearXAODHeaderData()
{
	if (m_nDataType==1)
	{
		//DCS IDs
		/*
		*m_pullDCSBlmID=0;
		*m_pullDCSHVChannelID=0;
		*m_pullDCSLocalMonitoringID=0;
		*m_pullDCSMovementID=0;
		*m_pullDCSRadMonID=0;
		*m_pullDCSTriggerRatesID=0;
		*m_pullDCSFEConfigurationID=0;
		*m_pullDCSTriggerSettingsID=0;*/

		//RawDataContainer
		m_vecScaler.resize(RPOTSCNT);
		fill_n(m_vecScaler.begin(),m_vecScaler.size(),-1);
		//m_nBCId=-1;
		//m_nTimeStamp=-1;
		//m_nTimeStamp_ns=-1;
		m_vecTrigPat.resize(RPOTSCNT*TRIGPATCNT);
		fill_n(m_vecTrigPat.begin(),m_vecTrigPat.size(),0);
	}

	//DigitCollection
	m_vecMDFiberHits.resize(RPOTSCNT*MDLAYERSCNT*MDPLATESCNT*MDFIBERSCNT);
	fill_n(m_vecMDFiberHits.begin(),m_vecMDFiberHits.size(),0);
	m_vecMDMultiplicity.resize(RPOTSCNT*MDLAYERSCNT*MDPLATESCNT);
	fill_n(m_vecMDMultiplicity.begin(),m_vecMDMultiplicity.size(),0);

	//ODDigitCollection
	m_vecODFiberHitsPos.resize(RPOTSCNT*ODPLATESCNT*ODLAYERSCNT*ODFIBERSCNT);
	fill_n(m_vecODFiberHitsPos.begin(),m_vecODFiberHitsPos.size(),0);
	m_vecODFiberHitsNeg.resize(RPOTSCNT*ODPLATESCNT*ODLAYERSCNT*ODFIBERSCNT);
	fill_n(m_vecODFiberHitsNeg.begin(),m_vecODFiberHitsNeg.size(),0);

	m_vecODMultiplicityPos.resize(RPOTSCNT*ODPLATESCNT);
	fill_n(m_vecODMultiplicityPos.begin(),m_vecODMultiplicityPos.size(),0);
	m_vecODMultiplicityNeg.resize(RPOTSCNT*ODPLATESCNT);
	fill_n(m_vecODMultiplicityNeg.begin(),m_vecODMultiplicityNeg.size(),0);
}
