/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALFAEVENT_H
#define CALFAEVENT_H

#include "TTree.h"
#include "TFile.h"
#include "TList.h"
#include <vector>
#include <map>
#include <string>

using namespace std;

#include "Constants.h"

#define PRINTMODULO 10000
#define MTHREADSTH 10000
#define EVENTTREENAME "AlfaD3PDTree"
#define DCSFOLDER "AlfaD3PDTreeMeta"

#define DCSTREENAME_BLM "alfa_BlmTree"
#define DCSTREENAME_HVCHANNEL "alfa_HVChannelTree"
#define DCSTREENAME_LOCALMONITORING "alfa_LocalMonitoringTree"
#define DCSTREENAME_MOVEMENT "alfa_MovementTree"
#define DCSTREENAME_RADMON "alfa_RadMonTree"
#define DCSTREENAME_TRIGGERRATES "alfa_TriggerRatesTree"
#define DCSTREENAME_FECONFIGURATION "alfa_FEConfigurationTree"
#define DCSTREENAME_TRIGGERSETTINGS "alfa_TriggerSettingsTree"

typedef struct _ALFAEVENTHEADER {
    _ALFAEVENTHEADER();

	//EventInfo
	UInt_t uRunNumber;
	UInt_t uLumiBlockNumber;
	UInt_t uEventNumber;
	UInt_t uTimeStamp;
	UInt_t uTimeStampNs;

	//DCS IDs
	ULong64_t ullDcsBlm;
	ULong64_t ullDcsHVChannel;
	ULong64_t ullDcsLocalMonitoring;
	ULong64_t ullDcsMovement;
	ULong64_t ullDcsRadMon;
	ULong64_t ullDcsTriggerRates;
	ULong64_t ullDcsFEConfiguration;
	ULong64_t ullDcsTriggerSettings;

	//RawDataContainer
	vector<Int_t>* pvecScaler;
	Int_t nBCId;
	Int_t nTimeStp;
	Int_t nTimeStpNs;
	vector<Int_t>* pvecTrigPat;

	//DigitCollection
	vector<Int_t>* pvecFiberHitsMD;
	vector<Int_t>* pvecMultiplicityMD;

	//ODDigitCollection
	vector<Int_t>* pvecFiberHitsODPos;
	vector<Int_t>* pvecFiberHitsODNeg;
	vector<Int_t>* pvecMultiplicityODPos;
	vector<Int_t>* pvecMultiplicityODNeg;

} ALFAEVENTHEADER, *PALFAEVENTHEADER;

typedef struct _ALFATRACKINGDATA {
    _ALFATRACKINGDATA();

    //LocRecEvCollection & LocRecODEvCollection
	Int_t nMaxTrackCnt;
	vector<Float_t>* pvecXDetCS;
	vector<Float_t>* pvecYDetCS;
	vector<Int_t>* pvecDetectorPartID;
	vector<Float_t>* pvecOverU;
	vector<Float_t>* pvecOverV;
	vector<Float_t>* pvecOverY;
	vector<Int_t>* pvecNumU;
	vector<Int_t>* pvecNumV;
	vector<Int_t>* pvecNumY;
	vector<Int_t>* pvecMDFibSel;
	vector<Int_t>* pvecODFibSel;

    //LocRecCorrEvCollection & LocRecCorrODEvCollection
	vector<Float_t>* pvecXLhcCS;
	vector<Float_t>* pvecYLhcCS;
	vector<Float_t>* pvecZLhcCS;
	vector<Float_t>* pvecXRPotCS;
	vector<Float_t>* pvecYRPotCS;
	vector<Float_t>* pvecXStatCS;
	vector<Float_t>* pvecYStatCS;
	vector<Float_t>* pvecXBeamCS;
	vector<Float_t>* pvecYBeamCS;

} ALFATRACKINGDATA, *PALFATRACKINGDATA;

typedef struct _DCSBLM {
	ULong64_t ulID;
	Float_t arrBLM[BLMCNT];
} DCSBLM, *PDCSBLM;

typedef struct _DCSHVCHANNEL {
	ULong64_t ulID;
	Float_t arrVoltage[HVCHANNELCNT];
	Float_t arrCurrent[HVCHANNELCNT];
} DCSHVCHANNEL, *PDCSHVCHANNEL;

