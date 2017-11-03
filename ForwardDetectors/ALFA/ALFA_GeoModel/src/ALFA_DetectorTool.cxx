/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelUtilities/StoredPhysVol.h"

#include "ALFA_DetectorTool.h"
#include "ALFA_DetectorFactory.h" 
#include "ALFA_GeoModel/ALFA_DetectorManager.h" 

using namespace std;

ALFA_DetectorTool::ALFA_DetectorTool( const std::string& type, 
				      const std::string& name, 
				      const IInterface* parent )
  : GeoModelTool( type, name, parent), 
    m_pALFADetectorFactory(NULL),
    m_iovSvc( "IOVDbSvc", name )
{
	m_Config.clear();

	declareProperty("MetrologyType",m_Config.GeometryConfig.eRPMetrologyGeoType=EMT_NOMINAL);//EMT_METROLOGY
	declareProperty("MetrologySource",m_Config.GeometryConfig.strRPMetrologyConnString=string(""));//"rpmetrology.dat"
	declareProperty("ShiftToX97Pos",m_Config.GeometryConfig.bShiftToX97Pos=false);
	declareProperty("ConstructBeampipe",m_Config.bConstructBeampipe=false);
	declareProperty("AddBPInStation",m_Config.bAddIBP=true);

	declareProperty("NominalZPosA7L1",m_Config.GeometryConfig.fNominalZPosA7L1=237388*CLHEP::mm);
	declareProperty("NominalZPosB7L1",m_Config.GeometryConfig.fNominalZPosB7L1=241528*CLHEP::mm);
	declareProperty("NominalZPosA7R1",m_Config.GeometryConfig.fNominalZPosA7R1=-237408*CLHEP::mm);
	declareProperty("NominalZPosB7R1",m_Config.GeometryConfig.fNominalZPosB7R1=-241548*CLHEP::mm);

	declareProperty("B7L1U_PosType",m_Config.GeometryConfig.CfgRPosParams[0].eRPPosType=ERPPT_ACTIVE);
	declareProperty("B7L1U_MDGeometryType",m_Config.GeometryConfig.CfgRPosParams[0].eMDGeoType=EGST_IDEALGEOMETRY);//EGST_DATABASE
	declareProperty("B7L1U_ODGeometryType",m_Config.GeometryConfig.CfgRPosParams[0].eODGeoType=EGST_IDEALGEOMETRY);//EGST_DATABASE
	declareProperty("B7L1U_MDSource",m_Config.GeometryConfig.CfgRPosParams[0].strMDConnString=string(""));//string("ALFA:ALFA-00:ALFAFibreUp")
	declareProperty("B7L1U_ODSource",m_Config.GeometryConfig.CfgRPosParams[0].strODConnString=string(""));//string("ALFA:ALFA-00:ALFAOvFibUp")
	declareProperty("B7L1U_CurrentLVDT",m_Config.GeometryConfig.CfgRPosParams[0].fCurrentLVDTmm=7.0*CLHEP::mm);
	
	declareProperty("B7L1L_PosType",m_Config.GeometryConfig.CfgRPosParams[1].eRPPosType=ERPPT_ACTIVE);
	declareProperty("B7L1L_MDGeometryType",m_Config.GeometryConfig.CfgRPosParams[1].eMDGeoType=EGST_IDEALGEOMETRY);
	declareProperty("B7L1L_ODGeometryType",m_Config.GeometryConfig.CfgRPosParams[1].eODGeoType=EGST_IDEALGEOMETRY);
	declareProperty("B7L1L_MDSource",m_Config.GeometryConfig.CfgRPosParams[1].strMDConnString=string(""));
	declareProperty("B7L1L_ODSource",m_Config.GeometryConfig.CfgRPosParams[1].strODConnString=string(""));
	declareProperty("B7L1L_CurrentLVDT",m_Config.GeometryConfig.CfgRPosParams[1].fCurrentLVDTmm=-7.0*CLHEP::mm);
	
	declareProperty("A7L1U_PosType",m_Config.GeometryConfig.CfgRPosParams[2].eRPPosType=ERPPT_ACTIVE);
	declareProperty("A7L1U_MDGeometryType",m_Config.GeometryConfig.CfgRPosParams[2].eMDGeoType=EGST_IDEALGEOMETRY);
	declareProperty("A7L1U_ODGeometryType",m_Config.GeometryConfig.CfgRPosParams[2].eODGeoType=EGST_IDEALGEOMETRY);
	declareProperty("A7L1U_MDSource",m_Config.GeometryConfig.CfgRPosParams[2].strMDConnString=string(string("")));
	declareProperty("A7L1U_ODSource",m_Config.GeometryConfig.CfgRPosParams[2].strODConnString=string(string("")));
	declareProperty("A7L1U_CurrentLVDT",m_Config.GeometryConfig.CfgRPosParams[2].fCurrentLVDTmm=7.0*CLHEP::mm);

	declareProperty("A7L1L_PosType",m_Config.GeometryConfig.CfgRPosParams[3].eRPPosType=ERPPT_ACTIVE);
	declareProperty("A7L1L_MDGeometryType",m_Config.GeometryConfig.CfgRPosParams[3].eMDGeoType=EGST_IDEALGEOMETRY);
	declareProperty("A7L1L_ODGeometryType",m_Config.GeometryConfig.CfgRPosParams[3].eODGeoType=EGST_IDEALGEOMETRY);
	declareProperty("A7L1L_MDSource",m_Config.GeometryConfig.CfgRPosParams[3].strMDConnString=string(""));
	declareProperty("A7L1L_ODSource",m_Config.GeometryConfig.CfgRPosParams[3].strODConnString=string(""));
	declareProperty("A7L1L_CurrentLVDT",m_Config.GeometryConfig.CfgRPosParams[3].fCurrentLVDTmm=-7.0*CLHEP::mm);

	declareProperty("A7R1U_PosType",m_Config.GeometryConfig.CfgRPosParams[4].eRPPosType=ERPPT_ACTIVE);
	declareProperty("A7R1U_MDGeometryType",m_Config.GeometryConfig.CfgRPosParams[4].eMDGeoType=EGST_IDEALGEOMETRY);
	declareProperty("A7R1U_ODGeometryType",m_Config.GeometryConfig.CfgRPosParams[4].eODGeoType=EGST_IDEALGEOMETRY);
	declareProperty("A7R1U_MDSource",m_Config.GeometryConfig.CfgRPosParams[4].strMDConnString=string(""));
	declareProperty("A7R1U_ODSource",m_Config.GeometryConfig.CfgRPosParams[4].strODConnString=string(""));
	declareProperty("A7R1U_CurrentLVDT",m_Config.GeometryConfig.CfgRPosParams[4].fCurrentLVDTmm=7.0*CLHEP::mm);

	declareProperty("A7R1L_PosType",m_Config.GeometryConfig.CfgRPosParams[5].eRPPosType=ERPPT_ACTIVE);
	declareProperty("A7R1L_MDGeometryType",m_Config.GeometryConfig.CfgRPosParams[5].eMDGeoType=EGST_IDEALGEOMETRY);
	declareProperty("A7R1L_ODGeometryType",m_Config.GeometryConfig.CfgRPosParams[5].eODGeoType=EGST_IDEALGEOMETRY);
	declareProperty("A7R1L_MDSource",m_Config.GeometryConfig.CfgRPosParams[5].strMDConnString=string(""));
	declareProperty("A7R1L_ODSource",m_Config.GeometryConfig.CfgRPosParams[5].strODConnString=string(""));
	declareProperty("A7R1L_CurrentLVDT",m_Config.GeometryConfig.CfgRPosParams[5].fCurrentLVDTmm=-7.0*CLHEP::mm);

	declareProperty("B7R1U_PosType",m_Config.GeometryConfig.CfgRPosParams[6].eRPPosType=ERPPT_ACTIVE);
	declareProperty("B7R1U_MDGeometryType",m_Config.GeometryConfig.CfgRPosParams[6].eMDGeoType=EGST_IDEALGEOMETRY);
	declareProperty("B7R1U_ODGeometryType",m_Config.GeometryConfig.CfgRPosParams[6].eODGeoType=EGST_IDEALGEOMETRY);
	declareProperty("B7R1U_MDSource",m_Config.GeometryConfig.CfgRPosParams[6].strMDConnString=string(""));
	declareProperty("B7R1U_ODSource",m_Config.GeometryConfig.CfgRPosParams[6].strODConnString=string(""));
	declareProperty("B7R1U_CurrentLVDT",m_Config.GeometryConfig.CfgRPosParams[6].fCurrentLVDTmm=7.0*CLHEP::mm);
	
	declareProperty("B7R1L_PosType",m_Config.GeometryConfig.CfgRPosParams[7].eRPPosType=ERPPT_ACTIVE);
	declareProperty("B7R1L_MDGeometryType",m_Config.GeometryConfig.CfgRPosParams[7].eMDGeoType=EGST_IDEALGEOMETRY);
	declareProperty("B7R1L_ODGeometryType",m_Config.GeometryConfig.CfgRPosParams[7].eODGeoType=EGST_IDEALGEOMETRY);
	declareProperty("B7R1L_MDSource",m_Config.GeometryConfig.CfgRPosParams[7].strMDConnString=string(""));
	declareProperty("B7R1L_ODSource",m_Config.GeometryConfig.CfgRPosParams[7].strODConnString=string(""));
	declareProperty("B7R1L_CurrentLVDT",m_Config.GeometryConfig.CfgRPosParams[7].fCurrentLVDTmm=-7.0*CLHEP::mm);

	declareProperty("bIsTransformInStation", m_Config.bIsTransformInStation, "status if transformation in station will be calculated");
	declareProperty("bIsTransformInDetector", m_Config.bIsTransformInDetector, "status if transformation in station will be calculated");

	declareProperty("pointTransformInDetectorB7L1U", m_Config.pointTransformInDetectorB7L1U, "transformation point in the detectorfor B7L1U RP");
	declareProperty("pointTransformInDetectorB7L1L", m_Config.pointTransformInDetectorB7L1L, "transformation point in the detectorfor B7L1L RP");
	declareProperty("pointTransformInDetectorA7L1U", m_Config.pointTransformInDetectorA7L1U, "transformation point in the detectorfor A7L1U RP");
	declareProperty("pointTransformInDetectorA7L1L", m_Config.pointTransformInDetectorA7L1L, "transformation point in the detectorfor A7L1L RP");
	declareProperty("pointTransformInDetectorA7R1U", m_Config.pointTransformInDetectorA7R1U, "transformation point in the detectorfor A7R1U RP");
	declareProperty("pointTransformInDetectorA7R1L", m_Config.pointTransformInDetectorA7R1L, "transformation point in the detectorfor A7R1L RP");
	declareProperty("pointTransformInDetectorB7R1U", m_Config.pointTransformInDetectorB7R1U, "transformation point in the detectorfor B7R1U RP");
	declareProperty("pointTransformInDetectorB7R1L", m_Config.pointTransformInDetectorB7R1L, "transformation point in the detectorfor B7R1L RP");

	declareProperty("vecTransformInDetectorB7L1U", m_Config.vecTransformInDetectorB7L1U, "transformation data in the detectorfor B7L1U RP");
	declareProperty("vecTransformInDetectorB7L1L", m_Config.vecTransformInDetectorB7L1L, "transformation data in the detectorfor B7L1L RP");
	declareProperty("vecTransformInDetectorA7L1U", m_Config.vecTransformInDetectorA7L1U, "transformation data in the detectorfor A7L1U RP");
	declareProperty("vecTransformInDetectorA7L1L", m_Config.vecTransformInDetectorA7L1L, "transformation data in the detectorfor A7L1L RP");
	declareProperty("vecTransformInDetectorA7R1U", m_Config.vecTransformInDetectorA7R1U, "transformation data in the detectorfor A7R1U RP");
	declareProperty("vecTransformInDetectorA7R1L", m_Config.vecTransformInDetectorA7R1L, "transformation data in the detectorfor A7R1L RP");
	declareProperty("vecTransformInDetectorB7R1U", m_Config.vecTransformInDetectorB7R1U, "transformation data in the detectorfor B7R1U RP");
	declareProperty("vecTransformInDetectorB7R1L", m_Config.vecTransformInDetectorB7R1L, "transformation data in the detectorfor B7R1L RP");

	declareProperty("vecTransformInStationB7L1U", m_Config.vecTransformInStationB7L1U, "transformation data in the stationfor B7L1U RP");
	declareProperty("vecTransformInStationB7L1L", m_Config.vecTransformInStationB7L1L, "transformation data in the stationfor B7L1L RP");
	declareProperty("vecTransformInStationA7L1U", m_Config.vecTransformInStationA7L1U, "transformation data in the stationfor A7L1U RP");
	declareProperty("vecTransformInStationA7L1L", m_Config.vecTransformInStationA7L1L, "transformation data in the stationfor A7L1L RP");
	declareProperty("vecTransformInStationA7R1U", m_Config.vecTransformInStationA7R1U, "transformation data in the stationfor A7R1U RP");
	declareProperty("vecTransformInStationA7R1L", m_Config.vecTransformInStationA7R1L, "transformation data in the stationfor A7R1L RP");
	declareProperty("vecTransformInStationB7R1U", m_Config.vecTransformInStationB7R1U, "transformation data in the stationfor B7R1U RP");
	declareProperty("vecTransformInStationB7R1L", m_Config.vecTransformInStationB7R1L, "transformation data in the stationfor B7R1L RP");
}

