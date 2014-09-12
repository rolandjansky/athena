/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/AGDD2GeoSvc.h"
#include "AGDD2Geo/IAGDD2GeoSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AGDD2Geo/AGDDController.h"
#include "AGDD2Geo/AGDDParameterStore.h"
#include "AGDD2Geo/AGDD2GeoModelBuilder.h"
#include "AGDD2Geo/AliasStore.h"
#include "AGDD2Geo/AGDDDetector.h"
#include "AGDD2Geo/AGDDDetectorStore.h"

#include "EventInfo/TagInfo.h"
#include "EventInfoMgt/ITagInfoMgr.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/StoreGateSvc.h"

#include <iostream>
#include <sstream>

using namespace MuonGM;

/**
 ** Constructor(s)
 **/
AGDD2GeoSvc::AGDD2GeoSvc(const std::string& name,ISvcLocator* svc)
  : Service(name,svc),log(msgSvc(),name),m_parserVerbosity(0),m_builderVerbosity(0),
    m_readAGDD(true),m_printSections(false),m_disableSections(true),
    m_locked(false),m_dumpAGDD(false),m_overrideConfiguration(false),m_defaultDetector("Muon"),
    m_navigateDetector(""),m_printLevel(0)
{
  Print();
  
//  MsgStream log(msgSvc(), name());
  
  declareProperty( "XMLFiles",      	m_xmlFiles);
  declareProperty( "Sections",      	m_sectionsToBuild);
  declareProperty( "Volumes" ,      	m_volumesToBuild);
  declareProperty( "Structures" ,      	m_structuresToBuild);
  declareProperty( "ParserVerbosity",   m_parserVerbosity);
  declareProperty( "BuilderVerbosity",  m_builderVerbosity);
  declareProperty( "ReadAGDD",   		m_readAGDD);
  declareProperty( "PrintSections",   	m_printSections);
  declareProperty( "Locked",			m_locked);
  declareProperty( "DumpAGDD",		m_dumpAGDD);
  declareProperty("DefaultDetector",	m_defaultDetector);
  declareProperty("NavigateDetector",	m_navigateDetector);
  declareProperty("DisableSections",	m_disableSections);
  declareProperty("OverrideConfiguration",	    m_overrideConfiguration=false);
  declareProperty("PrintLevel",         m_printLevel);

  //  m_detectors = new std::vector<GeoFullPhysVol*>;
  //  m_detectors->clear();

}

AGDD2GeoSvc::~AGDD2GeoSvc()
{
}

/**
 ** Initialize Service
 **/