typedef struct _DCSLOCALMONITORING {
	ULong64_t ulID;
	Float_t arrTempSensor01[RPOTSCNT];
	Float_t arrTempSensor02[RPOTSCNT];
	Float_t arrTempSensor03[RPOTSCNT];
	Float_t arrTempSensor04[RPOTSCNT];
	Float_t arrTempSensor05[RPOTSCNT];
} DCSLOCALMONITORING, *PDCSLOCALMONITORING;

typedef struct _DCSMOVEMENT {
	ULong64_t ulID;
	Float_t arrLVDT[RPOTSCNT];
	Float_t arrMotor[RPOTSCNT];
} DCSMOVEMENT, *PDCSMOVEMENT;

typedef struct _DCSRADMON {
	ULong64_t ulID;
	Float_t arrDose[RADMONCNT];
	Float_t arrFluence[RADMONCNT];
	Float_t arrTemperature[RADMONCNT];
} DCSRADMON, *PDCSRADMON;

typedef struct _DCSTRIGGERRATES {
	ULong64_t ulID;
	Int_t arrTriggerRates[RPOTSCNT];
} DCSTRIGGERRATES, *PDCSTRIGGERRATES;

typedef struct _DCSFECONFIGURATION {
	ULong64_t ulID;
	Int_t arrThreshold[RPOTSCNT][FECNFTHRESHLOLDCNT];
	Int_t arrGain[RPOTSCNT][FECNFGAINCNT];
	Char_t arrMDGainMode[RPOTSCNT][32];
} DCSFECONFIGURATION, *PDCSFECONFIGURATION;

typedef struct _DCSTRIGGERSETTINGS {
	ULong64_t ulID;
	Bool_t arrTriggerSet[RPOTSCNT][TRIGSETCNT];
	Int_t arrLatency[RPOTSCNT][TRIGSETLATENCYCNT];
} DCSTRIGGERSETTINGS, *PDCSTRIGGERSETTINGS;

typedef struct _WORKTHREADPARAMS {
	int nThreadID;
	Long64_t nEntryFrom;
	Long64_t nEntryTo;
	TList* plistEvents;
	Char_t szFileName[512];
} WORKTHREADPARAMS, *PWORKTHREADPARAMS;

class CAlfaEvent : public TObject
{
public:
	CAlfaEvent();
    ~CAlfaEvent();

public:
	static void* ThLoadEvents(void* pParams);
	static Long64_t LoadEvents(const char szRootFilename[], TList& listData, const Long64_t nMaxEntries=-1);
	static CAlfaEvent* GetEntry(const char szRootFilename[], const Long64_t nEntry);
	static CAlfaEvent* GetEntry(TFile* pFile, const Long64_t nEntry);
	static Long64_t ResolveEntries(const char szRootFilename[]);
	static Long64_t ResolveEntries(TFile* pFile);
	static bool CheckDcsLink(const char szRootFilename[], const Long64_t nMaxEntries=-1);
	static bool CheckDcsLink(TFile* pFile, const Long64_t nMaxEntries=-1);

