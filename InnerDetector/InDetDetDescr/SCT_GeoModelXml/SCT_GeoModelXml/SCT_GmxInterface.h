/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODELXML_SCT_GMXINTERFACE_H
#define SCT_GEOMODELXML_SCT_GMXINTERFACE_H

#include "GeoModelXml/GmxInterface.h"
//#include "GaudiKernel/MsgStream.h"

#include <map>
// For template functions getparm(s):-----------
#include <string>
#include <cstdlib>
#include <iostream>
#include <sstream>
//----------------------------------------------

#include "SCT_GeoModelXml/WaferTree.h"

class MsgStream;
namespace InDetDD {class SiDetectorDesign; class SCT_DetectorManager; class SiCommonItems;class SCT_ModuleSideDesign;}

class SCT_GmxInterface: public GmxInterface {
public:
    SCT_GmxInterface(InDetDD::SCT_DetectorManager *detectorManager, InDetDD::SiCommonItems *commonItems, WaferTree *waferTree);
    ~SCT_GmxInterface();
    int sensorId(std::map<std::string, int> &index) const;
    int splitSensorId(std::map<std::string, int> &index, std::pair<std::string, int> &extraIndex, std::map<std::string, int> &updatedIndex) const; //For "artificially" adding to Identifiers; specify the field (e.g. "eta_module") and the value to add

    void addSensorType(std::string clas, std::string typeName, std::map<std::string, std::string> parameters);
    void addSensor(std::string typeName, std::map<std::string, int> &index, int sequentialId, 
                   GeoVFullPhysVol *fpv);
    void addSplitSensor(std::string typeName, std::map<std::string, int> &index, std::pair<std::string, int> &extraIndex, int sequentialId, GeoVFullPhysVol *fpv);

    void addAlignable(int level, std::map<std::string, int> &index, GeoVFullPhysVol *fpv, 
                      GeoAlignableTransform *transform);
    void makeSiStripBox(std::string typeName, std::map<std::string, std::string> &par);
    void makeStereoAnnulus(std::string typeName, std::map<std::string, std::string> &par);

    template <typename T> bool checkparm(const std::string /*typeName*/, const std::string name, 
					 const std::map<std::string, std::string> &par, T &value){
      //Needs some kind of versioning to stop this being abused...
      std::map<std::string, std::string>::const_iterator found;
      if ((found = par.find(name)) != par.end()) {
	std::istringstream(found->second) >> value;
	return true;
      }
      return false;
    }


    template <typename T> void getparm(const std::string typeName, const std::string name, 
                                       const std::map<std::string, std::string> &par, T &value) {
        std::map<std::string, std::string>::const_iterator found;
        if ((found = par.find(name)) != par.end()) {
            std::istringstream(found->second) >> value;
        }
        else {
            std::cerr << "SCT_GmxInterface::addSensorType: Error: missing parameter " << name << " for " << typeName << std::endl;
            exit(999);
        }
    }

    template <typename T, typename A> void getparms(const std::string typeName, const std::string name, 
                                       const std::map<std::string, std::string> &par, std::vector<T, A> &vec) {
        std::map<std::string, std::string>::const_iterator found;
        if ((found = par.find(name)) != par.end()) {
            T value;
            std::string strVal(found->second);
            strVal.erase(strVal.find_last_not_of(" \t\n\r\f\v") + 1); // Strip trailing white space or you get an extra 0
            std::istringstream inString(strVal);
            do {
                inString >> value;
                vec.push_back(value);
            }
            while (inString.good());
        }
        else {
            std::cerr << "SCT_GmxInterface::addSensorType: Error: missing parameter " << name << " for " << typeName << std::endl;
            exit(999);
        }
    }

    std::string getstr(const std::string typeName, const std::string name, const std::map<std::string, std::string> &par);

private:
    std::map<std::string, const InDetDD::SiDetectorDesign *> m_geometryMap;
    std::map<std::string, const InDetDD::SCT_ModuleSideDesign *> m_motherMap;
    InDetDD::SCT_DetectorManager *m_detectorManager;
    InDetDD::SiCommonItems *m_commonItems;
    WaferTree *m_waferTree;
    MsgStream *m_log;
};



#endif // SCT_GEOMODELXML_SCT_GMXINTERFACE_H
