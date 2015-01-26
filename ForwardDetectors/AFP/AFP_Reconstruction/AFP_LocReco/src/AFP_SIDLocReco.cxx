/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_LocReco/AFP_SIDLocReco.h"

#include "AthenaKernel/errorcheck.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h" 


AFP_SIDLocReco::AFP_SIDLocReco(const string& name, ISvcLocator* pSvcLocator) :
AthAlgorithm(name, pSvcLocator)
{
	ATH_MSG_DEBUG("begin AFP_SIDLocReco::AFP_SIDLocReco");

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

	ATH_MSG_DEBUG("end AFP_SIDLocReco::AFP_SIDLocReco");
}

AFP_SIDLocReco::~AFP_SIDLocReco()
{
	ATH_MSG_DEBUG("begin AFP_SIDLocReco::~AFP_SIDLocReco");

// 	if(m_pGeometryReader!=NULL)
// 	{
// 		delete m_pGeometryReader;
// 		m_pGeometryReader = NULL;
// 	}

	ATH_MSG_DEBUG("begin AFP_SIDLocReco::~AFP_SIDLocReco");
}

StatusCode AFP_SIDLocReco::initialize()
{
	ATH_MSG_DEBUG("begin AFP_SIDLocReco::initialize()");

	StatusCode sc;
	ClearGeometry();

	sc = service("StoreGateSvc",m_storeGate);
	if(sc.isFailure())
	{
		ATH_MSG_ERROR("reconstruction: unable to retrieve pointer to StoreGateSvc");
		return sc;
	} 
	
	//read geometry
	if(ReadGeometryDetCS())
	{
		ATH_MSG_DEBUG("Geometry loaded successfully");
	}
	else
	{
		ATH_MSG_FATAL("Could not load geometry");
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

	ATH_MSG_DEBUG("end AFP_SIDLocReco::initialize()");
	return StatusCode::SUCCESS;
}

StatusCode AFP_SIDLocReco::execute()
{
	ATH_MSG_DEBUG("begin AFP_SIDLocReco::execute()");

	StatusCode sc;

	list<SIDHIT> ListSIDHits;
	ListSIDHits.clear();

	m_eventNum = 0;
	m_iRunNum  = 0;
	const EventInfo* eventInfo;
	sc = m_storeGate->retrieve( eventInfo );
	if (sc.isFailure())
	{
		ATH_MSG_ERROR("AFP_SIDLocReco, Cannot get event info.");
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
		ATH_MSG_ERROR("RecordCollection() failed");
		return StatusCode::SUCCESS;
	}

	sc = AFPCollectionReading(ListSIDHits);
	if (sc.isFailure())
	{
		ATH_MSG_WARNING("AFP_Collection_Reading Failed");
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
			ATH_MSG_FATAL("SID Algorithm " << strAlgoSID << " failure!");
			return sc;
			}
		}
	}


	
	
	
	m_iEvent++;
	ATH_MSG_DEBUG("end AFP_SIDLocReco::execute()");
	return StatusCode::SUCCESS;
}

StatusCode AFP_SIDLocReco::finalize()
{
	ATH_MSG_DEBUG("begin AFP_SIDLocReco::finalize()");


	ATH_MSG_DEBUG("end AFP_SIDLocReco::finalize()");
	return StatusCode::SUCCESS;
}

void AFP_SIDLocReco::ClearGeometry()
{
	ATH_MSG_DEBUG("begin AFP_SIDLocReco::ClearGeometry()");

	///////
	memset(&m_fsSID, 0, sizeof(m_fsSID));
	memset(&m_fxSID, 0, sizeof(m_fxSID));
	memset(&m_fySID, 0, sizeof(m_fySID));
	memset(&m_fzSID, 0, sizeof(m_fzSID));
	///////
	
	ATH_MSG_DEBUG("end AFP_SIDLocReco::ClearGeometry()");
}


StatusCode AFP_SIDLocReco::AFPCollectionReading(list<SIDHIT> &ListSIDHits)
{
	StatusCode sc;

	ATH_MSG_DEBUG("begin AFP_SIDLocReco::AFPCollectionReading()");

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

	ATH_MSG_DEBUG("end AFP_SIDLocReco::AFPCollectionReading()");

	return StatusCode::SUCCESS;
}


bool AFP_SIDLocReco::ReadGeometryDetCS()
{
	ATH_MSG_DEBUG("begin AFP_SIDLocReco::ReadGeometryDetCS()");

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
				ATH_MSG_WARNING("AFP_Geometry::GetPointInSIDSensorGlobalCS() Failed");

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
					
				ATH_MSG_DEBUG("Global edge position of SID sensor: " <<GloPoint.x()<< "\t" <<GloPoint.y()<< "\t" <<GloPoint.z()<< "\t");	
			}
			
		}
	}	
	//////
	
	//SaveGeometry();

	ATH_MSG_DEBUG("end AFP_SIDLocReco::ReadGeometryDetCS()");

	return 1;
}

bool AFP_SIDLocReco::StoreReconstructionGeometry(/*const char* szDataDestination*/)
{
	ATH_MSG_DEBUG("begin AFP_SIDLocReco::StoreReconstructionGeometry()");

	//////
	// (...)
	//////

	ATH_MSG_DEBUG("end AFP_SIDLocReco::StoreReconstructionGeometry()");

	return true;
}

void AFP_SIDLocReco::SaveGeometry()
{
	ATH_MSG_DEBUG("begin AFP_SIDLocReco::SaveGeometry()");

	/////
	// (...)
	/////

	ATH_MSG_DEBUG("end AFP_SIDLocReco::SaveGeometry()");
}

StatusCode AFP_SIDLocReco::ExecuteRecoMethod(const string strAlgo, const list<SIDHIT> &ListSIDHits)
{
	ATH_MSG_DEBUG("begin AFP_SIDLocReco::ExecuteRecoMethod()");

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
			ATH_MSG_ERROR("Unable to recognize selected algorithm");
			return StatusCode::FAILURE;
		}
	}

	ATH_MSG_DEBUG("end AFP_SIDLocReco::ExecuteRecoMethod()");

	return sc;
}

StatusCode AFP_SIDLocReco::RecordSIDCollection()
{
	ATH_MSG_DEBUG("begin AFP_SIDLocReco::RecordSIDCollection()");

	StatusCode sc = StatusCode::SUCCESS;

	m_pSIDLocRecoEvCollection = new AFP_SIDLocRecoEvCollection();
	sc = m_storeGate->record(m_pSIDLocRecoEvCollection, m_strKeySIDLocRecoEvCollection);

	if (sc.isFailure())
	{
		ATH_MSG_ERROR( m_strAlgoSID << "SID - Could not record the empty LocRecoEv collection in StoreGate");
		return sc;
	}
	else
	{
		ATH_MSG_DEBUG("SID - LocRecoEv collection was recorded in StoreGate");
	}

	ATH_MSG_DEBUG("end AFP_SIDLocReco::RecordSIDCollection()");

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
