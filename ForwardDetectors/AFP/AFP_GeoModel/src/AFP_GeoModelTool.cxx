/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "AFP_GeoModel/AFP_GeoModelTool.h"
#include "AFP_GeoModel/AFP_GeoModelFactory.h"
#include "AFP_GeoModel/AFP_GeoModelManager.h"

/**
 ** Constructor(s)
 **/
AFP_GeoModelTool::AFP_GeoModelTool( const std::string& type, const std::string& name, const IInterface* parent )
	: GeoModelTool( type, name, parent ), m_pAFPDetectorFactory(NULL), m_iovSvc( "IOVDbSvc", name )
{
	m_CfgParams.clear();
	m_pGeometry=NULL;

	arrAFP00XStaggering.clear(); arrAFP00YStaggering.clear();
	arrAFP01XStaggering.clear(); arrAFP01YStaggering.clear();
	arrAFP02XStaggering.clear(); arrAFP02YStaggering.clear();
	arrAFP03XStaggering.clear(); arrAFP03YStaggering.clear();


	declareProperty("MISC_VP1Mode",m_CfgParams.bVP1Mode=false);

	//Properties of Hamburg Beam Pipes
	declareProperty("HB_windowPlateThickness",m_CfgParams.hbpcfg.windowPlateThickness=0.3);
	declareProperty("HB_windowPlateAngle",m_CfgParams.hbpcfg.windowPlateAngle=90);
	declareProperty("HB_windowPlatesInsteadOfHB",m_CfgParams.hbpcfg.windowPlatesInsteadOfHB=false);
        declareProperty("HB_setMaterialToBeryllium",m_CfgParams.hbpcfg.setMaterialToBeryllium=false);

	//Properties of SID
	declareProperty("SID_Slope",m_CfgParams.sidcfg.fSlope=NOMINALSIDSLOPE);
	declareProperty("SID_NumberOfLayers",m_CfgParams.sidcfg.fLayerCount=SIDCNT);
	declareProperty("SID_SpacingBetweenLayers",m_CfgParams.sidcfg.fLayerSpacing=NOMINALSIDSPACING);
	declareProperty("SID_DistanceToFloor",m_CfgParams.sidcfg.fDistanceFromPocket=NOMINALSIDPOCKETDISTANCE);
	declareProperty("SID_AddVacuumSensors",m_CfgParams.sidcfg.bAddVacuumSensors=false);

	declareProperty("SID_AFP00XStaggering",arrAFP00XStaggering);
	declareProperty("SID_AFP00YStaggering",arrAFP00YStaggering);
	declareProperty("SID_AFP01XStaggering",arrAFP01XStaggering);
	declareProperty("SID_AFP01YStaggering",arrAFP01YStaggering);
	declareProperty("SID_AFP02XStaggering",arrAFP02XStaggering);
	declareProperty("SID_AFP02YStaggering",arrAFP02YStaggering);
	declareProperty("SID_AFP03XStaggering",arrAFP03XStaggering);
	declareProperty("SID_AFP03YStaggering",arrAFP03YStaggering);

	//Properties of TD
	declareProperty("TD_AddCover",m_CfgParams.tdcfg.bAddCover=false);

	declareProperty("TD_Q1_TrainsCnt",m_CfgParams.tdcfg.QuarticConf[0].nRowsCnt=TDQUARTICROWSCNT);
	declareProperty("TD_Q1_ColsCnt",m_CfgParams.tdcfg.QuarticConf[0].nColsCnt=TDQUARTICCOLSCNT);
	declareProperty("TD_Q1_QuarticLength",m_CfgParams.tdcfg.QuarticConf[0].fQuarticLength=TDQUARTICLENGTH);
	declareProperty("TD_Q1_Slope",m_CfgParams.tdcfg.QuarticConf[0].fSlope=NOMINALTDSLOPE);
	declareProperty("TD_Q1_CoverThickness",m_CfgParams.tdcfg.QuarticConf[0].fCoverThickness=0.0);
	declareProperty("TD_Q1_CoverDistanceToFloor",m_CfgParams.tdcfg.QuarticConf[0].fCoverDistanceToFloor=NOMINALTDPOCKETDISTANCE);
	declareProperty("TD_Q1_BarXDims",m_CfgParams.tdcfg.QuarticConf[0].vecBarXDim);
	declareProperty("TD_Q1_BarZDims",m_CfgParams.tdcfg.QuarticConf[0].vecBarZDim);
	declareProperty("TD_Q1_XGaps",m_CfgParams.tdcfg.QuarticConf[0].vecXGaps);
	declareProperty("TD_Q1_ZGaps",m_CfgParams.tdcfg.QuarticConf[0].vecZGaps);

	declareProperty("TD_Q2_TrainsCnt",m_CfgParams.tdcfg.QuarticConf[1].nRowsCnt=TDQUARTICROWSCNT);
	declareProperty("TD_Q2_ColsCnt",m_CfgParams.tdcfg.QuarticConf[1].nColsCnt=TDQUARTICCOLSCNT);
	declareProperty("TD_Q2_QuarticLength",m_CfgParams.tdcfg.QuarticConf[1].fQuarticLength=TDQUARTICLENGTH);
	declareProperty("TD_Q2_Slope",m_CfgParams.tdcfg.QuarticConf[1].fSlope=NOMINALTDSLOPE);
	declareProperty("TD_Q2_CoverThickness",m_CfgParams.tdcfg.QuarticConf[1].fCoverThickness=0.0);
	declareProperty("TD_Q2_CoverDistanceToFloor",m_CfgParams.tdcfg.QuarticConf[1].fCoverDistanceToFloor=NOMINALTDPOCKETDISTANCE+m_CfgParams.tdcfg.QuarticConf[0].vecBarXDim[0]);
	declareProperty("TD_Q2_BarXDims",m_CfgParams.tdcfg.QuarticConf[1].vecBarXDim);
	declareProperty("TD_Q2_BarZDims",m_CfgParams.tdcfg.QuarticConf[1].vecBarZDim);
	declareProperty("TD_Q2_XGaps",m_CfgParams.tdcfg.QuarticConf[1].vecXGaps);
	declareProperty("TD_Q2_ZGaps",m_CfgParams.tdcfg.QuarticConf[1].vecZGaps);

	//Properties of stations
	declareProperty("ST_ShortHBFloorXPos",m_CfgParams.fShortHBFloorXPos=SHORTHBFLOORXPOS);
	declareProperty("ST_ShortHBYPos",m_CfgParams.fShortHBYPos=STATIONSHIFTINYAXIS);
	declareProperty("ST_ShortHBZPos",m_CfgParams.fShortHBZPos=SHORTHBPZPOS);
	declareProperty("ST_LongHBFloorXPos",m_CfgParams.fLongHBFloorXPos=LONGHBFLOORXPOS);
	declareProperty("ST_LongHBYPos",m_CfgParams.fLongHBYPos=STATIONSHIFTINYAXIS);
	declareProperty("ST_LongHBZPos",m_CfgParams.fLongHBZPos=LONGHBPZPOS);
	declareProperty("ST_IsSHBRomanPotMode",m_CfgParams.bIsSHBRPotMode=false);
	declareProperty("ST_AddSecondQuartic",m_CfgParams.bAddSecondQuartic=true);
	declareProperty("ST_Q1_QuarticYPosInTube",m_CfgParams.fQuarticYPosInTube[0]=QUARTICYPOSINLHB);
	declareProperty("ST_Q2_QuarticYPosInTube",m_CfgParams.fQuarticYPosInTube[1]=QUARTICYPOSINLHB);
	declareProperty("ST_Q1_QuarticZPosInTube",m_CfgParams.fQuarticZPosInTube[0]=QUARTIC1ZPOSINLHB);
	declareProperty("ST_Q2_QuarticZPosInTube",m_CfgParams.fQuarticZPosInTube[1]=QUARTIC2ZPOSINLHB);

}

