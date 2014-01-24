/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TObject.h"
#include "TROOT.h"
#include "TThread.h"

#include "CAlfaEvent.h"

#if !defined(__CINT__)
	ClassImp(CAlfaEvent);
#endif

string CAlfaEvent::ms_EventTreeName=EVENTTREENAME;
string CAlfaEvent::ms_DCSFolder=DCSFOLDER;

ALFAEVENTHEADER::_ALFAEVENTHEADER()
{
    //EventInfo
	uRunNumber=0;
	uEventNumber=0;
	uTimeStamp=0;
	uTimeStampNs=0;

    //DCS IDs
	ullDcsBlm=0;
	ullDcsHVChannel=0;
	ullDcsLocalMonitoring=0;
	ullDcsMovement=0;
	ullDcsRadMon=0;
	ullDcsTriggerRates=0;
	ullDcsFEConfiguration=0;
	ullDcsTriggerSettings=0;

    //RawDataContainer
    pvecScaler=NULL;
	nBCId=0;
	nTimeStp=0;
	nTimeStpNs=0;
    pvecTrigPat=NULL;

    //DigitCollection
    pvecFiberHitsMD=NULL;
    pvecMultiplicityMD=NULL;

    //ODDigitCollection
    pvecFiberHitsODPos=NULL;
    pvecFiberHitsODNeg=NULL;
    pvecMultiplicityODPos=NULL;
    pvecMultiplicityODNeg=NULL;
}

ALFATRACKINGDATA::_ALFATRACKINGDATA()
{
    //LocRecEvCollection & LocRecODEvCollection
	nMaxTrackCnt=0;
    pvecXDetCS=NULL;
    pvecYDetCS=NULL;
    pvecDetectorPartID=NULL;
    pvecOverU=NULL;
    pvecOverV=NULL;
    pvecOverY=NULL;
    pvecNumU=NULL;
    pvecNumV=NULL;
    pvecNumY=NULL;
    pvecMDFibSel=NULL;
    pvecODFibSel=NULL;

    //LocRecCorrEvCollection & LocRecCorrODEvCollection
    pvecXLhcCS=NULL;
    pvecYLhcCS=NULL;
    pvecZLhcCS=NULL;
    pvecXRPotCS=NULL;
    pvecYRPotCS=NULL;
    pvecXStatCS=NULL;
    pvecYStatCS=NULL;
    pvecXBeamCS=NULL;
    pvecYBeamCS=NULL;
}

CAlfaEvent::CAlfaEvent()
{
    //DCS IDs
    m_ullDcsBlm=0;
    m_ullDcsHVChannel=0;
    m_ullDcsLocalMonitoring=0;
    m_ullDcsMovement=0;
    m_ullDcsRadMon=0;
    m_ullDcsTriggerRates=0;
    m_ullDcsFEConfiguration=0;
    m_ullDcsTriggerSettings=0;

    //RawDataContainer
    m_vecScaler.clear();
    m_nBCId=0;
    m_nTimeStp=0;
    m_nTimeStpNs=0;
    m_vecTrigPat.clear();

    //DigitCollection
    m_vecFiberHitsMD.clear();
    m_vecMultiplicityMD.clear();

    //ODDigitCollection
    m_vecFiberHitsODPos.clear();
    m_vecFiberHitsODNeg.clear();
    m_vecMultiplicityODPos.clear();
    m_vecMultiplicityODNeg.clear();

    //LocRecEvCollection & LocRecODEvCollection
    m_nMaxTrackCnt=0;
    m_vecXDetCS.clear();
    m_vecYDetCS.clear();
    m_vecDetectorPartID.clear();
    m_vecOverU.clear();
    m_vecOverV.clear();
    m_vecOverY.clear();
    m_vecNumU.clear();
    m_vecNumV.clear();
    m_vecNumY.clear();
    m_vecMDFibSel.clear();
    m_vecODFibSel.clear();

    //LocRecCorrEvCollection & LocRecCorrODEvCollection
    m_vecXLhcCS.clear();
    m_vecYLhcCS.clear();
    m_vecZLhcCS.clear();
    m_vecXRPotCS.clear();
    m_vecYRPotCS.clear();
    m_vecXStatCS.clear();
    m_vecYStatCS.clear();
    m_vecXBeamCS.clear();
    m_vecYBeamCS.clear();
}

CAlfaEvent::~CAlfaEvent()
{

}

void CAlfaEvent::SetNTuplePaths(string& EventTreeName, string& DCSFolder)
{
	ms_EventTreeName=EventTreeName;
	ms_DCSFolder=DCSFolder;
}

Long64_t CAlfaEvent::FillDcsBlmMap(const char szRootFilename[], map<ULong64_t,DCSBLM>& mapDcsData)
{
	Long64_t nEntries=0;

	TFile* pFile=new TFile(szRootFilename);
	nEntries=FillDcsBlmMap(pFile,mapDcsData);
	pFile->Close();

	return nEntries;
}

Long64_t CAlfaEvent::FillDcsBlmMap(TFile* pFile, map<ULong64_t,DCSBLM>& mapDcsData)
{
	int i,j;
	Long64_t nEntries;
	DCSBLM DcsData;

	mapDcsData.clear();

	ULong64_t ulID;
	vector<Float_t>* pvecBlm=NULL;

	if(pFile->cd(ms_DCSFolder.c_str())){
		TTree* pTree=(TTree*)gROOT->FindObject(DCSTREENAME_BLM);
		if(pTree!=NULL){
			nEntries=pTree->GetEntries();
			pTree->SetBranchAddress("ID", &ulID);
			pTree->SetBranchAddress("BLM", &pvecBlm);

			//printf("BLM Entries=%i\r\n",(int)nEntries);

			for(i=0;i<nEntries;i++){
				pTree->GetEntry(i);

				if(mapDcsData.find(ulID)==mapDcsData.end()){
					//add new record
					memset(&DcsData,0,sizeof(DCSBLM));
					DcsData.ulID=ulID;
					for(j=0;j<BLMCNT;j++) DcsData.arrBLM[j]=(*pvecBlm)[j];

					mapDcsData[DcsData.ulID]=DcsData;
				}
			}
		}
		else{
			printf("ERROR: Cannot find BLM tree\r\n");
		}

		pFile->cd();
	}

	return mapDcsData.size();
}

