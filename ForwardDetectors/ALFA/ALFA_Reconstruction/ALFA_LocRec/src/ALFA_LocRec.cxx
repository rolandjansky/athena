/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_LocRec/ALFA_LocRec.h"
#include "ALFA_Geometry/ALFA_GeometryReader.h"

#include "AthenaKernel/errorcheck.h"
using namespace std;

ALFA_LocRec::ALFA_LocRec(const std::string& name, ISvcLocator* pSvcLocator) :
AthAlgorithm(name, pSvcLocator)
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_LocRec::ALFA_LocRec");
	ATH_MSG_DEBUG("begin ALFA_LocRec::ALFA_LocRec");

	m_pGeometryReader = new ALFA_GeometryReader();

//	m_Config.eOwner=EOT_RECO;
	m_Config.clear();

	//begin of Geometry properties
	declareProperty("MetrologyType",m_Config.eRPMetrologyGeoType=EMT_NOMINAL);//EGST_FILE
	declareProperty("MetrologySource",m_Config.strRPMetrologyConnString=std::string(""));//"rpmetrology.dat"
	declareProperty("ShiftToX97Pos",m_Config.bShiftToX97Pos=false);

	declareProperty("B7L1U_PosType",m_Config.CfgRPosParams[0].eRPPosType=ERPPT_ACTIVE);
	declareProperty("B7L1U_MDGeometryType",m_Config.CfgRPosParams[0].eMDGeoType=EGST_IDEALGEOMETRY);
	declareProperty("B7L1U_ODGeometryType",m_Config.CfgRPosParams[0].eODGeoType=EGST_IDEALGEOMETRY);
	declareProperty("B7L1U_MDSource",m_Config.CfgRPosParams[0].strMDConnString=std::string(""));
	declareProperty("B7L1U_ODSource",m_Config.CfgRPosParams[0].strODConnString=std::string(""));
//	declareProperty("B7L1U_CurrentLVDT",m_Config.CfgRPosParams[0].fCurrentLVDTmm=7.0*CLHEP::mm);

	declareProperty("B7L1L_PosType",m_Config.CfgRPosParams[1].eRPPosType=ERPPT_ACTIVE);
	declareProperty("B7L1L_MDGeometryType",m_Config.CfgRPosParams[1].eMDGeoType=EGST_IDEALGEOMETRY);
	declareProperty("B7L1L_ODGeometryType",m_Config.CfgRPosParams[1].eODGeoType=EGST_IDEALGEOMETRY);
	declareProperty("B7L1L_MDSource",m_Config.CfgRPosParams[1].strMDConnString=std::string(""));
	declareProperty("B7L1L_ODSource",m_Config.CfgRPosParams[1].strODConnString=std::string(""));
//	declareProperty("B7L1L_CurrentLVDT",m_Config.CfgRPosParams[1].fCurrentLVDTmm=7.0*CLHEP::mm);

	declareProperty("A7L1U_PosType",m_Config.CfgRPosParams[2].eRPPosType=ERPPT_ACTIVE);
	declareProperty("A7L1U_MDGeometryType",m_Config.CfgRPosParams[2].eMDGeoType=EGST_IDEALGEOMETRY);
	declareProperty("A7L1U_ODGeometryType",m_Config.CfgRPosParams[2].eODGeoType=EGST_IDEALGEOMETRY);
	declareProperty("A7L1U_MDSource",m_Config.CfgRPosParams[2].strMDConnString=std::string(""));
	declareProperty("A7L1U_ODSource",m_Config.CfgRPosParams[2].strODConnString=std::string(""));
//	declareProperty("A7L1U_CurrentLVDT",m_Config.CfgRPosParams[2].fCurrentLVDTmm=7.0*CLHEP::mm);

	declareProperty("A7L1L_PosType",m_Config.CfgRPosParams[3].eRPPosType=ERPPT_ACTIVE);
	declareProperty("A7L1L_MDGeometryType",m_Config.CfgRPosParams[3].eMDGeoType=EGST_IDEALGEOMETRY);
	declareProperty("A7L1L_ODGeometryType",m_Config.CfgRPosParams[3].eODGeoType=EGST_IDEALGEOMETRY);
	declareProperty("A7L1L_MDSource",m_Config.CfgRPosParams[3].strMDConnString=std::string(""));
	declareProperty("A7L1L_ODSource",m_Config.CfgRPosParams[3].strODConnString=std::string(""));
//	declareProperty("A7L1L_CurrentLVDT",m_Config.CfgRPosParams[3].fCurrentLVDTmm=7.0*CLHEP::mm);

	declareProperty("A7R1U_PosType",m_Config.CfgRPosParams[4].eRPPosType=ERPPT_ACTIVE);
	declareProperty("A7R1U_MDGeometryType",m_Config.CfgRPosParams[4].eMDGeoType=EGST_IDEALGEOMETRY);
	declareProperty("A7R1U_ODGeometryType",m_Config.CfgRPosParams[4].eODGeoType=EGST_IDEALGEOMETRY);
	declareProperty("A7R1U_MDSource",m_Config.CfgRPosParams[4].strMDConnString=std::string(""));
	declareProperty("A7R1U_ODSource",m_Config.CfgRPosParams[4].strODConnString=std::string(""));
//	declareProperty("A7R1U_CurrentLVDT",m_Config.CfgRPosParams[4].fCurrentLVDTmm=7.0*CLHEP::mm);

	declareProperty("A7R1L_PosType",m_Config.CfgRPosParams[5].eRPPosType=ERPPT_ACTIVE);
	declareProperty("A7R1L_MDGeometryType",m_Config.CfgRPosParams[5].eMDGeoType=EGST_IDEALGEOMETRY);
	declareProperty("A7R1L_ODGeometryType",m_Config.CfgRPosParams[5].eODGeoType=EGST_IDEALGEOMETRY);
	declareProperty("A7R1L_MDSource",m_Config.CfgRPosParams[5].strMDConnString=std::string(""));
	declareProperty("A7R1L_ODSource",m_Config.CfgRPosParams[5].strODConnString=std::string(""));
//	declareProperty("A7L1U_CurrentLVDT",m_Config.CfgRPosParams[5].fCurrentLVDTmm=7.0*CLHEP::mm);

	declareProperty("B7R1U_PosType",m_Config.CfgRPosParams[6].eRPPosType=ERPPT_ACTIVE);
	declareProperty("B7R1U_MDGeometryType",m_Config.CfgRPosParams[6].eMDGeoType=EGST_IDEALGEOMETRY);
	declareProperty("B7R1U_ODGeometryType",m_Config.CfgRPosParams[6].eODGeoType=EGST_IDEALGEOMETRY);
	declareProperty("B7R1U_MDSource",m_Config.CfgRPosParams[6].strMDConnString=std::string(""));
	declareProperty("B7R1U_ODSource",m_Config.CfgRPosParams[6].strODConnString=std::string(""));
