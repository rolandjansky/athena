/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_LocReco/AFP_TDLocReco.h"
//#include "AFP_Geometry/AFP_GeometryReader.h"

#include "AthenaKernel/errorcheck.h"

AFP_TDLocReco::AFP_TDLocReco(const string& name, ISvcLocator* pSvcLocator) :
Algorithm(name, pSvcLocator)
{
	MsgStream LogStream(Athena::getMessageSvc(), "AFP_TDLocReco::AFP_TDLocReco");
	LogStream << MSG::DEBUG << "begin AFP_TDLocReco::AFP_TDLocReco" << endreq;

	//m_pGeometryReader = new AFP_GeometryReader();


	// data type using in the local reconstruction
	// for the simulation data the value is 0, for the real data the value is 1. Unset value is -1
	declareProperty("DataType", m_iDataType=0, "data type using in the local reconstruction");

	//reconstruction methods properties
	declareProperty("AmpThresh", m_AmpThresh=10.);


	//reconstruction method selection for TD
	declareProperty("AlgoTD", m_strAlgoTD="TDZVertex");


	m_vecListAlgoTD.clear();
	declareProperty("ListAlgoTD", m_vecListAlgoTD);



	m_strKeyGeometryForReco			= "AFP_GeometryForReco";
	m_strTDCollectionName			= "AFP_TDDigiCollection";
	m_strKeyTDLocRecoEvCollection		= "AFP_TDLocRecoEvCollection";


	m_pTDLocRecoEvCollection = NULL;
	m_pTDLocRecoEvent = NULL;
	m_storeGate = NULL;

	m_eventNum = 0;
	m_iEvent   = 0;
	m_iRunNum  = 0;

	LogStream << MSG::DEBUG << "end AFP_TDLocReco::AFP_TDLocReco" << endreq;
}

AFP_TDLocReco::~AFP_TDLocReco()
{
	MsgStream LogStream(Athena::getMessageSvc(), "AFP_TDLocReco::~AFP_TDLocReco");
	LogStream << MSG::DEBUG << "begin AFP_TDLocReco::~AFP_TDLocReco" << endreq;

// 	if(m_pGeometryReader!=NULL)
// 	{
// 		delete m_pGeometryReader;
// 		m_pGeometryReader = NULL;
// 	}

	LogStream << MSG::DEBUG << "begin AFP_TDLocReco::~AFP_TDLocReco" << endreq;
}

StatusCode AFP_TDLocReco::initialize()
{
	MsgStream LogStream(Athena::getMessageSvc(), "AFP_TDLocReco::initialize()");
	LogStream << MSG::DEBUG << "begin AFP_TDLocReco::initialize()" << endreq;

	StatusCode sc;
	//ClearGeometry();

	sc = service("StoreGateSvc",m_storeGate);
	if(sc.isFailure())
	{
		LogStream << MSG::ERROR << "reconstruction: unable to retrieve pointer to StoreGateSvc" << endreq;
		return sc;
	} 
	
	//read geometry
	/*if(ReadGeometryDetCS())
	{
		LogStream<<MSG::DEBUG<<"Geometry loaded successfully"<<endreq;
	}
	else
	{
		LogStream<<MSG::FATAL<<"Could not load geometry"<<endreq;
		return StatusCode::FAILURE;
	}*/

	//write AFP_GeometryReader to StoreGate
	/*if (StatusCode::SUCCESS!=service("DetectorStore",m_pDetStore))
	{
		LogStream << MSG::ERROR << "Detector store not found" << endreq;
		return StatusCode::FAILURE;
	}
 	sc = m_pDetStore->record(m_pGeometryReader, m_strKeyGeometryForReco);
	if(sc.isFailure())
	{
		LogStream << MSG::ERROR << "m_pGeometryReader: unable to record to StoreGate" << endreq;
		return sc;
	}*/

	m_iEvent = 0;

	LogStream << MSG::DEBUG << "end AFP_TDLocReco::initialize()" << endreq;
	return StatusCode::SUCCESS;
}