Long64_t CAlfaEvent::FillDcsHVChannelMap(const char szRootFilename[], map<ULong64_t,DCSHVCHANNEL>& mapDcsData)
{
	Long64_t nEntries=0;

	TFile* pFile=new TFile(szRootFilename);
	if(pFile!=NULL){
		nEntries=FillDcsHVChannelMap(pFile,mapDcsData);
		pFile->Close();
	}

	return nEntries;
}

Long64_t CAlfaEvent::FillDcsHVChannelMap(TFile* pFile, map<ULong64_t,DCSHVCHANNEL>& mapDcsData)
{
	int i,j;
	Long64_t nEntries;
	DCSHVCHANNEL DcsData;

	mapDcsData.clear();

	ULong64_t ulID;
	vector<Float_t>* pvecVoltage=NULL;
	vector<Float_t>* pvecCurrent=NULL;

	if(pFile->cd(ms_DCSFolder.c_str())){
		TTree* pTree=(TTree*)gROOT->FindObject(DCSTREENAME_HVCHANNEL);
		if(pTree!=NULL){
			nEntries=pTree->GetEntries();
			pTree->SetBranchAddress("ID", &ulID);
			pTree->SetBranchAddress("Voltage", &pvecVoltage);
			pTree->SetBranchAddress("Current", &pvecCurrent);

			//printf("HVCHANNEL Entries=%i\r\n",(int)nEntries);

			for(i=0;i<nEntries;i++){
				pTree->GetEntry(i);

				if(mapDcsData.find(ulID)==mapDcsData.end()){
					//add new record
					memset(&DcsData,0,sizeof(DCSHVCHANNEL));
					DcsData.ulID=ulID;

					for(j=0;j<HVCHANNELCNT;j++){
						DcsData.arrVoltage[j]=(*pvecVoltage)[j];
						DcsData.arrCurrent[j]=(*pvecCurrent)[j];
					}

					mapDcsData[DcsData.ulID]=DcsData;
				}
			}
		}
		else{
			printf("ERROR: Cannot find HVCHANNEL tree\r\n");
		}

		pFile->cd();
	}

	return mapDcsData.size();
}

Long64_t CAlfaEvent::FillDcsLocalMonitoringMap(const char szRootFilename[], map<ULong64_t,DCSLOCALMONITORING>& mapDcsData)
{
	Long64_t nEntries=0;

	TFile* pFile=new TFile(szRootFilename);
	if(pFile!=NULL){
		nEntries=FillDcsLocalMonitoringMap(pFile,mapDcsData);
		pFile->Close();
	}

	return nEntries;
}

Long64_t CAlfaEvent::FillDcsLocalMonitoringMap(TFile* pFile, map<ULong64_t,DCSLOCALMONITORING>& mapDcsData)
{
	int i,j;
	Long64_t nEntries;
	DCSLOCALMONITORING DcsData;

	mapDcsData.clear();

	ULong64_t ulID;
	vector<Float_t>* pvecTempSensor01=NULL;
	vector<Float_t>* pvecTempSensor02=NULL;
	vector<Float_t>* pvecTempSensor03=NULL;
	vector<Float_t>* pvecTempSensor04=NULL;
	vector<Float_t>* pvecTempSensor05=NULL;

	if(pFile->cd(ms_DCSFolder.c_str())){
		TTree* pTree=(TTree*)gROOT->FindObject(DCSTREENAME_LOCALMONITORING);
		if(pTree!=NULL){
			nEntries=pTree->GetEntries();
			pTree->SetBranchAddress("ID", &ulID);
			pTree->SetBranchAddress("TempSensor1", &pvecTempSensor01);
			pTree->SetBranchAddress("TempSensor2", &pvecTempSensor02);
			pTree->SetBranchAddress("TempSensor3", &pvecTempSensor03);
			pTree->SetBranchAddress("TempSensor4", &pvecTempSensor04);
			pTree->SetBranchAddress("TempSensor5", &pvecTempSensor05);

			//printf("LOCALMONITORING Entries=%i\r\n",(int)nEntries);

			for(i=0;i<nEntries;i++){
				pTree->GetEntry(i);

				if(mapDcsData.find(ulID)==mapDcsData.end()){
					//add new record
					memset(&DcsData,0,sizeof(DCSLOCALMONITORING));
					DcsData.ulID=ulID;

					for(j=0;j<RPOTSCNT;j++){
						DcsData.arrTempSensor01[j]=(*pvecTempSensor01)[j];
						DcsData.arrTempSensor02[j]=(*pvecTempSensor02)[j];
						DcsData.arrTempSensor03[j]=(*pvecTempSensor03)[j];
						DcsData.arrTempSensor04[j]=(*pvecTempSensor04)[j];
						DcsData.arrTempSensor05[j]=(*pvecTempSensor05)[j];
					}

					mapDcsData[DcsData.ulID]=DcsData;
				}
			}
		}
		else{
			printf("ERROR: Cannot find LOCALMONITORING tree\r\n");
		}

		pFile->cd();
	}

	return mapDcsData.size();
}

Long64_t CAlfaEvent::FillDcsMovementMap(const char szRootFilename[], map<ULong64_t,DCSMOVEMENT>& mapDcsData)
{
	Long64_t nEntries=0;

	TFile* pFile=new TFile(szRootFilename);
	if(pFile!=NULL){
		nEntries=FillDcsMovementMap(pFile, mapDcsData);
		pFile->Close();
	}

	return nEntries;
}

Long64_t CAlfaEvent::FillDcsMovementMap(TFile* pFile, map<ULong64_t,DCSMOVEMENT>& mapDcsData)
{
	int i,j;
	Long64_t nEntries;
	DCSMOVEMENT DcsData;

	mapDcsData.clear();

	ULong64_t ulID;
	vector<Float_t>* pvecLVDT=NULL;
	vector<Float_t>* pvecMotor=NULL;

	if(pFile->cd(ms_DCSFolder.c_str())){
		TTree* pTree=(TTree*)gROOT->FindObject(DCSTREENAME_MOVEMENT);
		if(pTree!=NULL){
			nEntries=pTree->GetEntries();
			pTree->SetBranchAddress("ID", &ulID);
			pTree->SetBranchAddress("LVDT", &pvecLVDT);
			pTree->SetBranchAddress("Motor", &pvecMotor);

			//printf("MOVEMENT Entries=%i\r\n",(int)nEntries);

			for(i=0;i<nEntries;i++){
				pTree->GetEntry(i);

				if(mapDcsData.find(ulID)==mapDcsData.end()){
					//add new record
					memset(&DcsData,0,sizeof(DCSMOVEMENT));
					DcsData.ulID=ulID;

					for(j=0;j<RPOTSCNT;j++){
						DcsData.arrLVDT[j]=(*pvecLVDT)[j];
						DcsData.arrMotor[j]=(*pvecMotor)[j];
					}

					mapDcsData[DcsData.ulID]=DcsData;
				}
			}
		}
		else{
			printf("ERROR: Cannot find MOVEMENT tree\r\n");
		}

		pFile->cd();
	}

	return mapDcsData.size();
}

