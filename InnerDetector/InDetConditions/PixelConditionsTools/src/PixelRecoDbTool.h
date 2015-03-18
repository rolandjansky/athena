/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelRecoDbTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef PIXELCALIBTOOL_PIXELRECODBTOOL_H
#define PIXELCALIBTOOL_PIXELRECODBTOOL_H

// Gaudi includes
#include <vector>
#include <string>
//#include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
//#include "GaudiKernel/MsgStream.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaKernel/IOVSvcDefs.h" 
#include "PixelConditionsData/PixelOfflineCalibData.h" 
//#include "StoreGate/DataHandle.h"
#include "AthenaKernel/IAddressProvider.h" 
#include "PixelConditionsTools/IPixelRecoDbTool.h"  // ????


class Identifier;
//class StoreGateSvc;
class IIOVRegistrationSvc; 
class IAthenaOutputStreamTool; 

class IToolSvcl; 
class IIOVSvc; 

static const InterfaceID IID_PixelRecoDbTool("IPixelRecoDbTool", 1, 0);

// No NameSpace selected 

/** @class PixelRecoDbTool

    The PixelRecoDbTool is used to create and manage the data objects holding 
the calibration data required for the Pixel Offline reconstruction. 
    It will read the calibration data from the offline COOL database via a 
callback and converts to PixelOfflineCalibData objects stored in the detector 
store in Athena. It will also provide a tool to read/write a text file from/to 
the COOL database.

    @author  Tommaso Lari <TOMMASO.LARI@MI.INFN.IT>
*/  

//namespace PixelCalib
//{

class PixelRecoDbTool: public AthAlgTool,
                        virtual public IPixelRecoDbTool{
			
public:

     /** Standard Athena-Algorithm Constructor */
  PixelRecoDbTool(const std::string& type, const std::string& name, 
                  const IInterface* parent); 

    /** required by the IAddressProvider interface **/
  virtual StatusCode updateAddress(StoreID::type storeID,SG::TransientAddress* tad); 

     /** Default Destructor */
  virtual ~PixelRecoDbTool();

     /** standard Athena-Algorithm method */
  virtual StatusCode          initialize();
     /** standard Athena-Algorithm method */
  virtual StatusCode          finalize();
  /** access to interfaceID */
  static const InterfaceID& interfaceID(){return IID_IPixelRecoDbTool;};
  //virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvIf ) ;

  /** Call back function for Calibration folders from COOL database */ 
  virtual StatusCode IOVCallBack(IOVSVC_CALLBACK_ARGS); 

  //<! create PixelOfflineCalibData Objects from scratch 
  StatusCode createPixelCalibObjects() const; 
  //<! write PixelOfflineCalibData from a text file to db
  StatusCode writePixelCalibTextFiletoDB() const; 
  //<! read PixelOfflineCalibData from db to a text file
  StatusCode readPixelCalibDBtoTextFile() const; 
  //<! print all PixelOfflineCalibData objects 
  void printPixelOfflineCalibObjects() const;

  // get Pixel Offline Calibration Data
  PixelCalib::PixelOfflineCalibData* getCalibPtr() const;

  const PixelCalib::PixelOfflineCalibData* cgetCalibPtr() const; 

private:

  void PrintConstants(const float* constants);    
  StatusCode registerCallBack();

  //mutable MsgStream m_log;
  //StoreGateSvc*  m_sgSvc;
  //StoreGateSvc* m_detStore; 
  IToolSvc* m_toolsvc; 
  IIOVSvc* m_IOVSvc; 
  int m_inputSource;
  std::string par_calibfolder;
  std::string par_caliblocation; 
  std::string m_textFileName1;
  std::string m_textFileName2;
  std::string m_textFileName3;
  int m_PixelClusterErrorDataVersion;
  int m_PixelClusterOnTrackErrorDataVersion;
  int m_PixelChargeInterpolationDataVersion;
  int m_dump;
  mutable PixelCalib::PixelOfflineCalibData* m_calibData; 
    
};
/*  inline StatusCode PixelRecoDbTool::queryInterface(  */
/*     const InterfaceID& riid, void** ppvIf ) */
/*    { */
/*      if ( riid == interfaceID() ) { */
/*        *ppvIf = this; */
/*        addRef(); */
/*        return StatusCode::SUCCESS; */
/*      } */
/*      return AthAlgTool::queryInterface( riid, ppvIf ); */
/*    } */
                                                                               
inline PixelCalib::PixelOfflineCalibData*
       PixelRecoDbTool::getCalibPtr() const {
    if(!m_calibData) msg(MSG::WARNING) << "No calibrations! " << endreq;
    return m_calibData;
 }

inline const PixelCalib::PixelOfflineCalibData*  
       PixelRecoDbTool::cgetCalibPtr() const {
    return m_calibData;
 }

//}
#endif 







