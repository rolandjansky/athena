/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// PixelDCSTool.h
// 
// An example tool that allows to write DCS data to a database
//
// 09-13-2007
//
// author Georges AAD <aad@cppm.in2p3.fr>
//
//****************************************************************************



#ifndef PIXEL_DCS_TOOL_H
#define PIXEL_DCS_TOOL_H

//Gaudi headers
//#include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
//#include "GaudiKernel/MsgStream.h"

//Athena headers
#include "AthenaKernel/IOVSvcDefs.h" 
//#include "StoreGate/DataHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
//#include "Identifier/Identifier.h"
//#include "InDetIdentifier/PixelID.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h" 

#include "PixelConditionsData/PixelDCSData.h"
#include "PixelConditionsTools/IPixelDCSTool.h"

//C++ standard library headers
#include <vector>
#include <string>
#include <stdint.h>
#include <fstream>

class AtlasDetectorID;
//class StoreGateSvc;
class IIOVRegistrationSvc; 
class IAthenaOutputStreamTool; 
class IToolSvcl; 
class IIOVSvc; 
class IClassIDSvc;
class Identifier;
class PixelID;

namespace InDetDD{ 
  class PixelDetectorManager; 
}


class PixelDCSTool: virtual public AthAlgTool,
		    virtual public IPixelDCSTool
{

 public:
  PixelDCSTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~PixelDCSTool();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  
  //virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvIf ) ;

  virtual StatusCode IOVCallBack(IOVSVC_CALLBACK_ARGS); 

  virtual StatusCode createDCSData();
  virtual StatusCode recordDCSData() const;
  virtual StatusCode writeDataToDB() const;

  virtual StatusCode printData() const;

 private:

  virtual StatusCode connectOutput() const;
  virtual StatusCode commitDataToDetectorStore(CondAttrListCollection* attrListColl, std::string dataKey) const;
  virtual StatusCode registerIOVData(std::string dataKey, std::string dataTag) const;


  //mutable MsgStream m_log;
  
  //  StoreGateSvc* m_detStore; 
  //ServiceHandle< StoreGateSvc > m_detStore; 

  //  IToolSvc* m_toolsvc; 
  ServiceHandle< IToolSvc > m_toolsvc; 

  //  IIOVSvc* m_IOVSvc; 
  ServiceHandle< IIOVSvc > m_IOVSvc; 

  //  IIOVRegistrationSvc* m_IOVRegistrationSvc;
  ServiceHandle< IIOVRegistrationSvc > m_IOVRegistrationSvc;

  //  IAthenaOutputStreamTool* m_streamer;
  ToolHandle< IAthenaOutputStreamTool > m_streamer;

  const InDetDD::PixelDetectorManager* m_pixman; 
  const PixelID* m_pixid; 

  PixelDCSData* m_pixelDCSData;
  // mutable PixelModuleDCSData* m_pixelDCSData;


  std::string m_temperatureKey;
  std::string m_HVKey;
  std::string m_FSMStatusKey;
  std::string m_FSMStateKey;

  std::string m_pixelDCSDataSGKey;

  bool m_registerCallback;

  bool m_temperature;
  bool m_HV;
  bool m_FSMStatus;
  bool m_FSMState;

  bool m_writeDefault;

  std::string m_temperatureFile;
  std::string m_HVFile;
  std::string m_FSMStatusFile;
  std::string m_FSMStateFile;

  std::string par_temperatureField;
  std::string par_HVField;
  std::string par_FSMStatusField;
  std::string par_FSMStateField;

  std::string m_temperatureTag;
  std::string m_HVTag;
  std::string m_FSMStatusTag;
  std::string m_FSMStateTag;

  float m_temperatureValue;
  float m_HVValue;
  std::string m_FSMStatusValue;
  std::string m_FSMStateValue;

  int m_maxAttempt;



};


/*
inline StatusCode PixelDCSTool::queryInterface(const InterfaceID& riid, void** ppvIf){
  if (riid == interfaceID()){
    *ppvIf = dynamic_cast<IPixelDCSTool*> (this);
    addRef();
    return StatusCode::SUCCESS;
  }
  return AthAlgTool::queryInterface( riid, ppvIf );
}
*/

#endif // PIXEL_DCS_TOOL_H




