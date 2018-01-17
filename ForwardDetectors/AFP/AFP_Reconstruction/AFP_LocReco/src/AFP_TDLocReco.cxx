/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_LocReco/AFP_TDLocReco.h"
//#include "AFP_Geometry/AFP_GeometryReader.h"

#include "AthenaKernel/errorcheck.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h" 

AFP_TDLocReco::AFP_TDLocReco(const std::string& name, ISvcLocator* pSvcLocator) :
AthAlgorithm(name, pSvcLocator)
{
	ATH_MSG_DEBUG("begin AFP_TDLocReco::AFP_TDLocReco");

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

	ATH_MSG_DEBUG("end AFP_TDLocReco::AFP_TDLocReco");
}

AFP_TDLocReco::~AFP_TDLocReco()
{
	ATH_MSG_DEBUG("begin AFP_TDLocReco::~AFP_TDLocReco");

// 	if(m_pGeometryReader!=NULL)
// 	{
// 		delete m_pGeometryReader;
// 		m_pGeometryReader = NULL;
// 	}

}

StatusCode AFP_TDLocReco::initialize()
{
	ATH_MSG_DEBUG("begin AFP_TDLocReco::initialize()");

	StatusCode sc;
	//ClearGeometry();

	sc = service("StoreGateSvc",m_storeGate);
	if(sc.isFailure())
	{
		ATH_MSG_ERROR("reconstruction: unable to retrieve pointer to StoreGateSvc");
		return sc;
	} 
	
	//read geometry
	/*if(ReadGeometryDetCS())
	{
		LogStream<<MSG::DEBUG<<"Geometry loaded successfully"<<endmsg;
	}
	else
	{
		LogStream<<MSG::FATAL<<"Could not load geometry"<<endmsg;
		return StatusCode::FAILURE;
	}*/

	//write AFP_GeometryReader to StoreGate
	/*if (StatusCode::SUCCESS!=service("DetectorStore",m_pDetStore))
	{
		LogStream << MSG::ERROR << "Detector store not found" << endmsg;
		return StatusCode::FAILURE;
	}
 	sc = m_pDetStore->record(m_pGeometryReader, m_strKeyGeometryForReco);
	if(sc.isFailure())
	{
		LogStream << MSG::ERROR << "m_pGeometryReader: unable to record to StoreGate" << endmsg;
		return sc;
	}*/

	m_iEvent = 0;

	ATH_MSG_DEBUG("end AFP_TDLocReco::initialize()");
	return StatusCode::SUCCESS;
}

StatusCode AFP_TDLocReco::execute()
{
	ATH_MSG_DEBUG("begin AFP_TDLocReco::execute()");

	StatusCode sc;

        std::list<TDHIT> ListTDHits;
	ListTDHits.clear();

	m_eventNum = 0;
	m_iRunNum  = 0;
	const EventInfo* eventInfo = nullptr;
	sc = m_storeGate->retrieve( eventInfo );
	if (sc.isFailure())
	{
		ATH_MSG_ERROR("AFP_TDLocReco, Cannot get event info.");
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
		LogStream << MSG::DEBUG << "Container "<< "TruthEvent" <<" NOT FOUND !!!!!!!" << endmsg;
		return StatusCode::FAILURE;
	}
*/	
	

	sc = RecordTDCollection();
	if (sc.isFailure())
	{
		ATH_MSG_ERROR("RecordCollection() failed");
		return StatusCode::SUCCESS;
	}

	sc = AFPCollectionReading(ListTDHits);
	if (sc.isFailure())
	{
		ATH_MSG_WARNING("AFP_Collection_Reading Failed");
//		return StatusCode::SUCCESS;
	}
	else
	{
                std::string strAlgoTD;
		for(unsigned int i=0; i<m_vecListAlgoTD.size(); i++)
		{
			strAlgoTD = m_vecListAlgoTD[i];

			//execute TD algorithm
			sc = ExecuteRecoMethod(strAlgoTD, ListTDHits);
			
			if (sc.isFailure())
			{
			ATH_MSG_FATAL("TD Algorithm " << strAlgoTD << " failure!");
			return sc;
			}
		}
	}


	
	
	
	m_iEvent++;
	ATH_MSG_DEBUG("end AFP_TDLocReco::execute()");
	return StatusCode::SUCCESS;
}

