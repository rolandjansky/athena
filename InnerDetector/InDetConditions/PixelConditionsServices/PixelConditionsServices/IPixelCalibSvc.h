/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelCalibSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef PIXELCALIBSVC_IPIXELCALIBSVC_H
#define PIXELCALIBSVC_IPIXELCALIBSVC_H

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IInterface.h"
#include "PixelConditionsData/PixelCalibDataColl.h"

#include "Identifier/Identifier.h"

static const InterfaceID IID_IPixelCalibSvc("IPixelCalibSvc",1,0);

class IPixelCalibSvc : virtual public IInterface {
  public:
    virtual ~IPixelCalibSvc (){};
    static const InterfaceID & interfaceID();

    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF)=0; 

    virtual bool hasCalibData(const Identifier& wafer_id) const =0; //<! true for wafer_id contains the calibration data 

    virtual int getThreshold(Identifier pix_id) const =0;
    virtual int getThresholdSigma(Identifier pix_id) const =0; 
    virtual int getNoise(Identifier pix_id) const =0; 
    virtual int getTimeWalk(Identifier pix_id) const =0; 
    virtual float getQ2TotA(Identifier pix_id) const =0; 
    virtual float getQ2TotE(Identifier pix_id) const =0;
    virtual float getQ2TotC(Identifier pix_id) const =0; 
    virtual float getTotRes(Identifier pix_id, float Q) const =0;
    virtual float getTotMean(Identifier pix_id, float Q) const =0;
    virtual float getCharge(Identifier pix_id, float ToT) const =0;

    virtual void disableDb() =0;
};

inline  const InterfaceID& IPixelCalibSvc::interfaceID() { 
  return IID_IPixelCalibSvc;
}

#endif 
