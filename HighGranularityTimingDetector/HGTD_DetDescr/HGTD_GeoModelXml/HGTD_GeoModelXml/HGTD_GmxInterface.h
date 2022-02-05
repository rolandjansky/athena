/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HGTD_GeoModelXml_HGTD_GMXINTERFACE_H
#define HGTD_GeoModelXml_HGTD_GMXINTERFACE_H

#include "AthenaBaseComps/AthMessaging.h"
#include "GeoModelXml/GmxInterface.h"

#include "HGTD_ReadoutGeometry/HGTD_DetectorManager.h"

#include <map>
#include <string>
#include <sstream>

namespace InDetDD {
    class HGTD_ModuleDesign;
    class SiCommonItems;
}

class HGTD_GmxInterface: public GmxInterface, public AthMessaging {
public:
    HGTD_GmxInterface(HGTD_DetectorManager* detectorManager,
                      InDetDD::SiCommonItems* commonItems);

    virtual int sensorId(std::map<std::string, int> &index) const override final;
    virtual void addSensorType(const std::string& clas,
                               const std::string& typeName,
                               const std::map<std::string, std::string>& parameters) override final;
    void addSensor(const std::string& typeName,
                   std::map<std::string, int> &index,
                   int sequentialId,
                   GeoVFullPhysVol *fpv) override final;  
private:

    void makeLgadModule(const std::string &typeName,
                        const std::map<std::string, std::string> &parameters);
                    
    std::map<std::string, const InDetDD::HGTD_ModuleDesign *> m_geometryMap;
    HGTD_DetectorManager* m_detectorManager{};
    InDetDD::SiCommonItems* m_commonItems{};
};

#endif // HGTD_GeoModelXml_HGTD_GMXINTERFACE_H
