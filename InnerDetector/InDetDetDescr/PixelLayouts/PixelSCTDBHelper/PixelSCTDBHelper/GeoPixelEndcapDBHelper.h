/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELENDCAPDBHELPER_H
#define GEOPIXELENDCAPDBHELPER_H

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <sstream>

#include "PixelGeoModel/PixelGeoBuilder.h"

#include "AthenaKernel/MsgStreamMember.h"
#include "InDetGeoModelUtils/OraclePixGeoAccessor.h"


  class GeoPixelEndcapDBHelper :  public PixelGeoBuilder {
  public:
    GeoPixelEndcapDBHelper( PixelGeoBuilderBasics* basics);
    ~GeoPixelEndcapDBHelper();

    int PixelEndcapNDisk() const;
    double  PixelEndcapRMin() const;
    double  PixelEndcapRMax() const;
    double  PixelEndcapZMin() const;
    double  PixelEndcapZMax() const;

    double PixelECCarbonRMin(std::string a, int idisk) const;
    double PixelECCarbonRMax(std::string a, int idisk) const;
    double PixelDiskSupportRMin(int isup, int idisk) const;
    double PixelDiskSupportRMax(int isup, int idisk) const;
    double  PixelDiskRMin(int idisk) const;

    double PixelDiskZPosition(int idisk) const; 
    double PixelECSiDz1(int idisk) const;
    double PixelECSiDz2(int idisk) const;
    int PixelECNSectors1(int idisk) const;
    int PixelECNSectors2(int idisk) const ;

  private:
    int m_disk;
    
    IRDBRecordset_ptr diskTable_ptr;
    IRDBRecordset_ptr ringTable_ptr;
    IRDBRecordset_ptr endcapGenTable_ptr;

  };


#endif
