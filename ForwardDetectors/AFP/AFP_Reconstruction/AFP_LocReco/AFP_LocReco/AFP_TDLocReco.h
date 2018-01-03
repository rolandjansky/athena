/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

	#ifndef AFP_TDLOCRECO_h
#define AFP_TDLOCRECO_h

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
//#include "AFP_Geometry/AFP_GeometryReader.h"
//#include "AFP_Geometry/AFP_constants.h"
#include "AFP_DigiEv/AFP_TDDigiCollection.h"
#include "AFP_LocRecoEv/AFP_TDLocRecoEvCollection.h"

//#include "AFP_LocRecoEv/AFP_SIDLocRecoEvCollection.h"

#include "AFP_LocReco/AFP_UserObjects.h"
#include "AFP_LocReco/AFP_TDZVertex.h"

#include "TROOT.h"

//for truth particles
#include "GeneratorObjects/McEventCollection.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"



#define NSIDE  3
#define NTRACK 10
#define NTRIG  4
#define NBPM   4

class StoreGateSvc;
class ActiveStoreSvc;

class AFP_TDLocReco : public AthAlgorithm
{
	public:
                AFP_TDLocReco(const std::string& name, ISvcLocator* pSvcLocator);
		~AFP_TDLocReco();

	private:
		//GEOMETRYCONFIGURATION m_Config;
		//AFP_GeometryReader* m_pGeometryReader;
		// a handle on Store Gate
		StoreGateSvc* m_storeGate;
		//StoreGateSvc* m_pDetStore;

		AFP_TDLocRecoEvCollection*	m_pTDLocRecoEvCollection;
		AFP_TDLocRecoEvent*			m_pTDLocRecoEvent;

	private:

		UInt_t m_eventNum;			//real event number
		Int_t m_iRunNum;
		Int_t m_iDataType;			//data type (simulation or real data) using in the local reconstruction
		Int_t m_iEvent;				//event number from zero value
		Float_t m_AmpThresh;			// TD signal amplitude threshold

                std::string m_strKeyGeometryForReco;
                std::vector<std::string> m_vecListAlgoTD;
                std::string m_strAlgoTD;
	
                std::string m_strKeyTDLocRecoEvCollection;
                std::string m_strTDCollectionName;

	public:
		StatusCode initialize();
		StatusCode execute();
		StatusCode finalize();
		
	private:
		bool ReadGeometryDetCS();
		bool StoreReconstructionGeometry(/*const char* szDataDestination*/);
		void SaveGeometry();
		void ClearGeometry();

		StatusCode AFPCollectionReading(std::list<TDHIT> &ListTDHits);	

		StatusCode RecordTDCollection();
		StatusCode ExecuteRecoMethod(const std::string strAlgo, const std::list<TDHIT> &ListMDHits);
};

#endif	//AFP_TDLOCRECO_h
