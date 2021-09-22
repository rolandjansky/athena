/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2GeoSvc/AGDD2GeoSvc.h"
#include "AGDDControl/IAGDD2GeoSvc.h"

#include "AGDDControl/IAGDDToolBase.h"
#include "AGDDControl/AGDDTokenizer.h"
#include "AGDDControl/AGDDController.h"
#include "AGDDKernel/AliasStore.h"

#include <iostream>
#include <sstream>

/**
 ** Constructor(s)
 **/
AGDDtoGeoSvc::AGDDtoGeoSvc(const std::string& name,ISvcLocator* svc)
  : base_class(name,svc)
  , m_builders(this)
{
  declareProperty( "Builders", m_builders, "Builders");
}

/**
 ** Initialize Service
 **/
StatusCode
AGDDtoGeoSvc::initialize()
{
  ATH_MSG_INFO(" this is AGDDtoGeoSvc::initialize()");

  localInitialization();

  ATH_CHECK(m_builders.retrieve());

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
	addHandler(new importHandler("import", m_controller));
	
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


void AGDDtoGeoSvc::addHandler (XMLHandler* v)
{
  std::scoped_lock lock (m_mutex);
  m_handlerVector.push_back(v);
}


IAGDDtoGeoSvc::LockedController AGDDtoGeoSvc::getController()
{
  std::unique_lock lock (m_mutex);
  return LockedController (m_controller, std::move (lock));
}