StatusCode
AGDD2GeoSvc::initialize()
{
  MsgStream log(msgSvc(), name());
  
  StatusCode result = Service::initialize();
  if (result.isFailure()) 
  {
    log << MSG::FATAL << "Unable to initialize the service!" << endreq;
    return result;
  }
  
  const DataHandle<TagInfo> tagInfoH;
  std::string tagInfoKey="";
  
  result=service("TagInfoMgr",m_tagInfoMgr);
  if (result.isFailure()) 
  {
    log << MSG::FATAL << "Unable to retrieve TagInfoMgr!" << endreq;
    return result;
  }
  else
  	tagInfoKey=m_tagInfoMgr->tagInfoKey();
  
  log << MSG::INFO << " initializing "<<endreq;
  Print();
  
  localInitialization();
  
  result=service("GeoModelSvc",p_GeoModelSvc);
  if (result.isFailure())
  {
//      log<<MSG::FATAL<<"unable to access GeoModelSvc "<<endreq;
  }
  result=service("RDBAccessSvc",p_RDBAccessSvc);
  if (result.isFailure())
  {
//      log<<MSG::FATAL<<"unable to access RBDAccessSvc "<<endreq;
  }
  p_RDBAccessSvc->connect();
  ReadAGDDFlags();
  
  log << MSG::INFO << " XML file "<<endreq ;
  for (unsigned int i=0;i<m_xmlFiles.size();i++)
  {
  		log << MSG::INFO << " \t file "<<m_xmlFiles[i]<<endreq ;
  }
  log<<MSG::INFO << " ---------"<<endreq;

  if (log.level()<=MSG::DEBUG)  
    log<<MSG::DEBUG <<" trying to get the controller"<<endreq;
  AGDDController* controller =AGDDController::GetController();
  if (log.level()<=MSG::DEBUG)
    log<<MSG::DEBUG <<" trying to get the builder"<<endreq;
//  AGDD2GeoModelBuilder* builder=dynamic_cast<AGDD2GeoModelBuilder*>(controller->GetBuilder());

  controller->ParseFiles();
  if (m_readAGDD) controller->ParseAMDC();
  
  if (log.level()<=MSG::DEBUG)
    log<<MSG::DEBUG << "trying to parse and build "<<endreq;

  if (m_printSections)
  {
    if (log.level()<=MSG::DEBUG)
  	  log<<MSG::DEBUG <<" printing sections "<<endreq;
  	controller->PrintSections();
	if (log.level()<=MSG::DEBUG)
  	  log<<MSG::DEBUG <<" done printing sections "<<endreq;
  }

  if (!m_defaultDetector.empty())
  {
  	 log<<MSG::INFO << " setting default detector to "<<m_defaultDetector<<endreq;
  	 controller->UseGeoModelDetector(m_defaultDetector);
  }
  if (!m_navigateDetector.empty())
  {
  	 log<<MSG::INFO << " navigating detector  "<<m_navigateDetector<<endreq;
  	 controller->NavigateGeoModelDetector(m_navigateDetector);
  }
  
  //  controller->BuildAll();
  if( !BuildMScomponents() ) return StatusCode::FAILURE;

  return result;
}

StatusCode
AGDD2GeoSvc::finalize()
{
	return StatusCode::SUCCESS;
}

StatusCode
AGDD2GeoSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IID_IAGDD2GeoSvc == riid )    {
    *ppvInterface = (IAGDD2GeoSvc*)this;
  }
  else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

void 
AGDD2GeoSvc::Print()
{
//	std::cout << " ++++++++++++++++++++++++ here +++++++++++++++++++"<<std::endl;
//	std::cout << " ++++++++++++++++++++++++ here +++++++++++++++++++"<<std::endl;
//	std::cout << " ++++++++++++++++++++++++ here +++++++++++++++++++"<<std::endl;
//	std::cout << " ++++++++++++++++++++++++ here +++++++++++++++++++"<<std::endl;

}

StatusCode AGDD2GeoSvc::SetTagInfo(std::string tag,std::string info) const
{
	MsgStream log(msgSvc(), name());
	StatusCode result;
	if (m_tagInfoMgr)
	{
		result=m_tagInfoMgr->addTag(tag,info);
		if (result.isFailure())
		{
			log<<MSG::ERROR<<"SetTagInfo: could not add tag "<<tag<<" value "<<
				info<<" to TagInfo "<<endreq;
		}
		return result;
	}
	else
	{
		log<<MSG::ERROR<<" SetTagInfo: the TagInfoMgr is not set!"<<endreq;
		return StatusCode::FAILURE;
	}
}

