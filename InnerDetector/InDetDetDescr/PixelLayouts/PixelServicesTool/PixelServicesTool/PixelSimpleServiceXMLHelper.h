/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelSimpleServiceXMLHelper_H
#define PixelSimpleServiceXMLHelper_H

#include "PixelGeoModel/PixelGeoBuilder.h"
#include "PixelServicesTool/SimpleServiceVolumeMaker.h"

// XML library
#include "PixelLayoutUtils/GeoXMLUtils.h"

class PixelSimpleServiceXMLHelper :  public GeoXMLUtils  {

 public:
  PixelSimpleServiceXMLHelper(IRDBRecordset_ptr table, const InDetDD::ServiceVolumeSchema & schema);
  PixelSimpleServiceXMLHelper(std::string envFileName);
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
    int    volId(int index) const;
    std::string shapeType(int index) const;
    std::string volName(int index) const;
    std::string materialName(int index) const;

    unsigned int numElements() const;
    int getServiceIndex( std::string srvName) const;
    
    bool SupportTubeExists(std::string srvName) const;
    double SupportTubeRMin(std::string srvName) const;
    double SupportTubeRMax(std::string srvName) const;
    double SupportTubeZMin(std::string srvName) const;
    double SupportTubeZMax(std::string srvName) const;

 private:

    InDetDD::ServiceVolumeSchema m_schema;
    bool m_bXMLdefined;

};

#endif