	static Long64_t FillDcsBlmMap(const char szRootFilename[], map<ULong64_t,DCSBLM>& mapDcsData);
	static Long64_t FillDcsBlmMap(TFile* pFile, map<ULong64_t,DCSBLM>& mapDcsData);
	static Long64_t FillDcsHVChannelMap(const char szRootFilename[], map<ULong64_t,DCSHVCHANNEL>& mapDcsData);
	static Long64_t FillDcsHVChannelMap(TFile* pFile, map<ULong64_t,DCSHVCHANNEL>& mapDcsData);
	static Long64_t FillDcsLocalMonitoringMap(const char szRootFilename[], map<ULong64_t,DCSLOCALMONITORING>& mapDcsData);
	static Long64_t FillDcsLocalMonitoringMap(TFile* pFile, map<ULong64_t,DCSLOCALMONITORING>& mapDcsData);
	static Long64_t FillDcsMovementMap(const char szRootFilename[], map<ULong64_t,DCSMOVEMENT>& mapDcsData);
	static Long64_t FillDcsMovementMap(TFile* pFile, map<ULong64_t,DCSMOVEMENT>& mapDcsData);
	static Long64_t FillDcsRadMonMap(const char szRootFilename[], map<ULong64_t,DCSRADMON>& mapDcsData);
	static Long64_t FillDcsRadMonMap(TFile* pFile, map<ULong64_t,DCSRADMON>& mapDcsData);
	static Long64_t FillDcsTriggerRatesMap(const char szRootFilename[], map<ULong64_t,DCSTRIGGERRATES>& mapDcsData);
	static Long64_t FillDcsTriggerRatesMap(TFile* pFile, map<ULong64_t,DCSTRIGGERRATES>& mapDcsData);
	static Long64_t FillDcsFEConfigurationMap(const char szRootFilename[], map<ULong64_t,DCSFECONFIGURATION>& mapDcsData);
	static Long64_t FillDcsFEConfigurationMap(TFile* pFile, map<ULong64_t,DCSFECONFIGURATION>& mapDcsData);
	static Long64_t FillDcsTriggerSettingsMap(const char szRootFilename[], map<ULong64_t,DCSTRIGGERSETTINGS>& mapDcsData);
	static Long64_t FillDcsTriggerSettingsMap(TFile* pFile, map<ULong64_t,DCSTRIGGERSETTINGS>& mapDcsData);

public:
	void PrintEventHeader();
	void PrintTrackingData();

private:
	static bool BookEventHeaderVariables(TTree* pTree, PALFAEVENTHEADER pAlfaEventHeader);
	static bool BookTrackingDataVariables(TTree* pTree, PALFATRACKINGDATA pAlfaTrackingData);

private:
	bool FillEventHeader(const PALFAEVENTHEADER pAlfaEventHeader);
	bool FillTrackingData(const PALFATRACKINGDATA pAlfaTrackingData);

private:
	static string ms_EventTreeName;
	static string ms_DCSFolder;

public:
	static void SetNTuplePaths(string& EventTreeName, string& DCSFolder);

public:
	//EventInfo
	UInt_t GetRunNumber() const { return m_uRunNumber; }
	UInt_t GetLumiBlockNumber() const { return m_uLumiBlockNumber; }
	UInt_t GetEventNumber() const { return m_uEventNumber; }
	UInt_t GetTimeStamp() const { return m_uTimeStamp; }
	UInt_t GetTimeStampNs() const { return m_uTimeStampNs; }

	//DCS IDs
	ULong64_t GetDcsBlmID() const { return m_ullDcsBlm; }
	ULong64_t GetDcsHVChannelID() const { return m_ullDcsHVChannel; }
	ULong64_t GetDcsLocalMonitoringID() const { return m_ullDcsLocalMonitoring; }
	ULong64_t GetDcsMovementID() const { return m_ullDcsMovement; }
	ULong64_t GetDcsRadMonID() const { return m_ullDcsRadMon; }
	ULong64_t GetDcsTriggerRateID() const { return m_ullDcsTriggerRates; }
	ULong64_t GetDcsFEConfigurationID() const { return m_ullDcsFEConfiguration; }
	ULong64_t GetDcsTriggerSettingsID() const { return m_ullDcsTriggerSettings; }

	//RawDataContainer
	Int_t GetScaler(const int nRPotID) const { return m_vecScaler[nRPotID]; }
	Int_t GetBCId() const { return m_nBCId; }
	Int_t GetRDTimeStp() const { return m_nTimeStp; }
	Int_t GetRDTimeStpNs() const { return m_nTimeStpNs; }
	Int_t GetTriggerPattern(const int nRPotID, const int  nTrigPatID) const { return m_vecTrigPat[TRIGPATCNT*nRPotID+nTrigPatID]; }

	//DigitCollection
	Int_t GetFiberHitsMD(const int nRPotID, const int  nLayerID, const int nFiberID) const { return m_vecFiberHitsMD[(MDLAYERSCNT*MDPLATESCNT*MDFIBERSCNT)*nRPotID+MDFIBERSCNT*nLayerID+nFiberID]; }
	Int_t GetMultiplicityMD(const int nRPotID, const int  nLayerID) const { return m_vecMultiplicityMD[(MDLAYERSCNT*MDPLATESCNT)*nRPotID+nLayerID]; }

