/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 
#ifndef PixelSimpleServiceXMLHelper_H
#define PixelSimpleServiceXMLHelper_H

#include "PixelGeoModel/PixelGeoBuilder.h"

// XML library
#include "PixelLayoutUtils/GeoXMLUtils.h"

class PixelSimpleServiceXMLHelper :  public GeoXMLUtils, public PixelGeoBuilder  {

 public:
  PixelSimpleServiceXMLHelper(IRDBRecordset_ptr table, const PixelGeoBuilderBasics* basics);
  PixelSimpleServiceXMLHelper(const std::string& envName, const PixelGeoBuilderBasics* basics);
  ~PixelSimpleServiceXMLHelper();
  
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
    int getServiceIndex( const std::string& srvName) const;
    
    bool SupportTubeExists(const std::string& srvName) const;
    double SupportTubeRMin(const std::string& srvName) const;
    double SupportTubeRMax(const std::string& srvName) const;
    double SupportTubeZMin(const std::string& srvName) const;
    double SupportTubeZMax(const std::string& srvName) const;

 private:
    void Setup(const std::string& envName);
    bool m_bXMLdefined;

};

#endif
