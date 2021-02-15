/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_NTUPLE_H
#define ALFA_NTUPLE_H

#include "AthenaBaseComps/AthHistogramAlgorithm.h"

#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/IIOVSvc.h"
#include "AthenaKernel/IIOVDbSvc.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include <string>
#include <iostream>
#include <fstream>
#include <list>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <vector>
#include <map>

#include "TTree.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TString.h"
#include "TLine.h"
#include "TPaveStats.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH2.h"
#include "THStack.h"
#include "TMath.h"
#include "TF1.h"
#include "TLatex.h"
#include "TList.h"
#include "TMathBase.h"

//for truth particles
#include "GeneratorObjects/McEventCollection.h"

// for ALFA
#include "ALFA_RawEv/ALFA_RawData.h"
#include "ALFA_RawEv/ALFA_RawDataContainer.h"
#include "ALFA_RawEv/ALFA_RawDataCollection.h"
#include "ALFA_RawEv/ALFA_DigitCollection.h"
#include "ALFA_RawEv/ALFA_ODDigitCollection.h"
#include "ALFA_LocRecEv/ALFA_LocRecEvCollection.h"
#include "ALFA_LocRecEv/ALFA_LocRecODEvCollection.h"
#include "ALFA_LocRecCorrEv/ALFA_LocRecCorrEvCollection.h"
#include "ALFA_LocRecCorrEv/ALFA_LocRecCorrODEvCollection.h"
#include "ALFA_GloRecEv/ALFA_GloRecEvCollection.h"
#include "ALFA_Geometry/ALFA_constants.h"
#include "ALFA_Geometry/ALFA_GeometryReader.h"
#include "ALFA_LocRec/ALFA_UserObjects.h"

#define MAXNUMTRACKS 100
#define MAXNUMGLOBTRACKS 100
#define MAXNUMGENPARTICLES 100

#define COOLFOLDER_BLM             "/RPO/DCS/BLM"
#define COOLFOLDER_HVCHANNEL       "/RPO/DCS/HVCHANNEL"
#define COOLFOLDER_LOCALMONITORING "/RPO/DCS/LOCALMONITORING"
#define COOLFOLDER_MOVEMENT        "/RPO/DCS/MOVEMENT"
#define COOLFOLDER_RADMON          "/RPO/DCS/RADMON"
#define COOLFOLDER_TRIGGERRATES    "/RPO/DCS/TRIGGERRATES"
#define COOLFOLDER_FECONFIGURATION "/RPO/DCS/FECONFIGURATION"
#define COOLFOLDER_TRIGGERSETTINGS "/RPO/DCS/TRIGGERSETTINGS"
#define COOLFOLDER_LHC_FILLSTATE   "/LHC/DCS/FILLSTATE"
#define COOLFOLDER_LHC_FILLPARAMS  "/TDAQ/OLC/LHC/FILLPARAMS"
#define COOLFOLDER_OLC_ALFA        "/TDAQ/OLC/ALFA"

typedef struct _MDLOCREC
{
	int iEvent;
	int iRPot;

	double fRecPosX;
	double fRecPosY;
} MDLOCREC, *PMDLOCREC;

typedef struct _ODLOCREC
{
	int iRPot;
	int iSide;

	double fRecPos;
} ODLOCREC, *PODLOCREC;

typedef struct _BLM
{
	uint64_t iTimeIOVstart;
	uint64_t iTimeIOVstop;
	Float_t fBLM[6];

	void clear();
} BLM, *PBLM;

typedef struct _HVCHANNEL
{
	uint64_t iTimeIOVstart;
	uint64_t iTimeIOVstop;
	Float_t fActualVMeas[216];
	Float_t fActualIMeas[216];

	void clear();
} HVCHANNEL, *PHVCHANNEL;

typedef struct _LOCALMONITORING
{
	uint64_t iTimeIOVstart;
	uint64_t iTimeIOVstop;
	Float_t fTempSensor1[RPOTSCNT];
	Float_t fTempSensor2[RPOTSCNT];
	Float_t fTempSensor3[RPOTSCNT];
	Float_t fTempSensor4[RPOTSCNT];
	Float_t fTempSensor5[RPOTSCNT];

	void clear();
} LOCALMONITORING, *PLOCALMONITORING;

