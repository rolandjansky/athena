/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelCalibSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef PIXELCALIBSVC_IPIXELCALIBSVC_H
#define PIXELCALIBSVC_IPIXELCALIBSVC_H

//#include "GaudiKernel/Service.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "PixelConditionsData/PixelCalibDataColl.h"


class AtlasDetectorID;
class Identifier;
class StoreGateSvc; 
//class PixelID; 
//class PixelIdMapping;

namespace InDetDD{ 
  class PixelDetectorManager; 
}


class PixelCalibDbTool;


  /** @class IPixelCalibSvc 

   The IPixelCalibSvc provides a set of services to access pixel calibration data objects in the detector store. 
   The creation of pixel calibration data is handled by PixelCalibDbTool via a callback function from the 
   cool database.
  
  @author  Weiming Yao <WMYAO@LBL.GOV>
  */  

static const InterfaceID IID_IPixelCalibSvc("IPixelCalibSvc",1,0);

class IPixelCalibSvc : virtual public IInterface 
{
    public:

       /** default destructor */
      virtual ~IPixelCalibSvc (){};
      static const InterfaceID & interfaceID();

      virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF)=0; 

      /** access to db constants with wafer_id: irow, icol per module index */ 
      virtual bool hasCalibData(const Identifier& wafer_id) const =0; //<! true for wafer_id contains the calibration data 
      virtual int getNFE(const Identifier& wafer_id) const =0; // number of FE Chips per module
      virtual bool isFEI4(const Identifier& wafer_id) const =0; // is FEI4 module


      /** Note: irow icol is for each chip, not phi_index, eta_index per module */ 
      virtual int getThreshold(const Identifier& wafer_id, int irow, int icol, int circ) const =0; //<! get threshold 
      virtual int getThresholdSigma(const Identifier& wafer_id, int irow, int icol,int circ) const =0; //<<! get threshold sigma
      virtual int getNoise(const Identifier& wafer_id, int irow, int icol, int circ) const =0; //<! get Noise 
      virtual int getTimeWalk(const Identifier& wafer_id, int irow, int icol, int circ) const =0; //<! get time walk 
      virtual float getQ2TotA(const Identifier& wafer_id, int irow, int icol, int circ) const =0; //<! get Q to tot conversion constant A
      virtual float getQ2TotE(const Identifier& wafer_id, int irow, int icol, int circ) const =0;//<! get tot conversion constant E
      virtual float getQ2TotC(const Identifier& wafer_id, int irow, int icol, int circ) const =0; //<! get tot conversion constant C
      virtual float getTotP1(const Identifier& wafer_id, int circ) const =0; //<! get tot resolution parameter P1
      virtual float getTotP2(const Identifier& wafer_id, int circ) const =0; //<! get tot resolution parameter P2
      virtual float getTotRes(const Identifier& wafer_id, int circ, float Q) const =0; //<! get tot resolution

      /**  access to db with pix_id: */ 
      virtual int getThreshold(const Identifier& pix_id) const =0;
      virtual int getThresholdSigma(const Identifier& pix_id) const =0; 
      virtual int getNoise(const Identifier& pix_id) const =0; 
      virtual int getTimeWalk(const Identifier& pix_id) const =0; 
      virtual float getQ2TotA(const Identifier& pix_id) const =0; 
      virtual float getQ2TotE(const Identifier& pix_id) const =0;
      virtual float getQ2TotC(const Identifier& pix_id) const =0; 
      virtual float getTotP1(const Identifier& pix_id) const =0;
      virtual float getTotP2(const Identifier& pix_id) const =0;
      virtual float getTotRes(const Identifier& pix_id, float Q) const =0;
      virtual float getTotMean(const Identifier& pix_id, float Q) const =0;
      virtual void disableDb() =0;
      
    }; 


inline  const InterfaceID& IPixelCalibSvc::interfaceID() { 
  return IID_IPixelCalibSvc;
}


#endif 
