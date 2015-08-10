/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelCalibSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef PIXELOFFLINECALIBSVC_PIXELCALIBSVC_H
#define PIXELOFFLINECALIBSVC_PIXELCALIBSVC_H

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthService.h"
#include "PixelConditionsServices/IPixelOfflineCalibSvc.h"
#include "GaudiKernel/ToolHandle.h"

class StoreGateSvc;
class Identifier;
class IPixelCablingSvc;
class PixelID;
class IBLParameterSvc;

namespace PixelCalib{
  class PixelOfflineCalibData;
}
class IPixelRecoDbTool;

// const InterfaceID IID_PixelOfflineCalibSvc("PixelOfflineCalibSvc", 1, 0); 


  /** @class PixelOfflineCalibSvc 

   The PixelOfflineCalibSvc provides a set of services to access pixel 
   calibration data objects in the detector store. 
   The creation of pixel calibration data is handled by PixelOfflineCalibDbTool
   via a callback function from the cool database.
  
  @author  Tommaso Lari <TLARI@MI.INFN.IT>
  */  

class PixelOfflineCalibSvc : public AthService, virtual public IPixelOfflineCalibSvc 
{
    public:
      PixelOfflineCalibSvc(const std::string& name, ISvcLocator* sl); 

       /** default destructor */
      virtual ~PixelOfflineCalibSvc ();

      //static const InterfaceID& interfaceID() { 
      //           return IID_PixelOfflineCalibSvc;}
      virtual StatusCode queryInterface(const InterfaceID& riid, 
                                        void** ppvIF); 

       /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();

      //virtual StatusCode readPixelCalibDBtoTextFile(std::string file) const;

      /** Pixel errors as a function of pseudorapidity and cluster sizes */ 
      virtual double getBarrelErrorPhi(double eta, int etasize, int phisize) const; 
             //<! get barrel error along phi 
      virtual double getBarrelErrorEta(double eta, int etasize, int phisize) const; 
             //<<! get barrel error along eta
      virtual double getEndCapErrorPhi(int etasize, int phisize) const; 
             //<! get endcap error along phi
      virtual double getEndCapErrorEta(int etasize, int phisize) const; 
             //<! get endcap error along R 
      virtual double getIBLErrorPhi(double eta, int etasize, int phisize) const; 
             //<! get IBL error along phi 
      virtual double getIBLErrorEta(double eta, int etasize, int phisize) const; 
             //<<! get IBL error along eta
      /** Pixel errors as a function of track angle and cluster size */
      virtual double getBarrelNewErrorPhi(double ang, int phisize); 
      virtual double getBarrelNewErrorEta(double eta, int phisize, int etasize); 
      virtual double getIBLNewErrorPhi(double ang, int phisize); 
      virtual double getIBLNewErrorEta(double eta, int etasize); 
      /** Normalization of the charge interpolation correction */ 
      virtual double getBarrelDeltaX(int nrows, float angle) const; // Pixel = 1  
      virtual double getBarrelDeltaY(int ncol, float eta) const;
      virtual bool includesIBLParams() const;
      virtual double getIBLDeltaX(int nrows, float angle) const; // IBL =0
      virtual double getIBLDeltaY(int ncol, float eta) const;
      virtual double getEndcapDeltaX() const;
      virtual double getEndcapDeltaY() const;

      virtual int getIBLToToverflow(Identifier* PixID) const; //{return -99; /* dummy return value */}
      virtual int getIBLToToverflow() const; //{return -99; /* dummy return value */}
      virtual void readHDC();

      virtual StatusCode IOVCallBack_HDC(IOVSVC_CALLBACK_ARGS_P(I, keys));


    private:

      ToolHandle<IPixelRecoDbTool >  m_dbTool;
      mutable PixelCalib::PixelOfflineCalibData* m_pat; 
      ServiceHandle< StoreGateSvc > m_detStore;
      ServiceHandle<IPixelCablingSvc> m_pixelCabling;
      ServiceHandle<StoreGateSvc>  m_sgSvc;
      ServiceHandle<IBLParameterSvc> m_IBLParameterSvc;
      int m_HitDiscCnfg;
      int m_IBLToToverflowBin;
      bool m_HDCfromCOOL;
    
      const PixelID* m_pixel_id;
    }; 

#endif 