//	declareProperty("B7R1U_CurrentLVDT",m_Config.CfgRPosParams[6].fCurrentLVDTmm=7.0*CLHEP::mm);

	declareProperty("B7R1L_PosType",m_Config.CfgRPosParams[7].eRPPosType=ERPPT_ACTIVE);
	declareProperty("B7R1L_MDGeometryType",m_Config.CfgRPosParams[7].eMDGeoType=EGST_IDEALGEOMETRY);
	declareProperty("B7R1L_ODGeometryType",m_Config.CfgRPosParams[7].eODGeoType=EGST_IDEALGEOMETRY);
	declareProperty("B7R1L_MDSource",m_Config.CfgRPosParams[7].strMDConnString=std::string(""));
	declareProperty("B7R1L_ODSource",m_Config.CfgRPosParams[7].strODConnString=std::string(""));
//	declareProperty("B7R1L_CurrentLVDT",m_Config.CfgRPosParams[7].fCurrentLVDTmm=7.0*CLHEP::mm);
	//end of Geometry properties

	// data type using in the local reconstruction
	// for the simulation data the value is 0, for the real data the value is 1. Unset value is -1
	declareProperty("DataType", m_iDataType=1, "data type using in the local reconstruction");

	//reconstruction methods properties - MDTracking, MDMultiple, ODTracking
	declareProperty("MultiplicityCutOD", m_iMultiplicityCutOD=30);
	declareProperty("DistanceCutOD", m_fDistanceCutOD=0.5);
	declareProperty("LayerCutOD", m_iLayerCutOD=3);

	declareProperty("OverlapCutMD", m_fOverlapCutMD=0.5);
	declareProperty("MultiplicityCutMD", m_iMultiplicityCutMD=5);
	declareProperty("NumLayerCutMD", m_iNumLayerCutMD=3);
	declareProperty("UVCutMD", m_iUVCutMD=3);
	declareProperty("UVCutMDHalfReco", m_iUVCutMDHalfReco=3);

	//reconstruction methods properties - EdgeMethod
	declareProperty("EdgeMethod_Opt_Sisters", m_bEdgeMethod_Opt_Sisters = false);
	declareProperty("EdgeMethod_Opt_UseGaps", m_bEdgeMethod_Opt_UseGaps = false);

	//reconstruction method selection for OD & MD
	declareProperty("AlgoOD", m_strAlgoOD="ODTracking");
	declareProperty("AlgoMD", m_strAlgoMD="MDTracking");

	m_vecListAlgoMD.clear();
	declareProperty("ListAlgoMD", m_vecListAlgoMD);
	m_vecListAlgoOD.clear();
	declareProperty("ListAlgoOD", m_vecListAlgoOD);

	m_ListExistingRPots.clear();

	m_strKeyGeometryForReco			= "ALFA_GeometryForReco";
	m_strCollectionName				= "ALFA_DigitCollection";
	m_strODCollectionName			= "ALFA_ODDigitCollection";
	m_strKeyLocRecEvCollection		= "ALFA_LocRecEvCollection";
	m_strKeyLocRecODEvCollection	= "ALFA_LocRecODEvCollection";

	m_pLocRecEvCollection = NULL;
	m_pLocRecEvent = NULL;
	m_pLocRecODEvCollection = NULL;
	m_pLocRecODEvent = NULL;
//	m_storeGate = NULL;
//	m_pDetStore = NULL;

	m_eventNum = 0;
	m_iEvent   = 0;
	m_iRunNum  = 0;

	ATH_MSG_DEBUG("end ALFA_LocRec::ALFA_LocRec");
}

ALFA_LocRec::~ALFA_LocRec()
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_LocRec::~ALFA_LocRec");
	ATH_MSG_DEBUG("begin ALFA_LocRec::~ALFA_LocRec");

// 	if(m_pGeometryReader!=NULL)
// 	{
// 		delete m_pGeometryReader;
// 		m_pGeometryReader = NULL;
// 	}

	ATH_MSG_DEBUG("begin ALFA_LocRec::~ALFA_LocRec");
}

StatusCode ALFA_LocRec::initialize()
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_LocRec::initialize()");
	ATH_MSG_DEBUG("begin ALFA_LocRec::initialize()");

	StatusCode sc;
	ClearGeometry();

//	sc = service("StoreGateSvc",m_storeGate);
//	if(sc.isFailure())
//	{
//		ATH_MSG_ERROR("reconstruction: unable to retrieve pointer to StoreGateSvc");
//		return sc;
//	}
	
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

	//write ALFA_GeometryReader to StoreGate
//	if (StatusCode::SUCCESS!=service("DetectorStore",m_pDetStore))
//	{
//		ATH_MSG_ERROR("Detector store not found");
//		return StatusCode::FAILURE;
//	}
	sc = detStore()->record(m_pGeometryReader, m_strKeyGeometryForReco);
	if(sc.isFailure())
	{
		ATH_MSG_ERROR("m_pGeometryReader: unable to record to StoreGate");
		return sc;
	}

// 	if (!detStore()->contains<ALFA_GeometryReader>(m_strKeyGeometryForReco))
// 	{
// 		ATH_MSG_ERROR("m_pGeometryReader is not in StoreGate");
// 	}
// 	else
// 	{
// 		ATH_MSG_DEBUG("m_pGeometryReader is in StoreGate");
// 	}

	m_iEvent = 0;

	ATH_MSG_DEBUG("end ALFA_LocRec::initialize()");
	return StatusCode::SUCCESS;
}

StatusCode ALFA_LocRec::execute()
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_LocRec::execute()");
	ATH_MSG_DEBUG("begin ALFA_LocRec::execute()");

	StatusCode sc;
	std::list<eRPotName>::const_iterator iterRPName;
	eRPotName eRPName;

	std::list<MDHIT> ListMDHits;
	std::list<ODHIT> ListODHits;
	ListMDHits.clear();
	ListODHits.clear();

	m_eventNum = 0;
	m_iRunNum  = 0;
	const EventInfo* eventInfo;
	sc = evtStore()->retrieve( eventInfo );
	if (sc.isFailure())
	{
		ATH_MSG_ERROR("ALFA_LocRec, Cannot get event info.");
//		return sc;
	}
	else
	{
		// current event number
		m_eventNum = eventInfo->event_ID()->event_number();
		m_iRunNum = eventInfo->event_ID()->run_number();
	}

	//************************************************************