	//ODDigitCollection
	Int_t GetFiberHitsODPos(const int nRPotID, const int  nLayerID, const int nFiberID) const { return m_vecFiberHitsODPos[(ODPLATESCNT*ODLAYERSCNT*ODFIBERSCNT)*nRPotID+(ODLAYERSCNT*ODFIBERSCNT)*nLayerID+nFiberID]; }
	Int_t GetMultiplicityODPos(const int nRPotID, const int  nLayerID) const { return m_vecMultiplicityODPos[(ODPLATESCNT)*nRPotID+nLayerID]; }
	Int_t GetFiberHitsODNeg(const int nRPotID, const int  nLayerID, const int nFiberID) const { return m_vecFiberHitsODNeg[(ODPLATESCNT*ODLAYERSCNT*ODFIBERSCNT)*nRPotID+(ODLAYERSCNT*ODFIBERSCNT)*nLayerID+nFiberID]; }
	Int_t GetMultiplicityODNeg(const int nRPotID, const int  nLayerID) const { return m_vecMultiplicityODNeg[(ODPLATESCNT)*nRPotID+nLayerID]; }

	//LocRecEvCollection & LocRecODEvCollection
	Int_t GetMaxTrackCnt() const { return m_nMaxTrackCnt; }
	Float_t GetXDetCS(const int nRPotID, const int nTrackID) const { return m_vecXDetCS[m_nMaxTrackCnt*nRPotID+nTrackID]; }
	Float_t GetYDetCS(const int nRPotID, const int nTrackID) const { return m_vecYDetCS[m_nMaxTrackCnt*nRPotID+nTrackID]; }
	Int_t GetDetectorPartID(const int nRPotID, const int nTrackID) const { return m_vecDetectorPartID[m_nMaxTrackCnt*nRPotID+nTrackID]; }
	Float_t GetOverU(const int nRPotID, const int nTrackID) const { return m_vecOverU[m_nMaxTrackCnt*nRPotID+nTrackID]; }
	Float_t GetOverV(const int nRPotID, const int nTrackID) const { return m_vecOverV[m_nMaxTrackCnt*nRPotID+nTrackID]; }
	Float_t GetOverY(const int nRPotID, const int nTrackID) const { return m_vecOverY[m_nMaxTrackCnt*nRPotID+nTrackID]; }
	Int_t GetNumU(const int nRPotID, const int nTrackID) const { return m_vecNumU[m_nMaxTrackCnt*nRPotID+nTrackID]; }
	Int_t GetNumV(const int nRPotID, const int nTrackID) const { return m_vecNumV[m_nMaxTrackCnt*nRPotID+nTrackID]; }
	Int_t GetNumY(const int nRPotID, const int nTrackID) const { return m_vecNumY[m_nMaxTrackCnt*nRPotID+nTrackID]; }
	Int_t GetMDFibSel(const int nRPotID, const int nTrackID, const int nLayerID) const { return m_vecMDFibSel[(m_nMaxTrackCnt*MDLAYERSCNT*MDPLATESCNT)*nRPotID+(MDLAYERSCNT*MDPLATESCNT)*nTrackID+nLayerID]; }
	Int_t GetODFibSel(const int nRPotID, const int nTrackID, const int nLayerID) const { return m_vecODFibSel[(m_nMaxTrackCnt*ODPLATESCNT)*nRPotID+(ODPLATESCNT)*nTrackID+nLayerID]; }

