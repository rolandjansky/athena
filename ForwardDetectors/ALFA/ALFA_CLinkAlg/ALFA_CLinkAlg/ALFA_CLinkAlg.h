/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_CLINKALG_H
#define ALFA_CLINKALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "AthLinks/DataLink.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/IIOVSvc.h"
#include "AthenaKernel/IIOVDbSvc.h"

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

enum eRecType { ERC_LOCUNCORRECTED, ERC_LOCCORRECTED, ERC_GLOBAL };

class ALFA_CLinkAlg : public AthAlgorithm
{
public:
	ALFA_CLinkAlg (const std::string& name, ISvcLocator* pSvcLocator);
	~ALFA_CLinkAlg();

private:
	DCSID m_CurrentDCSId;
	ServiceHandle< IIOVDbSvc > m_iovSvc;

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
	StatusCode AddCOOLFolderCallback(const std::string& szFolder);
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
	std::vector<float> m_vecXDetCS;
	std::vector<float> m_vecYDetCS;
	std::vector<int> m_vecDetectorPartID;
	std::vector<float> m_vecOverU;
	std::vector<float> m_vecOverV;
	std::vector<float> m_vecOverY;
	std::vector<int> m_vecNumU;
	std::vector<int> m_vecNumV;
	std::vector<int> m_vecNumY;
	std::vector<int> m_vecMDFibSel;
	std::vector<int> m_vecODFibSel;

	//xAOD variables: LocRecCorrEvCollection & LocRecCorrODEvCollection
	std::vector<float> m_vecXLhcCS;
	std::vector<float> m_vecYLhcCS;
	std::vector<float> m_vecZLhcCS;
	std::vector<float> m_vecXRPotCS;
	std::vector<float> m_vecYRPotCS;
	std::vector<float> m_vecXStatCS;
	std::vector<float> m_vecYStatCS;
	std::vector<float> m_vecXBeamCS;
	std::vector<float> m_vecYBeamCS;

	//RawDataContainer
	std::vector<int> m_vecScaler;
	//int m_nBCId;
	//int m_nTimeStamp;
	//int m_nTimeStamp_ns;
	std::vector<int> m_vecTrigPat;

	//DigitCollection
	std::vector<int> m_vecMDFiberHits;
	std::vector<int> m_vecMDMultiplicity;

	//ODDigitCollection
	std::vector<int> m_vecODFiberHitsPos;
	std::vector<int> m_vecODFiberHitsNeg;
	std::vector<int> m_vecODMultiplicityPos;
	std::vector<int> m_vecODMultiplicityNeg;
};

#endif // ALFA_CLINKALG_H