Long64_t CAlfaEvent::FillDcsRadMonMap(const char szRootFilename[], map<ULong64_t,DCSRADMON>& mapDcsData)
{
	Long64_t nEntries=0;

	TFile* pFile=new TFile(szRootFilename);
	if(pFile!=NULL){
		nEntries=FillDcsRadMonMap(pFile, mapDcsData);
		pFile->Close();
	}

	return nEntries;
}

Long64_t CAlfaEvent::FillDcsRadMonMap(TFile* pFile, map<ULong64_t,DCSRADMON>& mapDcsData)
{
	int i,j;
	Long64_t nEntries;
	DCSRADMON DcsData;

	mapDcsData.clear();

	ULong64_t ulID;
	vector<Float_t>* pvecDose=NULL;
	vector<Float_t>* pvecFluence=NULL;
	vector<Float_t>* pvecTemperature=NULL;

	if(pFile->cd(ms_DCSFolder.c_str())){
		TTree* pTree=(TTree*)gROOT->FindObject(DCSTREENAME_RADMON);
		if(pTree!=NULL){
			nEntries=pTree->GetEntries();
			pTree->SetBranchAddress("ID", &ulID);
			pTree->SetBranchAddress("Dose", &pvecDose);
			pTree->SetBranchAddress("Fluence", &pvecFluence);
			pTree->SetBranchAddress("Temperature", &pvecTemperature);

			//printf("RADMON Entries=%i\r\n",(int)nEntries);

			for(i=0;i<nEntries;i++){
				pTree->GetEntry(i);

				if(mapDcsData.find(ulID)==mapDcsData.end()){
					//add new record
					memset(&DcsData,0,sizeof(DCSRADMON));
					DcsData.ulID=ulID;

					for(j=0;j<RADMONCNT;j++){
						DcsData.arrDose[j]=(*pvecDose)[j];
						DcsData.arrFluence[j]=(*pvecFluence)[j];
						DcsData.arrTemperature[j]=(*pvecTemperature)[j];
					}

					mapDcsData[DcsData.ulID]=DcsData;
				}
			}
		}
		else{
			printf("ERROR: Cannot find RADMON tree\r\n");
		}

		pFile->cd();
	}

	return mapDcsData.size();
}

Long64_t CAlfaEvent::FillDcsTriggerRatesMap(const char szRootFilename[], map<ULong64_t,DCSTRIGGERRATES>& mapDcsData)
{
	Long64_t nEntries=0;

	TFile* pFile=new TFile(szRootFilename);
	if(pFile!=NULL){
		nEntries=FillDcsTriggerRatesMap(pFile, mapDcsData);
		pFile->Close();
	}

	return nEntries;
}

Long64_t CAlfaEvent::FillDcsTriggerRatesMap(TFile* pFile, map<ULong64_t,DCSTRIGGERRATES>& mapDcsData)
{
	int i,j;
	Long64_t nEntries;
	DCSTRIGGERRATES DcsData;

	mapDcsData.clear();

	ULong64_t ulID;
	vector<Int_t>* pvecTriggerRates=NULL;

	if(pFile->cd(ms_DCSFolder.c_str())){
		TTree* pTree=(TTree*)gROOT->FindObject(DCSTREENAME_TRIGGERRATES);
		if(pTree!=NULL){
			nEntries=pTree->GetEntries();
			pTree->SetBranchAddress("ID", &ulID);
			pTree->SetBranchAddress("TriggerRates", &pvecTriggerRates);

			//printf("TRIGGERRATES Entries=%i\r\n",(int)nEntries);

			for(i=0;i<nEntries;i++){
				pTree->GetEntry(i);

				if(mapDcsData.find(ulID)==mapDcsData.end()){
					//add new record
					memset(&DcsData,0,sizeof(DCSTRIGGERRATES));
					DcsData.ulID=ulID;

					for(j=0;j<RPOTSCNT;j++){
						DcsData.arrTriggerRates[j]=(*pvecTriggerRates)[j];
					}

					mapDcsData[DcsData.ulID]=DcsData;
				}
			}
		}
		else{
			printf("ERROR: Cannot find TRIGGERRATES tree\r\n");
		}

		pFile->cd();
	}

	return mapDcsData.size();
}

Long64_t CAlfaEvent::FillDcsFEConfigurationMap(const char szRootFilename[], map<ULong64_t,DCSFECONFIGURATION>& mapDcsData)
{
	Long64_t nEntries=0;

	TFile* pFile=new TFile(szRootFilename);
	if(pFile!=NULL){
		nEntries=FillDcsFEConfigurationMap(pFile, mapDcsData);
		pFile->Close();
	}

	return nEntries;
}

