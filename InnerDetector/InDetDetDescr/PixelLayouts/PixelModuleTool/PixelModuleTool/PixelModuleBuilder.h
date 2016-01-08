/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelModuleBuilder_H
#define PixelModuleBuilder_H

#include "PixelInterfaces/IPixelModuleSvc.h"

#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/MsgStreamMember.h" 

#include <vector>
//Gaudi Includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "PixelGeoModelModule/GeoDetModulePixelMap.h"
#include "PixelGeoComponent/GeoDetModule.h"

#include <string>
#include <vector>

class IGeoModelSvc;
class StoreGateSvc;
class OraclePixGeoAccessor;
class InDetMaterialManager;
class PixelGeoBuilderBasics;
class GeoDetModulePixel;

class PixelModuleBuilder:  public AthService, virtual public IPixelModuleSvc {
 public:
  
  PixelModuleBuilder(const std::string& name, ISvcLocator* sl);
  virtual ~PixelModuleBuilder();
  
  virtual StatusCode initialize();          //!< Service init
  virtual StatusCode finalize();            //!< Service finalize
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
  
  static const InterfaceID & interfaceID();
  
  // IOV CallBack
  virtual StatusCode callBack(IOVSVC_CALLBACK_ARGS);
    
  GeoDetModulePixel* getModule(const PixelGeoBuilderBasics*, int moduleIndex);
  GeoDetModulePixel* getModule(const PixelGeoBuilderBasics*, std::string moduleType);

  GeoDetModulePixel* getModule(const PixelGeoBuilderBasics*, int brl_ec, int layerdisk, int moduleIndex);
  GeoDetModulePixel* getModule(const PixelGeoBuilderBasics*, int brl_ec, int layerdisk, std::string moduleType);

  InDet::GeoDetModule* getModuleGeoComp(const PixelGeoBuilderBasics*, int brl_ec, int layerdisk, int moduleIndex);
  InDet::GeoDetModule* getModuleGeoComp(const PixelGeoBuilderBasics*, int brl_ec, int layerdisk, std::string moduleType);
  
  double getThickness(int moduleIndex) const;
  double getThicknessN(int moduleIndex) const;
  double getThicknessP(int moduleIndex) const;
  double getWidth(int moduleIndex) const;
  double getLength(int moduleIndex) const;
  int getChipNumber(int moduleIndex) const;
    
 private:
  
  static std::vector<GeoDetModulePixel*> s_geometries;  
  static std::vector<int> s_geometries_index;
  static std::vector<InDet::GeoDetModule*> s_geometries_geocomp;  
  GeoDetModulePixel* build(const PixelGeoBuilderBasics* basics, int moduleindex, int brl_ec=-1, int layerdisk=-1);

  GeoDetModulePixelMap m_moduleMap;

};

inline const InterfaceID & PixelModuleBuilder::interfaceID(){
  return IPixelModuleSvc::interfaceID();
}



#endif
