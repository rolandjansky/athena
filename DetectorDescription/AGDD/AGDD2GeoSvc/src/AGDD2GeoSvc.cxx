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
	addHandler(new AGDDHandler("AGDD", m_controller));
	addHandler(new BoltHandler("Bolt", m_controller));
	addHandler(new IbeamHandler("Ibeam", m_controller));
	addHandler(new UbeamHandler("Ubeam", m_controller));
	addHandler(new addmaterialHandler("addmaterial", m_controller));
	addHandler(new aliasHandler("alias", m_controller));
	addHandler(new arrayHandler("array", m_controller));
	addHandler(new boxHandler("box", m_controller));
	addHandler(new colorHandler("color", m_controller));
	
	addHandler(new compositeHandler("composite", m_controller));
	addHandler(new compositionHandler("composition", m_controller));
	addHandler(new consHandler("cons", m_controller));
	addHandler(new defaultsHandler("defaults", m_controller));
	addHandler(new elcylHandler("elcyl", m_controller));
	addHandler(new elementHandler("element", m_controller));
	addHandler(new foreachHandler("foreach", m_controller));
	addHandler(new fractionmassHandler("fractionmass", m_controller));
	addHandler(new gvxyHandler("gvxy", m_controller));
	
	addHandler(new gvxysxHandler("gvxysx", m_controller));
	addHandler(new gvxy_pointHandler("gvxy_point", m_controller));
	addHandler(new gvxysxyHandler("gvxysxy", m_controller));
	addHandler(new intersectionHandler("intersection", m_controller));
	addHandler(new materialHandler("material", m_controller));
	addHandler(new materialsHandler("materials", m_controller));
	addHandler(new mposPhiHandler("mposPhi", m_controller));
	addHandler(new mposWedgeHandler("mposWedge", m_controller));
	
	addHandler(new msgHandler("msg", m_controller));
	addHandler(new natomsHandler("natoms", m_controller));
	addHandler(new pconHandler("pcon", m_controller));
	addHandler(new pgonHandler("pgon", m_controller));
	addHandler(new polyplaneHandler("polyplane", m_controller));
	addHandler(new posRPhiZHandler("posRPhiZ", m_controller));
	addHandler(new posXYZHandler("posXYZ", m_controller));
	addHandler(new ringHandler("ring", m_controller));
	
	
	addHandler(new sectionHandler("section", m_controller));
	addHandler(new sectorHandler("sector", m_controller));
	addHandler(new snakeHandler("snake", m_controller));
	addHandler(new snake_pointHandler("snake_point", m_controller));
	addHandler(new subtractionHandler("subtraction", m_controller));
	addHandler(new trdHandler("trd", m_controller));
	addHandler(new tubsHandler("tubs", m_controller));
	addHandler(new unionHandler("union", m_controller));
	addHandler(new varHandler("var", m_controller));
	addHandler(new versionHandler("version", m_controller));
	addHandler(new importHandler("import", m_controller));
	
	addHandler(new chamberPositionerHandler("chamberPosition", m_controller));

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
