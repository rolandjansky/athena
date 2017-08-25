/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelRecoDbTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "PixelRecoDbTool.h"
#include "PixelConditionsData/PixelClusterErrorData.h"
#include "PixelConditionsData/PixelClusterOnTrackErrorData.h"
#include "PixelConditionsData/PixelChargeInterpolationParameters.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include "RegistrationServices/IIOVRegistrationSvc.h"
//#include "StoreGate/StoreGateSvc.h"
#include "SGTools/TransientAddress.h" 
#include "CoralBase/Attribute.h" 
#include "CoralBase/AttributeListSpecification.h" 
#include "AthenaPoolUtilities/AthenaAttributeList.h" 
#include "AthenaPoolUtilities/CondAttrListCollection.h" 
#include "AthenaKernel/IIOVDbSvc.h"
#include "DetDescrConditions/DetCondCFloat.h"
#include "PathResolver/PathResolver.h"

// Athena includes
//#include "StoreGate/StoreGate.h"
#include "Identifier/Identifier.h"

//#include "InDetReadoutGeometry/SiDetectorElement.h"
//#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

#include <fstream>
#include <string>
 
//namespace PixelCalib
//{

//================ Constructor =================================================

PixelRecoDbTool::PixelRecoDbTool(const std::string& type, 
         const std::string& name, const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_toolsvc(nullptr),
  m_IOVSvc(nullptr),
  m_par_caliblocation("PixRecoKey"),
  m_calibData(0),
  m_calibDataKey("PixelOfflineCalibData")
{
  declareInterface< IPixelRecoDbTool >(this); 

  declareProperty("InputSource",m_inputSource=2,"Source of data: 0 (none), 1 (text file), 2 (database)");
  declareProperty("PixelClusterErrorDataFile",
                  m_textFileName1="PixelClusterErrorData.txt","Read constants from this file"); 
  declareProperty("PixelClusterOnTrackErrorDataFile",
                  m_textFileName2="PixelClusterOnTrackErrorData.txt","Read constants from this file"); 
  declareProperty("PixelChargeInterpolationDataFile",
                  m_textFileName3="PixelChargeInterpolationData.txt","Read constants from this file"); 
  // declareProperty("PixelClusterErrorDataVersion",
  //              m_PixelClusterErrorDataVersion=1,"format version of pixel cluster error data"); 
  //declareProperty("PixelClusterOnTrackErrorDataVersion",
  //              m_PixelClusterOnTrackErrorDataVersion=1,"format version of pixel cluster on track error data"); 
  //declareProperty("PixelChargeInterpolationDataVersion",
  //              m_PixelClusterOnTrackErrorDataVersion=1,"PixelChargeInterpolationData.txt","Read constants from this file"); 

  declareProperty("CalibFolder", m_par_calibfolder="/PIXEL/PixReco","Folder name. Should not be changed.");
  declareProperty("CalibLocation", m_par_caliblocation);
  declareProperty("DumpConstants", m_dump=0, "Dump constants to text file"); 
  declareProperty("CalibDataKey", m_calibDataKey);
}

//================ Address update =============================================
// What is this stuff ???
StatusCode PixelRecoDbTool::updateAddress(StoreID::type, SG::TransientAddress* tad,
                                          const EventContext& /*ctx*/)
{ 
  CLID clid = tad->clID(); 
  std::string key = tad->name(); 
  if(146316417 == clid && m_par_caliblocation == key)
    { 
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) <<" OK with PixelRecoDataColl "<<endmsg; 
      return StatusCode::SUCCESS; 
    }
  return StatusCode::FAILURE; 
}

//================ Destructor =================================================

PixelRecoDbTool::~PixelRecoDbTool()
{
  delete m_calibData;
}

//================ Initialisation =============================================

