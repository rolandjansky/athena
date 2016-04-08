/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3E33EC600353.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3E33EC600353.cm

//## begin module%3E33EC600353.cp preserve=no
//## end module%3E33EC600353.cp

//## Module: GeoVDetectorManager%3E33EC600353; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoVDetectorManager.cxx

//## begin module%3E33EC600353.additionalIncludes preserve=no
//## end module%3E33EC600353.additionalIncludes

//## begin module%3E33EC600353.includes preserve=yes
//## end module%3E33EC600353.includes

// GeoVDetectorElement
#include "GeoModelKernel/GeoVDetectorElement.h"
// GeoVDetectorManager
#include "GeoModelKernel/GeoVDetectorManager.h"
//## begin module%3E33EC600353.additionalDeclarations preserve=yes
//## end module%3E33EC600353.additionalDeclarations


// Class GeoVDetectorManager 

GeoVDetectorManager::GeoVDetectorManager()
  //## begin GeoVDetectorManager::GeoVDetectorManager%3E33EC600353_const.hasinit preserve=no
  //## end GeoVDetectorManager::GeoVDetectorManager%3E33EC600353_const.hasinit
  //## begin GeoVDetectorManager::GeoVDetectorManager%3E33EC600353_const.initialization preserve=yes
  //## end GeoVDetectorManager::GeoVDetectorManager%3E33EC600353_const.initialization
{
  //## begin GeoVDetectorManager::GeoVDetectorManager%3E33EC600353_const.body preserve=yes
  //## end GeoVDetectorManager::GeoVDetectorManager%3E33EC600353_const.body
}


GeoVDetectorManager::~GeoVDetectorManager()
{
  //## begin GeoVDetectorManager::~GeoVDetectorManager%3E33EC600353_dest.body preserve=yes
  //## end GeoVDetectorManager::~GeoVDetectorManager%3E33EC600353_dest.body
}


// Additional Declarations
  //## begin GeoVDetectorManager%3E33EC600353.declarations preserve=yes
void GeoVDetectorManager::setName(const std::string & name) {
  m_name=name;
}

const std::string & GeoVDetectorManager::getName() const {
  return m_name;
}
  //## end GeoVDetectorManager%3E33EC600353.declarations
//## begin module%3E33EC600353.epilog preserve=yes
//## end module%3E33EC600353.epilog