typedef struct _MOVEMENT
{
	uint64_t iTimeIOVstart;
	uint64_t iTimeIOVstop;
	Float_t fPosLVDT[RPOTSCNT];
	Float_t fPosMotor[RPOTSCNT];

	void clear();
} MOVEMENT, *PMOVEMENT;

typedef struct _RADMON
{
	uint64_t iTimeIOVstart;
	uint64_t iTimeIOVstop;
	Float_t fDose[4];
	Float_t fFluence[4];
	Float_t fTemp[4];

	void clear();
} RADMON, *PRADMON;

typedef struct _TRIGGERRATE
{
	uint64_t iTimeIOVstart;
	uint64_t iTimeIOVstop;
	Int_t iTRate[RPOTSCNT];

	void clear();
} TRIGGERRATE, *PTRIGGERRATE;

typedef struct _BPMALFA
{
	uint64_t iTimeIOVstart;
	uint64_t iTimeIOVstop;
	Float_t bpmr_r_x_pos; 	// x position on BPMR.6R1.B1 (231.535 m from IP)
	Float_t bpmr_r_y_pos; 	// y position on BPMR.6R1.B1 (231.535 m from IP)
	Float_t bpmr_r_x_err; 	// Error on x position on BPMR.6R1.B1 (231.535 m from IP)
	Float_t bpmr_r_y_err; 	// Error on y position on BPMR.6R1.B1 (231.535 m from IP)
	Float_t bpmr_l_x_pos; 	// x position on BPMR.6L1.B2 (225.245 m from IP)
	Float_t bpmr_l_y_pos; 	// y position on BPMR.6L1.B2 (225.245 m from IP)
	Float_t bpmr_l_x_err; 	// Error on x position on BPMR.6L1.B2 (225.245 m from IP)
	Float_t bpmr_l_y_err; 	// Error on y position on BPMR.6L1.B2 (225.245 m from IP)
	Float_t bpmsa_r_x_pos; 	// x position on BPMSA.7R1.B1 (on ALFA station A7R1 237.7505 m from IP)
	Float_t bpmsa_r_y_pos; 	// y position on BPMSA.7R1.B1 (on ALFA station A7R1 237.7505 m from IP)
	Float_t bpmsa_r_x_err; 	// Error on x position on BPMSA.7R1.B1 (on ALFA station A7R1 237.7505 m from IP)
	Float_t bpmsa_r_y_err; 	// Error on y position on BPMSA.7R1.B1 (on ALFA station A7R1 237.7505 m from IP)
	Float_t bpmsa_l_x_pos; 	// x position on BPMSA.7L1.B2 (on ALFA station A7L1 237.7505 m from IP)
	Float_t bpmsa_l_y_pos; 	// y position on BPMSA.7L1.B2 (on ALFA station A7L1 237.7505 m from IP)
	Float_t bpmsa_l_x_err; 	// Error on x position on BPMSA.7L1.B2 (on ALFA station A7L1 237.7505 m from IP)
	Float_t bpmsa_l_y_err; 	// Error on y position on BPMSA.7L1.B2 (on ALFA station A7L1 237.7505 m from IP)
	Float_t bpmwb_r_x_pos; 	// x position on BPMWB.4R1.B1 (151.0945 m from IP)
	Float_t bpmwb_r_y_pos; 	// y position on BPMWB.4R1.B1 (151.0945 m from IP)
	Float_t bpmwb_r_x_err; 	// Error pn x position on BPMWB.4R1.B1 (151.0945 m from IP)
	Float_t bpmwb_r_y_err; 	// Error on y position on BPMWB.4R1.B1 (151.0945 m from IP)
	Float_t bpmya_l_x_pos; 	// x position on BPMYA.4L1.B2 (172.227 m from the IP)
	Float_t bpmya_l_y_pos; 	// y position on BPMYA.4L1.B2 (172.227 m from the IP)
	Float_t bpmya_l_x_err; 	// Error on x position on BPMYA.4L1.B2 (172.227 m from the IP)
	Float_t bpmya_l_y_err; 	// Error on y position on BPMYA.4L1.B2 (172.227 m from the IP)

	void clear();
} BPMALFA, *PBPMALFA;


