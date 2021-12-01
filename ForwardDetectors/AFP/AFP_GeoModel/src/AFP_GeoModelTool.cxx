/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
	AFP_CONSTANTS AfpConstants;
    m_CfgParams.clear();
    m_pGeometry=NULL;

	m_defsidcfg.clear();

	m_vecAFP00XStaggering.resize(AfpConstants.SiT_Plate_amount);
	m_vecAFP00YStaggering.resize(AfpConstants.SiT_Plate_amount);
	std::fill_n(m_vecAFP00XStaggering.begin(),AfpConstants.SiT_Plate_amount,AfpConstants.SiT_FarDistanceToFloor);
	std::fill_n(m_vecAFP00YStaggering.begin(),AfpConstants.SiT_Plate_amount,0.0*CLHEP::mm);
	m_vecAFP01XStaggering.resize(AfpConstants.SiT_Plate_amount);
	m_vecAFP01YStaggering.resize(AfpConstants.SiT_Plate_amount);
	std::fill_n(m_vecAFP01XStaggering.begin(),AfpConstants.SiT_Plate_amount,AfpConstants.SiT_NearDistanceToFloor);
	std::fill_n(m_vecAFP01YStaggering.begin(),AfpConstants.SiT_Plate_amount,0.0*CLHEP::mm);
	m_vecAFP02XStaggering.resize(AfpConstants.SiT_Plate_amount);
	m_vecAFP02YStaggering.resize(AfpConstants.SiT_Plate_amount);
	std::fill_n(m_vecAFP02XStaggering.begin(),AfpConstants.SiT_Plate_amount,AfpConstants.SiT_NearDistanceToFloor);
	std::fill_n(m_vecAFP02YStaggering.begin(),AfpConstants.SiT_Plate_amount,0.0*CLHEP::mm);
	m_vecAFP03XStaggering.resize(AfpConstants.SiT_Plate_amount);
	m_vecAFP03YStaggering.resize(AfpConstants.SiT_Plate_amount);
	std::fill_n(m_vecAFP03XStaggering.begin(),AfpConstants.SiT_Plate_amount,AfpConstants.SiT_FarDistanceToFloor);
	std::fill_n(m_vecAFP03YStaggering.begin(),AfpConstants.SiT_Plate_amount,0.0*CLHEP::mm);

    //Properties of SID
	declareProperty("SID_AddVacuumSensors",m_defsidcfg.bAddVacuumSensors=false);


    //Properties of TD


    //Properties of stations
	declareProperty("AFP00_RPotFloorDistance",m_CfgParams.vecRPotFloorDistance[0]=AfpConstants.Stat_RPotFloorDistance);
	declareProperty("AFP01_RPotFloorDistance",m_CfgParams.vecRPotFloorDistance[1]=AfpConstants.Stat_RPotFloorDistance);
	declareProperty("AFP02_RPotFloorDistance",m_CfgParams.vecRPotFloorDistance[2]=AfpConstants.Stat_RPotFloorDistance);
	declareProperty("AFP03_RPotFloorDistance",m_CfgParams.vecRPotFloorDistance[3]=AfpConstants.Stat_RPotFloorDistance);

	declareProperty("AFP00_RPotYPos",m_CfgParams.vecRPotYPos[0]=AfpConstants.Stat_ShiftInYAxis);
	declareProperty("AFP01_RPotYPos",m_CfgParams.vecRPotYPos[1]=AfpConstants.Stat_ShiftInYAxis);
	declareProperty("AFP02_RPotYPos",m_CfgParams.vecRPotYPos[2]=AfpConstants.Stat_ShiftInYAxis);
	declareProperty("AFP03_RPotYPos",m_CfgParams.vecRPotYPos[3]=AfpConstants.Stat_ShiftInYAxis);

	declareProperty("AFP00_ZPos",m_CfgParams.vecStatNominalZPos[0]=AfpConstants.Stat_OuterZDistance);
	declareProperty("AFP01_ZPos",m_CfgParams.vecStatNominalZPos[1]=AfpConstants.Stat_InnerZDistance);
	declareProperty("AFP02_ZPos",m_CfgParams.vecStatNominalZPos[2]=-AfpConstants.Stat_InnerZDistance);
	declareProperty("AFP03_ZPos",m_CfgParams.vecStatNominalZPos[3]=-AfpConstants.Stat_OuterZDistance);

	m_CfgParams.sidcfg[EAS_AFP00]=m_defsidcfg;
	m_CfgParams.sidcfg[EAS_AFP01]=m_defsidcfg;
	m_CfgParams.sidcfg[EAS_AFP02]=m_defsidcfg;
	m_CfgParams.sidcfg[EAS_AFP03]=m_defsidcfg;
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