Long64_t CAlfaEvent::FillDcsFEConfigurationMap(TFile* pFile, map<ULong64_t,DCSFECONFIGURATION>& mapDcsData)
{
	int i,j,k;
	Long64_t nEntries;
	DCSFECONFIGURATION DcsData;

	mapDcsData.clear();

	ULong64_t ulID;
	vector<Int_t>* pvecThreshold=NULL;
	vector<Int_t>* pvecGain=NULL;
	vector<string>* pvecMDGainMode=NULL;

	if(pFile->cd(ms_DCSFolder.c_str())){
		TTree* pTree=(TTree*)gROOT->FindObject(DCSTREENAME_FECONFIGURATION);
		if(pTree!=NULL){
			nEntries=pTree->GetEntries();
			pTree->SetBranchAddress("ID", &ulID);
			pTree->SetBranchAddress("Threshold", &pvecThreshold);
			pTree->SetBranchAddress("Gain", &pvecGain);
			pTree->SetBranchAddress("MDGainMode", &pvecMDGainMode);

			//printf("FECONFIGURATION Entries=%i\r\n",(int)nEntries);

			for(i=0;i<nEntries;i++){
				pTree->GetEntry(i);

				if(mapDcsData.find(ulID)==mapDcsData.end()){
					//add new record
					memset(&DcsData,0,sizeof(DCSFECONFIGURATION));
					DcsData.ulID=ulID;

					for(j=0;j<RPOTSCNT;j++){
						for(k=0;k<FECNFTHRESHLOLDCNT;k++) DcsData.arrThreshold[j][k]=(*pvecThreshold)[FECNFTHRESHLOLDCNT*j+k];
						for(k=0;k<FECNFGAINCNT;k++) DcsData.arrGain[j][k]=(*pvecGain)[FECNFGAINCNT*j+k];
						strcpy(DcsData.arrMDGainMode[j],(*pvecMDGainMode)[j].c_str());
					}

					mapDcsData[DcsData.ulID]=DcsData;
				}
			}
		}
		else{
			printf("ERROR: Cannot find FECONFIGURATION tree\r\n");
		}

		pFile->cd();
	}

	return mapDcsData.size();
}

Long64_t CAlfaEvent::FillDcsTriggerSettingsMap(const char szRootFilename[], map<ULong64_t,DCSTRIGGERSETTINGS>& mapDcsData)
{
	Long64_t nEntries=0;

	TFile* pFile=new TFile(szRootFilename);
	if(pFile!=NULL){
		nEntries=FillDcsTriggerSettingsMap(pFile, mapDcsData);
		pFile->Close();
	}

	return nEntries;
}

Long64_t CAlfaEvent::FillDcsTriggerSettingsMap(TFile* pFile, map<ULong64_t,DCSTRIGGERSETTINGS>& mapDcsData)
{
	int i,j,k;
	Long64_t nEntries;
	DCSTRIGGERSETTINGS DcsData;

	mapDcsData.clear();

	ULong64_t ulID;
	vector<Int_t>* pvecTriggerSettings=NULL;
	vector<Int_t>* pvecLatency=NULL;

	if(pFile->cd(ms_DCSFolder.c_str())){
		TTree* pTree=(TTree*)gROOT->FindObject(DCSTREENAME_TRIGGERSETTINGS);
		if(pTree!=NULL){
			nEntries=pTree->GetEntries();
			pTree->SetBranchAddress("ID", &ulID);
			pTree->SetBranchAddress("TriggerSettings", &pvecTriggerSettings);
			pTree->SetBranchAddress("Latency", &pvecLatency);

			//printf("TRIGGERSETTINGS Entries=%i\r\n",(int)nEntries);

			for(i=0;i<nEntries;i++){
				pTree->GetEntry(i);

				if(mapDcsData.find(ulID)==mapDcsData.end()){
					//add new record
					memset(&DcsData,0,sizeof(DCSTRIGGERSETTINGS));
					DcsData.ulID=ulID;

					for(j=0;j<RPOTSCNT;j++){//[RPOTSCNT][TRIGSETCNT]
						for(k=0;k<TRIGSETCNT;k++) DcsData.arrTriggerSet[j][k]=(*pvecTriggerSettings)[TRIGSETCNT*j+k];
						for(k=0;k<TRIGSETLATENCYCNT;k++) DcsData.arrLatency[j][k]=(*pvecLatency)[TRIGSETLATENCYCNT*j+k];
					}

					mapDcsData[DcsData.ulID]=DcsData;
				}
			}
		}
		else{
			printf("ERROR: Cannot find TRIGGERSETTINGS tree\r\n");
		}

		pFile->cd();
	}

	return mapDcsData.size();
}

Long64_t CAlfaEvent::ResolveEntries(const char szRootFilename[])
{
    Long64_t nEntries=0;

    TFile* pFile=new TFile(szRootFilename);
    nEntries=ResolveEntries(pFile);
    pFile->Close();

    return nEntries;
}

Long64_t CAlfaEvent::ResolveEntries(TFile* pFile)
{
    Long64_t nEntries=0;

    if(pFile!=NULL){
	TTree* pEventHeaderTree=(TTree*)pFile->Get(ms_EventTreeName.c_str());
	if(pEventHeaderTree!=NULL) nEntries=pEventHeaderTree->GetEntries();
    }

    return nEntries;
}

void* CAlfaEvent::ThLoadEvents(void* pParams)
{
	Long64_t i, nEntries=0;
	CAlfaEvent* pEventData=NULL;

	PWORKTHREADPARAMS pWorkParams=(PWORKTHREADPARAMS)pParams;
	pWorkParams->plistEvents->Clear();

	//TThread::Printf("[Thread #%i] start\r\n",pWorkParams->nThreadID);

	TFile* pFile=new TFile(pWorkParams->szFileName,"READ");
	if(pFile!=NULL){
		TTree* pEventTree=(TTree*)pFile->Get(ms_EventTreeName.c_str());
		if(pEventTree!=NULL){

			PALFAEVENTHEADER pEventHeader=new ALFAEVENTHEADER();
			PALFATRACKINGDATA pTrackingData=new ALFATRACKINGDATA();
			BookEventHeaderVariables(pEventTree, pEventHeader);
			BookTrackingDataVariables(pEventTree, pTrackingData);

			for(i=pWorkParams->nEntryFrom;i<=pWorkParams->nEntryTo;i++){
				nEntries++;
				//if(nEntries%PRINTMODULO==0) printf("[Thread #%i] scanned %llu entries\r\n",pWorkParams->nThreadID, nEntries);
				if(nEntries%PRINTMODULO==0) printf("scanned %llu entries\r\n", nEntries);

				pEventTree->GetEntry(i);
				pEventData=new CAlfaEvent();
				pEventData->FillEventHeader(pEventHeader);
				pEventData->FillTrackingData(pTrackingData);

				pWorkParams->plistEvents->Add(pEventData);
			}

			if(pEventHeader) delete pEventHeader;
			if(pTrackingData) delete pTrackingData;
		}
	}

	//TThread::Printf("[Thread #%i] stop\r\n",pWorkParams->nThreadID);

	return NULL;
}

