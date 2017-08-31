/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELRECODBTOOL_IPIXELRECODBTOOL_H 
#define PIXELRECODBTOOL_IPIXELRECODBTOOL_H 

// Includes for Gaudi 
#include "GaudiKernel/IAlgTool.h" 
#include "AthenaKernel/IOVSvcDefs.h" 
#include "AthenaKernel/IAddressProvider.h" 

namespace PixelCalib
{
  class PixelOfflineCalibData;
}

// No NameSpace selected 

/** @class IPixelRecoDbTool

    The IPixelRecoDbTool is used to create and manage the data objects holding 
the calibration data required for the Pixel Offline reconstruction. 
    It will read the calibration data from the offline COOL database via a 
callback and converts to PixelOfflineCalibData objects stored in the detector 
store in Athena. It will also provide a tool to read/write a text file from/to 
the COOL database.

    @author  Tommaso Lari <TOMMASO.LARI@MI.INFN.IT>
*/  

static const InterfaceID IID_IPixelRecoDbTool("IPixelRecoDbTool", 1, 0);


class IPixelRecoDbTool: virtual public IAlgTool, 
                        virtual public IAddressProvider { 
			   
public: 




    /** required by the IAddressProvider interface **/
  virtual StatusCode updateAddress(StoreID::type storeID,
                                   SG::TransientAddress* tad,
                                   const EventContext& ctx)=0; 

     /** Default Destructor */
  virtual ~IPixelRecoDbTool(){};

     /** standard Athena-Algorithm method */
  virtual StatusCode          initialize()=0;
     /** standard Athena-Algorithm method */
  virtual StatusCode          finalize()=0;
  /** access to interfaceID */
  static const InterfaceID& interfaceID(){return IID_IPixelRecoDbTool;};
  //virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvIf )=0 ;

  /** Call back function for Calibration folders from COOL database */ 
  virtual StatusCode IOVCallBack(IOVSVC_CALLBACK_ARGS)=0; 

  //<! create PixelOfflineCalibData Objects from scratch 
  virtual StatusCode createPixelCalibObjects() const=0; 
  //<! write PixelOfflineCalibData from a text file to db
  virtual StatusCode writePixelCalibTextFiletoDB() const=0; 
  //<! read PixelOfflineCalibData from db to a text file
  virtual StatusCode readPixelCalibDBtoTextFile() const=0; 
  //<! print all PixelOfflineCalibData objects 
  virtual void printPixelOfflineCalibObjects() const=0;

  //<! get Pixel Offline Calibration Data
  virtual PixelCalib::PixelOfflineCalibData* getCalibPtr() const=0;

  virtual const PixelCalib::PixelOfflineCalibData* cgetCalibPtr() const=0; 

};

#endif 
    