StatusCode AFP_GeoModelTool::checkPropertiesSettings()
{
	bool bRes=true;

	MsgStream LogStream(Athena::getMessageSvc(), "AFP_GeoModelTool::CheckPropertiesSettings");

	if(m_vecAFP00XStaggering.size()>0){
		if(m_vecAFP00XStaggering.size()==m_CfgParams.sidcfg[EAS_AFP00].fLayerCount){
			m_CfgParams.sidcfg[EAS_AFP00].vecXStaggering=m_vecAFP00XStaggering;
		}
		else{
			LogStream<<MSG::ERROR<<"Mismatch between SID_AFP01XStaggering and number of plates (SID_NumberOfLayers)"<<endmsg;
			bRes=false;
		}
	}
	if(m_vecAFP00YStaggering.size()>0){
		if(m_vecAFP00YStaggering.size()==m_CfgParams.sidcfg[EAS_AFP00].fLayerCount){
			m_CfgParams.sidcfg[EAS_AFP00].vecYStaggering=m_vecAFP00YStaggering;
		}
		else{
			LogStream<<MSG::ERROR<<"Mismatch between SID_AFP00YStaggering and number of plates (SID_NumberOfLayers)"<<endmsg;
			bRes=false;
		}
	}

	if(m_vecAFP01XStaggering.size()>0){
		if(m_vecAFP01XStaggering.size()==m_CfgParams.sidcfg[EAS_AFP01].fLayerCount){
			m_CfgParams.sidcfg[EAS_AFP01].vecXStaggering=m_vecAFP01XStaggering;
		}
		else{
			LogStream<<MSG::ERROR<<"Mismatch between SID_AFP01XStaggering and number of plates (SID_NumberOfLayers)"<<endmsg;
			bRes=false;
		}
	}
	if(m_vecAFP01YStaggering.size()>0){
		if(m_vecAFP01YStaggering.size()==m_CfgParams.sidcfg[EAS_AFP01].fLayerCount){
			m_CfgParams.sidcfg[EAS_AFP01].vecYStaggering=m_vecAFP01YStaggering;
		}
		else{
			LogStream<<MSG::ERROR<<"Mismatch between SID_AFP01YStaggering and number of plates (SID_NumberOfLayers)"<<endmsg;
			bRes=false;
		}
	}

	if(m_vecAFP02XStaggering.size()>0){
		if(m_vecAFP02XStaggering.size()==m_CfgParams.sidcfg[EAS_AFP02].fLayerCount){
			m_CfgParams.sidcfg[EAS_AFP02].vecXStaggering=m_vecAFP02XStaggering;
		}
		else{
			LogStream<<MSG::ERROR<<"Mismatch between SID_AFP02XStaggering and number of plates (SID_NumberOfLayers)"<<endmsg;
			bRes=false;
		}
	}
	if(m_vecAFP02YStaggering.size()>0){
		if(m_vecAFP02YStaggering.size()==m_CfgParams.sidcfg[EAS_AFP02].fLayerCount){
			m_CfgParams.sidcfg[EAS_AFP02].vecYStaggering=m_vecAFP02YStaggering;
		}
		else{
			LogStream<<MSG::ERROR<<"Mismatch between SID_AFP02YStaggering and number of plates (SID_NumberOfLayers)"<<endmsg;
			bRes=false;
		}
	}

	if(m_vecAFP03XStaggering.size()>0){
		if(m_vecAFP03XStaggering.size()==m_CfgParams.sidcfg[EAS_AFP03].fLayerCount){
			m_CfgParams.sidcfg[EAS_AFP03].vecXStaggering=m_vecAFP03XStaggering;
		}
		else{
			LogStream<<MSG::ERROR<<"Mismatch between SID_AFP03XStaggering and number of plates (SID_NumberOfLayers)"<<endmsg;
			bRes=false;
		}
	}
	if(m_vecAFP03YStaggering.size()>0){
		if(m_vecAFP03YStaggering.size()==m_CfgParams.sidcfg[EAS_AFP03].fLayerCount){
			m_CfgParams.sidcfg[EAS_AFP03].vecYStaggering=m_vecAFP03YStaggering;
		}
		else{
			LogStream<<MSG::ERROR<<"Mismatch between SID_AFP03YStaggering and number of plates (SID_NumberOfLayers)"<<endmsg;
			bRes=false;
		}
	}

	return bRes? StatusCode::SUCCESS:StatusCode::FAILURE;
}

StatusCode AFP_GeoModelTool::create( StoreGateSvc* detStore )
{ 
    MsgStream log(msgSvc(), name());
    //CHECK(m_iovSvc.retrieve()); //-- REMOVE THIS WHEN IOVSVC IS TO BE USED

    DataHandle<GeoModelExperiment> theExpt;
    StatusCode sc = detStore->retrieve( theExpt, "ATLAS" );
    if (StatusCode::SUCCESS != sc)
    {
        log << MSG::ERROR<< "Could not find GeoModelExperiment ATLAS"<< endmsg;
        return (StatusCode::FAILURE);
    }

	CHECK(checkPropertiesSettings());

    m_pGeometry=new AFP_Geometry(&m_CfgParams);
    m_pAFPDetectorFactory=new AFP_GeoModelFactory(detStore, m_pGeometry);

	if (m_detector==NULL)
	{
		try
		{
            GeoPhysVol *world=&*theExpt->getPhysVol();
            m_pAFPDetectorFactory->create(world);
		}
		catch (std::bad_alloc)
		{
            log << MSG::FATAL << "Could not create new DetectorNode!" << endmsg;
            return StatusCode::FAILURE;
        }

        // Register the DetectorNode instance with the Transient Detector Store
        theExpt->addManager(m_pAFPDetectorFactory->getDetectorManager());
        sc = detStore->record(m_pAFPDetectorFactory->getDetectorManager(),m_pAFPDetectorFactory->getDetectorManager()->getName());

		if (StatusCode::SUCCESS != sc)
		{
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