StatusCode AFP_TDLocReco::execute()
{
	MsgStream LogStream(Athena::getMessageSvc(), "AFP_TDLocReco::execute()");
	LogStream << MSG::DEBUG << "begin AFP_TDLocReco::execute()" << endreq;

	StatusCode sc;

	list<TDHIT> ListTDHits;
	ListTDHits.clear();

	m_eventNum = 0;
	m_iRunNum  = 0;
	const EventInfo* eventInfo;
	sc = m_storeGate->retrieve( eventInfo );
	if (sc.isFailure())
	{
		LogStream << MSG::ERROR << "AFP_TDLocReco, Cannot get event info." << endreq;
//		return sc;
	}
	else
	{
		// current event number
		m_eventNum = eventInfo->event_ID()->event_number();
		m_iRunNum = eventInfo->event_ID()->run_number();
	}

/*
	//  get truth information
	//////////////////////////////////////////
	const McEventCollection* mcTru = 0;     
	sc = m_storeGate->retrieve(mcTru,"TruthEvent");
	if(sc.isFailure() || !mcTru){
		LogStream << MSG::DEBUG << "Container "<< "TruthEvent" <<" NOT FOUND !!!!!!!" << endreq;
		return StatusCode::FAILURE;
	}
*/	
	

	sc = RecordTDCollection();
	if (sc.isFailure())
	{
		LogStream << MSG::ERROR << "RecordCollection() failed" << endreq;
		return StatusCode::SUCCESS;
	}

	sc = AFPCollectionReading(ListTDHits);
	if (sc.isFailure())
	{
		LogStream << MSG::WARNING << "AFP_Collection_Reading Failed" << endreq;
//		return StatusCode::SUCCESS;
	}
	else
	{
		string strAlgoTD;
		for(unsigned int i=0; i<m_vecListAlgoTD.size(); i++)
		{
			strAlgoTD = m_vecListAlgoTD[i];

			//execute TD algorithm
			sc = ExecuteRecoMethod(strAlgoTD, ListTDHits);
			
			if (sc.isFailure())
			{
			LogStream << MSG::FATAL << "TD Algorithm " << strAlgoTD << " failure!" << endreq;
			return sc;
			}
		}
	}


	
	
	
	m_iEvent++;
	LogStream << MSG::DEBUG << "end AFP_TDLocReco::execute()" << endreq;
	return StatusCode::SUCCESS;
}

StatusCode AFP_TDLocReco::finalize()
{
	MsgStream LogStream(Athena::getMessageSvc(), "AFP_TDLocReco::finalize()");
	LogStream << MSG::DEBUG << "begin AFP_TDLocReco::finalize()" << endreq;



	LogStream << MSG::DEBUG << "end AFP_TDLocReco::finalize()" << endreq;
	return StatusCode::SUCCESS;
}

void AFP_TDLocReco::ClearGeometry()
{
	MsgStream LogStream(Athena::getMessageSvc(), "AFP_TDLocReco::ClearGeometry()");
	LogStream << MSG::DEBUG << "begin AFP_TDLocReco::ClearGeometry()" << endreq;

	///////
	///////
	
	LogStream << MSG::DEBUG << "end AFP_TDLocReco::ClearGeometry()" << endreq;
}


StatusCode AFP_TDLocReco::AFPCollectionReading(list<TDHIT> &ListTDHits)
{
	StatusCode sc;

	MsgStream LogStream(Athena::getMessageSvc(), "AFP_TDLocReco::AFPCollectionReading()");
	LogStream << MSG::DEBUG << "begin AFP_TDLocReco::AFPCollectionReading()" << endreq;

	TDHIT TDHit;

	ListTDHits.clear();


	const AFP_TDDigiCollection* mcTDGen = 0;


	sc = m_storeGate->retrieve(mcTDGen,m_strTDCollectionName);
	if(sc.isFailure() || !mcTDGen)
	{
		return StatusCode::FAILURE;
	}

	AFP_TDDigiCollection::const_iterator mcTDGenBeg = mcTDGen->begin();
	AFP_TDDigiCollection::const_iterator mcTDGenEnd = mcTDGen->end();

	for(;mcTDGenBeg!=mcTDGenEnd;++mcTDGenBeg)
	{
		TDHit.iEvent 			= m_eventNum;
		TDHit.fADC  			= mcTDGenBeg->m_fADC;
		TDHit.fTDC 			= mcTDGenBeg->m_fTDC;
		TDHit.nDetectorID  		= mcTDGenBeg->m_nDetectorID;
		TDHit.nStationID 		= mcTDGenBeg->m_nStationID;
		TDHit.nSensitiveElementID	= mcTDGenBeg->m_nSensitiveElementID;

		ListTDHits.push_back(TDHit);
	}

	LogStream << MSG::DEBUG << "end AFP_TDLocReco::AFPCollectionReading()" << endreq;

	return StatusCode::SUCCESS;
}


bool AFP_TDLocReco::ReadGeometryDetCS()
{
	MsgStream LogStream(Athena::getMessageSvc(), "AFP_TDLocReco::ReadGeometryDetCS()");
	LogStream << MSG::DEBUG << "begin AFP_TDLocReco::ReadGeometryDetCS()" << endreq;

	//////
	//////
	//SaveGeometry();

	LogStream << MSG::DEBUG << "end AFP_TDLocReco::ReadGeometryDetCS()" << endreq;

	return 1;
}

