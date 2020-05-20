/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "BCMPrime/GeoBCMPrimeTool.h"
#include "BCMPrime/BCMPrimeXMLHelper.h"

GeoBCMPrimeTool::GeoBCMPrimeTool(const std::string &type, const std::string &name, const IInterface *parent)
    : AthAlgTool(type, name, parent),
    m_module()
{
  declareInterface<IGeoBCMPrimeTool>(this);

  declareProperty("ModulesOn", m_moduleon = 15);
}


//================ Initialisation =================================================

StatusCode GeoBCMPrimeTool::initialize()
{
  
  ATH_CHECK( AthAlgTool::initialize() );
  ATH_MSG_INFO("GeoBCMPrimeTool initialize() successful in " << name());
  
  return StatusCode::SUCCESS;
}


// Register callback function on ConDB object
// Empty for now
StatusCode GeoBCMPrimeTool::registerCallback( StoreGateSvc*)
{

  return StatusCode::SUCCESS;
}

GeoPhysVol *GeoBCMPrimeTool::buildModule(int iModule, const PixelGeoBuilderBasics *basics)
{

  ATH_MSG_DEBUG("GeoBCMPrime : StatusCode GeoBCMPrimeTool::buildModule( )");

  unsigned int mask = (1 << iModule);
  if ((mask & m_moduleon) != mask)
  {
    //this module is not supposed to be built
    return 0;
  }

  ATH_MSG_DEBUG(" --> Number succesfully read from XML file");

  GeoPhysVol* bcmModPhys = m_module.Build(iModule, basics);
  
  return bcmModPhys;
}

double GeoBCMPrimeTool::getTransX()
{
  return m_module.getTransX();
}

double GeoBCMPrimeTool::getTransY()
{
  return m_module.getTransY();
}

double GeoBCMPrimeTool::getRotX()
{
  return m_module.getRotX();
}

double GeoBCMPrimeTool::getRotY()
{
  return m_module.getRotY();
}

double GeoBCMPrimeTool::getRotZ()
{
  return m_module.getRotZ();
}

double GeoBCMPrimeTool::getTilt()
{
  return m_module.getTilt();
}

double GeoBCMPrimeTool::getRingRot()
{
  return m_module.getRingRot();
}

double GeoBCMPrimeTool::getRingOffset()
{
  return m_module.getRingOffset();
}