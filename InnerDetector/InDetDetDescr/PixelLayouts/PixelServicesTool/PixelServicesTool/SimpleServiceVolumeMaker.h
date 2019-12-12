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

  class SimpleServiceVolumeSchema {
  public:
    SimpleServiceVolumeSchema();

    std::string rmin() const {return m_rmin;}
    std::string rmax() const {return m_rmax;}
    std::string rmin2() const {return m_rmin2;}
    std::string rmax2() const {return m_rmax2;}
    std::string zmin() const {return m_zmin;}
    std::string zmax() const {return m_zmax;}
    std::string zsymm() const {return m_zsymm;}
    std::string shapeType() const {return m_shapeType;}
    std::string materialName() const {return m_materialName;}
    std::string volName() const {return m_volName;}
    std::string phiWidth() const {return m_phiWidth;}
    std::string width() const {return m_width;}
    std::string repeat() const {return m_repeat;}
    std::string phiStart() const {return m_phiStart;}
    std::string phiDelta() const {return m_phiDelta;}
    std::string phiStep() const {return m_phiStep;}
    std::string radialDiv() const {return m_radialDiv;}
    
  private:
    std::string m_rmin;
    std::string m_rmax;
    std::string m_rmin2;
    std::string m_rmax2;
    std::string m_zmin;
    std::string m_zmax;
    std::string m_zsymm;
    std::string m_shapeType;
    std::string m_materialName;
    std::string m_volName;
    std::string m_phiWidth;
    std::string m_width;
    std::string m_repeat;
    std::string m_phiStart;
    std::string m_phiDelta;
    std::string m_phiStep;
    std::string m_radialDiv;
  };

  class SimpleServiceVolumeMakerMgr : public GeoXMLUtils, public PixelGeoBuilder {

  public:
    SimpleServiceVolumeMakerMgr(const std::string & table, bool readDataFromDB, const PixelGeoBuilderBasics* basics);
    
    double rmin(int index) const;
    double rmax(int index) const;
    double rmin2(int index) const;
    double rmax2(int index) const;
    double zmin(int index) const;
    double zmax(int index) const;
    double width(int index) const;
    double phiStart(int index) const;
    double phiDelta(int index) const;
    double phiStep(int index) const;
    bool   zsymm(int index) const;
    int    repeat(int index) const;
    int    radialDiv(int index) const;
    std::string shapeType(int index) const;
    std::string volName(int index) const;
    std::string materialName(int index) const;
    
    unsigned int numElements() const;

    const IGeometryDBSvc *db() const;
    
  private:
    std::string m_table;
    PixelSimpleServiceXMLHelper* m_simpleSrvXMLHelper;
    bool m_readFromDB;
    bool m_XMLdefined; 
  };


  class SimpleServiceVolumeMaker {
  public:
    SimpleServiceVolumeMaker(const std::string &table, const std::string &label, const PixelGeoBuilderBasics* basics, bool readDataFromDB) ;
    ~SimpleServiceVolumeMaker();
    const std::vector<const ServiceVolume *> & makeAll();
    ServiceVolume * make(int index);
    unsigned int numElements() const;

  private:
    std::string m_table;
    std::string m_label;
    SimpleServiceVolumeMakerMgr * m_mgr;    
    std::vector<const ServiceVolume *> m_services;
    const OraclePixGeoAccessor* m_geoAccessor;
  };
} // end namespace

#endif
