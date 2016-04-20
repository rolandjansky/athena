/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// PixelDCSSvc.h
// 
// Service to get Pixel DCS data from database
//
// 09-13-2007
//
// author Georges AAD <aad@cppm.in2p3.fr>
//
//****************************************************************************


#ifndef PIXEL_DCS_SVC_H
#define PIXEL_DCS_SVC_H

//Athena headers
#include "AthenaBaseComps/AthService.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "InDetIdentifier/PixelID.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

#include "PixelConditionsData/PixelDCSData.h"
#include "PixelConditionsServices/IPixelDCSSvc.h"

//C++ standard library headers
#include <vector>
#include <string>



class StoreGateSvc;
namespace InDetDD{ 
  class PixelDetectorManager; 
}


class PixelDCSSvc: public AthService,
		    virtual public IPixelDCSSvc
{

 public:
  PixelDCSSvc(const std::string& type, ISvcLocator* sl);
  virtual ~PixelDCSSvc();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  
  static const InterfaceID& interfaceID();
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvIf );

  virtual StatusCode IOVCallBack(IOVSVC_CALLBACK_ARGS); 

  virtual double getTemperature(const Identifier& module_id) const;
  virtual double getHV(const Identifier& module_id) const;
  virtual std::string getFSMStatus(const Identifier& module_id) const;
  virtual std::string getFSMState(const Identifier& module_id) const;

  virtual double getTemperature(const IdentifierHash& id_hash) const;
  virtual double getHV(const IdentifierHash& id_hash) const;
  virtual std::string getFSMStatus(const IdentifierHash& id_hash) const;
  virtual std::string getFSMState(const IdentifierHash& id_hash) const;


  StatusCode printData() const;

 private:

  StatusCode createDCSData();

  ServiceHandle< StoreGateSvc > m_detStore; 

  PixelDCSData* m_pixelDCSData;

  const PixelID* m_pixid; 
  const InDetDD::PixelDetectorManager* m_pixman; 

  std::string par_temperatureKey; 
  std::string par_HVKey;  
  std::string par_FSMStatusKey; 
  std::string par_FSMStateKey; 
  std::string par_pixelDCSDataSGKey;

  std::string par_temperatureField;
  std::string par_HVField;
  std::string par_FSMStatusField;
  std::string par_FSMStateField;

  bool par_useTemperature;
  bool par_useHV;
  bool par_useFSMStatus;
  bool par_useFSMState;

  bool par_registerCallback;

};

inline const InterfaceID& PixelDCSSvc::interfaceID(){
  static const InterfaceID IID_PixelDCSSvc("PixelDCSSvc", 1, 0);
  return IID_PixelDCSSvc;
}


inline double PixelDCSSvc::getTemperature(const Identifier& module_id) const{

  if(!m_pixid->is_pixel(module_id)){
    msg(MSG::ERROR) << " getTemperature: wrong identifier returning -9999" << endreq;
    return -9999;
  }
  IdentifierHash id_hash = m_pixid->wafer_hash(module_id);
  return (*m_pixelDCSData)[(unsigned int)id_hash]->getTemperature();

}

inline double PixelDCSSvc::getHV(const Identifier& module_id) const{

  if(!m_pixid->is_pixel(module_id)){
    msg(MSG::ERROR) << "getHV : wrong identifier returning -9999" << endreq;
    return -9999;
  }
  IdentifierHash id_hash = m_pixid->wafer_hash(module_id);
  return (*m_pixelDCSData)[(unsigned int)id_hash]->getHV();

}

inline std::string PixelDCSSvc::getFSMStatus(const Identifier& module_id) const{

  if(!m_pixid->is_pixel(module_id)){
    msg(MSG::ERROR) << "getFSMStatus : wrong identifier returning NO_DATA" << endreq;
    return "NO_DATA";
  }
  IdentifierHash id_hash = m_pixid->wafer_hash(module_id);
  return (*m_pixelDCSData)[(unsigned int)id_hash]->getFSMStatus();

}

inline std::string PixelDCSSvc::getFSMState(const Identifier& module_id) const{

  if(!m_pixid->is_pixel(module_id)){
    msg(MSG::ERROR) << "getFSMState : wrong identifier returning NO_DATA" << endreq;
    return "NO_DATA";
  }
  IdentifierHash id_hash = m_pixid->wafer_hash(module_id);
  return (*m_pixelDCSData)[(unsigned int)id_hash]->getFSMState();

}

//// method with IdentifierHash

inline double PixelDCSSvc::getTemperature(const IdentifierHash& id_hash) const{

  unsigned int id = (unsigned int)id_hash;

  if( m_pixelDCSData->size() <= id ){
    if (msgLvl(MSG::ERROR)) msg() << " getTemperature: IdentifierHash "<< id << " not valid returning -9999" << endreq;
    return -9999;
  }

  return (*m_pixelDCSData)[id]->getTemperature();

}

inline double PixelDCSSvc::getHV(const IdentifierHash& id_hash) const{

  unsigned int id = (unsigned int)id_hash;

  if( m_pixelDCSData->size() <= id ){
    if (msgLvl(MSG::ERROR)) msg() <<" getHV: IdentifierHash "<< id << " not valid returning -9999" << endreq;
    return -9999;
  }

  return (*m_pixelDCSData)[id]->getHV();

}

inline std::string PixelDCSSvc::getFSMStatus(const IdentifierHash& id_hash) const{

   unsigned int id = (unsigned int)id_hash;

  if( m_pixelDCSData->size() <= id ){
    msg(MSG::ERROR) << " getFSMStatus: IdentifierHash "<< id << " not valid returning NO_DATA" << endreq;
    return "NO_DATA";
  }
  return (*m_pixelDCSData)[id]->getFSMStatus();

}

inline std::string PixelDCSSvc::getFSMState(const IdentifierHash& id_hash) const{


  unsigned int id = (unsigned int)id_hash;
   
  if( m_pixelDCSData->size() <= id ){
     msg(MSG::ERROR) << " getFSMState: IdentifierHash "<< id << " not valid returning NO_DATA" << endreq;
     return "NO_DATA";
  }
  return (*m_pixelDCSData)[id]->getFSMState();

}



#endif // PIXEL_DCS_SVC_H




