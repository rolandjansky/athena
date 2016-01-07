/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 /**
 * @file IPixelDesignSvc.h
 * @author Grant.Gorfine@cern.ch
**/

#ifndef IPixelDesignSvc_h
#define IPixelDesignSvc_h
//Gaudi Includes
#include "GaudiKernel/IInterface.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaKernel/MsgStreamMember.h" 

//forward declarations

namespace InDetDD {
  class PixelModuleDesign;
  class PixelDiodeMatrix;
}

class PixelGeoBuilderBasics;

class IPixelDesignSvc: virtual public IInterface {
  
 public:
  virtual ~IPixelDesignSvc(){}
  
  static const InterfaceID & interfaceID(); //!< reimplemented from IInterface
  
  // IOV CallBack
  virtual StatusCode callBack(IOVSVC_CALLBACK_ARGS) = 0;

  virtual InDetDD::PixelModuleDesign* getDesign(const PixelGeoBuilderBasics*, int moduleIndex) =0;
  virtual InDetDD::PixelModuleDesign* getDesign(const PixelGeoBuilderBasics*, std::string moduleType) =0;
  virtual InDetDD::PixelDiodeMatrix* buildMatrix( double phiPitch, double etaPitch, 
						  double etaPitchLong, double etaPitchLongEnd,
						  int circuitsPhi, int circuitsEta,
						  int diodeRowPerCirc, int diodeColPerCirc) = 0;

};

inline const InterfaceID & IPixelDesignSvc::interfaceID(){
  static const InterfaceID IID_IPixelDesignSvc("IPixelDesignSvc",1,0);
  return IID_IPixelDesignSvc;
}

#endif
  
