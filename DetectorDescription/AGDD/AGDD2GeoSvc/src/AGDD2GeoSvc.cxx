/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2GeoSvc/AGDD2GeoSvc.h"
#include "AGDDControl/IAGDD2GeoSvc.h"

#include "AGDDControl/IAGDDToolBase.h"
#include "AGDDControl/AGDDTokenizer.h"
#include "AGDDControl/AGDDController.h"
#include "AGDDKernel/AliasStore.h"
#include "AGDDKernel/AGDDPositionerStore.h"
#include "AGDDKernel/AGDDVolumeStore.h"
#include "AGDDKernel/AGDDSectionStore.h"

#include <iostream>
#include <sstream>

/**
 ** Constructor(s)
 **/
AGDDtoGeoSvc::AGDDtoGeoSvc(const std::string& name,ISvcLocator* svc)
  : base_class(name,svc),
    m_builders(this)
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

        AliasStore& as = m_controller.GetAliasStore();
	as.AddAlias("Aluminium1","std::Aluminium");
	as.AddAlias("Aluminium2","std::Aluminium");
	as.AddAlias("Aluminium3","std::Aluminium");
	as.AddAlias("Aluminium4","std::Aluminium");
	as.AddAlias("Aluminium5","std::Aluminium");
	as.AddAlias("Aluminium6","std::Aluminium");
	as.AddAlias("Aluminium7","std::Aluminium");
	as.AddAlias("Aluminium8","std::Aluminium");
	as.AddAlias("Aluminium9","std::Aluminium");
	
	as.AddAlias("Iron1","std::Iron");
	as.AddAlias("Iron2","std::Iron");
	as.AddAlias("Iron3","std::Iron");
	as.AddAlias("Iron4","std::Iron");
	as.AddAlias("Iron5","std::Iron");
	as.AddAlias("Iron6","std::Iron");
	as.AddAlias("Iron7","std::Iron");
	as.AddAlias("Iron8","std::Iron");
	as.AddAlias("Iron9","std::Iron");
	
	as.AddAlias("Iron","std::Iron");
	as.AddAlias("Copper","std::Copper");
	as.AddAlias("Lead","std::Lead");
	as.AddAlias("Aluminium","std::Aluminium");
	as.AddAlias("Carbon","std::Carbon");
	
	as.AddAlias("Brass","sct::Brass");
	as.AddAlias("PolyBoron5percent","std::Polyethylene");	
	as.AddAlias("Polylithium","shield::Polylithium");
	as.AddAlias("ShieldBrass","shield::ShieldBrass");
	as.AddAlias("ShieldIron","shield::ShieldIron");
	as.AddAlias("ShieldSteel","shield::ShieldSteel");
	as.AddAlias("PolyBoronB4C","shield::PolyboronB4C");
	as.AddAlias("PolyBoronB2O3","shield::PolyboronB2O3");
	as.AddAlias("PolyBoronH3B03","shield::PolyboronH3B03");
	as.AddAlias("PolyBoron207HD5","shield::Polyboron207HD5");
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
