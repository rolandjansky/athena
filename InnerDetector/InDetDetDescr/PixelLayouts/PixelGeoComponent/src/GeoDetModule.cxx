/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoComponent/GeoComponentHelper.h"
#include "PixelGeoComponent/GeoDetModule.h"
#include "PixelGeoComponent/GeoSensor.h"
#include "PixelGeoComponent/GeoSimpleObject.h"

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"

InDet::GeoDetModule::GeoDetModule( GeoFullPhysVol* vol):GeoComponent(vol)
{ // m_sensors.clear(); m_modLayers.clear(); 
//   m_servicesSensor.clear(); m_servicesAux.clear();
  m_etaModuleId =-9999;
  m_moduleType = -1;
  std::cout<<"InDet::GeoDetModule::GeoDetModule( GeoFullPhysVol* vol):GeoComponent(vol)"<<std::endl;

}
// InDet::GeoDetModule::GeoDetModule( GeoPhysVol* vol):GeoComponent(vol)
// { // m_sensors.clear(); m_modLayers.clear(); 
// //   m_servicesSensor.clear(); m_servicesAux.clear();
// }
InDet::GeoDetModule::GeoDetModule(GeoFullPhysVol* vol, int seqNum, const HepGeom::Transform3D& trf, const std::string& name, const bool& bFlag, int etaModule ):
  GeoComponent(vol, seqNum, trf, name, bFlag),
  m_etaModuleId(etaModule)
  
{ // m_sensors.clear(); m_modLayers.clear(); 
//   m_servicesSensor.clear(); m_servicesAux.clear();
  m_moduleType = -1;
}

