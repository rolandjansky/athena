/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 /**
 * @file iPixelModuleSvc.h
 * @author Grant.Gorfine@cern.ch
**/

#ifndef IPixelModuleSvc_h
#define IPixelModuleSvc_h
//Gaudi Includes
#include "GaudiKernel/IInterface.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaKernel/MsgStreamMember.h" 

//forward declarations

class GeoDetModulePixel;
class PixelGeoBuilderBasics;
namespace InDet{
  class GeoComponent;
}

class IPixelModuleSvc: virtual public IInterface {
  
 public:
  virtual ~IPixelModuleSvc(){}
  
  static const InterfaceID & interfaceID(); //!< reimplemented from IInterface
  
  // IOV CallBack
  virtual StatusCode callBack(IOVSVC_CALLBACK_ARGS) = 0;
  
  virtual GeoDetModulePixel* getModule(const PixelGeoBuilderBasics*, int moduleIndex) =0;
  virtual GeoDetModulePixel* getModule(const PixelGeoBuilderBasics*, std::string moduleType) =0;

  virtual GeoDetModulePixel* getModule(const PixelGeoBuilderBasics*, int brl_ec, int layerdisk, int moduleIndex) =0;
  virtual GeoDetModulePixel* getModule(const PixelGeoBuilderBasics*, int brl_ec, int layerdisk, std::string moduleType) =0;

  virtual InDet::GeoComponent* getModuleGeoComp(const PixelGeoBuilderBasics*, int brl_ec, int layerdisk, int moduleIndex) =0;
  virtual InDet::GeoComponent* getModuleGeoComp(const PixelGeoBuilderBasics*, int brl_ec, int layerdisk, std::string moduleType) =0;

  virtual double getThickness(int moduleIndex) const =0;
  virtual double getThicknessN(int moduleIndex) const =0;
  virtual double getThicknessP(int moduleIndex) const =0;
  virtual double getWidth(int moduleIndex) const =0;
  virtual double getLength(int moduleIndex) const =0;
  virtual int getChipNumber(int moduleIndex) const =0;

};

inline const InterfaceID & IPixelModuleSvc::interfaceID(){
  static const InterfaceID IID_IPixelModuleSvc("IPixelModuleSvc",1,0);
  return IID_IPixelModuleSvc;
}

#endif
  