	//LocRecCorrEvCollection & LocRecCorrODEvCollection
	Float_t GetXLhcCS(const int nRPotID, const int nTrackID) const { return m_vecXLhcCS[m_nMaxTrackCnt*nRPotID+nTrackID]; }
	Float_t GetYLhcCS(const int nRPotID, const int nTrackID) const { return m_vecYLhcCS[m_nMaxTrackCnt*nRPotID+nTrackID]; }
	Float_t GetZLhcCS(const int nRPotID, const int nTrackID) const { return m_vecZLhcCS[m_nMaxTrackCnt*nRPotID+nTrackID]; }
	Float_t GetXRPotCS(const int nRPotID, const int nTrackID) const { return m_vecXRPotCS[m_nMaxTrackCnt*nRPotID+nTrackID]; }
	Float_t GetYRPotCS(const int nRPotID, const int nTrackID) const { return m_vecYRPotCS[m_nMaxTrackCnt*nRPotID+nTrackID]; }
	Float_t GetXStatCS(const int nRPotID, const int nTrackID) const { return m_vecXStatCS[m_nMaxTrackCnt*nRPotID+nTrackID]; }
	Float_t GetYStatCS(const int nRPotID, const int nTrackID) const { return m_vecYStatCS[m_nMaxTrackCnt*nRPotID+nTrackID]; }
	Float_t GetXBeamCS(const int nRPotID, const int nTrackID) const { return m_vecXBeamCS[m_nMaxTrackCnt*nRPotID+nTrackID]; }
	Float_t GetYBeamCS(const int nRPotID, const int nTrackID) const { return m_vecYBeamCS[m_nMaxTrackCnt*nRPotID+nTrackID]; }

private:
	//EventInfo
	UInt_t m_uRunNumber;
	UInt_t m_uLumiBlockNumber;
	UInt_t m_uEventNumber;
	UInt_t m_uTimeStamp;
	UInt_t m_uTimeStampNs;

	//DCS IDs
	ULong64_t m_ullDcsBlm;
	ULong64_t m_ullDcsHVChannel;
	ULong64_t m_ullDcsLocalMonitoring;
	ULong64_t m_ullDcsMovement;
	ULong64_t m_ullDcsRadMon;
	ULong64_t m_ullDcsTriggerRates;
	ULong64_t m_ullDcsFEConfiguration;
	ULong64_t m_ullDcsTriggerSettings;

	//RawDataContainer
	vector<Int_t> m_vecScaler;//[RPOTSCNT]
	Int_t m_nBCId;
	Int_t m_nTimeStp;
	Int_t m_nTimeStpNs;
	vector<Int_t> m_vecTrigPat;//[RPOTSCNT][TRIGPATCNT]

	//DigitCollection
	vector<Int_t> m_vecFiberHitsMD;//[RPOTSCNT][MDLAYERSCNT*MDPLATESCNT][MDFIBERSCNT]
	vector<Int_t> m_vecMultiplicityMD;//[RPOTSCNT][MDLAYERSCNT*MDPLATESCNT]

	//ODDigitCollection
	vector<Int_t> m_vecFiberHitsODPos;//[RPOTSCNT][ODPLATESCNT][ODLAYERSCNT*ODFIBERSCNT]
	vector<Int_t> m_vecFiberHitsODNeg;//[RPOTSCNT][ODPLATESCNT][ODLAYERSCNT*ODFIBERSCNT]
	vector<Int_t> m_vecMultiplicityODPos;//[RPOTSCNT][ODPLATESCNT]
	vector<Int_t> m_vecMultiplicityODNeg;//[RPOTSCNT][ODPLATESCNT]

	//LocRecEvCollection & LocRecODEvCollection
	Int_t m_nMaxTrackCnt;
	vector<Float_t> m_vecXDetCS;
	vector<Float_t> m_vecYDetCS;
	vector<Int_t> m_vecDetectorPartID;
	vector<Float_t> m_vecOverU;
	vector<Float_t> m_vecOverV;
	vector<Float_t> m_vecOverY;
	vector<Int_t> m_vecNumU;
	vector<Int_t> m_vecNumV;
	vector<Int_t> m_vecNumY;
	vector<Int_t> m_vecMDFibSel;
	vector<Int_t> m_vecODFibSel;

	//LocRecCorrEvCollection & LocRecCorrODEvCollection
	vector<Float_t> m_vecXLhcCS;
	vector<Float_t> m_vecYLhcCS;
	vector<Float_t> m_vecZLhcCS;
	vector<Float_t> m_vecXRPotCS;
	vector<Float_t> m_vecYRPotCS;
	vector<Float_t> m_vecXStatCS;
	vector<Float_t> m_vecYStatCS;
	vector<Float_t> m_vecXBeamCS;
	vector<Float_t> m_vecYBeamCS;

public:
	ClassDef(CAlfaEvent,1);
};

#endif // CALFAEVENT_H
