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

#include "AFP_GeoModelTool.h"
#include "AFP_GeoModelFactory.h"
#include "AFP_GeoModel/AFP_GeoModelManager.h"

/**
 ** Constructor(s)
 **/
AFP_GeoModelTool::AFP_GeoModelTool( const std::string& type, const std::string& name, const IInterface* parent )
    : GeoModelTool( type, name, parent ), m_pAFPDetectorFactory(NULL), m_iovSvc( "IOVDbSvc", name )
{
    m_CfgParams.clear();
    m_pGeometry=NULL;

    m_vecAFP00XStaggering.clear(); m_vecAFP00YStaggering.clear();
    m_vecAFP01XStaggering.clear(); m_vecAFP01YStaggering.clear();
    m_vecAFP02XStaggering.clear(); m_vecAFP02YStaggering.clear();
    m_vecAFP03XStaggering.clear(); m_vecAFP03YStaggering.clear();

    //Properties of Hamburg Beam Pipes
    declareProperty("HB_windowPlateThickness",m_CfgParams.hbpcfg.windowPlateThickness=0.3);
    declareProperty("HB_windowPlateAngle",m_CfgParams.hbpcfg.windowPlateAngle=90);
    declareProperty("HB_windowPlatesInsteadOfHB",m_CfgParams.hbpcfg.windowPlatesInsteadOfHB=false);
    declareProperty("HB_setMaterialToBeryllium",m_CfgParams.hbpcfg.setMaterialToBeryllium=false);

    //Properties of SID
    declareProperty("SID_Slope",m_CfgParams.sidcfg.fSlope=SID_NOMINALSLOPE);
    declareProperty("SID_NumberOfLayers",m_CfgParams.sidcfg.fLayerCount=SIDCNT);
    declareProperty("SID_SpacingBetweenLayers",m_CfgParams.sidcfg.fLayerSpacing=SID_NOMINALSPACING);
    declareProperty("SID_DistanceToFloor",m_CfgParams.sidcfg.fXFloorDistance=SID_DISTANCETOFLOOR);
    declareProperty("SID_ZDistanceInRPot",m_CfgParams.sidcfg.fZDistanceInRPot=SID_ZDISTANCEINRPOT);
    declareProperty("SID_AddVacuumSensors",m_CfgParams.sidcfg.bAddVacuumSensors=false);
	declareProperty("SID_SupportThickness",m_CfgParams.sidcfg.fSupportThickness=SID_PLATETHICKNESS);

    declareProperty("SID_AFP00XStaggering",m_vecAFP00XStaggering);
    declareProperty("SID_AFP00YStaggering",m_vecAFP00YStaggering);
    declareProperty("SID_AFP01XStaggering",m_vecAFP01XStaggering);
    declareProperty("SID_AFP01YStaggering",m_vecAFP01YStaggering);
    declareProperty("SID_AFP02XStaggering",m_vecAFP02XStaggering);
    declareProperty("SID_AFP02YStaggering",m_vecAFP02YStaggering);
    declareProperty("SID_AFP03XStaggering",m_vecAFP03XStaggering);
    declareProperty("SID_AFP03YStaggering",m_vecAFP03YStaggering);

    //Properties of TD
    declareProperty("TD_DistanceToFloor",m_CfgParams.tdcfg.fXFloorDistance=TD_DISTANCETOFLOOR);
    declareProperty("TD_YPosInRPot",m_CfgParams.tdcfg.fYPosInRPot=0.0*CLHEP::mm);
    declareProperty("TD_ZDistanceInRPot",m_CfgParams.tdcfg.fZPosInRPot=TD_ZDISTANCEINRPOT);
    declareProperty("TD_VertArmXGap",m_CfgParams.tdcfg.fVertXGap=TD_VERTXGAP);
    declareProperty("TD_VertArmZGaps",m_CfgParams.tdcfg.fVertZGap=TD_VERTZGAP);
    declareProperty("TD_HorzArmYGaps",m_CfgParams.tdcfg.fHorzYGap=TD_HORZYGAP);

    //Properties of stations
    declareProperty("AFP00_RPotFloorDistance",m_CfgParams.vecRPotFloorDistance[0]=RPFLOORDISTANCE);
    declareProperty("AFP01_RPotFloorDistance",m_CfgParams.vecRPotFloorDistance[1]=RPFLOORDISTANCE);
    declareProperty("AFP02_RPotFloorDistance",m_CfgParams.vecRPotFloorDistance[2]=RPFLOORDISTANCE);
    declareProperty("AFP03_RPotFloorDistance",m_CfgParams.vecRPotFloorDistance[3]=RPFLOORDISTANCE);

    declareProperty("AFP00_RPotYPos",m_CfgParams.vecRPotYPos[0]=STATIONSHIFTINYAXIS);
    declareProperty("AFP01_RPotYPos",m_CfgParams.vecRPotYPos[1]=STATIONSHIFTINYAXIS);
    declareProperty("AFP02_RPotYPos",m_CfgParams.vecRPotYPos[2]=STATIONSHIFTINYAXIS);
    declareProperty("AFP03_RPotYPos",m_CfgParams.vecRPotYPos[3]=STATIONSHIFTINYAXIS);

    declareProperty("AFP00_ZPos",m_CfgParams.vecStatNominalZPos[0]=OUTERSTATZDISTANCE);
    declareProperty("AFP01_ZPos",m_CfgParams.vecStatNominalZPos[1]=INNERSTATZDISTANCE);
    declareProperty("AFP02_ZPos",m_CfgParams.vecStatNominalZPos[2]=-INNERSTATZDISTANCE);
    declareProperty("AFP03_ZPos",m_CfgParams.vecStatNominalZPos[3]=-OUTERSTATZDISTANCE);
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

        if(m_vecAFP00XStaggering.size()>0){
                if(m_vecAFP00XStaggering.size()==m_CfgParams.sidcfg.fLayerCount){
                        m_CfgParams.sidcfg.mapXStaggering["AFP00"]=m_vecAFP00XStaggering;
		}
		else{
			LogStream<<MSG::ERROR<<"Mismatch between SID_AFP01XStaggering and number of plates (SID_NumberOfLayers)"<<endmsg;
			bRes=false;
		}
	}
        if(m_vecAFP00YStaggering.size()>0){
                if(m_vecAFP00YStaggering.size()==m_CfgParams.sidcfg.fLayerCount){
                        m_CfgParams.sidcfg.mapYStaggering["AFP00"]=m_vecAFP00YStaggering;
		}
		else{
			LogStream<<MSG::ERROR<<"Mismatch between SID_AFP00YStaggering and number of plates (SID_NumberOfLayers)"<<endmsg;
			bRes=false;
		}
	}

        if(m_vecAFP01XStaggering.size()>0){
                if(m_vecAFP01XStaggering.size()==m_CfgParams.sidcfg.fLayerCount){
                        m_CfgParams.sidcfg.mapXStaggering["AFP01"]=m_vecAFP01XStaggering;
		}
		else{
			LogStream<<MSG::ERROR<<"Mismatch between SID_AFP01XStaggering and number of plates (SID_NumberOfLayers)"<<endmsg;
			bRes=false;
		}
	}
        if(m_vecAFP01YStaggering.size()>0){
                if(m_vecAFP01YStaggering.size()==m_CfgParams.sidcfg.fLayerCount){
                        m_CfgParams.sidcfg.mapYStaggering["AFP01"]=m_vecAFP01YStaggering;
		}
		else{
			LogStream<<MSG::ERROR<<"Mismatch between SID_AFP01YStaggering and number of plates (SID_NumberOfLayers)"<<endmsg;
			bRes=false;
		}
	}

        if(m_vecAFP02XStaggering.size()>0){
                if(m_vecAFP02XStaggering.size()==m_CfgParams.sidcfg.fLayerCount){
                        m_CfgParams.sidcfg.mapXStaggering["AFP02"]=m_vecAFP02XStaggering;
		}
		else{
			LogStream<<MSG::ERROR<<"Mismatch between SID_AFP02XStaggering and number of plates (SID_NumberOfLayers)"<<endmsg;
			bRes=false;
		}
	}
        if(m_vecAFP02YStaggering.size()>0){
                if(m_vecAFP02YStaggering.size()==m_CfgParams.sidcfg.fLayerCount){
                        m_CfgParams.sidcfg.mapYStaggering["AFP02"]=m_vecAFP02YStaggering;
		}
		else{
			LogStream<<MSG::ERROR<<"Mismatch between SID_AFP02YStaggering and number of plates (SID_NumberOfLayers)"<<endmsg;
			bRes=false;
		}
	}

        if(m_vecAFP03XStaggering.size()>0){
                if(m_vecAFP03XStaggering.size()==m_CfgParams.sidcfg.fLayerCount){
                        m_CfgParams.sidcfg.mapXStaggering["AFP03"]=m_vecAFP03XStaggering;
		}
		else{
			LogStream<<MSG::ERROR<<"Mismatch between SID_AFP03XStaggering and number of plates (SID_NumberOfLayers)"<<endmsg;
			bRes=false;
		}
	}
        if(m_vecAFP03YStaggering.size()>0){
                if(m_vecAFP03YStaggering.size()==m_CfgParams.sidcfg.fLayerCount){
                        m_CfgParams.sidcfg.mapYStaggering["AFP03"]=m_vecAFP03YStaggering;
		}
		else{
			LogStream<<MSG::ERROR<<"Mismatch between SID_AFP03YStaggering and number of plates (SID_NumberOfLayers)"<<endmsg;
			bRes=false;
		}
	}

	return bRes? StatusCode::SUCCESS:StatusCode::FAILURE;
}