#include "AGDD2Geo/HandlerList.h"
void AGDD2GeoSvc::localInitialization()
{
	addHandler(new AGDDHandler("AGDD"));
	addHandler(new BoltHandler("Bolt"));
	addHandler(new IbeamHandler("Ibeam"));
	addHandler(new UbeamHandler("Ubeam"));
	addHandler(new addmaterialHandler("addmaterial"));
	addHandler(new aliasHandler("alias"));
	addHandler(new arrayHandler("array"));
	addHandler(new boxHandler("box"));
	addHandler(new colorHandler("color"));
	
	addHandler(new compositeHandler("composite"));
	addHandler(new compositionHandler("composition"));
	addHandler(new consHandler("cons"));
	addHandler(new defaultsHandler("defaults"));
	addHandler(new elcylHandler("elcyl"));
	addHandler(new elementHandler("element"));
	addHandler(new foreachHandler("foreach"));
	addHandler(new fractionmassHandler("fractionmass"));
	addHandler(new gvxyHandler("gvxy"));
	
	addHandler(new gvxysxHandler("gvxysx"));
	addHandler(new gvxy_pointHandler("gvxy_point"));
	addHandler(new gvxysxyHandler("gvxysxy"));
	addHandler(new intersectionHandler("intersection"));
	addHandler(new materialHandler("material"));
	addHandler(new materialsHandler("materials"));
	addHandler(new mposPhiHandler("mposPhi"));
	addHandler(new mposWedgeHandler("mposWedge"));
	
	addHandler(new msgHandler("msg"));
	addHandler(new natomsHandler("natoms"));
	addHandler(new pconHandler("pcon"));
	addHandler(new pgonHandler("pgon"));
	addHandler(new polyplaneHandler("polyplane"));
	addHandler(new posRPhiZHandler("posRPhiZ"));
	addHandler(new posXYZHandler("posXYZ"));
	addHandler(new ringHandler("ring"));
	
	addHandler(new muonStationHandler("muonStation"));
	addHandler(new micromegasHandler("micromegas"));
	addHandler(new mm_TechHandler("mm_Tech"));
	addHandler(new sTGCHandler("sTGC"));
	addHandler(new sTGC_readoutHandler("sTGC_readout"));
	addHandler(new sTGC_TechHandler("sTGC_Tech"));
	addHandler(new mmSpacerHandler("mmSpacer"));
	addHandler(new mmSpacer_TechHandler("mmSpacer_Tech"));
	
	
	addHandler(new sectionHandler("section"));
	addHandler(new sectorHandler("sector"));
	addHandler(new snakeHandler("snake"));
	addHandler(new snake_pointHandler("snake_point"));
	addHandler(new subtractionHandler("subtraction"));
	addHandler(new trdHandler("trd"));
	addHandler(new tubsHandler("tubs"));
	addHandler(new unionHandler("union"));
	addHandler(new varHandler("var"));
	
	addHandler(new chamberPositionerHandler("chamberPosition"));

	AliasStore::GetAliasList()->AddAlias("Aluminium1","std::Aluminium");
	AliasStore::GetAliasList()->AddAlias("Aluminium2","std::Aluminium");
	AliasStore::GetAliasList()->AddAlias("Aluminium3","std::Aluminium");
	AliasStore::GetAliasList()->AddAlias("Aluminium4","std::Aluminium");
	AliasStore::GetAliasList()->AddAlias("Aluminium5","std::Aluminium");
	AliasStore::GetAliasList()->AddAlias("Aluminium6","std::Aluminium");
	AliasStore::GetAliasList()->AddAlias("Aluminium7","std::Aluminium");
	AliasStore::GetAliasList()->AddAlias("Aluminium8","std::Aluminium");
	AliasStore::GetAliasList()->AddAlias("Aluminium9","std::Aluminium");
	
	AliasStore::GetAliasList()->AddAlias("Iron1","std::Iron");
	AliasStore::GetAliasList()->AddAlias("Iron2","std::Iron");
	AliasStore::GetAliasList()->AddAlias("Iron3","std::Iron");
	AliasStore::GetAliasList()->AddAlias("Iron4","std::Iron");
	AliasStore::GetAliasList()->AddAlias("Iron5","std::Iron");
	AliasStore::GetAliasList()->AddAlias("Iron6","std::Iron");
	AliasStore::GetAliasList()->AddAlias("Iron7","std::Iron");
	AliasStore::GetAliasList()->AddAlias("Iron8","std::Iron");
	AliasStore::GetAliasList()->AddAlias("Iron9","std::Iron");
	
	AliasStore::GetAliasList()->AddAlias("Iron","std::Iron");
	AliasStore::GetAliasList()->AddAlias("Copper","std::Copper");
	AliasStore::GetAliasList()->AddAlias("Lead","std::Lead");
	AliasStore::GetAliasList()->AddAlias("Aluminium","std::Aluminium");
	AliasStore::GetAliasList()->AddAlias("Carbon","std::Carbon");
	
	AliasStore::GetAliasList()->AddAlias("Brass","sct::Brass");
	AliasStore::GetAliasList()->AddAlias("PolyBoron5percent","std::Polyethylene");	
	AliasStore::GetAliasList()->AddAlias("Polylithium","shield::Polylithium");
	AliasStore::GetAliasList()->AddAlias("ShieldBrass","shield::ShieldBrass");
	AliasStore::GetAliasList()->AddAlias("ShieldIron","shield::ShieldIron");
	AliasStore::GetAliasList()->AddAlias("ShieldSteel","shield::ShieldSteel");
	AliasStore::GetAliasList()->AddAlias("PolyBoronB4C","shield::PolyboronB4C");
	AliasStore::GetAliasList()->AddAlias("PolyBoronB2O3","shield::PolyboronB2O3");
	AliasStore::GetAliasList()->AddAlias("PolyBoronH3B03","shield::PolyboronH3B03");
	AliasStore::GetAliasList()->AddAlias("PolyBoron207HD5","shield::Polyboron207HD5");
}

