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

  class GeoPixelServicesDBHelper :  public PixelGeoBuilder {
  public:
    GeoPixelServicesDBHelper(const PixelGeoBuilderBasics* basics);
    ~GeoPixelServicesDBHelper();

    bool PixelBuildServices() const;

    bool detailedFrame() const;
    int PixelFrameSections() const;
    double PixelFrameRMinSide(int sectionIndex) const;
    double PixelFrameRMaxSide(int sectionIndex) const;
    double PixelFrameSideWidth(int sectionIndex) const;
    double PixelFrameZMin(int sectionIndex) const;
    double PixelFrameZMax(int sectionIndex) const;
    double PixelFramePhiStart(int sectionIndex) const;
    int    PixelFrameNumSides(int sectionIndex) const;
    bool   PixelFrameMirrorSides(int sectionIndex) const;
    std::string  PixelFrameSideMaterial(int sectionIndex) const;
    std::string  PixelFrameCornerMaterial(int sectionIndex) const;
    
    int PixelFrameNumSideElements(int sectionIndex);
    double PixelFrameElementZMin1(int sectionIndex, int element);
    double PixelFrameElementZMin2(int sectionIndex, int element);
    double PixelFrameElementZMax1(int sectionIndex, int element);
    double PixelFrameElementZMax2(int sectionIndex, int element);
    
    // helper functions
    int PixelFrameSectionFromIndex(int sectionIndex) const;
    void makeFrameIndexMap();
    int getFrameElementIndex(int sectionIndex, int element);

    // Pixel simple services
    double IBLServiceGetMinRadialPosition(const std::string& srvName, const std::string& srvType, 
					  double srvZmin, double srvZmax) const;
    double IBLServiceGetMaxRadialPosition(const std::string& srvName, const std::string& srvType, 
					  double srvZmin, double srvZmax) const;

    // Services
    std::string PixelServiceName(const std::string &, int) const;
    bool PixelServiceZsymm(const std::string &, int) const;
    double PixelServiceRMin(const std::string &, int) const;
    double PixelServiceRMax(const std::string &, int) const;
    double PixelServiceRMin2(const std::string &, int) const;
    double PixelServiceRMax2(const std::string &, int) const;
    double PixelServiceZMin(const std::string &, int) const;
    double PixelServiceZMax(const std::string &, int) const;
    double PixelServicePhiLoc(const std::string &a, int _n) const;
    double PixelServiceWidth(const std::string &a, int _n) const;
    int PixelServiceRepeat(const std::string &a, int _n) const;
    std::string PixelServiceShape(const std::string &a, int _n) const;
    std::string PixelServiceMaterial(const std::string &, int) const;
    int PixelServiceLD(const std::string &, int) const;
    int PixelServiceElements(const std::string &) const;
    int PixelServiceFrameNum(const std::string &, int) const;
    int PixelServiceEnvelopeNum(const std::string & type, int index) const;
    int PixelServiceParentEnvelopeNum(const std::string & type, int index) const;
    
    IRDBRecordset_ptr  getPixelServiceRecordset(const std::string & type) const;
    std::string getPixelServiceRecordString(const std::string & name, const std::string & type, int index) const;
    int getPixelServiceRecordInt(const std::string & name, const std::string & type, int index) const;
    double getPixelServiceRecordDouble(const std::string & name, const std::string & type, int index) const;
    bool getPixelServiceRecordTestField(const std::string & name, const std::string & type, int index) const;
    
    double CalculateThickness(double tck,std::string mat) const;

    double PixelECCablesThickness(int iDisk) const;
    double PixelECCablesRMin(int iDisk) const;
    double PixelECCablesRMax(int iDisk) const;
    double PixelECCablesDistance(int iDisk) const;

    int PixelDiskNumSupports() const;
    double PixelDiskSupportRMin(int idisk, int isup) const;
    double PixelDiskSupportRMax(int idisk, int isup) const;
    double PixelDiskSupportThickness(int idisk, int isup) const;
    int PixelDiskSupportMaterialTypeNum(int idisk, int isup) const;

  private:
    
    // frame element index map 
    std::map<int,std::vector<int> > *m_frameElementMap;


    IRDBRecordset_ptr switchTable_ptr;
    IRDBRecordset_ptr pixelFrame_ptr;
    IRDBRecordset_ptr pixelFrameSect_ptr;
    IRDBRecordset_ptr pixelSimpleSvc_ptr;
    IRDBRecordset_ptr pixelBarrelSvc_ptr;
    IRDBRecordset_ptr pixelEndcapSvc_ptr;
    IRDBRecordset_ptr pixelEnvelopeSvc_ptr;
    IRDBRecordset_ptr pixelDiskSvc_ptr;


  };


#endif
