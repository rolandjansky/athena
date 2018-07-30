/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelCalibSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef PIXELCALIBSVC_PIXELCALIBSVC_H
#define PIXELCALIBSVC_PIXELCALIBSVC_H

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "PixelConditionsServices/IPixelCalibSvc.h"
#include "PixelConditionsTools/IPixelCalibDbTool.h"
#include "PixelCabling/IPixelCablingSvc.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"

namespace InDetDD {
  class SiDetectorManager;
}  

class PixelCalibSvc : public AthService, virtual public IPixelCalibSvc {
  public:
    PixelCalibSvc(const std::string& name, ISvcLocator* sl); 

    virtual ~PixelCalibSvc ();
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF); 

    virtual StatusCode initialize();
    virtual StatusCode finalize  ();

    virtual  bool hasCalibData(const Identifier& wafer_id) const; //<! true for wafer_id contains the calibration data 

    virtual int getThreshold(Identifier pix_id) const;
    virtual int getThresholdSigma(Identifier pix_id) const; 
    virtual int getNoise(Identifier pix_id) const; 
    virtual int getTimeWalk(Identifier pix_id) const; 
    virtual float getQ2TotA(Identifier pix_id) const; 
    virtual float getQ2TotE(Identifier pix_id) const;
    virtual float getQ2TotC(Identifier pix_id) const; 
    virtual float getTotRes(Identifier pix_id, float Q) const;
    virtual float getTotMean(Identifier pix_id, float Q) const;
    virtual void disableDb() {m_disableDb = true;}
    virtual float getCharge(Identifier pix_id, float ToT) const;

  private:
    ServiceHandle<StoreGateSvc>      m_detStore;
    ToolHandle<IPixelCalibDbTool>    m_dbTool;
    ServiceHandle<IPixelCablingSvc>  m_pixelCabling;
    const PixelID*                   m_pixid;
    mutable Identifier                   m_wafer_id; //<! wafer_id  

    const InDetDD::SiDetectorManager * m_detManager;

    double m_totparA;
    double m_totparE;
    double m_totparC;
    double m_totparP1; /**< Correction for dispersion */
    double m_totparP2; 
    int    m_discrThresh;          /**< discriminator threshold value */
    int    m_discrThreshSigma;     /**< discriminator threshold sigma value */
    int    m_intimeThresh;         /**< intime threshold */
    int    m_noiseThresh;          /**< threshold noise*/
    bool   m_disableDb;
}; 

#endif 
