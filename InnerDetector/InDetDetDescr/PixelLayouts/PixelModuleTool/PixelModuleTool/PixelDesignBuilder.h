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
					  double etaPitchLong, double etaPitchEnd,
					  double phiPitchLong, double phiPitchEnd,
					  int nEtaLong, int nEtaEnd,
					  int nPhiLong, int nPhiEnd,
					  int circuitsPhi, int circuitsEta,
					  int diodeRowPerCirc, int diodeColPerCirc);
  // older interface, calls the more general one above:
  InDetDD::PixelDiodeMatrix* buildMatrix( double phiPitch, double etaPitch, 
					  double etaPitchLong, double etaPitchEnd,
					  int circuitsPhi, int circuitsEta,
					  int diodeRowPerCirc, int diodeColPerCirc);
  
  void initModuleMap(const PixelGeoBuilderBasics*);

 private:
 
  static std::vector<InDetDD::PixelModuleDesign*> s_designs; 
  InDetDD::PixelModuleDesign* build(  const PixelGeoBuilderBasics* basics, int moduleIndex);
 
  const GeoDetModulePixelMap * m_moduleMap;

  // get-functions for XML fields that are version dependent
  int getNLongOrEndPixels(int parentIndex, const char* childTag) const;
  double getLongOrEndPitch(int parentIndex, const char* childTag) const;
};

inline const InterfaceID & PixelDesignBuilder::interfaceID(){
  return IPixelDesignSvc::interfaceID();
}

#endif
