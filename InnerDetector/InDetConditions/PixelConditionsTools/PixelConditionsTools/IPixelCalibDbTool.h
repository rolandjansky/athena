/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCALIBTOOL_IPIXELCALIBDBTOOL_H 
#define PIXELCALIBTOOL_IPIXELCALIBDBTOOL_H 

// Includes for Gaudi 
#include "GaudiKernel/IAlgTool.h" 
#include "AthenaKernel/IOVSvcDefs.h" 
#include "AthenaKernel/IAddressProvider.h" 



//namespace PixelCalib
//{

static const InterfaceID IID_IPixelCalibDbTool("IPixelCalibDbTool", 1, 0);

class IPixelCalibDbTool: virtual public IAlgTool, 
                         virtual public IAddressProvider { 
			   
public: 
  
    /** required by the IAddressProvider interface **/
  virtual StatusCode updateAddress(StoreID::type storeID,
                                   SG::TransientAddress* tad)=0; 

     /** Default Destructor */
  virtual ~IPixelCalibDbTool(){};

     /** standard Athena-Algorithm method */
  virtual StatusCode          initialize()=0;
     /** standard Athena-Algorithm method */
  virtual StatusCode          finalize()=0;
  /** access to interfaceID */
  static const InterfaceID& interfaceID(){return IID_IPixelCalibDbTool;};

  //virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvIf )=0 ;

  /** Call back function for Calibration folders from COOL database */ 
  virtual StatusCode IOVCallBack(IOVSVC_CALLBACK_ARGS)=0; 

  virtual StatusCode createPixelCalibObjects() const =0; //<! create PixelCalibData Objects from scratch 
  virtual StatusCode writePixelCalibTextFiletoDB(std::string file) const =0; //<! write PixelcalibData from a text file to db
  virtual StatusCode readPixelCalibDBtoTextFile(std::string file) const =0; //<! read PixelcalibData from db to a text file
  virtual void printPixelCalibObjects() const =0; //<! print all PixelCalibData objects 

  virtual PixelCalib::PixelCalibData* getCalibPtr(const Identifier& idet) const =0; //<! get a Pixelcalib pointer for idet
  virtual const PixelCalib::PixelCalibData* cgetCalibPtr(const Identifier& idet ) const =0; //<! get a const pointer for idet

}; 

//}

#endif 
    
