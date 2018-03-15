/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelDesignBuilder_H
#define PixelDesignBuilder_H

#include "PixelInterfaces/IPixelDesignSvc.h"
#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/MsgStreamMember.h" 

#include "PixelGeoModelModule/GeoDetModulePixelMap.h"

// XML library
#include "PixelLayoutUtils/GeoXMLUtils.h"

#include <string>
#include <vector>

namespace InDetDD {
  class PixelModuleDesign;
  class PixelDiodeMatrix;
}

class IGeoModelSvc;
class StoreGateSvc;
class OraclePixGeoAccessor;
class InDetMaterialManager;
class PixelGeoBuilderBasics;

class PixelDesignBuilder:  public AthService, virtual public IPixelDesignSvc,  public GeoXMLUtils {
 public:
  
  PixelDesignBuilder(const std::string& name, ISvcLocator* sl);
  virtual ~PixelDesignBuilder();
  
  virtual StatusCode initialize();          //!< Service init
  virtual StatusCode finalize();            //!< Service finalize
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
  
  static const InterfaceID & interfaceID();
  
  // IOV CallBack
  virtual StatusCode callBack(IOVSVC_CALLBACK_ARGS);
  
  // GeoInit callback
  StatusCode geoInitCallback(IOVSVC_CALLBACK_ARGS);
  
  
  InDetDD::PixelModuleDesign* getDesign( const PixelGeoBuilderBasics* basics, int moduleIndex);
  InDetDD::PixelModuleDesign* getDesign( const PixelGeoBuilderBasics* basics, std::string moduleType);
  InDetDD::PixelDiodeMatrix* buildMatrix( double phiPitch, double etaPitch, 
					  double etaPitchLong, double etaPitchLongEnd,
					  int circuitsPhi, int circuitsEta,
					  int diodeRowPerCirc, int diodeColPerCirc);
  
 
 private:
 
  static std::vector<InDetDD::PixelModuleDesign*> s_designs; 
  InDetDD::PixelModuleDesign* build(  const PixelGeoBuilderBasics* basics, int moduleIndex);
 
  GeoDetModulePixelMap m_moduleMap;
};

inline const InterfaceID & PixelDesignBuilder::interfaceID(){
  return IPixelDesignSvc::interfaceID();
}

#endif