typedef struct _ALGOTREEMD
{
	int     iAlgoID;
	UInt_t  iEvent;
	Int_t   iNumTrack;
	Int_t   iDetector[MAXNUMTRACKS][RPOTSCNT];
	Int_t   iNumU[MAXNUMTRACKS][RPOTSCNT];
	Int_t   iNumV[MAXNUMTRACKS][RPOTSCNT];
	Int_t   iFibSelMD[MAXNUMTRACKS][RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT];
	Float_t fXDet[MAXNUMTRACKS][RPOTSCNT];
	Float_t fYDet[MAXNUMTRACKS][RPOTSCNT];
	Float_t fXPot[MAXNUMTRACKS][RPOTSCNT];
	Float_t fYPot[MAXNUMTRACKS][RPOTSCNT];
	Float_t fXLHC[MAXNUMTRACKS][RPOTSCNT];
	Float_t fYLHC[MAXNUMTRACKS][RPOTSCNT];
	Float_t fZLHC[MAXNUMTRACKS][RPOTSCNT];
	Float_t fXStat[MAXNUMTRACKS][RPOTSCNT];
	Float_t fYStat[MAXNUMTRACKS][RPOTSCNT];
	Float_t fXBeam[MAXNUMTRACKS][RPOTSCNT];
	Float_t fYBeam[MAXNUMTRACKS][RPOTSCNT];
	Float_t fOverU[MAXNUMTRACKS][RPOTSCNT];
	Float_t fOverV[MAXNUMTRACKS][RPOTSCNT];
	Float_t fInterceptDet[MAXNUMTRACKS][RPOTSCNT];
	Float_t fSlopeDet[MAXNUMTRACKS][RPOTSCNT];
	Bool_t  bRecFlag[MAXNUMTRACKS][RPOTSCNT];

	void clear();
} ALGOTREEMD, *PALGOTREEMD;

typedef struct _ALGOTREEOD
{
	int     iAlgoID;
	UInt_t  iEvent;
	Int_t   iNumTrack;
	Int_t   iDetector[MAXNUMTRACKS][RPOTSCNT];
	Int_t   iNumY[MAXNUMTRACKS][RPOTSCNT];
	Int_t   iFibSelOD[MAXNUMTRACKS][RPOTSCNT][ODPLATESCNT];
	Float_t fOverY[MAXNUMTRACKS][RPOTSCNT];
	Float_t fXDet[MAXNUMTRACKS][RPOTSCNT];
	Float_t fYDet[MAXNUMTRACKS][RPOTSCNT];
	Float_t fXPot[MAXNUMTRACKS][RPOTSCNT];
	Float_t fYPot[MAXNUMTRACKS][RPOTSCNT];
	Float_t fXLHC[MAXNUMTRACKS][RPOTSCNT];
	Float_t fYLHC[MAXNUMTRACKS][RPOTSCNT];
	Float_t fZLHC[MAXNUMTRACKS][RPOTSCNT];
	Float_t fXStat[MAXNUMTRACKS][RPOTSCNT];
	Float_t fYStat[MAXNUMTRACKS][RPOTSCNT];
	Float_t fXBeam[MAXNUMTRACKS][RPOTSCNT];
	Float_t fYBeam[MAXNUMTRACKS][RPOTSCNT];
	Float_t fInterceptDet[MAXNUMTRACKS][RPOTSCNT];
	Float_t fSlopeDet[MAXNUMTRACKS][RPOTSCNT];
	Bool_t  bRecFlag[MAXNUMTRACKS][RPOTSCNT];

	void clear();
} ALGOTREEOD, *PALGOTREEOD;

class StoreGateSvc;
class ActiveStoreSvc;

class ALFA_Ntuple : public AthHistogramAlgorithm
{
public:
	ALFA_Ntuple(const std::string& name, ISvcLocator* pSvcLocator);
	~ALFA_Ntuple();

	ALFA_Ntuple(const ALFA_Ntuple&);
	ALFA_Ntuple& operator= (const ALFA_Ntuple&);

private:
	ServiceHandle< IIOVDbSvc > m_iovSvc;
	//StoreGateSvc* m_storeGate;
	//StoreGateSvc* m_pDetStore;

	int m_iRunEpoch;				// epoch of LHC runs, Run I or Run II

