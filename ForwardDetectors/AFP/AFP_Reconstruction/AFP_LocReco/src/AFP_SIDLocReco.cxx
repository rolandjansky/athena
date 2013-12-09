/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_LocReco/AFP_SIDLocReco.h"


#include "AthenaKernel/errorcheck.h"

AFP_SIDLocReco::AFP_SIDLocReco(const string& name, ISvcLocator* pSvcLocator) :
Algorithm(name, pSvcLocator)
{
	MsgStream LogStream(Athena::getMessageSvc(), "AFP_SIDLocReco::AFP_SIDLocReco");
	LogStream << MSG::DEBUG << "begin AFP_SIDLocReco::AFP_SIDLocReco" << endreq;

	m_Config.clear();
	m_pGeometry = new AFP_Geometry(&m_Config);


	// data type using in the local reconstruction
	// for the simulation data the value is 0, for the real data the value is 1. Unset value is -1
	declareProperty("DataType", m_iDataType=0, "data type using in the local reconstruction");

	//reconstruction methods properties
	declareProperty("AmpThresh", m_AmpThresh=10.);


	//reconstruction method selection for TD
	declareProperty("AlgoSID", m_strAlgoSID="SIDBasicKalman");


	m_vecListAlgoSID.clear();
	declareProperty("ListAlgoSID", m_vecListAlgoSID);



	m_strKeyGeometryForReco			= "AFP_GeometryForReco";
	m_strSIDCollectionName			= "AFP_SiDigiCollection";
	m_strKeySIDLocRecoEvCollection		= "AFP_SIDLocRecoEvCollection";


	m_pSIDLocRecoEvCollection = NULL;
	m_pSIDLocRecoEvent = NULL;
	m_storeGate = NULL;

	m_eventNum = 0;
	m_iEvent   = 0;
	m_iRunNum  = 0;

	LogStream << MSG::DEBUG << "end AFP_SIDLocReco::AFP_SIDLocReco" << endreq;
}

AFP_SIDLocReco::~AFP_SIDLocReco()
{
	MsgStream LogStream(Athena::getMessageSvc(), "AFP_SIDLocReco::~AFP_SIDLocReco");
	LogStream << MSG::DEBUG << "begin AFP_SIDLocReco::~AFP_SIDLocReco" << endreq;

// 	if(m_pGeometryReader!=NULL)
// 	{
// 		delete m_pGeometryReader;
// 		m_pGeometryReader = NULL;
// 	}

	LogStream << MSG::DEBUG << "begin AFP_SIDLocReco::~AFP_SIDLocReco" << endreq;
}

StatusCode AFP_SIDLocReco::initialize()
{
	MsgStream LogStream(Athena::getMessageSvc(), "AFP_SIDLocReco::initialize()");
	LogStream << MSG::DEBUG << "begin AFP_SIDLocReco::initialize()" << endreq;

	StatusCode sc;
	ClearGeometry();

	sc = service("StoreGateSvc",m_storeGate);
	if(sc.isFailure())
	{
		LogStream << MSG::ERROR << "reconstruction: unable to retrieve pointer to StoreGateSvc" << endreq;
		return sc;
	} 
	
	//read geometry
	if(ReadGeometryDetCS())
	{
		LogStream<<MSG::DEBUG<<"Geometry loaded successfully"<<endreq;
	}
	else
	{
		LogStream<<MSG::FATAL<<"Could not load geometry"<<endreq;
		return StatusCode::FAILURE;
	}

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

	LogStream << MSG::DEBUG << "end AFP_SIDLocReco::initialize()" << endreq;
	return StatusCode::SUCCESS;
}