Long64_t CAlfaEvent::LoadEvents(const char szRootFilename[], TList& listData, const Long64_t nMaxEntries)
{
	const bool bSupportMultiThreadedReading=false;
	const int nMaxNumOfThreads=1;

	int i;
	Char_t szWorkThreadName[16];
	Long64_t  nEntriesPerThread, nTotEntries, nEntries=0, nEntryFrom, nEntryTo;

	nTotEntries=ResolveEntries(szRootFilename);
	if(nMaxEntries<0) nEntries=nTotEntries;
	else if(nMaxEntries>nTotEntries) nEntries=nTotEntries;
	else nEntries=nMaxEntries;

	if(bSupportMultiThreadedReading && nTotEntries>MTHREADSTH && nMaxNumOfThreads>1){
		printf("Multi thread mode\r\n");

		TThread** ppWorkThreads=new TThread*[nMaxNumOfThreads];
		TList* parrlistData=new TList[nMaxNumOfThreads];
		PWORKTHREADPARAMS parrWorkParams=new WORKTHREADPARAMS[nMaxNumOfThreads];
		nEntriesPerThread=nEntries/nMaxNumOfThreads;
		nEntryFrom=0;

		for(i=0;i<nMaxNumOfThreads;i++){

			nEntryTo=nEntryFrom+nEntriesPerThread-1;
			if(i==nMaxNumOfThreads-1) nEntryTo=nEntries-1;

			printf("Prepare thread #%i: from=%llu, to=%llu\r\n",i+1,nEntryFrom,nEntryTo);

			memset(parrWorkParams+i,0,sizeof(WORKTHREADPARAMS));
			parrWorkParams[i].nThreadID=i+1;
			parrWorkParams[i].nEntryFrom=nEntryFrom;
			parrWorkParams[i].nEntryTo=nEntryTo;
			parrWorkParams[i].plistEvents=parrlistData+i;
			strcpy(parrWorkParams[i].szFileName, szRootFilename);

			sprintf(szWorkThreadName,"WTH%02i",i+1);
			ppWorkThreads[i]=new TThread(szWorkThreadName,ThLoadEvents,(void*)(parrWorkParams+i));
			ppWorkThreads[i]->Run();

			nEntryFrom=nEntryTo+1;
		}

		//gSystem->Sleep(10000);
		TThread::Ps();

		for(i=0;i<nMaxNumOfThreads;i++){
			ppWorkThreads[i]->Join();
			listData.AddAll(parrlistData+i);
			delete ppWorkThreads[i];

		}

		delete [] parrWorkParams;
		delete [] parrlistData;
		delete [] ppWorkThreads;
	}
	else{
		//printf("Single thread mode\r\n");
		WORKTHREADPARAMS WorkParams;
		memset(&WorkParams,0,sizeof(WORKTHREADPARAMS));

		WorkParams.nEntryFrom=0;
		WorkParams.nEntryTo=nEntries-1;
		WorkParams.nThreadID=1;
		WorkParams.plistEvents=&listData;
		strcpy(WorkParams.szFileName, szRootFilename);

		ThLoadEvents(&WorkParams);
	}

	return listData.GetSize();
}

CAlfaEvent* CAlfaEvent::GetEntry(const char szRootFilename[], const Long64_t nEntry)
{
    CAlfaEvent* pEventData=NULL;

    TFile* pFile=new TFile(szRootFilename);
    pEventData=GetEntry(pFile,nEntry);
    pFile->Close();

    return pEventData;
}

CAlfaEvent* CAlfaEvent::GetEntry(TFile* pFile, const Long64_t nEntry)
{
    CAlfaEvent* pEventData=NULL;

    if(pFile!=NULL){
		TTree* pEventTree=(TTree*)pFile->Get(ms_EventTreeName.c_str());
		if(pEventTree!=NULL){
			PALFAEVENTHEADER pEventHeader=new ALFAEVENTHEADER();
			PALFATRACKINGDATA pTrackingData=new ALFATRACKINGDATA();
			BookEventHeaderVariables(pEventTree, pEventHeader);
			BookTrackingDataVariables(pEventTree, pTrackingData);

			pEventTree->GetEntry(nEntry);
			pEventData=new CAlfaEvent();
			pEventData->FillEventHeader(pEventHeader);
			pEventData->FillTrackingData(pTrackingData);

			if(pEventHeader) delete pEventHeader;
			if(pTrackingData) delete pTrackingData;
		}

    }

    return pEventData;
}


bool CAlfaEvent::FillEventHeader(const PALFAEVENTHEADER pAlfaEventHeader)
{
	TThread::Lock();

	//EventInfo
    m_uRunNumber=pAlfaEventHeader->uRunNumber;
	m_uLumiBlockNumber=pAlfaEventHeader->uLumiBlockNumber;
    m_uEventNumber=pAlfaEventHeader->uEventNumber;
    m_uTimeStamp=pAlfaEventHeader->uTimeStamp;
    m_uTimeStampNs=pAlfaEventHeader->uTimeStampNs;

    //DCS IDs
	m_ullDcsBlm=pAlfaEventHeader->ullDcsBlm;
	m_ullDcsHVChannel=pAlfaEventHeader->ullDcsHVChannel;
	m_ullDcsLocalMonitoring=pAlfaEventHeader->ullDcsLocalMonitoring;
	m_ullDcsMovement=pAlfaEventHeader->ullDcsMovement;
	m_ullDcsRadMon=pAlfaEventHeader->ullDcsRadMon;
	m_ullDcsTriggerRates=pAlfaEventHeader->ullDcsTriggerRates;
	m_ullDcsFEConfiguration=pAlfaEventHeader->ullDcsFEConfiguration;
	m_ullDcsTriggerSettings=pAlfaEventHeader->ullDcsTriggerSettings;

    //RawDataContainer
	m_vecScaler=*pAlfaEventHeader->pvecScaler;
	m_nBCId=pAlfaEventHeader->nBCId;
	m_nTimeStp=pAlfaEventHeader->nTimeStp;
	m_nTimeStpNs=pAlfaEventHeader->nTimeStpNs;
	m_vecTrigPat=*pAlfaEventHeader->pvecTrigPat;

    //DigitCollection
	m_vecFiberHitsMD=*pAlfaEventHeader->pvecFiberHitsMD;
	m_vecMultiplicityMD=*pAlfaEventHeader->pvecMultiplicityMD;

    //ODDigitCollection
	m_vecFiberHitsODPos=*pAlfaEventHeader->pvecFiberHitsODPos;
	m_vecFiberHitsODNeg=*pAlfaEventHeader->pvecFiberHitsODNeg;
	m_vecMultiplicityODPos=*pAlfaEventHeader->pvecMultiplicityODPos;
	m_vecMultiplicityODNeg=*pAlfaEventHeader->pvecMultiplicityODNeg;

	TThread::UnLock();

    return true;
}

