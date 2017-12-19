/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2GeoSvc/AGDD2GeoSvc.h"
#include "AGDD2GeoSvc/IAGDD2GeoSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/SvcFactory.h"

#include "AGDDControl/IAGDDToolBase.h"
#include "AGDDControl/AGDDTokenizer.h"
#include "AGDDKernel/AliasStore.h"

#include "EventInfo/TagInfo.h"
#include "EventInfoMgt/ITagInfoMgr.h"

#include "GaudiKernel/IToolSvc.h"

#include <iostream>
#include <sstream>

/**
 ** Constructor(s)
 **/
AGDDtoGeoSvc::AGDDtoGeoSvc(const std::string& name,ISvcLocator* svc)
  : AthService(name,svc),m_tagInfoMgr(0)
{
	ATH_MSG_INFO("creating AGDDtoGeoSvc!!!");
	declareProperty( "Builders",      	m_builders, "Builders");
}

AGDDtoGeoSvc::~AGDDtoGeoSvc()
{
}

/**
 ** Initialize Service
 **/
StatusCode
AGDDtoGeoSvc::initialize()
{
  ATH_MSG_INFO(" this is AGDDtoGeoSvc::initialize()");
  StatusCode result;
  // = Service::initialize();
  // if (result.isFailure()) 
//   {
//     ATH_MSG_FATAL("Unable to initialize the service!");
//     return result;
//   }
  
  localInitialization();
  
  for (unsigned int i=0;i<m_builders.size();i++)
  {
    IToolSvc* toolSvc=0;
	StatusCode mySvc=service("ToolSvc",toolSvc);
	if (mySvc.isFailure()) std::cout<<" could not get ToolSvc!!!"<<std::endl;
	
	IAGDDToolBase* aTest;
  
	AGDDTokenizer toolNames("/",m_builders[i]);
	StatusCode myTest;
	if (toolNames.size() == 1) myTest=toolSvc->retrieveTool(toolNames[0],aTest);
	else myTest=toolSvc->retrieveTool(toolNames[0],toolNames[1],aTest);
    if (myTest.isFailure()) std::cout<<"Failed retrieving tool!!"<<std::endl;
	
	StatusCode tConstruct=aTest->construct();
	if (tConstruct.isFailure()) std::cout<<"something wrong while calling construct()"<<std::endl;
  }  
  
  const DataHandle<TagInfo> tagInfoH;
  std::string tagInfoKey="";
  
  result=service("TagInfoMgr",m_tagInfoMgr);
  if (result.isFailure()) 
  {
    ATH_MSG_FATAL("Unable to retrieve TagInfoMgr!" );
    return result;
  }
  else
  	tagInfoKey=m_tagInfoMgr->tagInfoKey();
  
  ATH_MSG_INFO(" initializing ");

  return result;
}

StatusCode
AGDDtoGeoSvc::finalize()
{
	return StatusCode::SUCCESS;
}

StatusCode
AGDDtoGeoSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IID_IAGDDtoGeoSvc == riid )    {
    *ppvInterface = (IAGDDtoGeoSvc*)this;
  }
  else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}


#include "AGDDHandlers/HandlerList.h"
void AGDDtoGeoSvc::localInitialization()
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
	
	
	addHandler(new sectionHandler("section"));
	addHandler(new sectorHandler("sector"));
	addHandler(new snakeHandler("snake"));
	addHandler(new snake_pointHandler("snake_point"));
	addHandler(new subtractionHandler("subtraction"));
	addHandler(new trdHandler("trd"));
	addHandler(new tubsHandler("tubs"));
	addHandler(new unionHandler("union"));
	addHandler(new varHandler("var"));
	addHandler(new versionHandler("version"));
	addHandler(new importHandler("import"));
	
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