//	if (m_eventNum!=73681035) return StatusCode::SUCCESS;
	//************************************************************

	sc = RecordCollection();
	if (sc.isFailure())
	{
		ATH_MSG_ERROR("RecordCollection() failed");
		return StatusCode::SUCCESS;
	}

	sc = RecordODCollection();
	if (sc.isFailure())
	{
		ATH_MSG_ERROR("RecordODCollection() failed");
		return StatusCode::SUCCESS;
	}

	sc = ALFACollectionReading(ListMDHits, ListODHits);
	if (sc.isFailure())
	{
		ATH_MSG_WARNING("ALFA_Collection_Reading Failed");
//		return StatusCode::SUCCESS;
	}
	else
	{
		std::string strAlgoMD;
		for(unsigned int i=0; i<m_vecListAlgoMD.size(); i++)
		{
			strAlgoMD = m_vecListAlgoMD[i];
			for(iterRPName=m_ListExistingRPots.begin();iterRPName!=m_ListExistingRPots.end();iterRPName++)
			{
				eRPName = *iterRPName;

//				cout << "eRPName = " << eRPName << endl;
				//execute MD algorithm
				sc = ExecuteRecoMethod(strAlgoMD, eRPName, ListMDHits, ListODHits);
				if (sc.isFailure())
				{
					ATH_MSG_FATAL("Algorithm " << strAlgoMD << " for the pot " << m_pGeometryReader->GetRPotLabel(eRPName) << " failure!");
					return sc;
				}
			}
		}

		std::string strAlgoOD;
		for(unsigned int i=0; i<m_vecListAlgoOD.size(); i++)
		{
			strAlgoOD = m_vecListAlgoOD[i];
			for(iterRPName=m_ListExistingRPots.begin();iterRPName!=m_ListExistingRPots.end();iterRPName++)
			{
				eRPName = *iterRPName;

	//			cout << "eRPName = " << eRPName << endl;
				//execute OD algorithm
				sc = ExecuteRecoMethod(strAlgoOD, eRPName, ListMDHits, ListODHits);
				if (sc.isFailure())
				{
					ATH_MSG_FATAL("Algorithm " << strAlgoOD << " for the pot " << m_pGeometryReader->GetRPotLabel(eRPName) << " failure!");
					return sc;
				}
			}
		}




//		// local reconstruction process for all existing pots, MD & OD
//		for(iterRPName=m_ListExistingRPots.begin();iterRPName!=m_ListExistingRPots.end();iterRPName++)
//		{
//			eRPName = *iterRPName;
	
//			//execute MD reconstruction method
//			if (!m_strAlgoMD.empty())
//			{
//				sc = ExecuteRecoMethod(m_strAlgoMD, eRPName, ListMDHits, ListODHits);
//				if (sc.isFailure())
//				{
//					ATH_MSG_ERROR("Algorithm " << m_strAlgoMD << " for pot " << m_pGeometryReader->GetRPotLabel(eRPName) << " failure!");
//	//				return sc;
//				}
//			}
	
//			//execute OD reconstruction method
//			if (!m_strAlgoOD.empty())
//			{
//				sc = ExecuteRecoMethod(m_strAlgoOD, eRPName, ListMDHits, ListODHits);
//				if (sc.isFailure())
//				{
//					ATH_MSG_ERROR("Algorithm " << m_strAlgoOD << " for pot " << m_pGeometryReader->GetRPotLabel(eRPName) << " failure!");
//	//				return sc;
//				}
//			}
//		}
	}

	m_iEvent++;
	ATH_MSG_DEBUG("end ALFA_LocRec::execute()");
	return StatusCode::SUCCESS;
}

StatusCode ALFA_LocRec::finalize()
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_LocRec::finalize()");
	ATH_MSG_DEBUG("begin ALFA_LocRec::finalize()");



	ATH_MSG_DEBUG("end ALFA_LocRec::finalize()");
	return StatusCode::SUCCESS;
}

void ALFA_LocRec::ClearGeometry()
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_LocRec::ClearGeometry()");
	ATH_MSG_DEBUG("begin ALFA_LocRec::ClearGeometry()");

	memset(&m_faMD, 0, sizeof(m_faMD));
	memset(&m_fbMD, 0, sizeof(m_fbMD));
	memset(&m_fzMD, 0, sizeof(m_fzMD));
	memset(&m_faOD, 0, sizeof(m_faOD));
	memset(&m_fbOD, 0, sizeof(m_fbOD));
	memset(&m_fzOD, 0, sizeof(m_fzOD));

	ATH_MSG_DEBUG("end ALFA_LocRec::ClearGeometry()");
}

StatusCode ALFA_LocRec::ALFACollectionReading(std::list<MDHIT> &ListMDHits, std::list<ODHIT> &ListODHits)
{
	StatusCode sc;

	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_LocRec::ALFACollectionReading()");
	ATH_MSG_DEBUG("begin ALFA_LocRec::ALFACollectionReading()");

	ODHIT ODHit;
	MDHIT MDHit;

	ListMDHits.clear();
	ListODHits.clear();

	const ALFA_DigitCollection* mcGen = 0;
	const ALFA_ODDigitCollection* mcODGen = 0;

	sc = evtStore()->retrieve(mcGen,m_strCollectionName);
	if(sc.isFailure() || !mcGen)
	{
		return StatusCode::FAILURE;
	}

	ALFA_DigitCollection::const_iterator mcGenBeg = mcGen->begin();
	ALFA_DigitCollection::const_iterator mcGenEnd = mcGen->end();

	for(;mcGenBeg!=mcGenEnd;++mcGenBeg)
	{
		MDHit.iEvent = m_eventNum;

		MDHit.iRPot  = ((*mcGenBeg)->getStation());
		MDHit.iPlate = ((*mcGenBeg)->getPlate());
		MDHit.iFiber = ((*mcGenBeg)->getFiber());

		ListMDHits.push_back(MDHit);
	}

	sc = evtStore()->retrieve(mcODGen,m_strODCollectionName);
	if(sc.isFailure() || !mcODGen)
	{
		return StatusCode::FAILURE;
	}

	ALFA_ODDigitCollection::const_iterator mcODGenBeg = mcODGen->begin();
	ALFA_ODDigitCollection::const_iterator mcODGenEnd = mcODGen->end();

	for(;mcODGenBeg!=mcODGenEnd;++mcODGenBeg)
	{
		ODHit.iEvent = m_eventNum;
		ODHit.iRPot  = ((*mcODGenBeg)->getStation());
		ODHit.iPlate = ((*mcODGenBeg)->getPlate());
		ODHit.iSide  = ((*mcODGenBeg)->getSide());
		ODHit.iFiber = ((*mcODGenBeg)->getFiber());

		ListODHits.push_back(ODHit);
	}

	ATH_MSG_DEBUG("end ALFA_LocRec::ALFACollectionReading()");

	return StatusCode::SUCCESS;
}