StatusCode AFP_SIDLocReco::execute()
{
	MsgStream LogStream(Athena::getMessageSvc(), "AFP_SIDLocReco::execute()");
	LogStream << MSG::DEBUG << "begin AFP_SIDLocReco::execute()" << endreq;

	StatusCode sc;

	list<SIDHIT> ListSIDHits;
	ListSIDHits.clear();

	m_eventNum = 0;
	m_iRunNum  = 0;
	const EventInfo* eventInfo;
	sc = m_storeGate->retrieve( eventInfo );
	if (sc.isFailure())
	{
		LogStream << MSG::ERROR << "AFP_SIDLocReco, Cannot get event info." << endreq;
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
//		return StatusCode::FAILURE;
	}
*/	
	

	sc = RecordSIDCollection();
	if (sc.isFailure())
	{
		LogStream << MSG::ERROR << "RecordCollection() failed" << endreq;
		return StatusCode::SUCCESS;
	}

	sc = AFPCollectionReading(ListSIDHits);
	if (sc.isFailure())
	{
		LogStream << MSG::WARNING << "AFP_Collection_Reading Failed" << endreq;
//		return StatusCode::SUCCESS;
	}
	else
	{
		string strAlgoSID;
		for(unsigned int i=0; i<m_vecListAlgoSID.size(); i++)
		{
			strAlgoSID = m_vecListAlgoSID[i];

			//execute SID algorithm
			sc = ExecuteRecoMethod(strAlgoSID, ListSIDHits);
			
			if (sc.isFailure())
			{
			LogStream << MSG::FATAL << "SID Algorithm " << strAlgoSID << " failure!" << endreq;
			return sc;
			}
		}
	}


	
	
	
	m_iEvent++;
	LogStream << MSG::DEBUG << "end AFP_SIDLocReco::execute()" << endreq;
	return StatusCode::SUCCESS;
}

StatusCode AFP_SIDLocReco::finalize()
{
	MsgStream LogStream(Athena::getMessageSvc(), "AFP_SIDLocReco::finalize()");
	LogStream << MSG::DEBUG << "begin AFP_SIDLocReco::finalize()" << endreq;


	LogStream << MSG::DEBUG << "end AFP_SIDLocReco::finalize()" << endreq;
	return StatusCode::SUCCESS;
}

void AFP_SIDLocReco::ClearGeometry()
{
	MsgStream LogStream(Athena::getMessageSvc(), "AFP_SIDLocReco::ClearGeometry()");
	LogStream << MSG::DEBUG << "begin AFP_SIDLocReco::ClearGeometry()" << endreq;

	///////
	memset(&m_fsSID, 0, sizeof(m_fsSID));
	memset(&m_fxSID, 0, sizeof(m_fxSID));
	memset(&m_fySID, 0, sizeof(m_fySID));
	memset(&m_fzSID, 0, sizeof(m_fzSID));
	///////
	
	LogStream << MSG::DEBUG << "end AFP_SIDLocReco::ClearGeometry()" << endreq;
}


StatusCode AFP_SIDLocReco::AFPCollectionReading(list<SIDHIT> &ListSIDHits)
{
	StatusCode sc;

	MsgStream LogStream(Athena::getMessageSvc(), "AFP_SIDLocReco::AFPCollectionReading()");
	LogStream << MSG::DEBUG << "begin AFP_SIDLocReco::AFPCollectionReading()" << endreq;

	SIDHIT SIDHit;

	ListSIDHits.clear();


	const AFP_SiDigiCollection* mcSIDGen = 0;


	sc = m_storeGate->retrieve(mcSIDGen,m_strSIDCollectionName);
	if(sc.isFailure() || !mcSIDGen)
	{
		return StatusCode::FAILURE;
	}

	AFP_SiDigiCollection::const_iterator mcSIDGenBeg = mcSIDGen->begin();
	AFP_SiDigiCollection::const_iterator mcSIDGenEnd = mcSIDGen->end();

	for(;mcSIDGenBeg!=mcSIDGenEnd;++mcSIDGenBeg)
	{
		SIDHit.iEvent 			= m_eventNum;
		SIDHit.fADC  			= mcSIDGenBeg->m_fADC;
		SIDHit.fTDC 			= mcSIDGenBeg->m_fTDC;
		SIDHit.nDetectorID  		= mcSIDGenBeg->m_nDetectorID;
		SIDHit.nStationID 		= mcSIDGenBeg->m_nStationID;
		SIDHit.nPixelRow		= mcSIDGenBeg->m_nPixelRow;
		SIDHit.nPixelCol		= mcSIDGenBeg->m_nPixelCol;

		ListSIDHits.push_back(SIDHit);
	}

	LogStream << MSG::DEBUG << "end AFP_SIDLocReco::AFPCollectionReading()" << endreq;

	return StatusCode::SUCCESS;
}