void AGDD2GeoSvc::ReadAGDDFlags()
{
   MsgStream log(msgSvc(), name());
   std::string agdd2geoVersion = p_RDBAccessSvc->getChildTag("AGDD2GeoSwitches",p_GeoModelSvc->muonVersion(),"MuonSpectrometer");
//   m_AGDD2GeoSwitches.clear();
   if(!agdd2geoVersion.empty()) 
   {
//     m_AGDD2GeoSwitchesStamp = m_AGDD2GeoSwitchesStamp + 1;
     std::string TheKEYNAME;
     int TheKEYVALUE;
     const IRDBRecordset* pIRDBRecordset = p_RDBAccessSvc->getRecordset("AGDD2GeoSwitches",p_GeoModelSvc->muonVersion(),"MuonSpectrometer");
     for(unsigned int i=0; i<pIRDBRecordset->size(); i++) 
     {
       const IRDBRecord* record = (*pIRDBRecordset)[i];
       TheKEYNAME = record->getString("KEYNAME");
       TheKEYVALUE = record->getInt("KEYVALUE");
       if ( TheKEYVALUE == 1 )
       {
         log<< MSG::INFO << "  Add to m_AGDD2GeoSwitches " << TheKEYNAME << endreq;
//		 if (aliases->IsAliased(TheKEYNAME))
         m_structuresFromFlags.push_back(TheKEYNAME);
       }
     }
   }
   else
   {
      log<< MSG::INFO << "  agdd2geoVersion is empty " << endreq;
   }	
}

bool AGDD2GeoSvc::BuildMScomponents() const
{

  MsgStream log(msgSvc(), name());

  log<<MSG::INFO<<"In AGDD2GeoSvc::BuildMScomponents - start"<<endreq;
  AGDDController* controller =AGDDController::GetController();
  controller->BuildAll();
  log<<MSG::INFO<<"In AGDD2GeoSvc::BuildMScomponents - done"<<endreq;


  StoreGateSvc* pDetStore=0;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc=svcLocator->service("DetectorStore",pDetStore);
  if (sc.isFailure()) 
    {
      std::cout<<"AGDDController could not get at the detector store!"<<std::endl;
    }
  MuonGM::MuonDetectorManager*	muonMgr;
  sc = pDetStore->retrieve( muonMgr );
  if ( sc.isFailure() ) {
   log << MSG::ERROR << " Cannot retrieve MuonDetectorManager " << endreq;
   return false;
  }

  //  NSWGeoFullPhysVolContainer* aggdContainer = new NSWGeoFullPhysVolContainer;
  // Here create Readout Geometry 
  // std::map<std::string, GeoFullPhysVol*>* vec = GetMSdetectors();
  // if (vec->size()>0) 
  //   {
  //     log << MSG::INFO << "New Detectors have been created: "<<vec->size() << endreq;
  //   }
  // else 
  //   {
  //     log << MSG::INFO << "No New Detectors - no need to create any extra components of the readout geometry "<<vec->size() << endreq;
  //     return true;
  //   }

  bool readoutGeoDone =  BuildReadoutGeometry(muonMgr/*, GetMSdetectors*/);
  if (!readoutGeoDone) {
   log << MSG::ERROR << " Problems met while building the ReadoutGeometry " << endreq;
   return false;
  }
  else log << MSG::INFO << "ReadoutGeometry created" << endreq;

  // std::vector<GeoFullPhysVol*>::const_iterator it =  vec->begin();
  // for (; it!=vec->end(); ++it)
  //   {
  //     //aggdContainer->push_back(*it);
  //   }
  // //std::cout<<"Size of the NSWdetectors = <"<<vec->size()<<"> "<<aggdContainer->size()<<std::cout;
  // //sc=pDetStore->record( aggdContainer ,"NSWdetectors");
  // if (sc.isFailure()) 
  //   {
  //     std::cout<<"AGDDController could not record NSWdetectors!"<<std::endl;
  //   }

  return true;
}

