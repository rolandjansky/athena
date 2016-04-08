/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArAlignmentAlgs/LArAlignDbAlg.h"

//#include "StoreGate/StoreGate.h"
//#include "GaudiKernel/MsgStream.h"
//#include "GaudiKernel/ToolHandle.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "DetDescrConditions/DetCondKeyTrans.h"

#include <fstream>

#define LAR_ALIGN "/LAR/Align"

using HepGeom::Translate3D;
using HepGeom::Rotate3D;
using CLHEP::Hep3Vector;

/////////////////////////////////////////////////////////////////////////////

LArAlignDbAlg::LArAlignDbAlg(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator),
    m_writeCondObjs(false),
    m_regIOV(false),
    m_streamName("CondStream1"),
    m_inpFile("LArAlign.inp"),
    m_outpFile("LArAlign-TEST.pool.root"),
    m_outpTag("LARAlign-TEST"),
    m_evt(0),
    m_regSvc("IOVRegistrationSvc",name),
    m_streamer("AthenaPoolOutputStreamTool")
{
    declareProperty("WriteCondObjs",     m_writeCondObjs);
    declareProperty("RegisterIOV",       m_regIOV);
    declareProperty("StreamName",        m_streamName);
    declareProperty("InpFile",           m_inpFile);
    declareProperty("OutpFile",           m_outpFile);
    declareProperty("TagName",           m_outpTag);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
LArAlignDbAlg::~LArAlignDbAlg()
{ 
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode LArAlignDbAlg::initialize()
{
  msg(MSG::DEBUG) <<" in initialize()" <<endreq;

  // Get Output Stream tool for writing
  if(m_writeCondObjs) {
    if (m_streamer.retrieve().isFailure()) {
      msg(MSG::ERROR)	<< " Unable to find AthenaPoolOutputStreamTool" << endreq;
      return StatusCode::FAILURE;
    }  
  }
    
  // Get the IOVRegistrationSvc when needed
  if(m_regIOV) {
    if(m_regSvc.retrieve().isFailure()) {
      msg(MSG::ERROR) << "Unable to find IOVRegistrationSvc "	<< endreq;
      return StatusCode::FAILURE;
    }  
    else {
      msg(MSG::DEBUG) << "Found IOVRegistrationSvc "  << endreq;
    }
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode LArAlignDbAlg::execute() 
{
  msg(MSG::DEBUG) <<" in execute() " << endreq;

  StatusCode sc = evtStore()->retrieve(m_evt);
  if(sc.isFailure()) {
    msg(MSG::ERROR) << " could not get event info " << endreq;
    return sc;
  }

  int nrun = m_evt->event_ID()->run_number();
  int nevt = m_evt->event_ID()->event_number();
  msg(MSG::DEBUG) << " Event: [" << nrun << "," << nevt << "]" << endreq;

  // If I need to write out the conditions object I'll do that on the first event
  if (m_writeCondObjs && nevt==1) {
    msg(MSG::DEBUG) << "Creating conditions objects for run " << nrun << endreq;
      
    sc = createCondObjects();

    if(sc.isFailure()) {
      msg(MSG::ERROR) << " Could not create cond objects " << endreq;
      return sc;
    }
      
    //  Read objects from DetectorStore
    sc = printCondObjects();
    if(sc.isFailure()) {
      msg(MSG::ERROR) << " Could not print out cond objects" << endreq;
      return sc;
    }
  } else {
    //  Read objects from DetectorStore
    sc = printCondObjects();
    if(sc.isFailure()) {
      msg(MSG::ERROR) <<" Could not print out cond objects" << endreq;
      return sc;
    }
  }
    
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode LArAlignDbAlg::finalize()
{
  msg(MSG::DEBUG) << " in finalize() " << endreq;
  StatusCode sc = StatusCode::SUCCESS;

  if(m_writeCondObjs) {
    sc = streamOutCondObjects();
    if(sc.isFailure()) {
      msg(MSG::ERROR) << " Could not stream out objects" << endreq;
      return sc;
    } else {
      msg(MSG::DEBUG) << " Streamed out OK " << endreq;
    }
  }

  if(m_regIOV) {
    sc = registerCondObjects();
    if(sc.isFailure()) {
      msg(MSG::ERROR) << " Could not register objects" << endreq;
      return sc;
    } else {
      msg(MSG::DEBUG) << " Register OK " << endreq;
    }
  }
  
  return sc;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode LArAlignDbAlg::createCondObjects()
{
  msg(MSG::INFO) <<" in createCondObjects() " <<endreq;
  
  if(detStore()->contains<DetCondKeyTrans>(LAR_ALIGN)) {
    msg(MSG::INFO) << " DetCondKeyTrans already exists, do nothing " <<endreq;
    return StatusCode::SUCCESS;
  }

  DetCondKeyTrans* transforms = new DetCondKeyTrans();

  // Read input file, construct relevant transforms
  std::ifstream infile;
  infile.open(m_inpFile.value().c_str());

  if(!infile.is_open()) {
    msg(MSG::ERROR) << "Unable to open " << m_inpFile << " for reading" << endreq;
    return StatusCode::FAILURE;
  }

  char commentSign = '#';
  std::string commentLine;
  std::string key;
  double x, y, z, theta, phi, rotationAngle;

  while(!infile.eof()) {
    infile >> key;
    if(key.empty()) continue;
    if(key[0]==commentSign)
      std::getline(infile,commentLine);
    else {
      infile >> theta >> phi >> rotationAngle >> x >> y >> z;

      Hep3Vector axis(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));
      transforms->setTransform(key,Translate3D(x,y,z)*Rotate3D(rotationAngle,axis));
    }
  }
  infile.close();

  StatusCode sc = detStore()->record(transforms,LAR_ALIGN);
  
  if (sc.isFailure()) {
    msg(MSG::ERROR) << " Could not record LAR/Align " << endreq;
    return sc;
  }
  else
    msg(MSG::DEBUG) << " Recorded LAr/Align " << endreq;
  
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode LArAlignDbAlg::printCondObjects()
{
  const DetCondKeyTrans* align;
  StatusCode sc = detStore()->retrieve(align, LAR_ALIGN);

  if(sc.isFailure()) 
    msg(MSG::WARNING) << " Could not find DetCondKeyTrans" <<endreq;
  else if(0 == align) 
    msg(MSG::WARNING) <<" DetCondKeyTrans ptr is 0" <<endreq;
  else {
    std::cout << " \n\n**************************************************** \n";
    std::cout << " ****                                            **** \n";
    std::cout << " ****         Printing Conditions Objects        **** \n";
    std::cout << " ****                                            **** \n";
    std::cout << " **************************************************** \n";
    
    align->print();
    
    std::cout << " ****  **** **** ****     END     **** **** **** **** \n\n\n";
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode LArAlignDbAlg::streamOutCondObjects()
{
  msg(MSG::DEBUG) << " entering streamOutCondObjects "  << endreq;

  StatusCode sc = m_streamer->connectOutput(m_outpFile.value());
  if(sc.isFailure()) {
    msg(MSG::ERROR) << " Could not connect stream to output" <<endreq;
    return sc;
  }
  else
    msg(MSG::DEBUG) <<" Did connect stream to output" <<endreq;

  int npairs = 1;

  IAthenaOutputStreamTool::TypeKeyPairs typeKeys(npairs);

  IAthenaOutputStreamTool::TypeKeyPair align("DetCondKeyTrans", LAR_ALIGN);
  typeKeys[0] = align;
  
  sc = m_streamer->streamObjects(typeKeys);
  if(sc.isFailure()) {
    msg(MSG::ERROR) << " Could not stream out LAr Alignment " << endreq;
    return sc;
  }
    
  sc = m_streamer->commitOutput();
  if(sc.isFailure()) {
    msg(MSG::ERROR) << " Could not commit output stream " << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode LArAlignDbAlg::registerCondObjects()
{
  msg(MSG::DEBUG) << "entering registerCondObject()"  << endreq;
  
  std::string objname = "DetCondKeyTrans";

  // Register the IOV DB with the conditions data written out
  StatusCode sc = m_regSvc->registerIOV(objname, m_outpTag);
  if(sc.isFailure()) {
    msg(MSG::ERROR) << " Could not register (" << objname << ", " << m_outpTag << ") in IOV DB " << endreq;
    return sc;
  }
  
  return StatusCode::SUCCESS;
}