bool CAlfaEvent::FillTrackingData(const PALFATRACKINGDATA pAlfaTrackingData)
{
    //LocRecEvCollection & LocRecODEvCollection
	m_nMaxTrackCnt=pAlfaTrackingData->nMaxTrackCnt;
	m_vecXDetCS=*pAlfaTrackingData->pvecXDetCS;
	m_vecYDetCS=*pAlfaTrackingData->pvecYDetCS;
	m_vecDetectorPartID=*pAlfaTrackingData->pvecDetectorPartID;
	m_vecOverU=*pAlfaTrackingData->pvecOverU;
	m_vecOverV=*pAlfaTrackingData->pvecOverV;
	m_vecOverY=*pAlfaTrackingData->pvecOverY;
	m_vecNumU=*pAlfaTrackingData->pvecNumU;
	m_vecNumV=*pAlfaTrackingData->pvecNumV;
	m_vecNumY=*pAlfaTrackingData->pvecNumY;
	m_vecMDFibSel=*pAlfaTrackingData->pvecMDFibSel;
	m_vecODFibSel=*pAlfaTrackingData->pvecODFibSel;

    //LocRecCorrEvCollection & LocRecCorrODEvCollection
	m_vecXLhcCS=*pAlfaTrackingData->pvecXLhcCS;
	m_vecYLhcCS=*pAlfaTrackingData->pvecYLhcCS;
	m_vecZLhcCS=*pAlfaTrackingData->pvecZLhcCS;
	m_vecXRPotCS=*pAlfaTrackingData->pvecXRPotCS;
	m_vecYRPotCS=*pAlfaTrackingData->pvecYRPotCS;
	m_vecXStatCS=*pAlfaTrackingData->pvecXStatCS;
	m_vecYStatCS=*pAlfaTrackingData->pvecYStatCS;
	m_vecXBeamCS=*pAlfaTrackingData->pvecXBeamCS;
	m_vecYBeamCS=*pAlfaTrackingData->pvecYBeamCS;

    return true;
}

bool CAlfaEvent::BookEventHeaderVariables(TTree* pTree, PALFAEVENTHEADER pAlfaEventHeader)
{
    //pTree->Print();

	//EventInfo
    pTree->SetBranchAddress("RunNumber", &pAlfaEventHeader->uRunNumber);
	pTree->SetBranchAddress("lbn", &pAlfaEventHeader->uLumiBlockNumber);
    pTree->SetBranchAddress("EventNumber", &pAlfaEventHeader->uEventNumber);
    pTree->SetBranchAddress("timestamp", &pAlfaEventHeader->uTimeStamp);
    pTree->SetBranchAddress("timestamp_ns", &pAlfaEventHeader->uTimeStampNs);

    //DCS IDs
	pTree->SetBranchAddress("alfa_DCS_BLM", &pAlfaEventHeader->ullDcsBlm);
	pTree->SetBranchAddress("alfa_DCS_HVCHANNEL", &pAlfaEventHeader->ullDcsHVChannel);
	pTree->SetBranchAddress("alfa_DCS_LOCALMONITORING", &pAlfaEventHeader->ullDcsLocalMonitoring);
	pTree->SetBranchAddress("alfa_DCS_MOVEMENT", &pAlfaEventHeader->ullDcsMovement);
	pTree->SetBranchAddress("alfa_DCS_RADMON", &pAlfaEventHeader->ullDcsRadMon);
	pTree->SetBranchAddress("alfa_DCS_TRIGGERRATES", &pAlfaEventHeader->ullDcsTriggerRates);
	pTree->SetBranchAddress("alfa_DCS_FECONFIGURATION", &pAlfaEventHeader->ullDcsFEConfiguration);
	pTree->SetBranchAddress("alfa_DCS_TRIGGERSETTINGS", &pAlfaEventHeader->ullDcsTriggerSettings);

    //RawDataContainer
	pTree->SetBranchAddress("alfa_Scaler", &pAlfaEventHeader->pvecScaler);
	pTree->SetBranchAddress("alfa_BCId", &pAlfaEventHeader->nBCId);
	pTree->SetBranchAddress("alfa_TimeStp", &pAlfaEventHeader->nTimeStp);
	pTree->SetBranchAddress("alfa_TimeStp_ns", &pAlfaEventHeader->nTimeStpNs);
	pTree->SetBranchAddress("alfa_TrigPat", &pAlfaEventHeader->pvecTrigPat);

    //DigitCollection
	pTree->SetBranchAddress("alfa_FiberHitsMD", &pAlfaEventHeader->pvecFiberHitsMD);
	pTree->SetBranchAddress("alfa_MultiMD", &pAlfaEventHeader->pvecMultiplicityMD);

    //ODDigitCollection
	pTree->SetBranchAddress("alfa_FiberHitsODPos", &pAlfaEventHeader->pvecFiberHitsODPos);
	pTree->SetBranchAddress("alfa_FiberHitsODNeg", &pAlfaEventHeader->pvecFiberHitsODNeg);
	pTree->SetBranchAddress("alfa_MultiODPos", &pAlfaEventHeader->pvecMultiplicityODPos);
	pTree->SetBranchAddress("alfa_MultiODNeg", &pAlfaEventHeader->pvecMultiplicityODNeg);

    return true;
}