bool ALFA_LocRec::ReadGeometryDetCS()
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_LocRec::ReadGeometryDetCS()");
	ATH_MSG_DEBUG("begin ALFA_LocRec::ReadGeometryDetCS()");

	Int_t nPlateID, nFiberID;
	FIBERPARAMS FiberParams;
	eRPotName eRPName;

	bool bRes=false;
	std::list<eRPotName>::const_iterator iterRPName;

	if((bRes=m_pGeometryReader->Initialize(&m_Config,EFCS_ATLAS))==true)
	{
		ATH_MSG_DEBUG("Geometry successfully initialized");
		m_pGeometryReader->GetListOfExistingRPotIDs(&m_ListExistingRPots);
	}
	else
	{
		ATH_MSG_FATAL("Cannot initialize geometry");
		return bRes;
	}

	for(iterRPName=m_ListExistingRPots.begin();iterRPName!=m_ListExistingRPots.end();iterRPName++)
	{
		eRPName = *iterRPName;

		//for MD-fibers
		for (nPlateID = 0; nPlateID < ALFAPLATESCNT; nPlateID++)
		{
			for (nFiberID = 0; nFiberID < ALFAFIBERSCNT; nFiberID++)
			{
				//for V-fibers
				if (m_pGeometryReader->GetVFiberParams(&FiberParams, eRPName, nPlateID+1, nFiberID+1))
				{
					m_faMD[eRPName-1][2*nPlateID+1][nFiberID] = FiberParams.fSlope;
					m_fbMD[eRPName-1][2*nPlateID+1][nFiberID] = FiberParams.fOffset;
					m_fzMD[eRPName-1][2*nPlateID+1][nFiberID] = FiberParams.fZPos;
				}
				else
				{
					ATH_MSG_ERROR("Unable to get FiberV parameters");
					return false;
				}

				//for U-fibers
				if (m_pGeometryReader->GetUFiberParams(&FiberParams, eRPName, nPlateID+1, nFiberID+1))
				{
					m_faMD[eRPName-1][2*nPlateID][nFiberID]	= FiberParams.fSlope;
					m_fbMD[eRPName-1][2*nPlateID][nFiberID]	= FiberParams.fOffset;
					m_fzMD[eRPName-1][2*nPlateID][nFiberID]	= FiberParams.fZPos;
				}
				else
				{
					ATH_MSG_ERROR("Unable to get FiberU parameters");
					return false;
				}
			}
		}

		//m_ODfiber = (m_ODside==0)? m_ODfiber : 29-m_ODfiber;
		//for OD-fibers
		for (nPlateID = 0; nPlateID < ODPLATESCNT; nPlateID++)
		{
			for (nFiberID = 0; nFiberID < ODFIBERSCNT; nFiberID++)
			{
				//(+16 because U0-nFiberID is indexed from 16 to 30 in Geometry package)
				if (m_pGeometryReader->GetODFiberParams(&FiberParams, EFT_ODFIBERU0, eRPName, nPlateID+1, nFiberID+16))
				{
					m_faOD[eRPName-1][nPlateID][0][nFiberID+15] = FiberParams.fSlope;
					m_fbOD[eRPName-1][nPlateID][0][nFiberID+15] = FiberParams.fOffset;
					m_fzOD[eRPName-1][nPlateID][0][nFiberID+15] = FiberParams.fZPos;
				}
				else
				{
					ATH_MSG_ERROR("Unable to get ODFiberU0 parameters");
					return false;
				}

				if (m_pGeometryReader->GetODFiberParams(&FiberParams, EFT_ODFIBERV0, eRPName, nPlateID+1, nFiberID+1))
				{
					m_faOD[eRPName-1][nPlateID][0][nFiberID] = FiberParams.fSlope;
					m_fbOD[eRPName-1][nPlateID][0][nFiberID] = FiberParams.fOffset;
					m_fzOD[eRPName-1][nPlateID][0][nFiberID] = FiberParams.fZPos;
				}
				else
				{
					ATH_MSG_ERROR("Unable to get ODFiberV0 parameters");
					return false;
				}

				if (m_pGeometryReader->GetODFiberParams(&FiberParams, EFT_ODFIBERU1, eRPName, nPlateID+1, nFiberID+16))
				{
					if (m_iDataType==1)	//for the tunnel and testbeam data
					{
						m_faOD[eRPName-1][nPlateID][1][14-nFiberID] = FiberParams.fSlope;
						m_fbOD[eRPName-1][nPlateID][1][14-nFiberID] = FiberParams.fOffset;
						m_fzOD[eRPName-1][nPlateID][1][14-nFiberID] = FiberParams.fZPos;
					}
					else				//simulation
					{
						m_faOD[eRPName-1][nPlateID][1][nFiberID+15] = FiberParams.fSlope;
						m_fbOD[eRPName-1][nPlateID][1][nFiberID+15] = FiberParams.fOffset;
						m_fzOD[eRPName-1][nPlateID][1][nFiberID+15] = FiberParams.fZPos;
					}
				}
				else
				{
					ATH_MSG_ERROR("Unable to get ODFiberU1 parameters");
					return false;
				}
				//(+16 because V1-nFiberID is indexed from 16 to 30 in Geometry package)
				if (m_pGeometryReader->GetODFiberParams(&FiberParams, EFT_ODFIBERV1, eRPName, nPlateID+1, nFiberID+1))
				{
					if (m_iDataType==1)	//for the tunnel and testbeam data
					{
						m_faOD[eRPName-1][nPlateID][1][29-nFiberID] = FiberParams.fSlope;
						m_fbOD[eRPName-1][nPlateID][1][29-nFiberID] = FiberParams.fOffset;
						m_fzOD[eRPName-1][nPlateID][1][29-nFiberID] = FiberParams.fZPos;
					}
					else				//simulation
					{
						m_faOD[eRPName-1][nPlateID][1][nFiberID] = FiberParams.fSlope;
						m_fbOD[eRPName-1][nPlateID][1][nFiberID] = FiberParams.fOffset;
						m_fzOD[eRPName-1][nPlateID][1][nFiberID] = FiberParams.fZPos;
					}
				}
				else
				{
					ATH_MSG_ERROR("Unable to get ODFiberV1 parameters");
					return false;
				}
			}
		}
	}