bool AFP_SIDLocReco::ReadGeometryDetCS()
{
	MsgStream LogStream(Athena::getMessageSvc(), "AFP_SIDLocReco::ReadGeometryDetCS()");
	LogStream << MSG::DEBUG << "begin AFP_SIDLocReco::ReadGeometryDetCS()" << endreq;

	//////	
	StatusCode sc;
	
	for(Int_t nStationID = 0; nStationID < SIDSTATIONID; nStationID++)
	{				
		for (Int_t nPlateID = 0; nPlateID < SIDCNT; nPlateID++)
		{
			
			HepGeom::Point3D<double> LocPoint = HepGeom::Point3D<double>(-SID_SENSORXDIM+SID_DEATH_EDGE, -SID_SENSORYDIM+SID_LOWER_EDGE, 0.*CLHEP::mm); //changed! (death edge info from AFP_Geometry)
			HepGeom::Point3D<double> GloPoint = HepGeom::Point3D<double>();
			sc = m_pGeometry->GetPointInSIDSensorGlobalCS(nStationID, nPlateID, LocPoint, GloPoint);
			
			if (sc.isFailure())
			{
				LogStream << MSG::WARNING << "AFP_Geometry::GetPointInSIDSensorGlobalCS() Failed" << endreq;

				m_fsSID[nStationID][nPlateID] = NOMINALSIDSLOPE;
				m_fxSID[nStationID][nPlateID] = LocPoint.x();
				m_fySID[nStationID][nPlateID] = LocPoint.y();
				m_fzSID[nStationID][nPlateID] = NOMINALSIDSPACING*nPlateID;
			}
			
			else
			{
				m_fsSID[nStationID][nPlateID] = NOMINALSIDSLOPE;
				m_fxSID[nStationID][nPlateID] = GloPoint.x(); 
				m_fySID[nStationID][nPlateID] = GloPoint.y(); 
				m_fzSID[nStationID][nPlateID] = GloPoint.z();	
					
				LogStream << MSG::DEBUG << "Global edge position of SID sensor: " <<GloPoint.x()<< "\t" <<GloPoint.y()<< "\t" <<GloPoint.z()<< "\t" << endreq;	
			}
			
		}
	}	
	//////
	
	//SaveGeometry();

	LogStream << MSG::DEBUG << "end AFP_SIDLocReco::ReadGeometryDetCS()" << endreq;

	return 1;
}

bool AFP_SIDLocReco::StoreReconstructionGeometry(const char* szDataDestination)
{
	MsgStream LogStream(Athena::getMessageSvc(), "AFP_SIDLocReco::StoreReconstructionGeometry()");
	LogStream << MSG::DEBUG << "begin AFP_SIDLocReco::StoreReconstructionGeometry()" << endreq;

	//////
	// (...)
	//////

	LogStream << MSG::DEBUG << "end AFP_SIDLocReco::StoreReconstructionGeometry()" << endreq;

	return true;
}

void AFP_SIDLocReco::SaveGeometry()
{
	MsgStream LogStream(Athena::getMessageSvc(), "AFP_SIDLocReco::SaveGeometry()");
	LogStream << MSG::DEBUG << "begin AFP_SIDLocReco::SaveGeometry()" << endreq;

	/////
	// (...)
	/////

	LogStream << MSG::DEBUG << "end AFP_SIDLocReco::SaveGeometry()" << endreq;
}