	int m_iDataType;			//data type (simulation or real data) using in the local reconstruction
	int m_iGeneratorType;		//generator type (particle gun, pythia+beamTransport, flat file) using in the simulation
	int m_iEvtFromZero;			//event number for events counted from zero

	bool m_bVtxKinFillFlag;
	bool m_bCoolData;

	std::string m_strCollectionName;
	std::string m_strODCollectionName;
	std::string m_strTruthCollectionName;
	std::string m_strLocRecCollectionName;
	std::string m_strLocRecODCollectionName;
	std::string m_strLocRecCorrCollectionName;
	std::string m_strLocRecCorrODCollectionName;
	std::string m_strGloRecCollectionName;
	std::string m_strKeyRawDataCollection;
	std::string m_coolDirName;

	TTree *m_TreeTrackingData, *m_TreeRunHeader, *m_TreeGlobalTracks, *m_TreeEventHeader;
	TTree *m_Tree_Vtx_Kin_Info;

	std::vector<std::string> m_CoolFolders;
	std::vector<std::string> m_vecListAlgoMD;
	std::vector<std::string> m_vecListAlgoOD;
	std::map<std::string, int> m_MapAlgoNameToID;
	std::map<int, TTree*> m_MapTreeOtherAlgo;
	std::map<int, ALGOTREEMD> m_MapAlgoTreeMD;
	std::map<int, ALGOTREEOD> m_MapAlgoTreeOD;
	TTree *m_treeOtherAlgo;
	std::string m_strMainAlgoMD;
	std::string m_strMainAlgoOD;


private:
	UInt_t m_iEvent;
	Int_t  m_iFillNum, m_iRunNum;

	//RunHeader tree ----------------------------------------------------------------
	Bool_t  m_bTriggerSet[RPOTSCNT][6];
	Int_t   m_iLatency[RPOTSCNT][3];
	Float_t m_fTransformDetRP[RPOTSCNT][4][4], m_fProtonMomentum;

	//EventHeader tree --------------------------------------------------------------
	UInt_t  m_iLumBlock;
	Int_t   m_iQDCTrig[RPOTSCNT][2];
	Int_t   m_iMultiMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT], m_iMultiODPos[RPOTSCNT][ODPLATESCNT], m_iMultiODNeg[RPOTSCNT][ODPLATESCNT];
	Int_t   m_iScaler[RPOTSCNT], m_iBCId, m_iTimeStamp, m_iTimeStamp_ns;

//	Bool_t  m_bTrigPat[RPOTSCNT][16], m_bLVL1TrigSig[256], m_bLVL2TrigSig[256], m_bHLTrigSig[256], m_bDQFlag[25];
	Bool_t  m_bTrigPat[RPOTSCNT][16], m_bDQFlag[25], m_bLVL2TrigSig[256], m_bHLTrigSig[256];

	// in RUN II there are 512 LVL1 triggers, also LVL1
	Bool_t m_bLVL1TrigSig_TBP[512];
	Bool_t m_bLVL1TrigSig_TAP[512];
	Bool_t m_bLVL1TrigSig_TAV[512];

	Bool_t  m_bFibHitsMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT];
	Bool_t  m_bFibHitsODNeg[RPOTSCNT][ODPLATESCNT][ODLAYERSCNT*ODFIBERSCNT], m_bFibHitsODPos[RPOTSCNT][ODPLATESCNT][ODLAYERSCNT*ODFIBERSCNT];

	//BeamLossMon, HVChannel, LocalMonitoring, Movement, Radmon, TriggerRate, BPMALFA
	Int_t m_iFlagBLM, m_iFlagVolt, m_iFlagTemperature, m_iFlagPotPos, m_iFlagRadDose, m_iFlagTRate, m_iFlagBPM;

	//TrackingData tree -------------------------------------------------------------
	Int_t   m_iNumTrack;
	Int_t   m_iDetector[MAXNUMTRACKS][RPOTSCNT];
	Int_t   m_iNumU[MAXNUMTRACKS][RPOTSCNT], m_iNumV[MAXNUMTRACKS][RPOTSCNT], m_iNumY[MAXNUMTRACKS][RPOTSCNT];
	Int_t   m_iFibSelMD[MAXNUMTRACKS][RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT], m_iFibSelOD[MAXNUMTRACKS][RPOTSCNT][ODPLATESCNT];
	Float_t m_fXDet[MAXNUMTRACKS][RPOTSCNT], m_fYDet[MAXNUMTRACKS][RPOTSCNT];
	Float_t m_fXPot[MAXNUMTRACKS][RPOTSCNT], m_fYPot[MAXNUMTRACKS][RPOTSCNT];
	Float_t m_fXLHC[MAXNUMTRACKS][RPOTSCNT], m_fYLHC[MAXNUMTRACKS][RPOTSCNT], m_fZLHC[MAXNUMTRACKS][RPOTSCNT];
	Float_t m_fXStat[MAXNUMTRACKS][RPOTSCNT], m_fYStat[MAXNUMTRACKS][RPOTSCNT];
	Float_t m_fXBeam[MAXNUMTRACKS][RPOTSCNT], m_fYBeam[MAXNUMTRACKS][RPOTSCNT];
	Float_t m_fOverU[MAXNUMTRACKS][RPOTSCNT], m_fOverV[MAXNUMTRACKS][RPOTSCNT], m_fOverY[MAXNUMTRACKS][RPOTSCNT];
	Float_t m_fInterceptDet[MAXNUMTRACKS][RPOTSCNT], m_fSlopeDet[MAXNUMTRACKS][RPOTSCNT];
	Bool_t  m_bRecFlag[MAXNUMTRACKS][RPOTSCNT];

	//Vertex_and_IP_kinematics tree -------------------------------------------------
	Float_t m_fVtx_g[4], m_fVtx_A[4], m_fVtx_C[4], m_fp_beam1_i[4], m_fp_beam1_f[4], m_fp_beam2_i[4], m_fp_beam2_f[4];
	Float_t m_fp_A[4], m_fp_C[4];
	Float_t m_ft_13, m_ft_24;
	
	Float_t m_fvtx_beam1_f[4], m_fvtx_beam2_f[4];

	//GlobalTracks tree -------------------------------------------------------------
