/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELINTERFACES_IINDETSERVICESTOOL_H
#define PIXELINTERFACES_IINDETSERVICESTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"

class StoreGateSvc;
//class OraclePixGeoAccessor;
//class InDetMaterialManager;
class PixelGeoBuilderBasics;

class GeoPhysVol;
class GeoFullPhysVol;
namespace InDetDD{
  class TubeZone;
}

//namespace InDet { 
//  class GeoServiceAssembly;
//  class GeoSimpleObject;
  /*  class GeoEnvelope;*/

//  static const InterfaceID IID_IPixelServicesTool( "InDet::IPixelServicesTool", 1, 0 );
  static const InterfaceID IID_IPixelServicesTool( "IPixelServicesTool", 1, 0 );
  
  class IPixelServicesTool : public virtual IAlgTool {
  public:
    
    /// Retrieve Interface ID
    static const InterfaceID& interfaceID( ) { return IID_IPixelServicesTool; }
    
    //    virtual GeoEnvelope*  build( const OraclePixGeoAccessor*, InDetMaterialManager*) = 0;
    virtual void buildServices(const PixelGeoBuilderBasics*, std::vector<InDetDD::TubeZone*> v) = 0;

    virtual void buildAndPlace(const std::string & region, GeoPhysVol * parent, double zcenter = 0, 
			       std::vector<std::string> svcList = std::vector<std::string>(),
			       bool bStatic=true, bool bDynamic=true) =0;
    virtual void buildAndPlace(const std::string & region, GeoFullPhysVol * parent, double zcenter = 0, 
			       std::vector<std::string> svcList = std::vector<std::string>(),
			       bool bStatic=true, bool bDynamic=true) =0;
    virtual void resetServices() =0;

    virtual double computeRmin(const std::string & region, std::vector<std::string> svcList = std::vector<std::string>()) const =0;
    virtual double computeRmax(const std::string & region, std::vector<std::string> svcList = std::vector<std::string>()) const =0;
    virtual double computeZmin(const std::string & region, std::vector<std::string> svcList = std::vector<std::string>()) const =0;
    virtual double computeZmax(const std::string & region, std::vector<std::string> svcList = std::vector<std::string>()) const =0;
    virtual std::string getLayerModuleMaterialName(int, int) const =0;
    virtual std::string getLayerModuleMaterialName(int, std::vector<int>) const =0;

    virtual bool svcRouteAuto() const =0;
    //    virtual std::vector<InDet::GeoServiceAssembly* > getServiceAssemblies() =0;
    //    virtual std::vector<InDet::GeoSimpleObject* > getServiceObjects() =0;

  };
//}

#endif // GEOMODELINTERFACES_IGEOPIXELMODULETOOL_H


