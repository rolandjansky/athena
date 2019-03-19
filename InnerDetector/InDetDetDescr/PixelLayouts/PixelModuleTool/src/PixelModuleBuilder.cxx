/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelModuleTool/PixelModuleBuilder.h"
#include "PixelGeoModelModule/GeoDetModulePixel.h"

#include "PixelGeoModel/PixelGeoBuilder.h"

#include "StoreGate/StoreGate.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "PathResolver/PathResolver.h"

#include <algorithm>

std::vector<GeoDetModulePixel*> PixelModuleBuilder::s_geometries;
std::vector<InDet::GeoDetModule*> PixelModuleBuilder::s_geometries_geocomp;
std::vector<int> PixelModuleBuilder::s_geometries_index;

PixelModuleBuilder::PixelModuleBuilder( const std::string& name, ISvcLocator* pSvcLocator) : 
  AthService(name, pSvcLocator),
  m_moduleMap(nullptr)
{
  s_geometries.clear();
}

PixelModuleBuilder::~PixelModuleBuilder()
{
  for(int i=0; i<(int)s_geometries.size(); i++) delete s_geometries[i];
  if(m_moduleMap) delete m_moduleMap;
}

void PixelModuleBuilder::initModuleMap(const PixelGeoBuilderBasics* basics)
{
  m_moduleMap = new GeoDetModulePixelMap(basics);
 
}

StatusCode PixelModuleBuilder::initialize()
{ 
  StatusCode sc = AthService::initialize();
  if (sc.isFailure()) { 
    msg(MSG::FATAL) << "Unable to initialize the service!" << endreq;
    return sc;
  } 
  
  return StatusCode::SUCCESS;
}    

StatusCode PixelModuleBuilder::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode PixelModuleBuilder::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IPixelModuleSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IPixelModuleSvc *>(this);
  }  else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();

  return StatusCode::SUCCESS;
}

StatusCode PixelModuleBuilder::callBack(IOVSVC_CALLBACK_ARGS_P(/*I*/,/*keys*/))
{  
  return StatusCode::SUCCESS;
}

// -------- GeoModel ----------------------------------------------------

GeoDetModulePixel* PixelModuleBuilder::getModule(const PixelGeoBuilderBasics* basics, int /*moduleIndex*/)
{
  basics->msgStream()<<MSG::ERROR<<" try to retrieve module without specifying brl/ec and layer/disc"<<endreq;
  return 0;
}

GeoDetModulePixel* PixelModuleBuilder::getModule(const PixelGeoBuilderBasics* basics, std::string /*moduleType*/)
{
  basics->msgStream()<<MSG::ERROR<<" try to retrieve module without specifying brl/ec and layer/disc"<<std::endl;
  return 0;
}

GeoDetModulePixel* PixelModuleBuilder::getModule(const PixelGeoBuilderBasics* basics, int brl_ec, int layerdisk, int moduleIndex)
{
  
  if(brl_ec<0)brl_ec=-brl_ec;
  int vIndex = brl_ec*10000+layerdisk*100+moduleIndex;


  for(int i=0; i<(int)s_geometries_index.size(); i++) {
    if(s_geometries_index[i]==vIndex) return s_geometries[i];
  }

  GeoDetModulePixel* newDesign = build( basics, moduleIndex, brl_ec, layerdisk);
  s_geometries.push_back(newDesign);
  s_geometries_index.push_back(vIndex);

  return newDesign;
}


GeoDetModulePixel* PixelModuleBuilder::getModule(const PixelGeoBuilderBasics* basics,  int brl_ec, int layerdisk, std::string moduleType)
{

  if(!m_moduleMap) {
    basics->msgStream()<<MSG::ERROR<<"No Module Map avilable! Did you call initModuleMap?"<<endreq;
    return nullptr;
  }
  int moduleIndex = m_moduleMap->getModuleIndex(moduleType);
  return getModule(basics, brl_ec, layerdisk, moduleIndex);
}

// ------ GeoComponent ------------------------------------------------

InDet::GeoDetModule* PixelModuleBuilder::getModuleGeoComp(const PixelGeoBuilderBasics* basics, int brl_ec, int layerdisk, int moduleIndex)
{

  if(brl_ec<0)brl_ec=-brl_ec;
  int vIndex = brl_ec*10000+layerdisk*100+moduleIndex;

  for(int i=0; i<(int)s_geometries_index.size(); i++) {
    if(s_geometries_index[i]==vIndex) return s_geometries_geocomp[i];
  }

  GeoDetModulePixel* newDesign = build( basics, moduleIndex, brl_ec, layerdisk);
  s_geometries.push_back(newDesign);
  s_geometries_index.push_back(vIndex);
  s_geometries_geocomp.push_back( newDesign->buildGeoComp(brl_ec,layerdisk));

  return s_geometries_geocomp[s_geometries_index.size()-1];
}


InDet::GeoDetModule* PixelModuleBuilder::getModuleGeoComp(const PixelGeoBuilderBasics* basics, int brl_ec, int layerdisk, std::string moduleType)
{
  if(!m_moduleMap) {
    basics->msgStream()<<MSG::ERROR<<"No Module Map avilable! Did you call initModuleMap?"<<endreq;
    return nullptr;
  }
  int moduleIndex = m_moduleMap->getModuleIndex(moduleType);
  return getModuleGeoComp(basics, brl_ec, layerdisk, moduleIndex);
}

// -------------------------------------------------------------------

double PixelModuleBuilder::getThickness(int moduleIndex) const
{
  return s_geometries[moduleIndex]->Thickness();
}

double PixelModuleBuilder::getThicknessN(int moduleIndex) const
{
  return s_geometries[moduleIndex]->ThicknessN();
}

double PixelModuleBuilder::getThicknessP(int moduleIndex) const
{
  return s_geometries[moduleIndex]->ThicknessP();
}

double PixelModuleBuilder::getWidth(int moduleIndex) const
{
  return s_geometries[moduleIndex]->Width();
}

double PixelModuleBuilder::getLength(int moduleIndex) const
{
  return s_geometries[moduleIndex]->Length();
}

int PixelModuleBuilder::getChipNumber(int moduleIndex) const
{
  return s_geometries[moduleIndex]->ChipNumber();
}


GeoDetModulePixel* PixelModuleBuilder::build(const PixelGeoBuilderBasics* basics, int moduleIndex, int brl_ec, int layerdisk)
{

  GeoDetModulePixel *module = new GeoDetModulePixel(basics, moduleIndex, brl_ec, layerdisk);
  return module;
}