bool AGDD2GeoSvc::BuildReadoutGeometry(MuonGM::MuonDetectorManager* mgr/*, std::map<GeoFullPhysVol*, std::string>* vec*/) const
{
  bool geoBuilt = true;
  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"In AGDD2GeoSvc::BuildReadoutGeometry - start"<<endreq;

  std::map<std::string, GeoFullPhysVol*>* myMap =  NULL;
  myMap = GetMSdetectors();
  log<<MSG::INFO<<"In AGDD2GeoSvc::BuildReadoutGeometry - size of the detector map = "<<myMap->size()<<endreq;

  std::map<std::string, GeoFullPhysVol*>::const_iterator it;
  for (it=myMap->begin(); it!=myMap->end(); ++it)
    {
      std::string chTag = (*it).first;
      GeoFullPhysVol* vol = (*it).second;
      log<<MSG::INFO<<"Building RE for component named <"<<chTag<<">"<<endreq;
      
      std::string stName = chTag.substr(0,4);
      
      int etaIndex = 999;
      int phiIndex = 999;
      int mLayer   = 999;
      int iSide    = 0;
      int iLS      = atoi((chTag.substr(3,1)).c_str()); //sTG3 and sMD3 are small chambers for small sectors 
      if (iLS==3) iLS = 1; // small 
      else iLS = 0; // large 
      if (chTag.substr(13,1)=="A") iSide=1;
      else if (chTag.substr(13,1)=="C") iSide=-1;
      etaIndex = iSide*atoi((chTag.substr(5,1)).c_str());
      phiIndex = atoi((chTag.substr(12,1)).c_str());
      mLayer = atoi((chTag.substr(7,1)).c_str());
      
      //MuonReadoutElement* re = NULL;
      if (chTag.substr(0,3)=="sMD")
	{
	  MMReadoutElement* re = new MMReadoutElement((GeoVFullPhysVol*)vol, stName, etaIndex, phiIndex, mLayer, false, mgr);
	  std::string myVolName = (chTag.substr(0,8)).c_str();
	  AGDDParameterBagMM* MMparaBag = dynamic_cast<AGDDParameterBagMM*> (AGDDParameterStore::GetParameterStore()->GetParameterBag(myVolName));
	  if(!MMparaBag) {
	    std::cout << " not possible to retrieve parameters for <" << myVolName << ">" << std::endl;
	  }
	  double pitch;
	  if(iLS==1) pitch = MMparaBag->TechParameters->pitchSS; //small
	  else pitch = MMparaBag->TechParameters->pitchLS; //large
	  re->initDesign(MMparaBag->largeX, MMparaBag->smallX, MMparaBag->lengthY, pitch, MMparaBag->TechParameters->thickness);
	  re->fillCache();
	  mgr->addMMReadoutElement_withIdFields(re, iLS, etaIndex, phiIndex, mLayer);
	}
      else if (chTag.substr(0,3)=="sTG")
	{
	  sTgcReadoutElement* re = new sTgcReadoutElement((GeoVFullPhysVol*)vol, stName, etaIndex, phiIndex, mLayer, false, mgr);	  
	  std::string myVolName = (chTag.substr(0,8)).c_str();
	  /*
	  AGDDParameterBagsTGC* sTGCparaBag = dynamic_cast<AGDDParameterBagsTGC*> (AGDDParameterStore::GetParameterStore()->GetParameterBag(myVolName));
	  if(!sTGCparaBag) {
	    std::cout << " not possible to retrieve parameters for <" << myVolName << ">" << std::endl;
	  }
	  re->initDesign(sTGCparaBag->largeX, sTGCparaBag->smallX, sTGCparaBag->lengthY, sTGCparaBag->TechParameters->stripPitch,
			 sTGCparaBag->TechParameters->wirePitch, sTGCparaBag->TechParameters->stripWidth, sTGCparaBag->TechParameters->wireWidth,
			 sTGCparaBag->TechParameters->thickness);
	  
	  since the concept of Parameter Bag is dismissed things get hardcoded
	  initDesign for sTGCReadoutElement only uses stripPitch, strip Width and thickness - all other dimensions (here -999.) are (hard)coded there again
	  */
	  re->initDesign(-999., -999., -999., 3.2, -999., 2.7, -999., 2.6);
	  re->fillCache();
	  mgr->addsTgcReadoutElement_withIdFields(re, iLS, etaIndex, phiIndex, mLayer);
	}

      //aggdContainer->push_back(*it);
    }
  //std::cout<<"Size of the NSWdetectors = <"<<vec->size()<<"> "<<aggdContainer->size()<<std::cout;
  //sc=pDetStore->record( aggdContainer ,"NSWdetectors");
  return geoBuilt;

}

