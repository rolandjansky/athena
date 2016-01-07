/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelCalibDbTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef PIXELCALIBTOOL_PIXELCALIBDBTOOL_H
#define PIXELCALIBTOOL_PIXELCALIBDBTOOL_H

// Gaudi includes
#include <vector>
#include <string>
#include <map>
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h" 
#include "PixelConditionsData/PixelCalibDataColl.h"
#include "AthenaKernel/IAddressProvider.h" 
#include "PixelConditionsTools/IPixelCalibDbTool.h"

class AtlasDetectorID;
class Identifier;
class IIOVRegistrationSvc; 
class IAthenaOutputStreamTool; 
class PixelID; 
class IToolSvcl; 
class IIOVSvc; 

namespace InDetDD{ 
  class PixelDetectorManager; 
}

//static const InterfaceID IID_PixelCalibDbTool("IPixelCalibDbTool", 1, 0);

// No NameSpace selected 

/** @class PixelCalibDbTool

    The PixelCalibDbTool is used to create and manage the data objects holding the calibration data for the 
    Pixel detector. It reads the calibration data from the offline COOL database via a callback and converts 
    to PixelCalibData objects stored in the detector store in Athena. It also provides a tool to read/write 
    a text file from/to the COOL database.

    @author  Weiming Yao <WMYAO@LBL.GOV>
*/  

//namespace PixelCalib
//{

class PixelCalibDbTool: public AthAlgTool,
                        virtual public IPixelCalibDbTool{
			
public:

     /** Standard Athena-Algorithm Constructor */
  PixelCalibDbTool(const std::string& type, const std::string& name, const IInterface* parent); 

    /** required by the IAddressProvider interface **/
  virtual StatusCode updateAddress(StoreID::type storeID, SG::TransientAddress* tad); 

     /** Default Destructor */
  virtual ~PixelCalibDbTool();

     /** standard Athena-Algorithm method */
  virtual StatusCode          initialize();
     /** standard Athena-Algorithm method */
  virtual StatusCode          finalize();
  /** access to interfaceID */
  //static const InterfaceID& interfaceID(){return IID_PixelCalibDbTool;};
  //virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvIf ) ;

  /** Call back function for Calibration folders from COOL database */ 
  virtual StatusCode IOVCallBack(IOVSVC_CALLBACK_ARGS); 

  StatusCode createPixelCalibObjects() const; //<! create PixelCalibData Objects from scratch 
  StatusCode writePixelCalibTextFiletoDB(std::string file) const; //<! write PixelcalibData from a text file to db
  StatusCode readPixelCalibDBtoTextFile(std::string file) const; //<! read PixelcalibData from db to a text file
  void printPixelCalibObjects() const; //<! print all PixelCalibData objects 

  PixelCalib::PixelCalibData* getCalibPtr(const Identifier& idet) const; //<! get a Pixelcalib pointer for idet
  const PixelCalib::PixelCalibData* cgetCalibPtr(const Identifier& idet ) const; //<! get a const pointer for idet

private:
    
  IToolSvc* m_toolsvc; 
  IIOVSvc* m_IOVSvc; 
  const PixelID* m_pixid; 
  const InDetDD::PixelDetectorManager* m_pixman; 
  std::string par_calibfolder; 
  std::string par_caliblocation; 
  BooleanProperty par_writedb;

  // added -- A.X.
  int m_useCoral;
  std::string m_connString;
  std::string m_tableName;
  std::string m_dbTag;
  int m_dbRevision;

  std::vector<std::pair<Identifier,int> > m_calibobjs; 
  mutable PixelCalibDataColl* m_calibData; 
    
};

/*
 inline StatusCode PixelCalibDbTool::queryInterface( const InterfaceID& riid, void** ppvIf )
   {
     if ( riid == interfaceID() ) {
       *ppvIf = this;
       addRef();
       return StatusCode::SUCCESS;
     }
     return AthAlgTool::queryInterface( riid, ppvIf );
   }
*/                                                                                


//}
#endif 