bool CAlfaEvent::BookTrackingDataVariables(TTree* pTree, PALFATRACKINGDATA pAlfaTrackingData)
{
    //pTree->Print();

    //LocRecEvCollection & LocRecODEvCollection
	pTree->SetBranchAddress("alfa_NumTrack", &pAlfaTrackingData->nMaxTrackCnt);
	pTree->SetBranchAddress("alfa_x_Det", &pAlfaTrackingData->pvecXDetCS);
	pTree->SetBranchAddress("alfa_y_Det", &pAlfaTrackingData->pvecYDetCS);
	pTree->SetBranchAddress("alfa_Detector", &pAlfaTrackingData->pvecDetectorPartID);
	pTree->SetBranchAddress("alfa_OverU", &pAlfaTrackingData->pvecOverU);
	pTree->SetBranchAddress("alfa_OverV", &pAlfaTrackingData->pvecOverV);
	pTree->SetBranchAddress("alfa_OverY", &pAlfaTrackingData->pvecOverY);
	pTree->SetBranchAddress("alfa_NU", &pAlfaTrackingData->pvecNumU);
	pTree->SetBranchAddress("alfa_NV", &pAlfaTrackingData->pvecNumV);
	pTree->SetBranchAddress("alfa_NY", &pAlfaTrackingData->pvecNumY);
	pTree->SetBranchAddress("alfa_Fib_SelMD", &pAlfaTrackingData->pvecMDFibSel);
	pTree->SetBranchAddress("alfa_Fib_SelOD", &pAlfaTrackingData->pvecODFibSel);

    //LocRecCorrEvCollection & LocRecCorrODEvCollection
	pTree->SetBranchAddress("alfa_x_LHC", &pAlfaTrackingData->pvecXLhcCS);
	pTree->SetBranchAddress("alfa_y_LHC", &pAlfaTrackingData->pvecYLhcCS);
	pTree->SetBranchAddress("alfa_z_LHC", &pAlfaTrackingData->pvecZLhcCS);
	pTree->SetBranchAddress("alfa_x_Pot", &pAlfaTrackingData->pvecXRPotCS);
	pTree->SetBranchAddress("alfa_y_Pot", &pAlfaTrackingData->pvecYRPotCS);
	pTree->SetBranchAddress("alfa_x_Stat", &pAlfaTrackingData->pvecXStatCS);
	pTree->SetBranchAddress("alfa_y_Stat", &pAlfaTrackingData->pvecYStatCS);
	pTree->SetBranchAddress("alfa_x_Beam", &pAlfaTrackingData->pvecXBeamCS);
	pTree->SetBranchAddress("alfa_y_Beam", &pAlfaTrackingData->pvecYBeamCS);

    return true;
}

void CAlfaEvent::PrintEventHeader()
{
    int i, j, k;

    //EventInfo
    printf("RunNumber: %u\r\n", m_uRunNumber);
	printf("LumiBlockNumber: %u\r\n", m_uLumiBlockNumber);
    printf("EventNumber: %u\r\n", m_uEventNumber);
    printf("TimeStamp: %u\r\n", m_uTimeStamp);
    printf("TimeStampNs: %u\r\n", m_uTimeStampNs);

	printf("\r\nDCS BLM ID=%llu\r\n",m_ullDcsBlm);
	printf("DCS HVCHANNEL ID=%llu\r\n",m_ullDcsHVChannel);
	printf("DCS LOCALMONITORING ID=%llu\r\n",m_ullDcsLocalMonitoring);
	printf("DCS MOVEMENT ID=%llu\r\n",m_ullDcsMovement);
	printf("DCS RADMON ID=%llu\r\n",m_ullDcsRadMon);
	printf("DCS TRIGGERRATES ID=%llu\r\n",m_ullDcsTriggerRates);
	printf("DCS FECONFIGURATION ID=%llu\r\n",m_ullDcsFEConfiguration);
	printf("DCS TRIGGERSETTINGS ID=%llu\r\n",m_ullDcsTriggerSettings);

    printf("\r\nScaler info:\r\n");
    for(i=0;i<RPOTSCNT;i++) printf("Roman Pot #%i: %i\r\n",i,GetScaler(i));

    printf("\r\nTrigger pattern info:\r\n");
    for(i=0;i<RPOTSCNT;i++){
	printf("#%i: ",i);
	for(j=0;j<TRIGPATCNT;j++){
	    printf("%i ",GetTriggerPattern(i,j));
	}
	printf("\r\n");
    }
    printf("\r\n");

    printf("Fiber Hits in MD info:\r\n");
    for(i=0;i<RPOTSCNT;i++){
	for(j=0;j<MDLAYERSCNT*MDPLATESCNT;j++){
	    printf("Roman Pot #%i, Plate #%i:",i,j);
	    for(k=0;k<MDFIBERSCNT;k++){
		printf("%i ",GetFiberHitsMD(i,j,k));
	    }
	    printf("\r\n");
	}
	printf("\r\n");
    }

    printf("Multiplicity in MD info:\r\n");
    for(i=0;i<RPOTSCNT;i++){
	for(j=0;j<MDLAYERSCNT*MDPLATESCNT;j++){
	    printf("Roman Pot #%i, Plate #%i: %i\r\n",i,j,GetMultiplicityMD(i,j));
	}
    }
    printf("\r\n");


    printf("\r\nFiber Hits in ODPos info:\r\n");
    for(i=0;i<RPOTSCNT;i++){
	for(j=0;j<ODPLATESCNT;j++){
	    printf("Roman Pot #%i, Plate #%i:",i,j);
	    for(k=0;k<ODLAYERSCNT*ODFIBERSCNT;k++){
		printf("%i ",GetFiberHitsODPos(i,j,k));
	    }
	    printf("\r\n");
	}
	printf("\r\n");
    }

    printf("Multiplicity in ODPos info:\r\n");
    for(i=0;i<RPOTSCNT;i++){
	for(j=0;j<ODPLATESCNT;j++){
	    printf("Roman Pot #%i, Plate #%i: %i\r\n",i,j,GetMultiplicityODPos(i,j));
	}
    }
    printf("\r\n");

    printf("\r\nFiber Hits in ODNeg info:\r\n");
    for(i=0;i<RPOTSCNT;i++){
		for(j=0;j<ODPLATESCNT;j++){
			printf("Roman Pot #%i, Plate #%i:",i,j);
			for(k=0;k<ODLAYERSCNT*ODFIBERSCNT;k++){
				printf("%i ",GetFiberHitsODNeg(i,j,k));
			}
			printf("\r\n");
		}
		printf("\r\n");
    }

    printf("Multiplicity in ODNeg info:\r\n");
    for(i=0;i<RPOTSCNT;i++){
		for(j=0;j<ODPLATESCNT;j++){
			printf("Roman Pot #%i, Plate #%i: %i\r\n",i,j,GetMultiplicityODNeg(i,j));
		}
    }
    printf("\r\n");

}