StatusCode AFP_SIDLocReco::ExecuteRecoMethod(const string strAlgo, const list<SIDHIT> &ListSIDHits)
{
	MsgStream LogStream(Athena::getMessageSvc(), "AFP_SIDLocReco::ExecuteRecoMethod()");
	LogStream << MSG::DEBUG << "begin AFP_SIDLocReco::ExecuteRecoMethod()" << endreq;

	StatusCode sc = StatusCode::SUCCESS;
	SIDRESULT SIDResults;
	list<SIDRESULT> listSIDResults;
	listSIDResults.clear();


	map<string, int> mapRecoMethods;
	mapRecoMethods.clear();
	mapRecoMethods.insert(pair<string, int>("SIDBasicKalman", 1));


	switch(mapRecoMethods[strAlgo])
	{
	case 1:
		{
			AFP_SIDBasicKalman* pSIDBasicKalman = new AFP_SIDBasicKalman();

			sc = pSIDBasicKalman->Initialize(m_AmpThresh, m_iDataType, ListSIDHits, m_fsSID, m_fxSID, m_fySID, m_fzSID);
			sc = pSIDBasicKalman->Execute();
			sc = pSIDBasicKalman->Finalize(&listSIDResults);

			list<SIDRESULT>::const_iterator iter;
			for(iter=listSIDResults.begin(); iter!=listSIDResults.end(); iter++)
			{
				SIDResults.nStationID  	= (*iter).nStationID;
				SIDResults.x_pos	= (*iter).x_pos;
				SIDResults.y_pos	= (*iter).y_pos;
				SIDResults.z_pos	= (*iter).z_pos;
				SIDResults.x_slope	= (*iter).x_slope;
				SIDResults.y_slope	= (*iter).y_slope;
				SIDResults.z_slope	= (*iter).z_slope;
	
				SIDResults.nHits	= (*iter).nHits;
				SIDResults.nHoles	= (*iter).nHoles;
				SIDResults.fChi2	= (*iter).fChi2;	
				
				if ((SIDResults.nStationID != -1))
				{

					m_pSIDLocRecoEvCollection->push_back(new AFP_SIDLocRecoEvent(1, SIDResults.nStationID, SIDResults.x_pos, SIDResults.y_pos, SIDResults.z_pos, SIDResults.x_slope, SIDResults.y_slope, SIDResults.z_slope, SIDResults.nHits, SIDResults.nHoles, SIDResults.fChi2));
					
				}
			}

			delete pSIDBasicKalman;
			break;
		}




	default:
		{
			LogStream << MSG::ERROR << "Unable to recognize selected algorithm" << endreq;
			return StatusCode::FAILURE;
		}
	}

	LogStream << MSG::DEBUG << "end AFP_SIDLocReco::ExecuteRecoMethod()" << endreq;

	return sc;
}

StatusCode AFP_SIDLocReco::RecordSIDCollection()
{
	MsgStream LogStream(Athena::getMessageSvc(), "AFP_SIDLocReco::RecordSIDCollection()");
	LogStream << MSG::DEBUG << "begin AFP_SIDLocReco::RecordSIDCollection()" << endreq;

	StatusCode sc = StatusCode::SUCCESS;

	m_pSIDLocRecoEvCollection = new AFP_SIDLocRecoEvCollection();
	sc = m_storeGate->record(m_pSIDLocRecoEvCollection, m_strKeySIDLocRecoEvCollection);

	if (sc.isFailure())
	{
		LogStream << MSG::ERROR << m_strAlgoSID << "SID - Could not record the empty LocRecoEv collection in StoreGate" << endreq;
		return sc;
	}
	else
	{
		LogStream << MSG::DEBUG << "SID - LocRecoEv collection was recorded in StoreGate" << endreq;
	}

	LogStream << MSG::DEBUG << "end AFP_SIDLocReco::RecordSIDCollection()" << endreq;

	return sc;
}


void SIDRESULT::clear()
{
	nStationID=-1;	
	x_pos=0.;
	y_pos=0.;
	z_pos=0.;
	x_slope=0.;
	y_slope=0.;
	z_slope=0.;
	
	nHits=0;
	nHoles=0;
	fChi2=0.;
}
