/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO_MODEL_XML_GMX_INTERFACE_H
#define GEO_MODEL_XML_GMX_INTERFACE_H
//
//    Interface between SCT_SLHC_GeoModel and GeoModelXml.
//    Provides 4 call-back routines:
//        (0) To get a copy number with which to label a sensitive ID. 
//            ...gets copied to the G4 volume. Then SCT_G4_SD package uses this
//            to get the position indexes or hashId
//        (1) To add sensitive detector types
//        (2) To add sensitive elements
//        (3) To add alignable transforms
//    - these get added to the detector manager.
//
#include <string>
#include <vector>
#include <map>
#include "GeoModelKernel/GeoVFullPhysVol.h"
#include "GeoModelKernel/GeoAlignableTransform.h"

class GmxInterface {
public:
    virtual int sensorId(std::map<std::string, int> &index);
    virtual void addSensorType(std::string type, std::string name, std::map<std::string, std::string> parameters);
    virtual void addSensor(std::string name, std::map<std::string, int> &index, int id, 
                           GeoVFullPhysVol *fpv);
    virtual void addAlignable(int level, std::map<std::string, int> &index, 
                              GeoVFullPhysVol *fpv, GeoAlignableTransform *transform);

};
#endif // GMX_INTERFACE_H