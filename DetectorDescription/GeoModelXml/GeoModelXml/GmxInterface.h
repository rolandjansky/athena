/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
    virtual int sensorId(std::map<std::string, int> &index) const;
    virtual int splitSensorId(std::map<std::string, int> &index, std::pair<std::string, int> &extraIndex, std::map<std::string, int> &updatedIndex) const; //For "artificially" adding to Identifiers; specify the field (e.g. "eta_module") and the value to add
    virtual void addSensorType(std::string type, std::string name, std::map<std::string, std::string> parameters);
    virtual void addSensor(std::string name, std::map<std::string, int> &index, int id, GeoVFullPhysVol *fpv);
    virtual void addSplitSensor(std::string name, std::map<std::string, int> &index, std::pair<std::string, int> &extraIndex, int id, GeoVFullPhysVol *fpv);
    virtual void addAlignable(int level, std::map<std::string, int> &index, 
                              GeoVFullPhysVol *fpv, GeoAlignableTransform *transform);

    // helpers
    template <typename T>
    bool checkParameter(const std::string /* &typeName */,  
                        const std::map<std::string, std::string> &parameters,
                        const std::string &name,
                        T &value)
    {
        // Needs some kind of versioning to stop this being abused...
        auto it = parameters.find(name);
        if (it != parameters.end()) {
            std::istringstream(it->second) >> value;
            return true;
        }
        return false;
    }

    template <typename T>
    void getParameter(const std::string &typeName,
                      const std::map<std::string, std::string> &parameters,
                      const std::string &name,
                      T &value) const
    {
        auto it = parameters.find(name);
        if (it != parameters.end()) {
            std::istringstream(it->second) >> value;
        } else {
            std::string err("GmxInterface Error: missing parameter " + name + " for " + typeName);
            throw std::runtime_error(err);
        }
    }

    template <typename T, typename A>
    void getParameters(const std::string &typeName,
                       const std::map<std::string, std::string> &parameters,
                       const std::string &name, 
                       std::vector<T, A> &vec) const
    {
        auto it = parameters.find(name);
        if (it != parameters.end()) {
            T value;
            std::string strVal(it->second);
            strVal.erase(strVal.find_last_not_of(" \t\n\r\f\v") + 1); // trailing white space or you get an extra 0
            std::istringstream inString(strVal);
            do {
                inString >> value;
                vec.push_back(value);
            } while (inString.good());
        } else {
            std::string err("GmxInterface Error: missing parameters " + name + " for " + typeName);
            throw std::runtime_error(err);
        }
    }

};
#endif // GMX_INTERFACE_H