ALFA_DetectorTool::~ALFA_DetectorTool()
{
}


StatusCode ALFA_DetectorTool::create()
{ 
  MsgStream log(msgSvc(), name()); 
  
  log << MSG::INFO << "Building ALFA_ geometry" << endmsg;

  if(((eMetrologyType)m_Config.GeometryConfig.eRPMetrologyGeoType)==EMT_SWCORRECTIONS){
	  CHECK(m_iovSvc.retrieve());
  }

  // Retrieve GeoModel Experiment
  GeoModelExperiment * theExpt; 
  if (StatusCode::SUCCESS != detStore()->retrieve( theExpt, "ATLAS" )) 
  { 
    log << MSG::ERROR << "Could not find GeoModelExperiment ATLAS" << endmsg; 
    return StatusCode::FAILURE; 
  } 
  
  if(0 == m_detector)
  {
    GeoPhysVol *world=&*theExpt->getPhysVol();

    // Get pointer to the RDBAccessSvc
    // Use this pointer later for Geometry DB access
    IRDBAccessSvc* raccess = 0;
    StatusCode sc = service("RDBAccessSvc",raccess);
    if(sc.isFailure()) 
    {
      log << MSG::FATAL << "Could not locate RDBAccessSvc" << endmsg;
      return StatusCode::FAILURE;
    }

	/*
	int nChannel, nSign;
	const CondAttrListCollection* listAttrColl;
	CondAttrListCollection::const_iterator iterAttr;
	if(detStore->retrieve(listAttrColl,COOLFOLDER_DETSWCORR )==StatusCode::SUCCESS){
		for(iterAttr=listAttrColl->begin();iterAttr!=listAttrColl->end();iterAttr++){
			nChannel=iterAttr->first; //RPot ID
			m_Config.GeometryConfig.CfgRPosParams[nChannel].swcorr.fXOffset=((iterAttr->second)[0]).data<float>();
			m_Config.GeometryConfig.CfgRPosParams[nChannel].swcorr.fTheta=((iterAttr->second)[1]).data<float>();
			m_Config.GeometryConfig.CfgRPosParams[nChannel].swcorr.fYOffset=((iterAttr->second)[2]).data<float>();

			nSign=(nChannel%2==0)? +1:-1;
			m_Config.GeometryConfig.CfgRPosParams[nChannel].swcorr.fXOffset=0.0;
			m_Config.GeometryConfig.CfgRPosParams[nChannel].swcorr.fTheta=0.0;
			m_Config.GeometryConfig.CfgRPosParams[nChannel].swcorr.fYOffset=nSign*2.0;
			if(nChannel==2) m_Config.GeometryConfig.CfgRPosParams[nChannel].swcorr.fYOffset=6.0;
		 }
	}
	*/

	// Construct Factory
	//ALFA_DetectorFactory theALFA_Factory(detStore,raccess,&m_Config);
    m_pALFADetectorFactory=new ALFA_DetectorFactory(detStore().operator->(),raccess,&m_Config);

	// Build geometry
	//theALFA_Factory.create(world);
	m_pALFADetectorFactory->create(world);

    // Add ALFA_ manager to the Store Gate and GeoModel Experiment
	//theExpt->addManager(theALFA_Factory.getDetectorManager());
	//sc = detStore->record(theALFA_Factory.getDetectorManager(), theALFA_Factory.getDetectorManager()->getName());
	theExpt->addManager(m_pALFADetectorFactory->getDetectorManager());
	sc = detStore()->record(m_pALFADetectorFactory->getDetectorManager(), m_pALFADetectorFactory->getDetectorManager()->getName());
    
    if (sc.isFailure()) 
    {
      log << MSG::ERROR << "Could not register ALFA_ detector manager" << endmsg;
      return StatusCode::FAILURE; 
    }
    return StatusCode::SUCCESS;
  }
  
  return StatusCode::FAILURE;
}