//	SaveGeometry();

	ATH_MSG_DEBUG("end ALFA_LocRec::ReadGeometryDetCS()");

	return bRes;
}

bool ALFA_LocRec::StoreReconstructionGeometry(const eRPotName eRPName, const eFiberType eFType, const char* szDataDestination)
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_LocRec::StoreReconstructionGeometry()");
	ATH_MSG_DEBUG("begin ALFA_LocRec::StoreReconstructionGeometry()");

	Int_t iLayer, iFiber, iPlate, iSide;
	Float_t fParamB, fZ, fSlope;
	FILE * pFile;

	pFile = fopen(szDataDestination, "w");
	if(pFile==NULL) return false;
	fprintf(pFile, "xxxxxxxxxxxxxxxxxxx\n");
	if (eFType==EFT_FIBERMD)
	{
		fprintf(pFile, "20\n");
		for(iLayer=0; iLayer<2*ALFAPLATESCNT; iLayer++)
		{
			for(iFiber=0; iFiber<ALFAFIBERSCNT; iFiber++)
			{
				fSlope=m_faMD[eRPName-1][iLayer][iFiber];
				fParamB=m_fbMD[eRPName-1][iLayer][iFiber];
				fZ=m_fzMD[eRPName-1][iLayer][iFiber];

				fprintf(pFile, " %2d  %2d  %7.5f  %7.4f  %7.3f\n", iLayer+1, iFiber+1, fSlope, fParamB, fZ);
			}
		}
	}
	else if (eFType==EFT_FIBEROD)
	{
		fprintf(pFile, "6\n");
		for(iPlate=0; iPlate<ODPLATESCNT; iPlate++)
		{
			for (iSide=0; iSide<ODSIDESCNT; iSide++)
			{
				for(iFiber=0; iFiber<ODLAYERSCNT*ODFIBERSCNT; iFiber++)
				{
					fSlope=m_faOD[eRPName-1][iPlate][iSide][iFiber];
					fParamB=m_fbOD[eRPName-1][iPlate][iSide][iFiber];
					fZ=m_fzOD[eRPName-1][iPlate][iSide][iFiber];

					fprintf(pFile, " %2d  %2d  %7.5f  %7.4f  %7.3f\n", (2*iPlate+iSide+1), iFiber+1, fSlope, fParamB, fZ);
				}
			}
		}
	}
	fclose(pFile);

	ATH_MSG_DEBUG("end ALFA_LocRec::StoreReconstructionGeometry()");

	return true;
}

void ALFA_LocRec::SaveGeometry()
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_LocRec::SaveGeometry()");
	ATH_MSG_DEBUG("begin ALFA_LocRec::SaveGeometry()");


	eGeoSourceType eGeoType;
	char szFilename[64];
	std::list<eRPotName>::const_iterator iterRPName;

	for(iterRPName=m_ListExistingRPots.begin();iterRPName!=m_ListExistingRPots.end();iterRPName++)
	{
		eGeoType=m_pGeometryReader->GetRPGeometryType(*iterRPName, EFT_FIBERMD);
		switch(eGeoType)
		{
			case EGST_IDEALGEOMETRY:
				sprintf(szFilename,"ALFA_LocRec_Idealgeometry_RP-%s.txt",m_pGeometryReader->GetRPotLabel(*iterRPName));
				StoreReconstructionGeometry(*iterRPName, EFT_FIBERMD, szFilename);
				ATH_MSG_DEBUG("The ALFA Fiber geometry was stored in the "<<szFilename<<" file");
				break;
			case EGST_FILE:
				sprintf(szFilename,"ALFA_LocRec_Realgeometry_RP-%s.txt",m_pGeometryReader->GetRPotLabel(*iterRPName));
				StoreReconstructionGeometry(*iterRPName, EFT_FIBERMD, szFilename);
				ATH_MSG_DEBUG("The ALFA Fiber geometry was stored in the "<<szFilename<<" file");
				break;
			case EGST_DATABASE:
				sprintf(szFilename,"ALFA_LocRec_Realdbgeometry_RP-%s.txt",m_pGeometryReader->GetRPotLabel(*iterRPName));
				StoreReconstructionGeometry(*iterRPName, EFT_FIBERMD, szFilename);
				ATH_MSG_DEBUG("The ALFA Fiber geometry was stored in the "<<szFilename<<" file");
				break;
			default:
				ATH_MSG_DEBUG("Unrecognized MD geometry!");
				return;
				break;
		}

		eGeoType=m_pGeometryReader->GetRPGeometryType(*iterRPName, EFT_FIBEROD);
		switch(eGeoType)
		{
			case EGST_IDEALGEOMETRY:
				sprintf(szFilename,"ALFA_LocRec_Idealgeometry_OD_RP-%s.txt",m_pGeometryReader->GetRPotLabel(*iterRPName));
				StoreReconstructionGeometry(*iterRPName, EFT_FIBEROD, szFilename);
				ATH_MSG_DEBUG("The ODFiber geometry was stored in the "<<szFilename<<" file");
				break;
			case EGST_FILE:
				sprintf(szFilename,"ALFA_LocRec_Realgeometry_OD_RP-%s.txt",m_pGeometryReader->GetRPotLabel(*iterRPName));
				StoreReconstructionGeometry(*iterRPName, EFT_FIBEROD, szFilename);
				ATH_MSG_DEBUG("The ODFiber geometry was stored in the "<<szFilename<<" file");
				break;
			case EGST_DATABASE:
				sprintf(szFilename,"ALFA_LocRec_Realdbgeometry_OD_RP-%s.txt",m_pGeometryReader->GetRPotLabel(*iterRPName));
				StoreReconstructionGeometry(*iterRPName, EFT_FIBEROD, szFilename);
				ATH_MSG_DEBUG("The ODFiber geometry was stored in the "<<szFilename<<" file");
				break;
			default:
				ATH_MSG_DEBUG("Unrecognized OD geometry!");
				return;
				break;
		}
	}

	ATH_MSG_DEBUG("end ALFA_LocRec::SaveGeometry()");
}