/**
 ** Destructor
 **/
AFP_GeoModelTool::~AFP_GeoModelTool()
{
	// This will need to be modified once we register the  DetectorNode in
	// the Transient Detector Store
	if(m_detector!=NULL) {
		delete m_detector;
		m_detector=NULL;
	}

	if(m_pGeometry!=NULL){
		delete m_pGeometry;
		m_pGeometry=NULL;
	}
}

StatusCode AFP_GeoModelTool::CheckPropertiesSettings()
{
	bool bRes=true;

	MsgStream LogStream(Athena::getMessageSvc(), "AFP_GeoModelTool::CheckPropertiesSettings");

	if(arrAFP00XStaggering.size()>0){
		if(arrAFP00XStaggering.size()==m_CfgParams.sidcfg.fLayerCount){
			m_CfgParams.sidcfg.mapXStaggering["AFP00"]=arrAFP00XStaggering;
		}
		else{
			LogStream<<MSG::ERROR<<"Mismatch between SID_AFP01XStaggering and number of plates (SID_NumberOfLayers)"<<endreq;
			bRes=false;
		}
	}
	if(arrAFP00YStaggering.size()>0){
		if(arrAFP00YStaggering.size()==m_CfgParams.sidcfg.fLayerCount){
			m_CfgParams.sidcfg.mapYStaggering["AFP00"]=arrAFP00YStaggering;
		}
		else{
			LogStream<<MSG::ERROR<<"Mismatch between SID_AFP00YStaggering and number of plates (SID_NumberOfLayers)"<<endreq;
			bRes=false;
		}
	}

	if(arrAFP01XStaggering.size()>0){
		if(arrAFP01XStaggering.size()==m_CfgParams.sidcfg.fLayerCount){
			m_CfgParams.sidcfg.mapXStaggering["AFP01"]=arrAFP01XStaggering;
		}
		else{
			LogStream<<MSG::ERROR<<"Mismatch between SID_AFP01XStaggering and number of plates (SID_NumberOfLayers)"<<endreq;
			bRes=false;
		}
	}
	if(arrAFP01YStaggering.size()>0){
		if(arrAFP01YStaggering.size()==m_CfgParams.sidcfg.fLayerCount){
			m_CfgParams.sidcfg.mapYStaggering["AFP01"]=arrAFP01YStaggering;
		}
		else{
			LogStream<<MSG::ERROR<<"Mismatch between SID_AFP01YStaggering and number of plates (SID_NumberOfLayers)"<<endreq;
			bRes=false;
		}
	}

	if(arrAFP02XStaggering.size()>0){
		if(arrAFP02XStaggering.size()==m_CfgParams.sidcfg.fLayerCount){
			m_CfgParams.sidcfg.mapXStaggering["AFP02"]=arrAFP02XStaggering;
		}
		else{
			LogStream<<MSG::ERROR<<"Mismatch between SID_AFP02XStaggering and number of plates (SID_NumberOfLayers)"<<endreq;
			bRes=false;
		}
	}
	if(arrAFP02YStaggering.size()>0){
		if(arrAFP02YStaggering.size()==m_CfgParams.sidcfg.fLayerCount){
			m_CfgParams.sidcfg.mapYStaggering["AFP02"]=arrAFP02YStaggering;
		}
		else{
			LogStream<<MSG::ERROR<<"Mismatch between SID_AFP02YStaggering and number of plates (SID_NumberOfLayers)"<<endreq;
			bRes=false;
		}
	}

	if(arrAFP03XStaggering.size()>0){
		if(arrAFP03XStaggering.size()==m_CfgParams.sidcfg.fLayerCount){
			m_CfgParams.sidcfg.mapXStaggering["AFP03"]=arrAFP03XStaggering;
		}
		else{
			LogStream<<MSG::ERROR<<"Mismatch between SID_AFP03XStaggering and number of plates (SID_NumberOfLayers)"<<endreq;
			bRes=false;
		}
	}
	if(arrAFP03YStaggering.size()>0){
		if(arrAFP03YStaggering.size()==m_CfgParams.sidcfg.fLayerCount){
			m_CfgParams.sidcfg.mapYStaggering["AFP03"]=arrAFP03YStaggering;
		}
		else{
			LogStream<<MSG::ERROR<<"Mismatch between SID_AFP03YStaggering and number of plates (SID_NumberOfLayers)"<<endreq;
			bRes=false;
		}
	}

#ifdef USE_TDLBARS
#else
	if(m_CfgParams.tdcfg.bAddCover){
		if(m_CfgParams.tdcfg.QuarticConf[0].fCoverThickness<((1E-4)*CLHEP::mm)) m_CfgParams.tdcfg.QuarticConf[0].fCoverThickness=TDCOVERTHICKNESS;
		if(m_CfgParams.tdcfg.QuarticConf[1].fCoverThickness<((1E-4)*CLHEP::mm)) m_CfgParams.tdcfg.QuarticConf[1].fCoverThickness=TDCOVERTHICKNESS;
	}
	else{
		m_CfgParams.tdcfg.QuarticConf[0].fCoverThickness=0.0;
		m_CfgParams.tdcfg.QuarticConf[1].fCoverThickness=0.0;
	}

	if(m_CfgParams.tdcfg.QuarticConf[0].vecBarXDim.size()!=(unsigned int)(m_CfgParams.tdcfg.QuarticConf[0].nRowsCnt)){
		LogStream<<MSG::ERROR<<"Mismatch between TD_BarXDims and TD_TrainsCnt (quartic 1)"<<endreq;
		bRes=false;
	}
	if(m_CfgParams.tdcfg.QuarticConf[0].vecXGaps.size()!=(unsigned int)(m_CfgParams.tdcfg.QuarticConf[0].nRowsCnt-1)){
		LogStream<<MSG::ERROR<<"Mismatch between TD_XGaps and TD_TrainsCnt (quartic 1)"<<endreq;
		bRes=false;
	}
	if(m_CfgParams.tdcfg.QuarticConf[0].vecBarZDim.size()!=(unsigned int)(m_CfgParams.tdcfg.QuarticConf[0].nColsCnt)){
		LogStream<<MSG::ERROR<<"Mismatch between TD_BarZDims and TD_ColsCnt (quartic 1)"<<endreq;
		bRes=false;
	}
	if(m_CfgParams.tdcfg.QuarticConf[0].vecZGaps.size()!=(unsigned int)(m_CfgParams.tdcfg.QuarticConf[0].nColsCnt-1)){
		LogStream<<MSG::ERROR<<"Mismatch between TD_ZGaps and TD_ColsCnt (quartic 1)"<<endreq;
		bRes=false;
	}


	if(m_CfgParams.tdcfg.QuarticConf[1].vecBarXDim.size()!=(unsigned int)(m_CfgParams.tdcfg.QuarticConf[1].nRowsCnt)){
		LogStream<<MSG::ERROR<<"Mismatch between TD_BarXDims and TD_TrainsCnt (quartic 2)"<<endreq;
		bRes=false;
	}
	if(m_CfgParams.tdcfg.QuarticConf[1].vecXGaps.size()!=(unsigned int)(m_CfgParams.tdcfg.QuarticConf[1].nRowsCnt-1)){
		LogStream<<MSG::ERROR<<"Mismatch between TD_XGaps and TD_TrainsCnt (quartic 2)"<<endreq;
		bRes=false;
	}
	if(m_CfgParams.tdcfg.QuarticConf[1].vecBarZDim.size()!=(unsigned int)(m_CfgParams.tdcfg.QuarticConf[1].nColsCnt)){
		LogStream<<MSG::ERROR<<"Mismatch between TD_BarZDims and TD_ColsCnt (quartic 2)"<<endreq;
		bRes=false;
	}
	if(m_CfgParams.tdcfg.QuarticConf[1].vecZGaps.size()!=(unsigned int)(m_CfgParams.tdcfg.QuarticConf[1].nColsCnt-1)){
		LogStream<<MSG::ERROR<<"Mismatch between TD_ZGaps and TD_ColsCnt (quartic 2)"<<endreq;
		bRes=false;
	}
#endif

	return bRes? StatusCode::SUCCESS:StatusCode::FAILURE;
}