StatusCode AFP_TDLocReco::finalize()
{
	ATH_MSG_DEBUG("begin AFP_TDLocReco::finalize()");



	ATH_MSG_DEBUG("end AFP_TDLocReco::finalize()");
	return StatusCode::SUCCESS;
}

void AFP_TDLocReco::ClearGeometry()
{
	ATH_MSG_DEBUG("begin AFP_TDLocReco::ClearGeometry()");

	///////
	///////
	
	ATH_MSG_DEBUG("end AFP_TDLocReco::ClearGeometry()");
}


StatusCode AFP_TDLocReco::AFPCollectionReading(std::list<TDHIT> &ListTDHits)
{
	StatusCode sc;

	ATH_MSG_DEBUG("begin AFP_TDLocReco::AFPCollectionReading()");

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

	ATH_MSG_DEBUG("end AFP_TDLocReco::AFPCollectionReading()");

	return StatusCode::SUCCESS;
}


bool AFP_TDLocReco::ReadGeometryDetCS()
{
	ATH_MSG_DEBUG("begin AFP_TDLocReco::ReadGeometryDetCS()");

	//////
	//////
	//SaveGeometry();

	ATH_MSG_DEBUG("end AFP_TDLocReco::ReadGeometryDetCS()");

	return 1;
}

bool AFP_TDLocReco::StoreReconstructionGeometry(/*const char* szDataDestination*/)
{
	ATH_MSG_DEBUG("begin AFP_TDLocReco::StoreReconstructionGeometry()");

	//////
	//////

	ATH_MSG_DEBUG("end AFP_LocReco::StoreReconstructionGeometry()");

	return true;
}

void AFP_TDLocReco::SaveGeometry()
{
	ATH_MSG_DEBUG("begin AFP_TDLocReco::SaveGeometry()");


	/////
	/////

	ATH_MSG_DEBUG("end AFP_TDLocReco::SaveGeometry()");
}

StatusCode AFP_TDLocReco::ExecuteRecoMethod(const std::string strAlgo, const std::list<TDHIT> &ListTDHits)
{
	ATH_MSG_DEBUG("begin AFP_TDLocReco::ExecuteRecoMethod()");

	StatusCode sc = StatusCode::SUCCESS;
	TDRESULT TDResults;
        std::list<TDRESULT> listTDResults;
	listTDResults.clear();


        std::map<std::string, int> mapRecoMethods;
	mapRecoMethods.clear();
	mapRecoMethods.insert(std::pair<std::string, int>("TDZVertex", 1));


	switch(mapRecoMethods[strAlgo])
	{
	case 1:
		{
			AFP_TDZVertex* pTDZVertex = new AFP_TDZVertex();

			sc = pTDZVertex->Initialize(m_AmpThresh, m_iDataType);
			sc = pTDZVertex->Execute(ListTDHits);
			sc = pTDZVertex->Finalize(&listTDResults);

                        std::list<TDRESULT>::const_iterator iter;
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
			ATH_MSG_ERROR("Unable to recognize selected algorithm");
			return StatusCode::FAILURE;
		}
	}

	ATH_MSG_DEBUG("end AFP_TDLocReco::ExecuteRecoMethod()");

	return sc;
}

StatusCode AFP_TDLocReco::RecordTDCollection()
{
	ATH_MSG_DEBUG("begin AFP_TDLocReco::RecordTDCollection()");

	StatusCode sc = StatusCode::SUCCESS;

	m_pTDLocRecoEvCollection = new AFP_TDLocRecoEvCollection();
	sc = m_storeGate->record(m_pTDLocRecoEvCollection, m_strKeyTDLocRecoEvCollection);

	if (sc.isFailure())
	{
		ATH_MSG_ERROR("TD - Could not record the empty LocRecoEv collection in StoreGate");
		return sc;
	}
	else
	{
		ATH_MSG_DEBUG("TD - LocRecoEv collection was recorded in StoreGate");
	}

	ATH_MSG_DEBUG("end AFP_TDLocReco::RecordTDCollection()");

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
