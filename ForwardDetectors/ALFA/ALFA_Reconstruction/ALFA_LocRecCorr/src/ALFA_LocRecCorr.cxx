/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_LocRecCorr/ALFA_LocRecCorr.h"
#include "ALFA_Geometry/ALFA_GeometryReader.h"

#include "AthenaKernel/errorcheck.h"


ALFA_LocRecCorr::ALFA_LocRecCorr(const string& name, ISvcLocator* pSvcLocator) :
AthAlgorithm(name, pSvcLocator)
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_LocRecCorr::ALFA_LocRecCorr");
	ATH_MSG_DEBUG("begin ALFA_LocRecCorr::ALFA_LocRecCorr");

	m_pGeometryReader = 0;

//	m_Config.eOwner=EOT_RECO;
	m_Config.clear();

	//begin of Geometry properties
	declareProperty("MetrologyType",m_Config.eRPMetrologyGeoType=EMT_NOMINAL);//EGST_FILE
	declareProperty("MetrologySource",m_Config.strRPMetrologyConnString=string(""));//"rpmetrology.dat"
	declareProperty("ShiftToX97Pos",m_Config.bShiftToX97Pos=false);

	declareProperty("B7L1U_PosType",m_Config.CfgRPosParams[0].eRPPosType=ERPPT_ACTIVE);
	declareProperty("B7L1U_MDGeometryType",m_Config.CfgRPosParams[0].eMDGeoType=EGST_IDEALGEOMETRY);
	declareProperty("B7L1U_ODGeometryType",m_Config.CfgRPosParams[0].eODGeoType=EGST_IDEALGEOMETRY);
	declareProperty("B7L1U_MDSource",m_Config.CfgRPosParams[0].strMDConnString=string(""));
	declareProperty("B7L1U_ODSource",m_Config.CfgRPosParams[0].strODConnString=string(""));
	declareProperty("B7L1U_CurrentLVDT",m_Config.CfgRPosParams[0].fCurrentLVDTmm=7.0*CLHEP::mm);

	declareProperty("B7L1L_PosType",m_Config.CfgRPosParams[1].eRPPosType=ERPPT_ACTIVE);
	declareProperty("B7L1L_MDGeometryType",m_Config.CfgRPosParams[1].eMDGeoType=EGST_IDEALGEOMETRY);
	declareProperty("B7L1L_ODGeometryType",m_Config.CfgRPosParams[1].eODGeoType=EGST_IDEALGEOMETRY);
	declareProperty("B7L1L_MDSource",m_Config.CfgRPosParams[1].strMDConnString=string(""));
	declareProperty("B7L1L_ODSource",m_Config.CfgRPosParams[1].strODConnString=string(""));
	declareProperty("B7L1L_CurrentLVDT",m_Config.CfgRPosParams[1].fCurrentLVDTmm=7.0*CLHEP::mm);

	declareProperty("A7L1U_PosType",m_Config.CfgRPosParams[2].eRPPosType=ERPPT_ACTIVE);
	declareProperty("A7L1U_MDGeometryType",m_Config.CfgRPosParams[2].eMDGeoType=EGST_IDEALGEOMETRY);
	declareProperty("A7L1U_ODGeometryType",m_Config.CfgRPosParams[2].eODGeoType=EGST_IDEALGEOMETRY);
	declareProperty("A7L1U_MDSource",m_Config.CfgRPosParams[2].strMDConnString=string(string("")));
	declareProperty("A7L1U_ODSource",m_Config.CfgRPosParams[2].strODConnString=string(string("")));
	declareProperty("A7L1U_CurrentLVDT",m_Config.CfgRPosParams[2].fCurrentLVDTmm=7.0*CLHEP::mm);

	declareProperty("A7L1L_PosType",m_Config.CfgRPosParams[3].eRPPosType=ERPPT_ACTIVE);
	declareProperty("A7L1L_MDGeometryType",m_Config.CfgRPosParams[3].eMDGeoType=EGST_IDEALGEOMETRY);
	declareProperty("A7L1L_ODGeometryType",m_Config.CfgRPosParams[3].eODGeoType=EGST_IDEALGEOMETRY);
	declareProperty("A7L1L_MDSource",m_Config.CfgRPosParams[3].strMDConnString=string(""));
	declareProperty("A7L1L_ODSource",m_Config.CfgRPosParams[3].strODConnString=string(""));
	declareProperty("A7L1L_CurrentLVDT",m_Config.CfgRPosParams[3].fCurrentLVDTmm=7.0*CLHEP::mm);

	declareProperty("A7R1U_PosType",m_Config.CfgRPosParams[4].eRPPosType=ERPPT_ACTIVE);
	declareProperty("A7R1U_MDGeometryType",m_Config.CfgRPosParams[4].eMDGeoType=EGST_IDEALGEOMETRY);
	declareProperty("A7R1U_ODGeometryType",m_Config.CfgRPosParams[4].eODGeoType=EGST_IDEALGEOMETRY);
	declareProperty("A7R1U_MDSource",m_Config.CfgRPosParams[4].strMDConnString=string(""));
	declareProperty("A7R1U_ODSource",m_Config.CfgRPosParams[4].strODConnString=string(""));
	declareProperty("A7R1U_CurrentLVDT",m_Config.CfgRPosParams[4].fCurrentLVDTmm=7.0*CLHEP::mm);

	declareProperty("A7R1L_PosType",m_Config.CfgRPosParams[5].eRPPosType=ERPPT_ACTIVE);
	declareProperty("A7R1L_MDGeometryType",m_Config.CfgRPosParams[5].eMDGeoType=EGST_IDEALGEOMETRY);
	declareProperty("A7R1L_ODGeometryType",m_Config.CfgRPosParams[5].eODGeoType=EGST_IDEALGEOMETRY);
	declareProperty("A7R1L_MDSource",m_Config.CfgRPosParams[5].strMDConnString=string(""));
	declareProperty("A7R1L_ODSource",m_Config.CfgRPosParams[5].strODConnString=string(""));
	declareProperty("A7R1L_CurrentLVDT",m_Config.CfgRPosParams[5].fCurrentLVDTmm=7.0*CLHEP::mm);

	declareProperty("B7R1U_PosType",m_Config.CfgRPosParams[6].eRPPosType=ERPPT_ACTIVE);
	declareProperty("B7R1U_MDGeometryType",m_Config.CfgRPosParams[6].eMDGeoType=EGST_IDEALGEOMETRY);
	declareProperty("B7R1U_ODGeometryType",m_Config.CfgRPosParams[6].eODGeoType=EGST_IDEALGEOMETRY);
	declareProperty("B7R1U_MDSource",m_Config.CfgRPosParams[6].strMDConnString=string(""));
	declareProperty("B7R1U_ODSource",m_Config.CfgRPosParams[6].strODConnString=string(""));
	declareProperty("B7R1U_CurrentLVDT",m_Config.CfgRPosParams[6].fCurrentLVDTmm=7.0*CLHEP::mm);

	declareProperty("B7R1L_PosType",m_Config.CfgRPosParams[7].eRPPosType=ERPPT_ACTIVE);
	declareProperty("B7R1L_MDGeometryType",m_Config.CfgRPosParams[7].eMDGeoType=EGST_IDEALGEOMETRY);
	declareProperty("B7R1L_ODGeometryType",m_Config.CfgRPosParams[7].eODGeoType=EGST_IDEALGEOMETRY);
	declareProperty("B7R1L_MDSource",m_Config.CfgRPosParams[7].strMDConnString=string(""));
	declareProperty("B7R1L_ODSource",m_Config.CfgRPosParams[7].strODConnString=string(""));
	declareProperty("B7R1L_CurrentLVDT",m_Config.CfgRPosParams[7].fCurrentLVDTmm=7.0*CLHEP::mm);
	//end of Geometry properties

	//for transformations
	m_bIsTransformInStation.assign(8, false);
	m_bIsTransformInDetector.assign(8, false);

	m_pointTransformInDetectorB7L1U.assign(3, 0.0);
	m_pointTransformInDetectorB7L1L.assign(3, 0.0);
	m_pointTransformInDetectorA7L1U.assign(3, 0.0);
	m_pointTransformInDetectorA7L1L.assign(3, 0.0);
	m_pointTransformInDetectorB7R1U.assign(3, 0.0);
	m_pointTransformInDetectorB7R1L.assign(3, 0.0);
	m_pointTransformInDetectorA7R1U.assign(3, 0.0);
	m_pointTransformInDetectorA7R1L.assign(3, 0.0);

	m_vecTransformInDetectorB7L1U.assign(7, 0.0);
	m_vecTransformInDetectorB7L1L.assign(7, 0.0);
	m_vecTransformInDetectorA7L1U.assign(7, 0.0);
	m_vecTransformInDetectorA7L1L.assign(7, 0.0);
	m_vecTransformInDetectorB7R1U.assign(7, 0.0);
	m_vecTransformInDetectorB7R1L.assign(7, 0.0);
	m_vecTransformInDetectorA7R1U.assign(7, 0.0);
	m_vecTransformInDetectorA7R1L.assign(7, 0.0);

	m_vecTransformInStationB7L1U.assign(7, 0.0);
	m_vecTransformInStationB7L1L.assign(7, 0.0);
	m_vecTransformInStationA7L1U.assign(7, 0.0);
	m_vecTransformInStationA7L1L.assign(7, 0.0);
	m_vecTransformInStationB7R1U.assign(7, 0.0);
	m_vecTransformInStationB7R1L.assign(7, 0.0);
	m_vecTransformInStationA7R1U.assign(7, 0.0);
	m_vecTransformInStationA7R1L.assign(7, 0.0);

	declareProperty("bIsTransformInStation", m_bIsTransformInStation, "status if transformation in station will be calculated");
	declareProperty("bIsTransformInDetector", m_bIsTransformInDetector, "status if transformation in station will be calculated");

	declareProperty("pointTransformInDetectorB7L1U", m_pointTransformInDetectorB7L1U, "transformation point in the detectorfor B7L1U RP");
	declareProperty("pointTransformInDetectorB7L1L", m_pointTransformInDetectorB7L1L, "transformation point in the detectorfor B7L1L RP");
	declareProperty("pointTransformInDetectorA7L1U", m_pointTransformInDetectorA7L1U, "transformation point in the detectorfor A7L1U RP");
	declareProperty("pointTransformInDetectorA7L1L", m_pointTransformInDetectorA7L1L, "transformation point in the detectorfor A7L1L RP");
	declareProperty("pointTransformInDetectorA7R1U", m_pointTransformInDetectorA7R1U, "transformation point in the detectorfor A7R1U RP");
	declareProperty("pointTransformInDetectorA7R1L", m_pointTransformInDetectorA7R1L, "transformation point in the detectorfor A7R1L RP");
	declareProperty("pointTransformInDetectorB7R1U", m_pointTransformInDetectorB7R1U, "transformation point in the detectorfor B7R1U RP");
	declareProperty("pointTransformInDetectorB7R1L", m_pointTransformInDetectorB7R1L, "transformation point in the detectorfor B7R1L RP");

	declareProperty("vecTransformInDetectorB7L1U", m_vecTransformInDetectorB7L1U, "transformation data in the detectorfor B7L1U RP");
	declareProperty("vecTransformInDetectorB7L1L", m_vecTransformInDetectorB7L1L, "transformation data in the detectorfor B7L1L RP");
	declareProperty("vecTransformInDetectorA7L1U", m_vecTransformInDetectorA7L1U, "transformation data in the detectorfor A7L1U RP");
	declareProperty("vecTransformInDetectorA7L1L", m_vecTransformInDetectorA7L1L, "transformation data in the detectorfor A7L1L RP");
	declareProperty("vecTransformInDetectorA7R1U", m_vecTransformInDetectorA7R1U, "transformation data in the detectorfor A7R1U RP");
	declareProperty("vecTransformInDetectorA7R1L", m_vecTransformInDetectorA7R1L, "transformation data in the detectorfor A7R1L RP");
	declareProperty("vecTransformInDetectorB7R1U", m_vecTransformInDetectorB7R1U, "transformation data in the detectorfor B7R1U RP");
	declareProperty("vecTransformInDetectorB7R1L", m_vecTransformInDetectorB7R1L, "transformation data in the detectorfor B7R1L RP");

	declareProperty("vecTransformInStationB7L1U", m_vecTransformInStationB7L1U, "transformation data in the stationfor B7L1U RP");
	declareProperty("vecTransformInStationB7L1L", m_vecTransformInStationB7L1L, "transformation data in the stationfor B7L1L RP");
	declareProperty("vecTransformInStationA7L1U", m_vecTransformInStationA7L1U, "transformation data in the stationfor A7L1U RP");
	declareProperty("vecTransformInStationA7L1L", m_vecTransformInStationA7L1L, "transformation data in the stationfor A7L1L RP");
	declareProperty("vecTransformInStationA7R1U", m_vecTransformInStationA7R1U, "transformation data in the stationfor A7R1U RP");
	declareProperty("vecTransformInStationA7R1L", m_vecTransformInStationA7R1L, "transformation data in the stationfor A7R1L RP");
	declareProperty("vecTransformInStationB7R1U", m_vecTransformInStationB7R1U, "transformation data in the stationfor B7R1U RP");
	declareProperty("vecTransformInStationB7R1L", m_vecTransformInStationB7R1L, "transformation data in the stationfor B7R1L RP");

	m_ListExistingRPots.clear();

	// data type using in the local reconstruction
	// for the simulation data the value is 0, for the real data the value is 1. Unset value is -1
	declareProperty("DataType", m_iDataType=1, "data type using in the local reconstruction");

	m_strKeyGeometryForReco				= "ALFA_GeometryForReco";
	m_strLocRecCollectionName			= "ALFA_LocRecEvCollection";
	m_strLocRecODCollectionName			= "ALFA_LocRecODEvCollection";
	m_strKeyLocRecEvCollection			= "ALFA_LocRecEvCollection";
	m_strKeyLocRecODEvCollection		= "ALFA_LocRecODEvCollection";
	m_strKeyLocRecCorrEvCollection		= "ALFA_LocRecCorrEvCollection";
	m_strKeyLocRecCorrODEvCollection	= "ALFA_LocRecCorrODEvCollection";

	m_bCoolData = true;
	declareProperty("CoolData", m_bCoolData);

	m_pLocRecCorrEvCollection   = 0;
	m_pLocRecCorrODEvCollection = 0;
	m_iEvt = 0;

	ATH_MSG_DEBUG("end ALFA_LocRecCorr::ALFA_LocRecCorr");
}