//	Int_t   m_iNumGlobTrack, m_iTrack[2], m_iRPot[2];
//	Float_t m_fInterceptLHC[MAXNUMGLOBTRACKS], m_fSlopeLHC[MAXNUMGLOBTRACKS];
	Int_t   m_iNumGloTrack;
	Int_t m_iArmGlo[MAXNUMGLOBTRACKS];
	Float_t m_fxPosGlo[MAXNUMGLOBTRACKS], m_fyPosGlo[MAXNUMGLOBTRACKS];
	Float_t m_fxSlopeGlo[MAXNUMGLOBTRACKS], m_fySlopeGlo[MAXNUMGLOBTRACKS];

	//COOL data trees ---------------------------------------------------------------
	TTree *m_TreeMovement, *m_TreeLocalMon, *m_TreeBLM, *m_TreeHVChannel, *m_TreeRadmon, *m_TreeTriggerRate, *m_TreeBPMALFA;
	Int_t m_iThreshold[RPOTSCNT][3], m_iGain[RPOTSCNT][5];
	std::vector<std::string>* m_vecMainDetGainMode;

private:
	BLM             m_BeamLossMonitor;
	HVCHANNEL       m_HVChannel;
	LOCALMONITORING m_LocalMonitoring;
	MOVEMENT        m_Movement;
	RADMON          m_Radmon;
	TRIGGERRATE     m_TriggerRate;
	BPMALFA         m_BPMALFA;
	ALGOTREEMD      m_AlgoTreeMD;
	ALGOTREEOD      m_AlgoTreeOD;

public:
	StatusCode initialize();
	StatusCode execute();
	StatusCode finalize();

private:

	StatusCode ALFA_info();
	StatusCode TruthInfo();
	StatusCode GetRawDataCollection();
	StatusCode GetLocRecData();
	StatusCode GetLocRecCorrData();
	StatusCode GetGloRecData();
	StatusCode ALFACollectionReading();

	StatusCode COOLUpdate(IOVSVC_CALLBACK_ARGS_P(I, keys));
	StatusCode AddCOOLFolderCallback(const std::string& szFolder);
	StatusCode AddCOOLFolderCallbackAthenaAttributeList(const std::string& Folder);
	StatusCode COOLIOVRange(std::string strFolder, uint64_t &iTimeIOVStart, uint64_t &iTimeIOVStop);

};

#endif // ALFA_Ntuple_h