StatusCode ALFA_DetectorTool::registerCallback()
{

	StatusCode sc=StatusCode::FAILURE;

	if(((eMetrologyType)m_Config.GeometryConfig.eRPMetrologyGeoType)==EMT_SWCORRECTIONS){
		const DataHandle<CondAttrListCollection> DataPtr;
		sc=detStore()->regFcn(&IGeoModelTool::align,dynamic_cast<IGeoModelTool*>(this), DataPtr, COOLFOLDER_DETSWCORR, true);
		if(sc!=StatusCode::SUCCESS){
		  msg(MSG::ERROR) << "Cannot register COOL callback for folder '"<<COOLFOLDER_DETSWCORR <<"'" << endmsg;
		}
		else
		  msg(MSG::INFO) << "Call-back to ALFA_DetectorTool::align() against folder "<< COOLFOLDER_DETSWCORR <<" registered "<<endmsg;
	}
	else{
	  msg(MSG::INFO) <<  "No callback registed" << endmsg;
	  sc=StatusCode::FAILURE;
	}

	return sc;
}

StatusCode ALFA_DetectorTool::align(IOVSVC_CALLBACK_ARGS)
{
	int nChannel;
	StatusCode sc=StatusCode::SUCCESS;
	const CondAttrListCollection* listAttrColl;
	CondAttrListCollection::const_iterator iterAttr;

	ALIGNPARAMETERS AlignParams;
	memset(&AlignParams,0,sizeof(ALIGNPARAMETERS));

	if(((eMetrologyType)m_Config.GeometryConfig.eRPMetrologyGeoType)==EMT_SWCORRECTIONS){
		// debug printout of global positions:
		StoredPhysVol* pStPhysRPBox=NULL;
		sc=detStore()->retrieve(pStPhysRPBox,"StRPBox[03]");
		if(sc.isSuccess()){
			const GeoFullPhysVol* pPhysRPBox=pStPhysRPBox->getPhysVol();
			const HepGeom::Transform3D& xf= pPhysRPBox->getAbsoluteTransform();

			msg(MSG::INFO) << "Translation of RPBOX: "<< xf.getTranslation() << endmsg;
		}

		if(detStore()->retrieve(listAttrColl,COOLFOLDER_DETSWCORR )==StatusCode::SUCCESS){
			for(iterAttr=listAttrColl->begin();iterAttr!=listAttrColl->end();iterAttr++){
				 nChannel=iterAttr->first; //RPot ID
				 AlignParams.fXOffset[nChannel]=((iterAttr->second)[0]).data<float>();
				 AlignParams.fTheta[nChannel]=((iterAttr->second)[1]).data<float>();
				 AlignParams.fYOffset[nChannel]=((iterAttr->second)[2]).data<float>();
			}

			m_pALFADetectorFactory->UpdateTransforms(&AlignParams);

			// debug printout of global positions:
			sc=detStore()->retrieve(pStPhysRPBox,"StRPBox[03]");
			if(sc.isSuccess()){
				const GeoFullPhysVol* pPhysRPBox=pStPhysRPBox->getPhysVol();
				const HepGeom::Transform3D& xf= pPhysRPBox->getAbsoluteTransform();
				msg(MSG::INFO) << "Translation of RPBOX after update: "<< xf.getTranslation() << endmsg;
			}
		}
		else{
			msg(MSG::ERROR) << "Folder '"<<"/FWD/ALFA/position_calibration"<<"' not found" << endmsg;
			sc=StatusCode::FAILURE;
		}
	}

	return sc;
}