StatusCode AFP_GeoModelTool::create( StoreGateSvc* detStore )
{ 
	MsgStream log(msgSvc(), name());
	//CHECK(m_iovSvc.retrieve()); //-- REMOVE THIS WHEN IOVSVC IS TO BE USED

	//
	// Locate the top level experiment node
	//
	DataHandle<GeoModelExperiment> theExpt;
	StatusCode sc = detStore->retrieve( theExpt, "ATLAS" );
	if (StatusCode::SUCCESS != sc)
	{
		log << MSG::ERROR<< "Could not find GeoModelExperiment ATLAS"<< endreq;
		return (StatusCode::FAILURE);
	}

	CHECK(CheckPropertiesSettings());

	m_pGeometry=new AFP_Geometry(&m_CfgParams);
	m_pAFPDetectorFactory=new AFP_GeoModelFactory(detStore, m_pGeometry);

	HepGeom::Point3D<double> LocalPoint;
	HepGeom::Point3D<double> GlobalPoint(-102.164*CLHEP::mm,-0.0438429*CLHEP::mm,-204473*CLHEP::mm);
	CHECK(m_pGeometry->GetPointInSIDSensorLocalCS(3,0,GlobalPoint,LocalPoint));
	GlobalPoint=HepGeom::Point3D<double>(-102.164*CLHEP::mm,-0.0438429*CLHEP::mm,-204483*CLHEP::mm);
	CHECK(m_pGeometry->GetPointInSIDSensorLocalCS(3,1,GlobalPoint,LocalPoint));


	if ( 0 == m_detector ) {
		// Create the DetectorNode instance
		try {
			//
			// This strange way of casting is to avoid an
			// utterly brain damaged compiler warning.
			//
			GeoPhysVol *world=&*theExpt->getPhysVol();


			m_pAFPDetectorFactory->create(world);
			//theFactory.create(world);
		} catch (std::bad_alloc) {
			log << MSG::FATAL << "Could not create new DetectorNode!" << endreq;
			return StatusCode::FAILURE;
		}

		// Register the DetectorNode instance with the Transient Detector Store
		theExpt->addManager(m_pAFPDetectorFactory->getDetectorManager());
		sc = detStore->record(m_pAFPDetectorFactory->getDetectorManager(),m_pAFPDetectorFactory->getDetectorManager()->getName());
		//theExpt->addManager(theFactory.getDetectorManager());
		//sc = detStore->record(theFactory.getDetectorManager(),theFactory.getDetectorManager()->getName());
		if (StatusCode::SUCCESS != sc) {
			log << MSG::ERROR << "Could not register DetectorNode" << endreq;
			return (StatusCode::FAILURE);
		}

		return StatusCode::SUCCESS;
	}

	return StatusCode::FAILURE;
}