bool AFP_TDLocReco::StoreReconstructionGeometry(const char* szDataDestination)
{
	MsgStream LogStream(Athena::getMessageSvc(), "AFP_TDLocReco::StoreReconstructionGeometry()");
	LogStream << MSG::DEBUG << "begin AFP_TDLocReco::StoreReconstructionGeometry()" << endreq;

	//////
	//////

	LogStream << MSG::DEBUG << "end AFP_LocReco::StoreReconstructionGeometry()" << endreq;

	return true;
}

void AFP_TDLocReco::SaveGeometry()
{
	MsgStream LogStream(Athena::getMessageSvc(), "AFP_TDLocReco::SaveGeometry()");
	LogStream << MSG::DEBUG << "begin AFP_TDLocReco::SaveGeometry()" << endreq;


	/////
	/////

	LogStream << MSG::DEBUG << "end AFP_TDLocReco::SaveGeometry()" << endreq;
}

StatusCode AFP_TDLocReco::ExecuteRecoMethod(const string strAlgo, const list<TDHIT> &ListTDHits)
{
	MsgStream LogStream(Athena::getMessageSvc(), "AFP_TDLocReco::ExecuteRecoMethod()");
	LogStream << MSG::DEBUG << "begin AFP_TDLocReco::ExecuteRecoMethod()" << endreq;

	StatusCode sc = StatusCode::SUCCESS;
	TDRESULT TDResults;
	list<TDRESULT> listTDResults;
	listTDResults.clear();


	map<string, int> mapRecoMethods;
	mapRecoMethods.clear();
	mapRecoMethods.insert(pair<string, int>("TDZVertex", 1));


	switch(mapRecoMethods[strAlgo])
	{
	case 1:
		{
			AFP_TDZVertex* pTDZVertex = new AFP_TDZVertex();

			sc = pTDZVertex->Initialize(m_AmpThresh, m_iDataType);
			sc = pTDZVertex->Execute(ListTDHits);
			sc = pTDZVertex->Finalize(&listTDResults);

			list<TDRESULT>::const_iterator iter;
			for(iter=listTDResults.begin(); iter!=listTDResults.end(); iter++)
			{
				TDResults.nStationID  = (*iter).nStationID;
				TDResults.nDetectorID = (*iter).nDetectorID;
				TDResults.nTrainID    = (*iter).nTrainID;
				TDResults.fTrainTime  = (*iter).fTrainTime;
				TDResults.nTrainSize  = (*iter).nTrainSize;
                                TDResults.nTrainNSat  = (*iter).nTrainNSat;
				
				if ((TDResults.nStationID != -1))
				{

					m_pTDLocRecoEvCollection->push_back(new AFP_TDLocRecoEvent(1, TDResults.nStationID, TDResults.nDetectorID, TDResults.nTrainID, TDResults.fTrainTime, TDResults.nTrainSize, TDResults.nTrainNSat));
				}
			}

			delete pTDZVertex;
			break;
		}




	default:
		{
			LogStream << MSG::ERROR << "Unable to recognize selected algorithm" << endreq;
			return StatusCode::FAILURE;
		}
	}

	LogStream << MSG::DEBUG << "end AFP_TDLocReco::ExecuteRecoMethod()" << endreq;

	return sc;
}

StatusCode AFP_TDLocReco::RecordTDCollection()
{
	MsgStream LogStream(Athena::getMessageSvc(), "AFP_TDLocReco::RecordTDCollection()");
	LogStream << MSG::DEBUG << "begin AFP_TDLocReco::RecordTDCollection()" << endreq;

	StatusCode sc = StatusCode::SUCCESS;

	m_pTDLocRecoEvCollection = new AFP_TDLocRecoEvCollection();
	sc = m_storeGate->record(m_pTDLocRecoEvCollection, m_strKeyTDLocRecoEvCollection);

	if (sc.isFailure())
	{
		LogStream << MSG::ERROR << m_strAlgoTD << "TD - Could not record the empty LocRecoEv collection in StoreGate" << endreq;
		return sc;
	}
	else
	{
		LogStream << MSG::DEBUG << "TD - LocRecoEv collection was recorded in StoreGate" << endreq;
	}

	LogStream << MSG::DEBUG << "end AFP_TDLocReco::RecordTDCollection()" << endreq;

	return sc;
}


void TDRESULT::clear()
{
	nStationID  = 0;
	nDetectorID = 0;
	nTrainID    = 0;
	fTrainTime  = 0.;
	nTrainSize  = 0;
        nTrainNSat  = 0;
}