StatusCode AFP_GeoModelTool::create()
{ 
    MsgStream log(msgSvc(), name());
    //CHECK(m_iovSvc.retrieve()); //-- REMOVE THIS WHEN IOVSVC IS TO BE USED

    //
    // Locate the top level experiment node
    //
    DataHandle<GeoModelExperiment> theExpt;
    StatusCode sc = detStore()->retrieve( theExpt, "ATLAS" );
    if (StatusCode::SUCCESS != sc)
    {
        log << MSG::ERROR<< "Could not find GeoModelExperiment ATLAS"<< endmsg;
        return (StatusCode::FAILURE);
    }

    CHECK(CheckPropertiesSettings());

    m_pGeometry=new AFP_Geometry(&m_CfgParams);
    m_pAFPDetectorFactory=new AFP_GeoModelFactory(detStore().operator->(), m_pGeometry);

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
        } catch (const std::bad_alloc&) {
            log << MSG::FATAL << "Could not create new DetectorNode!" << endmsg;
            return StatusCode::FAILURE;
        }

        // Register the DetectorNode instance with the Transient Detector Store
        theExpt->addManager(m_pAFPDetectorFactory->getDetectorManager());
        sc = detStore()->record(m_pAFPDetectorFactory->getDetectorManager(),m_pAFPDetectorFactory->getDetectorManager()->getName());
        //theExpt->addManager(theFactory.getDetectorManager());
        //sc = detStore->record(theFactory.getDetectorManager(),theFactory.getDetectorManager()->getName());
        if (StatusCode::SUCCESS != sc) {
            log << MSG::ERROR << "Could not register DetectorNode" << endmsg;
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
		msg(MSG::ERROR) << "Cannot register COOL callback for folder '"<< COOLFOLDER_BPM <<"'" << endmsg;
	}
	else{
		msg(MSG::INFO) << "Call-back to ALFA_DetectorTool::align() against folder "<< COOLFOLDER_BPM <<" registered "<<endmsg;
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
		msg(MSG::ERROR) << "Folder '"<<"/FWD/ALFA/position_calibration"<<"' not found" << endmsg;
		sc=StatusCode::FAILURE;
	}

	return sc;
}
*/