/*
StatusCode AFP_GeoModelTool::registerCallback(StoreGateSvc* )
{

	StatusCode sc=StatusCode::FAILURE;


	const DataHandle<CondAttrListCollection> DataPtr;
	sc=detStore->regFcn(&IGeoModelTool::align,dynamic_cast<IGeoModelTool*>(this), DataPtr, COOLFOLDER_BPM, true);
	if(sc!=StatusCode::SUCCESS){
		msg(MSG::ERROR) << "Cannot register COOL callback for folder '"<< COOLFOLDER_BPM <<"'" << endreq;
	}
	else{
		msg(MSG::INFO) << "Call-back to ALFA_DetectorTool::align() against folder "<< COOLFOLDER_BPM <<" registered "<<endreq;
	}

	return sc;

	return StatusCode::FAILURE;
}*/

/*
StatusCode AFP_GeoModelTool::align(IOVSVC_CALLBACK_ARGS)
{
	StatusCode sc=StatusCode::SUCCESS;
	AFP_BPMCOOLPARAMS BpmParams;
	const AthenaAttributeList* pAttrList=NULL;
	//const CondAttrListCollection* listAttrColl;
	//CondAttrListCollection::const_iterator iterAttr;

	if(detStore()->retrieve(pAttrList,COOLFOLDER_BPM )==StatusCode::SUCCESS)
	{
		BpmParams.fBpmMRRX=(*pAttrList)["bpmr_r_x_pos"].data<float>();
		BpmParams.fBpmMRRY=(*pAttrList)["bpmr_r_y_pos"].data<float>();
		BpmParams.fBpmMRLX=(*pAttrList)["bpmr_l_x_pos"].data<float>();
		BpmParams.fBpmMRLY=(*pAttrList)["bpmr_l_y_pos"].data<float>();
		BpmParams.fBpmSARX=(*pAttrList)["bpmsa_r_x_pos"].data<float>();
		BpmParams.fBpmSARY=(*pAttrList)["bpmsa_r_y_pos"].data<float>();
		BpmParams.fBpmSALX=(*pAttrList)["bpmsa_l_x_pos"].data<float>();
		BpmParams.fBpmSALY=(*pAttrList)["bpmsa_l_y_pos"].data<float>();
		BpmParams.fBpmWBRX=(*pAttrList)["bpmwb_r_x_pos"].data<float>();
		BpmParams.fBpmWBRY=(*pAttrList)["bpmwb_r_y_pos"].data<float>();
		BpmParams.fBpmYALX=(*pAttrList)["bpmya_l_x_pos"].data<float>();
		BpmParams.fBpmYALY=(*pAttrList)["bpmya_l_y_pos"].data<float>();

		m_pAFPDetectorFactory->UpdatePositions(&BpmParams);
	}
	else{
		msg(MSG::ERROR) << "Folder '"<<"/FWD/ALFA/position_calibration"<<"' not found" << endreq;
		sc=StatusCode::FAILURE;
	}

	return sc;
}
*/
