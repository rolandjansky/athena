/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 
#ifndef SimpleServiceVolumeMaker_H 
#define SimpleServiceVolumeMaker_H 

#include <string>
#include <vector>
#include "RDBAccessSvc/IRDBAccessSvc.h"

// XML library
#include "PixelGeoModel/PixelGeoBuilder.h"
#include "PixelLayoutUtils/GeoXMLUtils.h" 

class IGeometryDBSvc;
class PixelSimpleServiceXMLHelper;

namespace InDetDD {

  class ServiceVolume;
  class AthenaComps;

  class SimpleServiceVolumeMaker : public GeoXMLUtils, public PixelGeoBuilder {
  public:
    SimpleServiceVolumeMaker(const std::string &table, const std::string &label, const PixelGeoBuilderBasics* basics, bool readDataFromDB) ;
    ~SimpleServiceVolumeMaker();
    const std::vector<const ServiceVolume *> & makeAll();
    ServiceVolume * make(int index);
    unsigned int numElements() const;

  private:
    std::string m_table;
    std::string m_label;
    PixelSimpleServiceXMLHelper* m_simpleSrvXMLHelper;
    std::vector<const ServiceVolume *> m_services;
    const OraclePixGeoAccessor* m_geoAccessor;
    bool m_readFromDB;
    bool m_XMLdefined;
  };
} // end namespace

#endif