StatusCode PixelRecoDbTool::initialize()
{
  // Code entered here will be executed once at program start.
  
  //m_log.setLevel(outputLevel());
  if(msgLvl(MSG::INFO))msg(MSG::INFO) << name() << " initialize()" << endmsg;

  CHECK(service("ToolSvc",m_toolsvc));

  // Get interface to IOVSvc 
  m_IOVSvc = 0; 
  bool CREATEIF(true); 
  CHECK(service("IOVSvc",m_IOVSvc,CREATEIF));

  StatusCode sc;
  switch (m_inputSource) {
  case 0: // just the hardwired constants 
      sc = createPixelCalibObjects();
      if(sc.isFailure()&&msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Create pixel calib objects failed " << endmsg; 
      return StatusCode::SUCCESS;
  case 1: // read the constants from text file
      sc = writePixelCalibTextFiletoDB();
      if(sc.isFailure()&&msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Reading constant from text file failed " << endmsg; 
      return StatusCode::SUCCESS;
  case 2: // read from DB
      sc = m_calibDataKey.initialize();
      if(sc.isSuccess())
	ATH_MSG_DEBUG("Successfully initialized Conditions Handle Key " << m_calibDataKey.fullKey());
      else
	ATH_MSG_DEBUG("Failed to initialize Conditions Handle Key " << m_calibDataKey.fullKey());
      /*
      sc = registerCallBack();
      // text file fall back 
      if(sc.isFailure()){
        ATH_MSG_FATAL( name() << " is configured to use COOL database, but no data found.");
        ATH_MSG_FATAL( "This point to a serious configuration error" );
        ATH_MSG_FATAL( "Please modify your configuration script according to the following:" );
        ATH_MSG_FATAL( "1) If you do not want to acces the conditions information, set" );
        ATH_MSG_FATAL( name() << ".InputSource = 1" );
        ATH_MSG_FATAL( "2) If you want to get proper condition information, add to the IOVDbSvc the pixel reconstruction folder:");
	ATH_MSG_FATAL( "from IOVDbSvc.CondDB import conddb" );
	ATH_MSG_FATAL( "if not conddb.folderRequested('PIXEL/PixReco'):");
	ATH_MSG_FATAL( "    conddb.addFolder('PIXEL_OFL','/PIXEL/PixReco')" );
	ATH_MSG_FATAL( "and check the conditions tag you are using." );
	return StatusCode::FAILURE;
      }
      return StatusCode::SUCCESS;
      */
  }
  
  return sc; 
}

//================ Finalisation ===============================================

StatusCode  PixelRecoDbTool::finalize()
{
  if(msgLvl(MSG::INFO))msg(MSG::INFO) << "PixelRecoDbTool finalize method called" << endmsg; 
  return StatusCode::SUCCESS; 
} 

//-------------------------------------------------------------------------
StatusCode PixelRecoDbTool::createPixelCalibObjects() const 
{
  // Create PixelCalib objects when constants are read in from textfile 
  // Make sure that the objects are not read in by IOV before 
  
  if(msgLvl(MSG::INFO))msg(MSG::INFO)<< "createPixelCalibObjects method called " <<endmsg; 
  // remove the existing PixelCalibDataColl objects 
  if(m_calibData) delete m_calibData; 
  m_calibData = new PixelCalib::PixelOfflineCalibData;

  if(m_calibData){
    return StatusCode::SUCCESS; 
  }
  else{
    return StatusCode::FAILURE; 
  }
}

 

//===========================================================================

StatusCode PixelRecoDbTool::readPixelCalibDBtoTextFile() const{ 

  if(msgLvl(MSG::INFO))msg(MSG::INFO) << " read PixelCalibData to text file " << endmsg;

  // std::ofstream* outfile = new std::ofstream(file.c_str()); 
  
  const PixelCalib::PixelOfflineCalibData* pat = cgetCalibPtr(); 
  PixelCalib::PixelClusterErrorData* pced = pat->getPixelClusterErrorData();
  PixelCalib::PixelChargeInterpolationParameters* pcip = pat->getPixelChargeInterpolationParameters();
  PixelCalib::PixelClusterOnTrackErrorData* pcoted = pat->getPixelClusterOnTrackErrorData();

  pced->Print(m_textFileName1);
  pcoted->Print(m_textFileName2); 
  pcip->Print(m_textFileName3);
  
  if(msgLvl(MSG::INFO))msg(MSG::INFO)<< "Written numbers to text files "<<endmsg; 
  return StatusCode::SUCCESS; 
} 

//===========================================================================
StatusCode PixelRecoDbTool::writePixelCalibTextFiletoDB() const{ 

  // create the calib objects; if all went well, they will be filled with the constants read from text file
  StatusCode sc = createPixelCalibObjects();
  if(sc.isFailure()&&msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Creation of Pixel Calib objects failed"<<endmsg; 

  PixelCalib::PixelClusterErrorData* pced = m_calibData->getPixelClusterErrorData();
  PixelCalib::PixelChargeInterpolationParameters* pcip =  m_calibData->getPixelChargeInterpolationParameters();
  PixelCalib::PixelClusterOnTrackErrorData* pcoted = m_calibData->getPixelClusterOnTrackErrorData();

  // Find and open the text file
  if(msgLvl(MSG::INFO))msg(MSG::INFO)<< "Load PixelErrorData constants from text file" <<endmsg; 
  std::string file_name1 = PathResolver::find_file (m_textFileName1, "DATAPATH");
  if (file_name1.size()==0) {
    if(msgLvl(MSG::WARNING))msg(MSG::WARNING) << "Input file " << file_name1 
          << " not found! Default (hardwired) values to be used!" 
          << endmsg;
  }
  else{
    pced->Load(file_name1);  
  }

  if(msgLvl(MSG::INFO))msg(MSG::INFO)<< "Load PixelClusterOnTrackErrorData constants from text file" <<endmsg; 
  std::string file_name2 = PathResolver::find_file (m_textFileName2, "DATAPATH");
  if (file_name2.size()==0) {
    if(msgLvl(MSG::WARNING))msg(MSG::WARNING) << "Input file " << file_name2 
          << " not found! Default (hardwired) values to be used!" 
          << endmsg;
  }
  else{
    pcoted->Load(file_name2);
  }

  if(msgLvl(MSG::INFO))msg(MSG::INFO)<< "Load PixelChargeInterpolationData constants from text file" <<endmsg;
  std::string file_name3 = PathResolver::find_file (m_textFileName3, "DATAPATH");
  if (file_name3.size()==0) {
    if(msgLvl(MSG::WARNING))msg(MSG::WARNING) << "Input file " << file_name3 
          << " not found! Default (hardwired) values to be used!" 
          << endmsg;
  }
  else{
    pcip->Load(file_name3);  
  }

  if(m_dump != 0){
    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Dump the constants to file" <<endmsg; 
    m_calibData->Dump();
  }
  // First constants are info on the number of bins of parametrizations
  if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Get error constants" <<endmsg; 
  float* constants = m_calibData->GetConstants();
  if(constants){
     if(msgLvl(MSG::VERBOSE))msg(MSG::VERBOSE) << "constants pointer is defined" <<endmsg; 
  }
  else{
     if(msgLvl(MSG::ERROR))msg(MSG::ERROR) << "constants pointer is NULL!!!" <<endmsg;
  } 
  // compute the number of constants
  const int v1 =  lrintf(constants[0]);  // format version number (pixel cluster error)
  const int v2 =  lrintf(constants[1]);  // format version number (pixel cluster on track error)
  const int v3 =  lrintf(constants[2]);  // format version number (pixel charge interpolation)
  const int n1 =  lrintf(constants[3]); // pixel cluster errors for barrel
  const int n2 =  lrintf(constants[4]); // pixel cluster errors for endcap
  const int n3a = lrintf(constants[5]); // PixelClusterOnTrackError barrel: x cluster size bins
  const int n3b = lrint(constants[6]); // PixelClusterOnTrackError barrel: y cluster size bins
  const int n3c = lrint(constants[7]); // PixelClusterOnTrackError barrel: eta bins
  const int n3d = lrint(constants[8]); // PixelClusterOnTrackError barrel: incidence angle bins
  const int n4a = lrintf(constants[9]); // PixelClusterOnTrackError x cluster size bins
  const int n4b = lrint(constants[10]); // PixelChargeInterpolation y cluster size bins
  const int n4c = lrint(constants[11]); // PixelChargeInterpolation eta bins
  const int n4d = lrint(constants[12]); // PixelChargeInterpolation incidence angle bins
  int offset = 13;
  if(v1<-1||v2<-1||v3<-1)offset +=8;
  const int ncxibl = offset>13 ? lrint(constants[13]): 0; // PixelClusterOnTrackError IBL row size 
  const int ncyibl = offset>13 ? lrint(constants[14]): 0; // PixelClusterOnTrackError IBL column size 
  const int n3e = offset>13 ? lrint(constants[15]): 0; // PixelClusterOnTrackError IBL eta bins 
  const int n3f = offset>13 ? lrint(constants[16]): 0; // PixelClusterOnTrackError IBL phi bins
  const int ncxibl2 = offset>13 ? lrint(constants[17]): 0; // PixelChargeInterpolation IBL row bins 
  const int ncyibl2 = offset>13 ? lrint(constants[18]): 0; // PixelChargeInterpolation IBL column bins 
  const int n4e = offset>13 ? lrint(constants[19]): 0; // PixelChargeInterpolation IBL eta bins
  const int n4f = offset>13 ? lrint(constants[20]): 0; // PixelChargeInterpolation IBL phi bins

  if(msgLvl(MSG::VERBOSE)){ 
      msg(MSG::VERBOSE) << " version numbering: "
			<< v1 << " " << v2 << " " << v3 << endmsg;
      msg(MSG::VERBOSE) 
        << " number of pixel cluster errors for barrel  and endcap is "
	<< constants[3] << " " << constants[4] << endmsg;
      msg(MSG::VERBOSE) 
        << " PixelClusterOnTrack numbers of barrel bins is "  
        << constants[5] << " " << constants[6] << " " 
	<< constants[7] << " " << constants[8] << endmsg; 
      if(offset>13)msg(MSG::VERBOSE) 
		     << " PixelClusterOnTrack numbers of IBL bins is "  
		     << constants[13] << " " << constants[14] <<" "
		     <<constants[15]<<" "<<constants[16]<<endmsg; 
      msg(MSG::VERBOSE) 
	<< " number of charge interpolation bins is "
	<< constants[9] << " " << constants[10] << " " 
	<< constants[11] << " " << constants[12] << endmsg; 
      if(offset>13) msg(MSG::VERBOSE) << " number of charge interpolation IBL bins is "
				      << constants[17] << " " << constants[18] <<" "
				      <<  constants[19] << " " << constants[20] << endmsg;
  }
  const int N0 = 2*n1+2*n2;  // number of cluster error values
  const int N1a = n3a*n3d; // number of cluster on track error values (x direction)
  const int N1b = n3a*n3b*n3c; // number of cluster on track error values (y direction)
  const int N1c = n3a+n3b+n3c+n3d; // number of cluster on track bin values
  const int N2a = 6*n4a*n4d+6*n4b*n4c; // number of charge interpolation values
  const int N2b = n4a+n4b+n4c+n4d+4; // number of charge interpolation bin interval values
  int datasize = N0+N1a+N1b+N1c+N2a+N2b+offset;
  if(offset>13) datasize +=2*n1 + n3e+1 + n3f+1 + ncyibl*n3e + ncxibl*n3f +n4e+1 + n4f+1 + ncxibl2*n4e + ncyibl2*n4f;
  if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Number of offline constants: " << datasize <<endmsg; 

  // check if collection already exits
  if(!detStore()->contains<DetCondCFloat>(m_par_calibfolder)) { 
    DetCondCFloat *const1 = new DetCondCFloat(datasize,m_par_calibfolder); 
    if(msgLvl(MSG::VERBOSE))msg(MSG::VERBOSE) << "Adding constants to the DetCondCFloat object " 
					      <<endmsg;
    const1->add(Identifier(1),constants); 
    if(msgLvl(MSG::VERBOSE))msg(MSG::VERBOSE) << "That's it. " <<endmsg;

  // check if collection already exits 
  //if(!detStore()->contains<DetCondCFloat>(m_par_calibfolder)) { 
    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< " Creating new DetCondCFloat for folder "<<m_par_calibfolder <<endmsg; 
    if(StatusCode::SUCCESS !=detStore()->record(const1,m_par_calibfolder)){ 
      if(msgLvl(MSG::ERROR))msg(MSG::ERROR) <<" Could not create DetCondCFloat "<<m_par_calibfolder 
					    << "with offline parameters" << endmsg; 
      delete const1;
    }
    else{
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << " Created DetCondCFloat "<<m_par_calibfolder 
					    << " with offline parameters" << endmsg; 
    }
  }  
  delete [] constants;
  return StatusCode::SUCCESS; 
}

void PixelRecoDbTool::printPixelOfflineCalibObjects() const
{ }