StatusCode ALFA_LocRec::ExecuteRecoMethod(const std::string strAlgo, const eRPotName eRPName, const std::list<MDHIT> &ListMDHits, const std::list<ODHIT> &ListODHits)
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_LocRec::ExecuteRecoMethod()");
	ATH_MSG_DEBUG("begin ALFA_LocRec::ExecuteRecoMethod()");

	StatusCode sc = StatusCode::SUCCESS;
	ODRESULT ODResults;
	std::list<ODRESULT> listODResults;
	listODResults.clear();
	Float_t fRecPosX = -9999.0;
	Float_t fRecPosY = -9999.0;

	std::map<string, int> mapRecoMethods;
	mapRecoMethods.clear();
	mapRecoMethods.insert(std::pair<string, int>("ODTracking", 1));
	mapRecoMethods.insert(std::pair<string, int>("MDTracking", 2));
	mapRecoMethods.insert(std::pair<string, int>("MDOverlap", 3));
	mapRecoMethods.insert(std::pair<string, int>("CenterGravity", 4));
	mapRecoMethods.insert(std::pair<string, int>("MDMultiple", 5));
	mapRecoMethods.insert(std::pair<string, int>("HalfReco", 6));
	mapRecoMethods.insert(std::pair<string, int>("MDGap", 7));
	mapRecoMethods.insert(std::pair<string, int>("EdgeMethod", 8));

	std::vector<int> vecFibSel;
	vecFibSel.clear();

	switch(mapRecoMethods[strAlgo])
	{
	case 1:
		{
			ALFA_ODTracking* pODTracking = new ALFA_ODTracking();

			sc = pODTracking->Initialize(m_iMultiplicityCutOD, m_fDistanceCutOD, m_iLayerCutOD, m_iDataType);
			sc = pODTracking->Execute(eRPName-1, ListODHits, m_faOD, m_fbOD);
			sc = pODTracking->Finalize(&listODResults);

			std::list<ODRESULT>::const_iterator iter;
			for(iter=listODResults.begin(); iter!=listODResults.end(); iter++)
			{
				ODResults.iSide   = (*iter).iSide;
				ODResults.fRecPos = (*iter).fRecPos;
				ODResults.fOverY  = (*iter).fOverY;
				ODResults.iNumY   = (*iter).iNumY;

				if ((ODResults.fRecPos != -9999.0))
				{
					Int_t iFibSel[ODSIDESCNT][ODPLATESCNT];
					std::fill_n(&iFibSel[0][0], sizeof(iFibSel)/sizeof(Int_t), -9999);

					pODTracking->GetData(iFibSel);
					vecFibSel.clear();
					for (int iPlate=0; iPlate<ODPLATESCNT; iPlate++)
					{
						vecFibSel.push_back(iFibSel[ODResults.iSide][iPlate]);
					}

					m_pLocRecODEvCollection->push_back(new ALFA_LocRecODEvent(1, eRPName-1, ODResults.iSide, ODResults.fRecPos, ODResults.fOverY, ODResults.iNumY, vecFibSel));
				}
			}

			delete pODTracking;
			break;
		}
	case 2:
		{
			ALFA_MDTracking* pMDTracking = new ALFA_MDTracking();
			sc = pMDTracking->Initialize(m_faMD, m_fbMD, m_iMultiplicityCutMD, m_iUVCutMD, m_fOverlapCutMD);
			sc = pMDTracking->Execute(eRPName-1, ListMDHits);
			sc = pMDTracking->Finalize(fRecPosX, fRecPosY);

			if (fRecPosX!=-9999.0 && fRecPosY!=-9999.0)
			{
				Int_t iFibSel[ALFALAYERSCNT*ALFAPLATESCNT];
				Int_t iNumU = 0, iNumV = 0;
				Float_t fOverlapU = -9999.0, fOverlapV = -9999.0;
				std::fill_n(&iFibSel[0], sizeof(iFibSel)/sizeof(Int_t), -9999);

				pMDTracking->GetData(iNumU, iNumV, fOverlapU, fOverlapV, iFibSel);
				vecFibSel.clear();
				for (Int_t iLayer=0; iLayer<ALFALAYERSCNT*ALFAPLATESCNT; iLayer++)
				{
					vecFibSel.push_back(iFibSel[iLayer]);
				}

				m_pLocRecEvCollection->push_back(new ALFA_LocRecEvent(2, eRPName-1, fRecPosX, fRecPosY, fOverlapU, fOverlapV, iNumU, iNumV, vecFibSel));
			}

			delete pMDTracking;
			break;
		}
	case 3:
		{
			ALFA_MDOverlap* pMDOverlap = new ALFA_MDOverlap();

			sc = pMDOverlap->Initialize(eRPName, ListMDHits, m_faMD, m_fbMD, m_fzMD);
			sc = pMDOverlap->Execute();
			sc = pMDOverlap->Finalize(fRecPosX, fRecPosY);

			Int_t iFibSel[ALFALAYERSCNT*ALFAPLATESCNT];
			Int_t iNumU = 0, iNumV = 0;
			Float_t fOverlapU = -9999.0, fOverlapV = -9999.0;
			std::fill_n(&iFibSel[0], sizeof(iFibSel)/sizeof(Int_t), -9999);

			pMDOverlap->GetData(iFibSel);
			if (fRecPosX!=-9999 && fRecPosY!=-9999)
			{
				vecFibSel.clear();
				for (Int_t iLayer=0; iLayer<ALFALAYERSCNT*ALFAPLATESCNT; iLayer++)
				{
					vecFibSel.push_back(iFibSel[iLayer]);
				}

				m_pLocRecEvCollection->push_back(new ALFA_LocRecEvent(3, eRPName-1, fRecPosX, fRecPosY, fOverlapU, fOverlapV, iNumU, iNumV, vecFibSel));
			}

			delete pMDOverlap;
			break;
		}
	case 4:
		{
			ALFA_CenterGravity* pCenterGravity = new ALFA_CenterGravity();

			sc = pCenterGravity->Initialize(eRPName, ListMDHits, m_faMD, m_fbMD, m_fzMD);
			sc = pCenterGravity->Execute();
			sc = pCenterGravity->Finalize(fRecPosX, fRecPosY);

			Int_t iFibSel[ALFALAYERSCNT*ALFAPLATESCNT];
			Int_t iNumU = 0, iNumV = 0;
			Float_t fOverlapU = -9999.0, fOverlapV = -9999.0;
			std::fill_n(&iFibSel[0], sizeof(iFibSel)/sizeof(Int_t), -9999);

			pCenterGravity->GetData(iFibSel);
			if (fRecPosX!=-9999 && fRecPosY!=-9999)
			{
				vecFibSel.clear();
				for (Int_t iLayer=0; iLayer<ALFALAYERSCNT*ALFAPLATESCNT; iLayer++)
				{
					vecFibSel.push_back(iFibSel[iLayer]);
				}

				m_pLocRecEvCollection->push_back(new ALFA_LocRecEvent(4, eRPName-1, fRecPosX, fRecPosY, fOverlapU, fOverlapV, iNumU, iNumV, vecFibSel));
			}

			delete pCenterGravity;
			break;
		}
	case 5:
		{
			// MDMultiple algorihm ------------------------------------------------------
			Float_t fRecPosX[NTRACK];
			Float_t fRecPosY[NTRACK];
			ALFA_MDMultiple* pMDMultiple = new ALFA_MDMultiple();

			sc = pMDMultiple->Initialize(eRPName-1, m_faMD, m_fbMD, m_iMultiplicityCutMD, m_iNumLayerCutMD, m_iUVCutMD, m_fOverlapCutMD);
			sc = pMDMultiple->Execute(ListMDHits);
			sc = pMDMultiple->Finalize(fRecPosX, fRecPosY);

			Int_t iNumU[NTRACK], iNumV[NTRACK], iFibSel[NTRACK][ALFALAYERSCNT*ALFAPLATESCNT];
			Float_t fOverU[NTRACK], fOverV[NTRACK];
			std::fill_n(&iFibSel[0][0], sizeof(iFibSel)/sizeof(Int_t), -9999);
			std::fill_n(&fOverU[0], sizeof(fOverU)/sizeof(Float_t), -9999.0);
			std::fill_n(&fOverV[0], sizeof(fOverV)/sizeof(Float_t), -9999.0);
			memset(&iNumU, 0, sizeof(iNumU));
			memset(&iNumV, 0, sizeof(iNumV));

			pMDMultiple->GetData(iNumU, iNumV, fOverU, fOverV, iFibSel);
			for (Int_t iTrack=0; iTrack<NTRACK; iTrack++)
			{
				if (fRecPosX[iTrack] != -9999.0 && fRecPosY[iTrack] != -9999.0)
				{
					vecFibSel.clear();
					for (Int_t iLayer=0; iLayer<ALFALAYERSCNT*ALFAPLATESCNT; iLayer++)
					{
						vecFibSel.push_back(iFibSel[iTrack][iLayer]);
					}

					m_pLocRecEvCollection->push_back(new ALFA_LocRecEvent(5, eRPName-1, fRecPosX[iTrack], fRecPosY[iTrack], fOverU[iTrack], fOverV[iTrack], iNumU[iTrack], iNumV[iTrack], vecFibSel));
				}
			}

			delete pMDMultiple;
			break;
		}
	case 6:
		{
		//TODO - cele prepracovat
			ALFA_HalfReco* pHalfReco = new ALFA_HalfReco();

			Int_t iNumU[2] = {0, 0};
			Int_t iNumV[2] = {0, 0};
			Int_t iHalfFirst=0;
			Int_t iHalfSecond=1;
			Float_t fOverlapU[2] = {-9999, -9999};
			Float_t fOverlapV[2] = {-9999, -9999};
			Float_t fRecPosX[2] = {-9999, -9999};
			Float_t fRecPosY[2] = {-9999, -9999};

			Int_t iFibSel[ALFALAYERSCNT*ALFAPLATESCNT];
			std::fill_n(&iFibSel[0], sizeof(iFibSel)/sizeof(Int_t), -9999);

			std::vector<int> vecFibSel0;
			std::vector<int> vecFibSel1;
			vecFibSel0.clear();
			vecFibSel1.clear();

			if (eRPName==1 || eRPName==3 || eRPName==6 || eRPName==8)
			{
				iHalfFirst  = 0;
				iHalfSecond = 1;
			}
			else
			{
				iHalfFirst  = 1;
				iHalfSecond = 0;
			}

			sc = pHalfReco->Initialize(m_faMD, m_fbMD, iHalfFirst, m_iMultiplicityCutMD, m_iUVCutMDHalfReco, m_fOverlapCutMD);
			sc = pHalfReco->Execute(eRPName-1, ListMDHits);
			sc = pHalfReco->Finalize(fRecPosX[0], fRecPosY[0]);

			pHalfReco->GetData(iNumU[0], iNumV[0], fOverlapU[0], fOverlapV[0], iFibSel);
			vecFibSel0.clear();
			for (Int_t iLayer=0; iLayer<ALFALAYERSCNT*ALFAPLATESCNT; iLayer++)
			{
				vecFibSel0.push_back(iFibSel[iLayer]);
			}

			sc = pHalfReco->Initialize(m_faMD, m_fbMD, iHalfSecond, m_iMultiplicityCutMD, m_iUVCutMDHalfReco, m_fOverlapCutMD);
			sc = pHalfReco->Execute(eRPName-1, ListMDHits);
			sc = pHalfReco->Finalize(fRecPosX[1], fRecPosY[1]);

			pHalfReco->GetData(iNumU[1], iNumV[1], fOverlapU[1], fOverlapV[1], iFibSel);
			vecFibSel1.clear();
			for (Int_t iLayer=0; iLayer<ALFALAYERSCNT*ALFAPLATESCNT; iLayer++)
			{
				vecFibSel1.push_back(iFibSel[iLayer]);
			}

			if (fRecPosX[0]!=-9999.0 && fRecPosY[0]!=-9999.0 && fRecPosX[1]!=-9999.0 && fRecPosY[1]!=-9999.0)
			{
				m_pLocRecEvCollection->push_back(new ALFA_LocRecEvent(6, eRPName-1, fRecPosX[0], fRecPosY[0], fOverlapU[0], fOverlapV[0], iNumU[0], iNumV[0], vecFibSel0));
				m_pLocRecEvCollection->push_back(new ALFA_LocRecEvent(6, eRPName-1, fRecPosX[1], fRecPosY[1], fOverlapU[1], fOverlapV[1], iNumU[1], iNumV[1], vecFibSel1));
			}

			delete pHalfReco;
			break;
		}
	case 7:
		{
			Float_t fRecPosX[NTRACK];
			Float_t fRecPosY[NTRACK];
			ALFA_MDGap* pMDGap = new ALFA_MDGap();

			sc = pMDGap->Initialize(eRPName-1, m_faMD, m_fbMD, m_iUVCutMD, m_fOverlapCutMD);
			sc = pMDGap->Execute(ListMDHits);
			sc = pMDGap->Finalize(fRecPosX, fRecPosY);

			Int_t iNumU[NTRACK], iNumV[NTRACK];
			Int_t iFibSel[ALFALAYERSCNT*ALFAPLATESCNT];
			Float_t fOverU[NTRACK], fOverV[NTRACK];

			std::fill_n(&iFibSel[0], sizeof(iFibSel)/sizeof(Int_t), -9999);
			std::fill_n(&fOverU[0], sizeof(fOverU)/sizeof(Float_t), -9999.0);
			std::fill_n(&fOverV[0], sizeof(fOverV)/sizeof(Float_t), -9999.0);

			pMDGap->GetData(iNumU, iNumV, fOverU, fOverV, iFibSel);
			for (Int_t iTrack=0; iTrack<NTRACK; iTrack++)
			{
				if (fRecPosX[iTrack] != -9999.0 && fRecPosY[iTrack] != -9999.0)
				{
					vecFibSel.clear();
					for (Int_t iLayer=0; iLayer<ALFALAYERSCNT*ALFAPLATESCNT; iLayer++)
					{
						vecFibSel.push_back(iFibSel[iLayer]);
					}

					m_pLocRecEvCollection->push_back(new ALFA_LocRecEvent(7, eRPName-1, fRecPosX[iTrack], fRecPosY[iTrack], fOverU[iTrack], fOverV[iTrack], iNumU[iTrack], iNumV[iTrack], vecFibSel));
				}
			}

			delete pMDGap;
			break;
		}
	case 8:
		{
			Float_t fRecPosX[NTRACK];
			Float_t fRecPosY[NTRACK];
			ALFA_EdgeMethod* pEdgeMethod = new ALFA_EdgeMethod(m_bEdgeMethod_Opt_Sisters, m_bEdgeMethod_Opt_UseGaps);
			pEdgeMethod->Initialize(eRPName-1, m_faMD, m_fbMD, ListMDHits);

			Int_t iNumU[NTRACK], iNumV[NTRACK];
			Int_t iFibSel[ALFALAYERSCNT*ALFAPLATESCNT];
			Float_t fOverU[NTRACK], fOverV[NTRACK];

			std::fill_n(&iFibSel[0], sizeof(iFibSel)/sizeof(Int_t), -9999);
			std::fill_n(&fOverU[0], sizeof(fOverU)/sizeof(Float_t), -9999.0);
			std::fill_n(&fOverV[0], sizeof(fOverV)/sizeof(Float_t), -9999.0);


			std::vector<ALFA_EdgeMethod::Track> tracks;
			pEdgeMethod->EdgeMethod(eRPName - 1, tracks);

			for (UInt_t iTrack=0; iTrack<tracks.size(); iTrack++)
			{
				pEdgeMethod->selectedFibers(eRPName - 1, tracks.at(iTrack), iFibSel);

				iNumU[iTrack]    = tracks.at(iTrack).first.second;
				iNumV[iTrack]    = tracks.at(iTrack).second.second;
				fOverU[iTrack]   = tracks.at(iTrack).first.first.second;
				fOverV[iTrack]   = tracks.at(iTrack).second.first.second;
				fRecPosX[iTrack] = 0.5*TMath::Sqrt2()*(-tracks.at(iTrack).first.first.first-tracks.at(iTrack).second.first.first);
				fRecPosY[iTrack] = 0.5*TMath::Sqrt2()*(-tracks.at(iTrack).first.first.first+tracks.at(iTrack).second.first.first);

				if (fRecPosX[iTrack] != -9999.0 && fRecPosY[iTrack] != -9999.0)
				{
					vecFibSel.clear();
					for (Int_t iLayer=0; iLayer<ALFALAYERSCNT*ALFAPLATESCNT; iLayer++)
					{
						vecFibSel.push_back(iFibSel[iLayer]);
					}

					m_pLocRecEvCollection->push_back(new ALFA_LocRecEvent(8, eRPName-1, fRecPosX[iTrack], fRecPosY[iTrack], fOverU[iTrack], fOverV[iTrack], iNumU[iTrack], iNumV[iTrack], vecFibSel));
				}
			}

			delete pEdgeMethod;
			sc = StatusCode::SUCCESS;
			break;
		}
	default:
		{
			ATH_MSG_ERROR("Unable to recognize selected algorithm");
			std::cout << "strAlgo: " << strAlgo << std::endl;
			return StatusCode::SUCCESS;
		}
	}

	ATH_MSG_DEBUG("end ALFA_LocRec::ExecuteRecoMethod()");

	return sc;
}

