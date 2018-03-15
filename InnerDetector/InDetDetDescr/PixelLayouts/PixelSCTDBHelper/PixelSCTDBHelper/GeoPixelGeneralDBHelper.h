/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELGENERALDBHELPER_H
#define GEOPIXELGENERALDBHELPER_H

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <sstream>

#include "PixelGeoModel/PixelGeoBuilder.h"

#include "AthenaKernel/MsgStreamMember.h"
#include "InDetGeoModelUtils/OraclePixGeoAccessor.h"


  class GeoPixelGeneralDBHelper :  public PixelGeoBuilder {
  public:
    GeoPixelGeneralDBHelper( const PixelGeoBuilderBasics* basics);
    ~GeoPixelGeneralDBHelper();

    bool PixelBuildServices() const;
    
    bool ibl() const { return m_bIBL; }
    bool slhc() const { return m_bSLHC; }
    std::string PixelVersionName() const { return m_versionName; }

    bool partPresent(std::string /*name*/) const {return true; }

    int PixelStaveLayout(int layer) const;
    int PixelStaveIndex(int layer) const;

    //  Pixel container
    double PixelRMin() const;
    double PixelRMax() const;
    double PixelHalfLength() const;
    
    // Pixel Envelope
    bool PixelSimpleEnvelope() const;
    unsigned int PixelEnvelopeNumPlanes() const; 
    double PixelEnvelopeZ(int i) const;
    double PixelEnvelopeRMin(int i) const; 
    double PixelEnvelopeRMax(int i) const;
    
    // Pixel Barrel  (from PixelBarrelGeneral)
    int PixelBarrelNLayer() const;
    double PixelBarrelRMin() const;
    double PixelBarrelRMax() const;
    double PixelBarrelHalfLength() const;

    int PixelEndcapNDisk() const;
    // Endcap container
    double  PixelEndcapRMin() const;
    double  PixelEndcapRMax()  const;
    double  PixelEndcapZMin()  const;
    double  PixelEndcapZMax() const;


  private:

    std::string m_versionName;
    bool m_bIBL;
    bool m_bSLHC;
    
    IRDBRecordset_ptr switchTable_ptr;
    IRDBRecordset_ptr pixelEnvelope_ptr;
    IRDBRecordset_ptr pixelBarrelGen_ptr;
    IRDBRecordset_ptr pixelEndcapGen_ptr;
    IRDBRecordset_ptr pixelStave_ptr;
    IRDBRecordset_ptr pixelLayer_ptr;
    IRDBRecordset_ptr pixelCommon_ptr;

  };

#endif
