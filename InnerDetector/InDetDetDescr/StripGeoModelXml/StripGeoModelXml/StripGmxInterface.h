/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STRIPGEOMODELXML_STRIPGMXINTERFACE_H
#define STRIPGEOMODELXML_STRIPGMXINTERFACE_H

#include "GeoModelXml/GmxInterface.h"
//#include "GaudiKernel/MsgStream.h"

#include <map>
// For template functions getparm(s):-----------
#include <string>
#include <cstdlib>
#include <iostream>
#include <sstream>
//----------------------------------------------

#include "StripGeoModelXml/WaferTree.h"

class MsgStream;
namespace InDetDD {class SiDetectorDesign; class SCT_DetectorManager; class SiCommonItems;}

class StripGmxInterface: public GmxInterface {
public:
    StripGmxInterface(InDetDD::SCT_DetectorManager *detectorManager, InDetDD::SiCommonItems *commonItems, WaferTree *waferTree);
    ~StripGmxInterface();
    int sensorId(std::map<std::string, int> &index);
    void addSensorType(std::string clas, std::string typeName, std::map<std::string, std::string> parameters);
    void addSensor(std::string typeName, std::map<std::string, int> &index, int sequentialId, 
                   GeoVFullPhysVol *fpv);
    void addAlignable(int level, std::map<std::string, int> &index, GeoVFullPhysVol *fpv, 
                      GeoAlignableTransform *transform);
    void makeSiStripBox(std::string typeName, std::map<std::string, std::string> &par);
    void makeStereoAnnulus(std::string typeName, std::map<std::string, std::string> &par);

    template <typename T> void getparm(const std::string typeName, const std::string name, 
                                       const std::map<std::string, std::string> &par, T &value) {
        std::map<std::string, std::string>::const_iterator found;
        if ((found = par.find(name)) != par.end()) {
            std::istringstream(found->second) >> value;
        }
        else {
            std::cerr << "StripGmxInterface::addSensorType: Error: missing parameter " << name << " for " << typeName << std::endl;
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
            std::cerr << "StripGmxInterface::addSensorType: Error: missing parameter " << name << " for " << typeName << std::endl;
            exit(999);
        }
    }

    std::string getstr(const std::string typeName, const std::string name, const std::map<std::string, std::string> &par);

private:
    std::map<std::string, int> m_geometryMap;
    InDetDD::SCT_DetectorManager *m_detectorManager;
    InDetDD::SiCommonItems *m_commonItems;
    WaferTree *m_waferTree;
    MsgStream *m_log;
};



#endif // STRIPGEOMODELXML_STRIPGMXINTERFACE_H