StatusCode ALFA_LocRec::RecordCollection()
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_LocRec::RecordCollection()");
	ATH_MSG_DEBUG("begin ALFA_LocRec::RecordCollection()");

	StatusCode sc = StatusCode::SUCCESS;

	m_pLocRecEvCollection = new ALFA_LocRecEvCollection();
	sc = evtStore()->record(m_pLocRecEvCollection, m_strKeyLocRecEvCollection);

	if (sc.isFailure())
	{
		ATH_MSG_ERROR(m_strAlgoMD << "MD - Could not record the empty LocRecEv collection in StoreGate");
		return sc;
	}
	else
	{
		ATH_MSG_DEBUG("MD - LocRecEv collection was recorded in StoreGate");
	}

	ATH_MSG_DEBUG("end ALFA_LocRec::RecordCollection()");

	return sc;
}

StatusCode ALFA_LocRec::RecordODCollection()
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_LocRec::RecordODCollection()");
	ATH_MSG_DEBUG("begin ALFA_LocRec::RecordODCollection()");

	StatusCode sc = StatusCode::SUCCESS;

	m_pLocRecODEvCollection = new ALFA_LocRecODEvCollection();
	sc = evtStore()->record(m_pLocRecODEvCollection, m_strKeyLocRecODEvCollection);

	if (sc.isFailure())
	{
		ATH_MSG_ERROR(m_strAlgoOD << "OD - Could not record the empty LocRecEv collection in StoreGate");

		return sc;
	}
	else
	{
		 ATH_MSG_DEBUG("OD - LocRecEv collection is recorded in StoreGate");
	}

	ATH_MSG_DEBUG("end ALFA_LocRec::RecordODCollection()");

	return sc;
}

void ODRESULT::clear()
{
	iSide	= -1;
	fRecPos	= -9999.0;
	fOverY  = -9999.0;
	iNumY   = -1;
}
