/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelCalibSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef PIXELOFFLINECALIBSVC_IPIXELCALIBSVC_H
#define PIXELOFFLINECALIBSVC_IPIXELCALIBSVC_H

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ToolHandle.h"

class StoreGateSvc; 
class Identifier;

namespace PixelCalib{
  class PixelOfflineCalibData;
}
class PixelRecoDbTool;

const InterfaceID IID_IPixelOfflineCalibSvc("IPixelOfflineCalibSvc", 1, 0); 


  /** @class PixelOfflineCalibSvc 

   The PixelOfflineCalibSvc provides a set of services to access pixel 
   calibration data objects in the detector store. 
   The creation of pixel calibration data is handled by PixelOfflineCalibDbTool
   via a callback function from the cool database.
  
  @author  Tommaso Lari <TLARI@MI.INFN.IT>
  */  

class IPixelOfflineCalibSvc : virtual public IInterface 
{
    public:

       /** default destructor */
      virtual ~IPixelOfflineCalibSvc (){};

      static const InterfaceID& interfaceID() { 
                 return IID_IPixelOfflineCalibSvc;}
      virtual StatusCode queryInterface(const InterfaceID& riid, 
                                        void** ppvIF)=0; 

      //virtual StatusCode readPixelCalibDBtoTextFile(std::string file) const =0;

      /** Pixel errors as a function of pseudorapidity and cluster sizes */ 
      virtual double getBarrelErrorPhi(double eta, int etasize, int phisize) const =0; 
             //<! get barrel error along phi 
      virtual double getBarrelErrorEta(double eta, int etasize, int phisize) const =0; 
             //<<! get barrel error along eta
      virtual double getEndCapErrorPhi(int etasize, int phisize) const =0; 
             //<! get endcap error along phi
      virtual double getEndCapErrorEta(int etasize, int phisize) const =0; 
             //<! get endcap error along R 

      virtual bool includesIBLParams() const =0;
      /** IBL errors as a function of pseudorapidity and cluster sizes */
      virtual double getIBLErrorPhi(double eta, int etasize, int phisize) const =0;
      //<! get barrel error along phi
      virtual double getIBLErrorEta(double eta, int etasize, int phisize) const =0;
      //<<! get barrel error along eta

      /** Pixel errors as a function of track angle and cluster size */
      virtual double getBarrelNewErrorPhi(double ang, int phisize)=0; 
      virtual double getBarrelNewErrorEta(double eta, int phisize, int etasize)=0; 
      /** IBL errors as a function of track angle and cluster size */
      virtual double getIBLNewErrorPhi(double ang, int phisize)=0;
      virtual double getIBLNewErrorEta(double eta, int etasize)=0;

      /** Normalization of the charge interpolation correction */ 
      virtual double getBarrelDeltaX(int nrows, float angle) const =0; 
      virtual double getBarrelDeltaY(int ncol, float eta) const =0;
      /** IBL Normalization of the charge interpolation correction */
      virtual double getIBLDeltaX(int nrows, float angle) const =0; 
      virtual double getIBLDeltaY(int ncol, float eta) const =0;

      virtual double getEndcapDeltaX() const =0;
      virtual double getEndcapDeltaY() const =0;

      virtual int getIBLToToverflow(Identifier*) const = 0;
      virtual int getIBLToToverflow() const = 0;

    virtual StatusCode IOVCallBack_HDC(IOVSVC_CALLBACK_ARGS_P(I, keys)) = 0;

    }; 

#endif 
