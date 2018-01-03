/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SIDLOCRECO_h
#define AFP_SIDLOCRECO_h

#include <iostream>
#include <string>
#include <list>
#include <map>
#include <vector>
#include <fstream>

//#include "GaudiKernel/Algorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IToolSvc.h"

#include "AthenaKernel/getMessageSvc.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

//#include "AFP_RawEv/AFP_RawData.h"
//#include "AFP_RawEv/AFP_RawDataContainer.h"
//#include "AFP_RawEv/AFP_RawDataCollection.h"
//#include "AFP_RawEv/AFP_SIDDigitCollection.h"
//#include "AFP_RawEv/AFP_TDDigitCollection.h"

#include "AFP_Geometry/AFP_constants.h"
#include "AFP_Geometry/AFP_Geometry.h"
#include "AFP_Geometry/AFP_ConfigParams.h"
#include "CLHEP/Geometry/Point3D.h"

#include "AFP_DigiEv/AFP_SiDigiCollection.h"
#include "AFP_LocRecoEv/AFP_SIDLocRecoEvCollection.h"

//#include "AFP_LocRecoEv/AFP_SIDLocRecoEvCollection.h"

#include "AFP_LocReco/AFP_UserObjects.h"
#include "AFP_LocReco/AFP_SIDBasicKalman.h"

#include "TROOT.h"

//for truth particles
#include "GeneratorObjects/McEventCollection.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"

// xAOD 
#include "xAODForward/AFPTrackContainer.h"


#define SIDSTATIONID 4

class StoreGateSvc;
class ActiveStoreSvc;

class AFP_SIDLocReco : public AthAlgorithm
{
	public:
                AFP_SIDLocReco(const std::string& name, ISvcLocator* pSvcLocator);
		~AFP_SIDLocReco();

	private:
		AFP_CONFIGURATION m_Config;
		AFP_Geometry* m_pGeometry;
		
		// a handle on Store Gate
		StoreGateSvc* m_storeGate;
		//StoreGateSvc* m_pDetStore;

		AFP_SIDLocRecoEvCollection*	m_pSIDLocRecoEvCollection;
		AFP_SIDLocRecoEvent*			m_pSIDLocRecoEvent;

	private:

		UInt_t m_eventNum;			//real event number
		Int_t m_iRunNum;
		Int_t m_iDataType;			//data type (simulation or real data) using in the local reconstruction
		Int_t m_iEvent;				//event number from zero value
		Float_t m_AmpThresh;			// TD signal amplitude threshold
		
		//slope and X,Y,Z-pos for SID plates [4][6]
		Float_t m_fsSID[SIDSTATIONID][SIDCNT];
		Float_t m_fxSID[SIDSTATIONID][SIDCNT];
		Float_t m_fySID[SIDSTATIONID][SIDCNT];
		Float_t m_fzSID[SIDSTATIONID][SIDCNT];


                std::string m_strKeyGeometryForReco;
                std::vector<std::string> m_vecListAlgoSID;
                std::string m_strAlgoSID;
	
                std::string m_strKeySIDLocRecoEvCollection;
                std::string m_strSIDCollectionName;

	public:
		StatusCode initialize();
		StatusCode execute();
		StatusCode finalize();
		
	private:
		bool ReadGeometryDetCS();
		bool StoreReconstructionGeometry(/*const char* szDataDestination*/);
		void SaveGeometry();
		void ClearGeometry();

		StatusCode AFPCollectionReading(std::list<SIDHIT> &ListSIDHits);	

		StatusCode RecordSIDCollection();
                StatusCode ExecuteRecoMethod(const std::string strAlgo, const std::list<SIDHIT> &ListSIDHits, xAOD::AFPTrackContainer* resultContainer);
};

#endif	//AFP_TDLOCRECO_h