void CAlfaEvent::PrintTrackingData()
{
    int i, j, k;
    int nDetID;

    printf("\r\nMaximal number of local tracks: %i\r\n", m_nMaxTrackCnt);

    for(i=0;i<RPOTSCNT;i++){
		printf("\r\nRoman pot #%i\r\n",i);

		for(j=0;j<m_nMaxTrackCnt;j++){
			nDetID=GetDetectorPartID(i,j);

			if(nDetID>0){
				printf("track #%i CS:\r\n",j);

				printf("detector=%i\r\n",nDetID);
				printf("DetCS=[%.3f,%.3f], ",GetXDetCS(i,j),GetYDetCS(i,j));
				printf("BeamCS=[%.3f,%.3f], ",GetXBeamCS(i,j),GetYBeamCS(i,j));
				printf("RPotCS=[%.3f,%.3f], ",GetXRPotCS(i,j),GetYRPotCS(i,j));
				printf("StatCS=[%.3f,%.3f], ",GetXStatCS(i,j),GetYStatCS(i,j));
				printf("LhcCS=[%.3f,%.3f,%.3f]",GetXLhcCS(i,j),GetYLhcCS(i,j),GetZLhcCS(i,j));
				printf("\r\n");

				printf("OverU=%.3f, OverV=%.3f, OverY=%.3f\r\n",GetOverU(i,j),GetOverV(i,j),GetOverY(i,j));
				printf("NumU=%i, NumV=%i, NumY=%i\r\n",GetNumU(i,j),GetNumV(i,j),GetNumY(i,j));

				printf("MD fiber selection:\r\n");
				for(k=0;k<MDLAYERSCNT*MDPLATESCNT;k++) printf("%i ",GetMDFibSel(i,j,k));
				printf("\r\nOD fiber selection:\r\n");
				for(k=0;k<ODPLATESCNT;k++) printf("%i ",GetODFibSel(i,j,k));
				printf("\r\n");
			}
		}
    }
}

bool CAlfaEvent::CheckDcsLink(const char szRootFilename[], const Long64_t nMaxEntries)
{
	bool bRes=false;

	TFile* pFile=new TFile(szRootFilename);
	bRes=CheckDcsLink(pFile, nMaxEntries);
	pFile->Close();

	return bRes;
}

bool CAlfaEvent::CheckDcsLink(TFile* pFile, const Long64_t nMaxEntries)
{
	bool bRes=true;
	Long64_t i;
	Long64_t nEventCnt=nMaxEntries;

	map<ULong64_t,DCSBLM> mapDcsBlm;
	map<ULong64_t,DCSHVCHANNEL> mapDcsHVChannel;
	map<ULong64_t,DCSLOCALMONITORING> mapDcsLocalMonitoring;
	map<ULong64_t,DCSMOVEMENT> mapDcsMovement;
	map<ULong64_t,DCSRADMON> mapDcsRadMon;
	map<ULong64_t,DCSTRIGGERRATES> mapDcsTriggerRates;
	map<ULong64_t,DCSFECONFIGURATION> mapDcsFEConfiguration;
	map<ULong64_t,DCSTRIGGERSETTINGS> mapDcsTriggerSettings;

	FillDcsBlmMap(pFile, mapDcsBlm);
	FillDcsHVChannelMap(pFile, mapDcsHVChannel);
	FillDcsLocalMonitoringMap(pFile, mapDcsLocalMonitoring);
	FillDcsMovementMap(pFile, mapDcsMovement);
	FillDcsRadMonMap(pFile, mapDcsRadMon);
	FillDcsTriggerRatesMap(pFile, mapDcsTriggerRates);
	FillDcsFEConfigurationMap(pFile, mapDcsFEConfiguration);
	FillDcsTriggerSettingsMap(pFile, mapDcsTriggerSettings);

	CAlfaEvent* pEventData;
	Long64_t nEntries=CAlfaEvent::ResolveEntries(pFile);

	if(nEventCnt<0) nEventCnt=nEntries;
	else if(nEventCnt>nEntries) nEventCnt=nEntries;

	for(i=0;i<nEventCnt;i++){
		pEventData=CAlfaEvent::GetEntry(pFile,i);
		if(i%10000==0 && i>0) printf("Scanned %llu entries\r\n",i);

		if(pEventData!=NULL){
			if(mapDcsBlm.find(pEventData->GetDcsBlmID())==mapDcsBlm.end()){
				bRes=false;
				printf("WARNING: Cannot find proper DCS BLM data for event %u\r\n",pEventData->GetEventNumber());
			}
			if(mapDcsHVChannel.find(pEventData->GetDcsHVChannelID())==mapDcsHVChannel.end()){
				bRes=false;
				printf("WARNING: Cannot find proper DCS HVCHANNEL data for event %u\r\n",pEventData->GetEventNumber());
			}
			if(mapDcsLocalMonitoring.find(pEventData->GetDcsLocalMonitoringID())==mapDcsLocalMonitoring.end()){
				bRes=false;
				printf("WARNING: Cannot find proper DCS LOCALMONITORING data for event %u\r\n",pEventData->GetEventNumber());
			}
			if(mapDcsMovement.find(pEventData->GetDcsMovementID())==mapDcsMovement.end()){
				bRes=false;
				printf("WARNING: Cannot find proper DCS MOVEMENT data for event %u\r\n",pEventData->GetEventNumber());
			}
			if(mapDcsRadMon.find(pEventData->GetDcsRadMonID())==mapDcsRadMon.end()){
				bRes=false;
				printf("WARNING: Cannot find proper DCS RADMON data for event %u\r\n",pEventData->GetEventNumber());
			}
			if(mapDcsTriggerRates.find(pEventData->GetDcsTriggerRateID())==mapDcsTriggerRates.end()){
				bRes=false;
				printf("WARNING: Cannot find proper DCS TRIGGERRATES data for event %u\r\n",pEventData->GetEventNumber());
			}
			if(mapDcsFEConfiguration.find(pEventData->GetDcsFEConfigurationID())==mapDcsFEConfiguration.end()){
				bRes=false;
				printf("WARNING: Cannot find proper DCS FECONFIGURATION data for event %u\r\n",pEventData->GetEventNumber());
			}
			if(mapDcsTriggerSettings.find(pEventData->GetDcsTriggerSettingsID())==mapDcsTriggerSettings.end()){
				bRes=false;
				printf("WARNING: Cannot find proper DCS TRIGGERSETTINGS data for event %u\r\n",pEventData->GetEventNumber());
			}

			delete pEventData;
			pEventData=NULL;
		}
	}

	return bRes;
}