std::map<std::string, GeoFullPhysVol*>* AGDD2GeoSvc::GetMSdetectors() const
{
  AGDDController* controller =AGDDController::GetController();
  return controller->GetMSdetectors();
}

detectorList& AGDD2GeoSvc::GetDetectorList() const
{
	return AGDDDetectorStore::GetDetectorStore()->GetDetectorList();
}

std::vector<AGDDDetector*> AGDD2GeoSvc::GetDetectorsByType(std::string dt) const
{
	std::vector<AGDDDetector*> detectors;
	detectorList& theDetectors=GetDetectorList();
	detectorList::const_iterator it=theDetectors.begin();
	for (;it!=theDetectors.end();it++)
	{
		AGDDDetector* det=(*it).second;
		if (dt==det->DetectorType()) detectors.push_back(det);
	}
	return detectors;
}

AGDDDetector* AGDD2GeoSvc::GetDetectorByID(std::string dt) const
{
	detectorList& theDetectors=GetDetectorList();
	detectorList::const_iterator it=theDetectors.begin();
	for (;it!=theDetectors.end();it++)
	{
		AGDDDetector* det=(*it).second;
		if (dt==det->DetectorID()) return det;
	}
	return 0;
}

template<class T> std::vector<T*> AGDD2GeoSvc::GetDetectorsByType() const 
{
	std::vector<T*> detectors;
	detectorList& theDetectors=GetDetectorList();
	detectorList::const_iterator it=theDetectors.begin();
	for (;it!=theDetectors.end();it++)
	{
		T* det=dynamic_cast<T*>((*it).second);
		if (det) detectors.push_back(det);
	}
	return detectors;
}
template<class T> T* AGDD2GeoSvc::GetDetectorByID(std::string id) const 
{
	detectorList& theDetectors=GetDetectorList();
	detectorList::const_iterator it=theDetectors.begin();
	for (;it!=theDetectors.end();it++)
	{
		if (id==(*it).second->DetectorID())
	    {
			T* det=dynamic_cast<T*>((*it).second);
			if (det) return det;
		}
	}
	return 0;
}
