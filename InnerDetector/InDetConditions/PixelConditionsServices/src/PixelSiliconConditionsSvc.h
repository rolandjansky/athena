/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// PixelSiliconConditionsSvc.h
// 
// Implementation of ISiliconConditionsSvc interface to be used by ID 
// reconstruction 
//
// 02-25-2008
//
// author Georges Aad <aad@cppm.in2p3.fr>
//
#ifndef PIXELCONDITIONSSERVICES_PIXELSILICONCONDITIONSSVC_H
#define PIXELCONDITIONSSERVICES_PIXELSILICONCONDITIONSSVC_H

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaBaseComps/AthService.h"

#include "StoreGate/StoreGateSvc.h"
#include "InDetConditionsSummaryService/ISiliconConditionsSvc.h"
#include "PixelConditionsServices/IPixelDCSSvc.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"

class PixelSiliconConditionsSvc: public AthService, virtual public ISiliconConditionsSvc {
  public:

    PixelSiliconConditionsSvc(const std::string& type, ISvcLocator* sl);
    virtual ~PixelSiliconConditionsSvc();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIf); 

    static const InterfaceID & interfaceID();

    virtual  float temperature(const Identifier & detectorElement);
    virtual  float biasVoltage(const Identifier & detectorElement);
    virtual  float depletionVoltage(const Identifier & detectorElement);

    virtual  float temperature(const IdentifierHash & detectorElement);
    virtual  float biasVoltage(const IdentifierHash & detectorElement);
    virtual  float depletionVoltage(const IdentifierHash & detectorElement);

    virtual StatusCode callBack(IOVSVC_CALLBACK_ARGS);
    virtual bool hasCallBack();

  private:
    ServiceHandle< StoreGateSvc > m_detStore;
    ServiceHandle< IPixelDCSSvc > m_pixelDCSSvc;

    const InDetDD::PixelDetectorManager* m_pixman;
    const PixelID* m_pixid;

    bool m_useDB;
    float m_defaultTemperature;
    float m_defaultBiasVoltage;
    float m_defaultDepletionVoltage;
};

inline const InterfaceID & PixelSiliconConditionsSvc::interfaceID(){
  return ISiliconConditionsSvc::interfaceID();
}

#endif