ALFA_LocRecCorr::~ALFA_LocRecCorr()
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_LocRecCorr::~ALFA_LocRecCorr");
	ATH_MSG_DEBUG("begin ALFA_LocRecCorr::~ALFA_LocRecCorr");

	ATH_MSG_DEBUG("end ALFA_LocRecCorr::~ALFA_LocRecCorr");
}

StatusCode ALFA_LocRecCorr::initialize()
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_LocRecCorr::Initialize()");
	ATH_MSG_DEBUG("begin ALFA_LocRecCorr::Initialize()");

//	StatusCode sc;

//	sc = service("StoreGateSvc",m_storeGate);
//	if(sc.isFailure())
//	{
//		ATH_MSG_ERROR("reconstruction: unable to retrieve pointer to StoreGateSvc");
//		return sc;
//	}

//	if (StatusCode::SUCCESS!=service("DetectorStore",m_pDetStore))
//	{
//		ATH_MSG_ERROR("Detector store not found");
//		return StatusCode::FAILURE;
//	}

	//read ALFA_Geometry from StoreGate
	if (!detStore()->contains<ALFA_GeometryReader>(m_strKeyGeometryForReco))
	{
		ATH_MSG_ERROR("m_pGeometryReader is not in StoreGate");
	}
	else ATH_MSG_DEBUG("m_pGeometryReader is in StoreGate");

	if (StatusCode::SUCCESS != detStore()->retrieve(m_pGeometryReader, m_strKeyGeometryForReco))
	{
		ATH_MSG_ERROR("m_pGeometryReader: unable to retrieve data from SG");
	}
	else ATH_MSG_DEBUG("m_pGeometryReader: retrieved from SG");

	if (m_Config.eRPMetrologyGeoType==EMT_SWCORRECTIONS)
	{
		CHECK(AddCOOLFolderCallback("/FWD/ALFA/position_calibration"));
	}

	if(m_Config.eRPMetrologyGeoType==EMT_NOMINAL)
		SetNominalGeometry();

	// do update geometry at the end of the initialization process
	if (UpdateGeometryAtlas())
		ATH_MSG_DEBUG("Geometry updated successfully.");
	else
		ATH_MSG_FATAL("Unable to update a geometry!");

	ATH_MSG_DEBUG("end ALFA_LocRecCorr::initialize()");

	return StatusCode::SUCCESS;
}

