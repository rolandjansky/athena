/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_CLINKALG_H
#define ALFA_CLINKALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "DataModel/DataLink.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/IIOVSvc.h"
#include "AthenaKernel/IIOVDbSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "ALFA_CLinkEv/ALFA_CLinkEvent.h"
#include "xAODForward/ALFADataContainer.h"

#define EVCOLLNAME_XAODALFADATACONTAINER "ALFADataContainer"
#define EVCOLLNAME_XAODALFADATAAUXCONTAINER "ALFADataContainerAux."

#define RPOTSCNT 8
#define MDLAYERSCNT	2
#define MDPLATESCNT	10
#define MDFIBERSCNT	64
#define ODLAYERSCNT	2
#define ODPLATESCNT	3
#define ODFIBERSCNT	15

#define TRIGPATCNT 16
#define BLMCNT 6
#define HVCHANNELCNT 216
#define RADMONCNT 4
#define FECNFTHRESHLOLDCNT 3
#define FECNFGAINCNT 5
#define TRIGSETCNT 6
#define TRIGSETLATENCYCNT 3

#define MAXNUMTRACKS 100
#define MAXNUMGLOBTRACKS 100
#define MAXNUMGENPARTICLES 100
#define MAXPILEUP 500

#include  <string>
using namespace std;

enum eRecType { ERC_LOCUNCORRECTED, ERC_LOCCORRECTED, ERC_GLOBAL };

class ALFA_CLinkAlg : public AthAlgorithm
{
public:
	ALFA_CLinkAlg (const std::string& name, ISvcLocator* pSvcLocator);
	~ALFA_CLinkAlg();

private:
	DCSID m_CurrentDCSId;
	ServiceHandle< IIOVDbSvc > m_iovSvc;
	//ServiceHandle<IIncidentSvc> m_incidentSvc;

	int m_nDataType; //data type (simulation or real data) using in the local reconstruction
	int m_nProcessingMode; //1=offline, 2=online

public:
	StatusCode initialize();
	StatusCode execute();
	StatusCode finalize();

public:
	StatusCode LoadAllEventData(ALFA_CLinkEvent* pDataEvent);
private:
	StatusCode COOLUpdate(IOVSVC_CALLBACK_ARGS_P(I, keys));
	StatusCode AddCOOLFolderCallback(const string& szFolder);
	unsigned long long CalcDCSId(eDCSItem eItem);
	StatusCode CalcAllDCSIds(PDCSID pDCSIds);

private:
	StatusCode GenerateXAOD();
	StatusCode FillXAOD_TrackingData(xAOD::ALFADataContainer* pxAODContainer);
	StatusCode FillXAOD_HeaderData(xAOD::ALFADataContainer* pxAODContainer);
	void ClearXAODTrackingData(const int nMaxTrackCnt, eRecType eType);
	void ClearXAODHeaderData();

private:
	//xAOD variables: LocRecEvCollection & LocRecODEvCollection
	int m_nMaxTrackCnt;
	vector<float> m_vecXDetCS;
	vector<float> m_vecYDetCS;
	vector<int> m_vecDetectorPartID;
	vector<float> m_vecOverU;
	vector<float> m_vecOverV;
	vector<float> m_vecOverY;
	vector<int> m_vecNumU;
	vector<int> m_vecNumV;
	vector<int> m_vecNumY;
	vector<int> m_vecMDFibSel;
	vector<int> m_vecODFibSel;

	//xAOD variables: LocRecCorrEvCollection & LocRecCorrODEvCollection
	vector<float> m_vecXLhcCS;
	vector<float> m_vecYLhcCS;
	vector<float> m_vecZLhcCS;
	vector<float> m_vecXRPotCS;
	vector<float> m_vecYRPotCS;
	vector<float> m_vecXStatCS;
	vector<float> m_vecYStatCS;
	vector<float> m_vecXBeamCS;
	vector<float> m_vecYBeamCS;

	//RawDataContainer
	vector<int> m_vecScaler;
	//int m_nBCId;
	//int m_nTimeStamp;
	//int m_nTimeStamp_ns;
	vector<int> m_vecTrigPat;

	//DigitCollection
	vector<int> m_vecMDFiberHits;
	vector<int> m_vecMDMultiplicity;

	//ODDigitCollection
	vector<int> m_vecODFiberHitsPos;
	vector<int> m_vecODFiberHitsNeg;
	vector<int> m_vecODMultiplicityPos;
	vector<int> m_vecODMultiplicityNeg;
};

#endif // ALFA_CLINKALG_H
