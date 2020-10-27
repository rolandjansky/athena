/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELGEOMODELXML_PIXELGMXINTERFACE_H
#define PIXELGEOMODELXML_PIXELGMXINTERFACE_H

#include "GeoModelXml/GmxInterface.h"
//#include "GaudiKernel/MsgStream.h"

#include <map>
// For template functions getparm(s):-----------
#include <string>
#include <cstdlib>
#include <iostream>
#include <sstream>
//----------------------------------------------

#include "PixelGeoModelXml/ModuleTree.h"

class MsgStream;
namespace InDetDD {class SiDetectorDesign; class PixelDetectorManager; class SiCommonItems;}

class PixelGmxInterface: public GmxInterface {
 public:
  PixelGmxInterface(InDetDD::PixelDetectorManager *detectorManager, InDetDD::SiCommonItems *commonItems, ModuleTree *moduleTree);
  ~PixelGmxInterface();
  int moduleId(std::map<std::string, int> &index);
  void addModuleType(std::string clas, std::string typeName, std::map<std::string, std::string> parameters);
  void addModule(std::string typeName, std::map<std::string, int> &index, int sequentialId, 
		 GeoVFullPhysVol *fpv);
  //  void addAlignable(int level, std::map<std::string, int> &index, GeoVFullPhysVol *fpv, GeoAlignableTransform *transform);
  void makePixelModule(std::string typeName, std::map<std::string, std::string> &par);
  //  /*
  template <typename T> void getparm(const std::string typeName, const std::string name, 
				     const std::map<std::string, std::string> &par, T &value) {
    std::map<std::string, std::string>::const_iterator found;
    if ((found = par.find(name)) != par.end()) {
      std::istringstream(found->second) >> value;
    }
    else {
      std::cerr << "PixelGmxInterface::addModuleType: Error: missing parameter " << name << " for " << typeName << std::endl;
      exit(999);
    }
  }

  template <typename T, typename A> void getparms(const std::string typeName, const std::string name, 
						  const std::map<std::string, std::string> &par, std::vector<T, A> &vec) {
    std::map<std::string, std::string>::const_iterator found;
    if ((found = par.find(name)) != par.end()) {
      T value;
      std::string strVal(found->second);
      strVal.erase(strVal.find_last_not_of(" \t\n\r\f\v") + 1); // Pixel trailing white space or you get an extra 0
      std::istringstream inString(strVal);
      do {
	inString >> value;
	vec.push_back(value);
      }
      while (inString.good());
    }
    else {
      std::cerr << "PixelGmxInterface::addModuleType: Error: missing parameter " << name << " for " << typeName << std::endl;
      exit(999);
    }
  }
  //*/
  std::string getstr(const std::string typeName, const std::string name, const std::map<std::string, std::string> &par);

 private:
  std::map<std::string, int> m_geometryMap;
  InDetDD::PixelDetectorManager *m_detectorManager;
  InDetDD::SiCommonItems *m_commonItems;
  ModuleTree *m_moduleTree;
  MsgStream *m_log;
};



#endif // PIXELGEOMODELXML_PIXELGMXINTERFACE_H