StatusCode ALFA_LocRecCorr::execute()
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_LocRecCorr::Execute()");
	ATH_MSG_DEBUG("begin ALFA_LocRecCorr::Execute()");

	StatusCode sc = StatusCode::SUCCESS;

	sc = RecordCollection();
	if (sc.isFailure())
	{
		ATH_MSG_ERROR("ALFA_LocRecCorr recordCollection failed");
		return StatusCode::SUCCESS;
	}

	sc = RecordODCollection();
	if (sc.isFailure())
	{
		ATH_MSG_ERROR("ALFA_LocRecCorr recordODCollection failed");
		return StatusCode::SUCCESS;
	}

	int iRPot, iSide;
	int iAlgo;
	double fRecPosX = -9999.0, fRecPosY = -9999.0, fRecPosOD = -9999.0;
	HepGeom::Point3D<double> PointInDetCS;
	HepGeom::Point3D<double> PointInPotCS;
	HepGeom::Point3D<double> PointInStatCS;
	HepGeom::Point3D<double> PointInAtlasCS;

	const ALFA_LocRecEvCollection* pLocRecCorrCol = 0;
	const ALFA_LocRecODEvCollection* pLocRecCorrODCol = 0;

	sc = evtStore()->retrieve(pLocRecCorrCol, m_strLocRecCollectionName);
	if(sc.isFailure() || !pLocRecCorrCol)
	{
		ATH_MSG_ERROR("Container "<< m_strLocRecCollectionName <<" NOT FOUND !!!!!!!");
//		return StatusCode::FAILURE;
	}
	else
	{
		ALFA_LocRecEvCollection::const_iterator mcColBeg = pLocRecCorrCol->begin();
		ALFA_LocRecEvCollection::const_iterator mcColEnd = pLocRecCorrCol->end();

		//loop over collection (container) with hits (i.e. over 1 event)
		for(; mcColBeg!=mcColEnd; ++mcColBeg)
		{
			iAlgo       = (*mcColBeg)->getAlgoNum();
			iRPot		= (*mcColBeg)->getPotNum();
			fRecPosX	= (*mcColBeg)->getXposition();
			fRecPosY	= (*mcColBeg)->getYposition();
	
			PointInDetCS = HepGeom::Point3D<double>(fRecPosX, fRecPosY, 10.0);
			PointInAtlasCS = m_pGeometryReader->GetDetPointInAtlas((eRPotName)(iRPot+1), PointInDetCS);
			PointInPotCS = m_pGeometryReader->GetDetPointInRPot((eRPotName)(iRPot+1), PointInDetCS);
			PointInStatCS = m_TransMatrixSt[iRPot]*PointInDetCS;
	
			m_pLocRecCorrEvCollection->push_back(new ALFA_LocRecCorrEvent(iAlgo, iRPot, PointInAtlasCS[0], PointInAtlasCS[1], PointInAtlasCS[2], PointInPotCS[0], PointInPotCS[1], PointInStatCS[0], PointInStatCS[1], -9999.0, -9999.0));
		}
	}

	sc = evtStore()->retrieve(pLocRecCorrODCol, m_strLocRecODCollectionName);
	if(sc.isFailure() || !pLocRecCorrODCol)
	{
		ATH_MSG_ERROR("Container "<< m_strLocRecODCollectionName <<" NOT FOUND !!!!!!!");
//		return StatusCode::FAILURE;
	}
	else
	{
		ALFA_LocRecODEvCollection::const_iterator mcColODBeg = pLocRecCorrODCol->begin();
		ALFA_LocRecODEvCollection::const_iterator mcColODEnd = pLocRecCorrODCol->end();
	
		//loop over collection (container) with hits (i.e. over 1 event)
		Int_t iSign;
		for(; mcColODBeg!=mcColODEnd; ++mcColODBeg)
		{
			iAlgo       = (*mcColODBeg)->getAlgoNum();
			iRPot		= (*mcColODBeg)->getPotNum();
			iSide		= (*mcColODBeg)->getSide();
			fRecPosOD	= (*mcColODBeg)->getYposition();
	
			iSign = (iSide==0)? -1 : 1;
			PointInDetCS = HepGeom::Point3D<double>(iSign*22, fRecPosOD, 35.5);
			PointInAtlasCS = m_pGeometryReader->GetDetPointInAtlas((eRPotName)(iRPot+1), PointInDetCS);
			PointInPotCS = m_pGeometryReader->GetDetPointInRPot((eRPotName)(iRPot+1), PointInDetCS);
			PointInStatCS = m_TransMatrixSt[iRPot]*PointInDetCS;
			m_pLocRecCorrODEvCollection->push_back(new ALFA_LocRecCorrODEvent(iAlgo, iRPot, iSide, PointInAtlasCS[1], PointInAtlasCS[2], PointInPotCS[1], PointInStatCS[1], -9999.0));
		}
	}

	ATH_MSG_DEBUG("end ALFA_LocRecCorr::execute()");
	return StatusCode::SUCCESS;
}

