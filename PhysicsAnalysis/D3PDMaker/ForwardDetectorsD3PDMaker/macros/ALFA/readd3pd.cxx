/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// The "main" function

#include "Constants.h"
//#include "CAlfaEvent.h"

// ROOT
#include "TROOT.h"
#include "TFile.h"
#include "TList.h"

#include <map>
//using namespace std;

void readd3pd(const char* szFilename, Long64_t nEventCnt)
{
	if(!TClass::GetDict("CAlfaEvent")){
		printf("Loading CAlfaEvent ...\r\n");
		gROOT->ProcessLine(".L CAlfaEvent.cxx+");
	}

	gROOT->ProcessLine(".L DictLoader.cxx+");

	//--- Load DCS data
	printf("Load DCS data ...\r\n");
	map<ULong64_t,DCSBLM> mapDcsBlm;
	map<ULong64_t,DCSHVCHANNEL> mapDcsHVChannel;
	map<ULong64_t,DCSLOCALMONITORING> mapDcsLocalMonitoring;
	map<ULong64_t,DCSMOVEMENT> mapDcsMovement;
	map<ULong64_t,DCSRADMON> mapDcsRadMon;
	map<ULong64_t,DCSTRIGGERRATES> mapDcsTriggerRates;
	map<ULong64_t,DCSFECONFIGURATION> mapDcsFEConfiguration;
	map<ULong64_t,DCSTRIGGERSETTINGS> mapDcsTriggerSettings;

	//Specify paths
	CAlfaEvent::SetNTuplePaths("MinBiasTree","MinBiasTreeMeta");

	CAlfaEvent::FillDcsBlmMap(szFilename, mapDcsBlm);
	CAlfaEvent::FillDcsHVChannelMap(szFilename, mapDcsHVChannel);
	CAlfaEvent::FillDcsLocalMonitoringMap(szFilename, mapDcsLocalMonitoring);
	CAlfaEvent::FillDcsMovementMap(szFilename, mapDcsMovement);
	CAlfaEvent::FillDcsRadMonMap(szFilename, mapDcsRadMon);
	CAlfaEvent::FillDcsTriggerRatesMap(szFilename, mapDcsTriggerRates);
	CAlfaEvent::FillDcsFEConfigurationMap(szFilename, mapDcsFEConfiguration);
	CAlfaEvent::FillDcsTriggerSettingsMap(szFilename, mapDcsTriggerSettings);

	printf("Number of BLM records=%i\r\n",mapDcsBlm.size());
	printf("Number of HVCHANNEL records=%i\r\n",mapDcsHVChannel.size());
	printf("Number of LOCALMONITORING records=%i\r\n",mapDcsLocalMonitoring.size());
	printf("Number of MOVEMENT records=%i\r\n",mapDcsMovement.size());
	printf("Number of RADMON records=%i\r\n",mapDcsRadMon.size());
	printf("Number of TRIGGERRATE records=%i\r\n",mapDcsTriggerRates.size());
	printf("Number of FECONFIGURATION records=%i\r\n",mapDcsFEConfiguration.size());
	printf("Number of TRIGGERSETTINGS records=%i\r\n",mapDcsTriggerSettings.size());
	printf("... done.\r\n");

	//--- Check if all events are properly linked to all DCS data
	printf("\r\nCheck DCS data ...\r\n");
	bool bRes=CAlfaEvent::CheckDcsLink(szFilename, nEventCnt);
	printf("Check status: %s\r\n",bRes? "success":"failure");
	printf("... done.\r\n");

	//--- Read single entry and print out
	printf("\r\nRead single entry ...\r\n");
	Long64_t nEventEntry=5;
	CAlfaEvent* pEventData=CAlfaEvent::GetEntry(szFilename, nEventEntry);
    if(pEventData!=NULL)
    {
		//print event's info
		pEventData->PrintEventHeader();
		//pEventData->PrintTrackingData();

		printf("Event #%i: LVDT(A7L1U)=%.3f mm\r\n",pEventData->GetEventNumber(), mapDcsMovement[pEventData->GetDcsMovementID()].arrLVDT[2]);
    }
	printf("... done.\r\n");

	//--- Load all data into TList in one step
	/*
	printf("\r\nLoad all data ...\r\n");
    Long64_t nEventCntRead=0;
    TList listAlfaData;
	nEventCntRead=CAlfaEvent::LoadEvents(szFilename, listAlfaData, nEventCnt);
	printf("Number of loaded events: %i\r\n", nEventCntRead);*/

    gROOT->Reset();
}