StatusCode ALFA_LocRecCorr::finalize()
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_LocRecCorr::finalize()");
	ATH_MSG_DEBUG("begin ALFA_LocRecCorr::finalize()");

	StatusCode sc = StatusCode::SUCCESS;

	ATH_MSG_DEBUG("end ALFA_LocRecCorr::finalize()");

	return sc;
}



HepGeom::Transform3D ALFA_LocRecCorr::UserTransform3DInStation(eRPotName eRPName)
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_LocRecCorr::UserTransform3DInStation(eRPotName eRPName)");
	ATH_MSG_DEBUG("begin ALFA_LocRecCorr::UserTransform3DInStation()");

	CLHEP::HepRep3x3 matRotation;
	USERTRANSFORM structUserTransform;

	switch(eRPName)
	{
		case ERPN_B7L1U:
			structUserTransform.fAngle = m_vecTransformInStationB7L1U[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_vecTransformInStationB7L1U[1], m_vecTransformInStationB7L1U[2], m_vecTransformInStationB7L1U[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_vecTransformInStationB7L1U[4], m_vecTransformInStationB7L1U[5], m_vecTransformInStationB7L1U[6]);
			break;
		case ERPN_B7L1L:
			structUserTransform.fAngle = m_vecTransformInStationB7L1L[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_vecTransformInStationB7L1L[1], m_vecTransformInStationB7L1L[2], m_vecTransformInStationB7L1L[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_vecTransformInStationB7L1L[4], m_vecTransformInStationB7L1L[5], m_vecTransformInStationB7L1L[6]);
			break;
		case ERPN_A7L1U:
			structUserTransform.fAngle = m_vecTransformInStationA7L1U[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_vecTransformInStationA7L1U[1], m_vecTransformInStationA7L1U[2], m_vecTransformInStationA7L1U[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_vecTransformInStationA7L1U[4], m_vecTransformInStationA7L1U[5], m_vecTransformInStationA7L1U[6]);
			break;
		case ERPN_A7L1L:
			structUserTransform.fAngle = m_vecTransformInStationA7L1L[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_vecTransformInStationA7L1L[1], m_vecTransformInStationA7L1L[2], m_vecTransformInStationA7L1L[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_vecTransformInStationA7L1L[4], m_vecTransformInStationA7L1L[5], m_vecTransformInStationA7L1L[6]);
			break;
		case ERPN_A7R1U:
			structUserTransform.fAngle = m_vecTransformInStationA7R1U[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_vecTransformInStationA7R1U[1], m_vecTransformInStationA7R1U[2], m_vecTransformInStationA7R1U[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_vecTransformInStationA7R1U[4], m_vecTransformInStationA7R1U[5], m_vecTransformInStationA7R1U[6]);
			break;
		case ERPN_A7R1L:
			structUserTransform.fAngle = m_vecTransformInStationA7R1L[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_vecTransformInStationA7R1L[1], m_vecTransformInStationA7R1L[2], m_vecTransformInStationA7R1L[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_vecTransformInStationA7R1L[4], m_vecTransformInStationA7R1L[5], m_vecTransformInStationA7R1L[6]);
			break;
		case ERPN_B7R1U:
			structUserTransform.fAngle = m_vecTransformInStationB7R1U[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_vecTransformInStationB7R1U[1], m_vecTransformInStationB7R1U[2], m_vecTransformInStationB7R1U[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_vecTransformInStationB7R1U[4], m_vecTransformInStationB7R1U[5], m_vecTransformInStationB7R1U[6]);
			break;
		case ERPN_B7R1L:
			structUserTransform.fAngle = m_vecTransformInStationB7R1L[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_vecTransformInStationB7R1L[1], m_vecTransformInStationB7R1L[2], m_vecTransformInStationB7R1L[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_vecTransformInStationB7R1L[4], m_vecTransformInStationB7R1L[5], m_vecTransformInStationB7R1L[6]);
			break;
		default:
			ATH_MSG_WARNING("Unknown Roman pot, station transformation will be set to default (zero) values");
			structUserTransform.fAngle = 0.0;
			structUserTransform.vecRotation = CLHEP::Hep3Vector(0.0, 0.0, 0.0);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(0.0, 0.0, 0.0);
	}

	double fPhi = structUserTransform.fAngle;

	CLHEP::Hep3Vector vRotation = CLHEP::Hep3Vector(structUserTransform.vecRotation);
	CLHEP::Hep3Vector vTranslation = CLHEP::Hep3Vector(structUserTransform.vecTranslation);

	matRotation.xx_ = vRotation.x()*vRotation.x()*(1 - cos(fPhi)) + cos(fPhi);
	matRotation.xy_ = vRotation.x()*vRotation.y()*(1 - cos(fPhi)) - vRotation.z()*sin(fPhi);
	matRotation.xz_ = vRotation.x()*vRotation.z()*(1 - cos(fPhi)) + vRotation.y()*sin(fPhi);

	matRotation.yx_ = vRotation.y()*vRotation.x()*(1 - cos(fPhi)) + vRotation.z()*sin(fPhi);
	matRotation.yy_ = vRotation.y()*vRotation.y()*(1 - cos(fPhi)) + cos(fPhi);
	matRotation.yz_ = vRotation.y()*vRotation.z()*(1 - cos(fPhi)) - vRotation.x()*sin(fPhi);

	matRotation.zx_ = vRotation.z()*vRotation.x()*(1 - cos(fPhi)) - vRotation.y()*sin(fPhi);
	matRotation.zy_ = vRotation.z()*vRotation.y()*(1 - cos(fPhi)) + vRotation.x()*sin(fPhi);
	matRotation.zz_ = vRotation.z()*vRotation.z()*(1 - cos(fPhi)) + cos(fPhi);

	ATH_MSG_DEBUG("end ALFA_LocRecCorr::UserTransform3DInStation()");

	return HepGeom::Transform3D(CLHEP::HepRotation(matRotation), vTranslation);
}

HepGeom::Transform3D ALFA_LocRecCorr::UserTransform3DInDetector(eRPotName eRPName)
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_LocRecCorr::UserTransform3DInDetector(eRPotName eRPName)");
	ATH_MSG_DEBUG("begin ALFA_LocRecCorr::UserTransform3DInDetector()");

	CLHEP::HepRep3x3 matRotation;
	USERTRANSFORM structUserTransform;

	switch(eRPName)
	{
		case ERPN_B7L1U:
			structUserTransform.fAngle = m_vecTransformInDetectorB7L1U[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_vecTransformInDetectorB7L1U[1], m_vecTransformInDetectorB7L1U[2], m_vecTransformInDetectorB7L1U[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_vecTransformInDetectorB7L1U[4], m_vecTransformInDetectorB7L1U[5], m_vecTransformInDetectorB7L1U[6]);
			break;
		case ERPN_B7L1L:
			structUserTransform.fAngle = m_vecTransformInDetectorB7L1L[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_vecTransformInDetectorB7L1L[1], m_vecTransformInDetectorB7L1L[2], m_vecTransformInDetectorB7L1L[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_vecTransformInDetectorB7L1L[4], m_vecTransformInDetectorB7L1L[5], m_vecTransformInDetectorB7L1L[6]);
			break;
		case ERPN_A7L1U:
			structUserTransform.fAngle = m_vecTransformInDetectorA7L1U[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_vecTransformInDetectorA7L1U[1], m_vecTransformInDetectorA7L1U[2], m_vecTransformInDetectorA7L1U[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_vecTransformInDetectorA7L1U[4], m_vecTransformInDetectorA7L1U[5], m_vecTransformInDetectorA7L1U[6]);
			break;
		case ERPN_A7L1L:
			structUserTransform.fAngle = m_vecTransformInDetectorA7L1L[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_vecTransformInDetectorA7L1L[1], m_vecTransformInDetectorA7L1L[2], m_vecTransformInDetectorA7L1L[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_vecTransformInDetectorA7L1L[4], m_vecTransformInDetectorA7L1L[5], m_vecTransformInDetectorA7L1L[6]);
			break;
		case ERPN_A7R1U:
			structUserTransform.fAngle = m_vecTransformInDetectorA7R1U[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_vecTransformInDetectorA7R1U[1], m_vecTransformInDetectorA7R1U[2], m_vecTransformInDetectorA7R1U[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_vecTransformInDetectorA7R1U[4], m_vecTransformInDetectorA7R1U[5], m_vecTransformInDetectorA7R1U[6]);
			break;
		case ERPN_A7R1L:
			structUserTransform.fAngle = m_vecTransformInDetectorA7R1L[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_vecTransformInDetectorA7R1L[1], m_vecTransformInDetectorA7R1L[2], m_vecTransformInDetectorA7R1L[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_vecTransformInDetectorA7R1L[4], m_vecTransformInDetectorA7R1L[5], m_vecTransformInDetectorA7R1L[6]);
			break;
		case ERPN_B7R1U:
			structUserTransform.fAngle = m_vecTransformInDetectorB7R1U[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_vecTransformInDetectorB7R1U[1], m_vecTransformInDetectorB7R1U[2], m_vecTransformInDetectorB7R1U[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_vecTransformInDetectorB7R1U[4], m_vecTransformInDetectorB7R1U[5], m_vecTransformInDetectorB7R1U[6]);
			break;
		case ERPN_B7R1L:
			structUserTransform.fAngle = m_vecTransformInDetectorB7R1L[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_vecTransformInDetectorB7R1L[1], m_vecTransformInDetectorB7R1L[2], m_vecTransformInDetectorB7R1L[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_vecTransformInDetectorB7R1L[4], m_vecTransformInDetectorB7R1L[5], m_vecTransformInDetectorB7R1L[6]);
			break;
		default:
			ATH_MSG_WARNING("Unknown Roman pot, detector transformation will be set to default (zero) values");
			structUserTransform.fAngle = 0.0;
			structUserTransform.vecRotation = CLHEP::Hep3Vector(0.0, 0.0, 0.0);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(0.0, 0.0, 0.0);
	}

	double fPhi = structUserTransform.fAngle;

	CLHEP::Hep3Vector vRotation = CLHEP::Hep3Vector(structUserTransform.vecRotation);
	CLHEP::Hep3Vector vTranslation = CLHEP::Hep3Vector(structUserTransform.vecTranslation);

	matRotation.xx_ = vRotation.x()*vRotation.x()*(1 - cos(fPhi)) + cos(fPhi);
	matRotation.xy_ = vRotation.x()*vRotation.y()*(1 - cos(fPhi)) - vRotation.z()*sin(fPhi);
	matRotation.xz_ = vRotation.x()*vRotation.z()*(1 - cos(fPhi)) + vRotation.y()*sin(fPhi);

	matRotation.yx_ = vRotation.y()*vRotation.x()*(1 - cos(fPhi)) + vRotation.z()*sin(fPhi);
	matRotation.yy_ = vRotation.y()*vRotation.y()*(1 - cos(fPhi)) + cos(fPhi);
	matRotation.yz_ = vRotation.y()*vRotation.z()*(1 - cos(fPhi)) - vRotation.x()*sin(fPhi);

	matRotation.zx_ = vRotation.z()*vRotation.x()*(1 - cos(fPhi)) - vRotation.y()*sin(fPhi);
	matRotation.zy_ = vRotation.z()*vRotation.y()*(1 - cos(fPhi)) + vRotation.x()*sin(fPhi);
	matRotation.zz_ = vRotation.z()*vRotation.z()*(1 - cos(fPhi)) + cos(fPhi);

	ATH_MSG_DEBUG("end ALFA_LocRecCorr::UserTransform3DInDetector()");

	return HepGeom::Transform3D(CLHEP::HepRotation(matRotation), vTranslation);
}

HepGeom::Point3D<double> ALFA_LocRecCorr::Point3DInDetector(eRPotName eRPName)
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_LocRecCorr::Point3DInDetector(eRPotName eRPName)");
	ATH_MSG_DEBUG("begin ALFA_LocRecCorr::Point3DInDetector()");

	HepGeom::Point3D<double> Point;

	switch(eRPName)
	{
		case ERPN_B7L1U:
			Point = HepGeom::Point3D<double>(m_pointTransformInDetectorB7L1U[0], m_pointTransformInDetectorB7L1U[1], m_pointTransformInDetectorB7L1U[2]);
			break;
		case ERPN_B7L1L:
			Point = HepGeom::Point3D<double>(m_pointTransformInDetectorB7L1L[0], m_pointTransformInDetectorB7L1L[1], m_pointTransformInDetectorB7L1L[2]);
			break;
		case ERPN_A7L1U:
			Point = HepGeom::Point3D<double>(m_pointTransformInDetectorA7L1U[0], m_pointTransformInDetectorA7L1U[1], m_pointTransformInDetectorA7L1U[2]);
			break;
		case ERPN_A7L1L:
			Point = HepGeom::Point3D<double>(m_pointTransformInDetectorA7L1L[0], m_pointTransformInDetectorA7L1U[1], m_pointTransformInDetectorA7L1L[2]);
			break;
		case ERPN_A7R1U:
			Point = HepGeom::Point3D<double>(m_pointTransformInDetectorA7R1U[0], m_pointTransformInDetectorA7R1U[1], m_pointTransformInDetectorA7R1U[2]);
			break;
		case ERPN_A7R1L:
			Point = HepGeom::Point3D<double>(m_pointTransformInDetectorA7R1L[0], m_pointTransformInDetectorA7R1L[1], m_pointTransformInDetectorA7R1L[2]);
			break;
		case ERPN_B7R1U:
			Point = HepGeom::Point3D<double>(m_pointTransformInDetectorB7R1U[0], m_pointTransformInDetectorB7R1U[1], m_pointTransformInDetectorB7R1U[2]);
			break;
		case ERPN_B7R1L:
			Point = HepGeom::Point3D<double>(m_pointTransformInDetectorB7R1L[0], m_pointTransformInDetectorB7R1L[1], m_pointTransformInDetectorB7R1L[2]);
			break;
		default:
			ATH_MSG_WARNING("Unknown Roman pot, transformation point will be set to default (zero) value");
	}

	ATH_MSG_DEBUG("end ALFA_LocRecCorr::Point3DInDetector()");

	return Point;
}

StatusCode ALFA_LocRecCorr::RecordCollection()
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_LocRecCorr::RecordCollection()");
	ATH_MSG_DEBUG("begin ALFA_LocRecCorr::RecordCollection()");

	StatusCode sc = StatusCode::SUCCESS;

	m_pLocRecCorrEvCollection = new ALFA_LocRecCorrEvCollection();
	sc = evtStore()->record(m_pLocRecCorrEvCollection, m_strKeyLocRecCorrEvCollection);

	if (sc.isFailure())
	{
		ATH_MSG_FATAL("MD - Could not record the empty LocRecCorrEv collection in StoreGate");
		return sc;
	}
	else
	{
		ATH_MSG_DEBUG("MD - LocRecCorrEv collection was recorded in StoreGate");
	}

	ATH_MSG_DEBUG("end ALFA_LocRecCorr::RecordCollection()");

	return sc;
}

StatusCode ALFA_LocRecCorr::RecordODCollection()
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_LocRecCorr::RecordODCollection()");
	ATH_MSG_DEBUG("begin ALFA_LocRecCorr::RecordODCollection()");

	StatusCode sc = StatusCode::SUCCESS;

	m_pLocRecCorrODEvCollection = new ALFA_LocRecCorrODEvCollection();
	sc = evtStore()->record(m_pLocRecCorrODEvCollection, m_strKeyLocRecCorrODEvCollection);

	if (sc.isFailure())
	{
		ATH_MSG_FATAL("OD - Could not record the empty LocRecCorrEv collection in StoreGate");

		return sc;
	}
	else
	{
		 ATH_MSG_DEBUG("OD - LocRecCorrEv collection is recorded in StoreGate");
	}

	ATH_MSG_DEBUG("end ALFA_LocRecCorr::RecordODCollection()");

	return sc;
}

StatusCode ALFA_LocRecCorr::AddCOOLFolderCallback(const string& szFolder)
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_LocRecCorr::AddCOOLFolderCallback()");
	ATH_MSG_DEBUG("begin ALFA_LocRecCorr::AddCOOLFolderCallback()");

	StatusCode sc=StatusCode::FAILURE;

	const DataHandle<CondAttrListCollection> DataPtr;
	sc=detStore()->regFcn(&ALFA_LocRecCorr::COOLUpdate, this, DataPtr, szFolder, true);
	if(sc!=StatusCode::SUCCESS)
	{
		ATH_MSG_ERROR("Cannot register COOL callback for folder '"<<szFolder<<"'");
	}

	ATH_MSG_DEBUG("end ALFA_LocRecCorr::AddCOOLFolderCallback()");

	return sc;
}

StatusCode ALFA_LocRecCorr::COOLUpdate(IOVSVC_CALLBACK_ARGS_P(/*I*/, keys))
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_LocRecCorr::COOLUpdate()");
	ATH_MSG_DEBUG("begin ALFA_LocRecCorr::COOLUpdate()");

	int iChannel;
	StatusCode sc=StatusCode::SUCCESS;
	list<string>::const_iterator iter;
	const CondAttrListCollection* listAttrColl;
	CondAttrListCollection::const_iterator iterAttr;

	for(iter=keys.begin();iter!=keys.end();iter++)
	{
		if((*iter)=="/FWD/ALFA/position_calibration")
		{
			ATH_MSG_DEBUG(" IOV/COOL Notification '"<<"/FWD/ALFA/position_calibration"<<"'");

			if(detStore()->retrieve(listAttrColl,"/FWD/ALFA/position_calibration")==StatusCode::SUCCESS)
			{
				for(iterAttr=listAttrColl->begin();iterAttr!=listAttrColl->end();iterAttr++)
				{
					 iChannel=iterAttr->first; //RPot ID
					 m_Config.CfgRPosParams[iChannel].swcorr.fXOffset=((iterAttr->second)[0]).data<float>();
					 m_Config.CfgRPosParams[iChannel].swcorr.fTheta  =((iterAttr->second)[1]).data<float>();
					 m_Config.CfgRPosParams[iChannel].swcorr.fYOffset=((iterAttr->second)[2]).data<float>();

					 ATH_MSG_DEBUG("iChannel, fXOffset, fTheta, fYOffset = " << iChannel << ", " << m_Config.CfgRPosParams[iChannel].swcorr.fXOffset);
					 ATH_MSG_DEBUG(", " << m_Config.CfgRPosParams[iChannel].swcorr.fTheta << ", " << m_Config.CfgRPosParams[iChannel].swcorr.fYOffset);
				}

				// update SW corrections - needs to be updated during the COOL DB check
				if (UpdateGeometryAtlas())
				{
					ATH_MSG_DEBUG("Geometry for the SW corrections updated successfully");
				}
				else
				{
					ATH_MSG_FATAL("Unable to update a geometry of the SW corrections!");
					ATH_MSG_DEBUG("end ALFA_LocRecCorr::COOLUpdate()");
					return StatusCode::FAILURE;
				}
			}
			else
			{
				ATH_MSG_ERROR("The folder '"<<"/FWD/ALFA/position_calibration"<<"' not found");
				ATH_MSG_DEBUG("end ALFA_LocRecCorr::COOLUpdate()");
				return StatusCode::FAILURE;
			}
		}
	}

	ATH_MSG_DEBUG("end ALFA_LocRecCorr::COOLUpdate()");

	return sc;
}

bool ALFA_LocRecCorr::UpdateGeometryAtlas()
{
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_LocRecCorr::UpdateGeometryAtlas()");
	ATH_MSG_DEBUG("begin ALFA_LocRecCorr::UpdateGeometryAtlas()");

	if(m_pGeometryReader->Initialize(&m_Config,EFCS_ATLAS))
	{
		ATH_MSG_DEBUG("Geometry successfully updated.");
		m_pGeometryReader->GetListOfExistingRPotIDs(&m_ListExistingRPots);
	}
	else
	{
		ATH_MSG_FATAL("Cannot update geometry. Initialization process failed.");
		ATH_MSG_DEBUG("end ALFA_LocRecCorr::UpdateGeometryAtlas()");
		return false;
	}

	ATH_MSG_DEBUG("end ALFA_LocRecCorr::UpdateGeometryAtlas()");
	return true;
}

void ALFA_LocRecCorr::SetNominalGeometry()
{
	eRPotName eRPName;
	for(int i=0;i<RPOTSCNT;i++)
	{
		eRPName=(eRPotName)(i+1);
		if(m_bIsTransformInDetector[i]==true)
		{
			m_Config.CfgRPosParams[i].usercorr.bIsEnabledUserTranform=true;
			m_Config.CfgRPosParams[i].usercorr.UserOriginOfDetTransInRPot=Point3DInDetector(eRPName);
			m_Config.CfgRPosParams[i].usercorr.UserTransformOfDetInRPot=UserTransform3DInDetector(eRPName);
		}
		if(m_bIsTransformInStation[i]==true)
		{
			m_Config.CfgRPosParams[i].usercorr.bIsEnabledUserTranform=true;
			m_Config.CfgRPosParams[i].usercorr.UserTransformOfRPInStation=UserTransform3DInStation(eRPName);
		}
	}
}

