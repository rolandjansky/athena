/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file TRT_StrawAlignDbSvc.cxx
 *  @brief AlgTool to manage TRT Conditions data
 *  @author Peter Hansen <phansen@nbi.dk>
 **/

#include "TRT_StrawAlignDbSvc.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/IToolSvc.h"
#include "RegistrationServices/IIOVRegistrationSvc.h"
#include "AthenaKernel/IAthenaOutputStreamTool.h"

#include "Identifier/Identifier.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"


TRT_StrawAlignDbSvc::TRT_StrawAlignDbSvc( const std::string& name,
					  ISvcLocator* pSvcLocator )
  : AthService(name,pSvcLocator),
    m_detStore("DetectorStore",name),
    m_par_dxcontainerkey("/TRT/Calib/DX"),
    m_par_strawtextfile(""),
    m_par_forcecallback(false),
    m_trtid(0),
    m_trtman(0),
    m_streamer("AthenaPoolOutputStreamTool/CondStream1")
{
  declareProperty("StrawTextFile",m_par_strawtextfile);
  declareProperty("ForceCallback",m_par_forcecallback);
  declareProperty("StreamTool",m_streamer);
  declareProperty("DetectorStore",m_detStore);
}


TRT_StrawAlignDbSvc::~TRT_StrawAlignDbSvc()
{}


StatusCode TRT_StrawAlignDbSvc::initialize() 
{
  msg(MSG::INFO) << "TRT_StrawAlignDbSvc initialize method called" << endmsg;

  // Get the TRT ID helper
  if (StatusCode::SUCCESS!=m_detStore->retrieve(m_trtid,"TRT_ID")) {
    msg(MSG::FATAL) << "Problem retrieving TRTID helper" << endmsg;
    return StatusCode::FAILURE;
  }

  // Get the geometry.
  if (StatusCode::SUCCESS!=m_detStore->retrieve(m_trtman,"TRT") || 
      m_trtman==0) {
    msg(MSG::FATAL) << "Could not find TRT manager " << endmsg;
    return StatusCode::FAILURE;
  }
  

  bool dxcontainerexists = m_detStore->StoreGateSvc::contains<StrawDxContainer>(m_par_dxcontainerkey) ;
  
  if( dxcontainerexists ) {
    msg(MSG::INFO) << " dx container exists - reg callback " << endmsg ;
    if( (m_detStore->regFcn(&TRT_StrawAlignDbSvc::IOVCallBack,this,m_dxcontainer,m_par_dxcontainerkey)).isFailure()) 
      msg(MSG::ERROR) << "Could not register IOV callback for key: " << m_par_dxcontainerkey << endmsg ;

  } else {
    
    // create, record and update data handle
    msg(MSG::INFO) << "Creating new dx container" << endmsg ;
    const StrawDxContainer* dxcontainer = new StrawDxContainer() ;

    if( (m_detStore->record(dxcontainer,m_par_dxcontainerkey))!=StatusCode::SUCCESS ) {
      msg(MSG::ERROR) << "Could not record StrawDxContainer for key " << m_par_dxcontainerkey << endmsg;
    }

    if(StatusCode::SUCCESS!=m_detStore->retrieve(m_dxcontainer,m_par_dxcontainerkey)) {
      msg(MSG::FATAL) << "Could not retrieve data handle for StrawDxContainer " << endmsg;
      return StatusCode::FAILURE ;
    }
    
    msg(MSG::INFO) << "Created StrawDxContainer in detstore" << endmsg;
    
    // reading from file 
    if( !m_par_strawtextfile.empty() ) {
      if(StatusCode::SUCCESS!=this->readTextFile(m_par_strawtextfile)) {
	msg(MSG::FATAL) << "Could not read objects from text file" << m_par_strawtextfile << endmsg;
	return StatusCode::FAILURE ;
      }
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode TRT_StrawAlignDbSvc::finalize()
{
  msg(MSG::INFO) << "TRT_StrawAlignDbSvc finalize method called" << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode TRT_StrawAlignDbSvc::writeTextFile(const std::string& filename) const
{
  msg(MSG::INFO) << " Write straw alignment data to text file " << filename << endmsg ;
  std::ofstream outfile(filename.c_str());


  // first reduce the container as much as possible
  // getDxContainer()->crunch() ;

  StrawDxContainer::FlatContainer packedstrawdata ;
  m_dxcontainer->getall( packedstrawdata ) ;
  
  // now, we store the entries
  msg(MSG::INFO) << "Number of entries in flatcontainer: " 
	<< packedstrawdata.size() << std::endl ;
  
  for( TRTCond::StrawDxContainer::FlatContainer::iterator it = packedstrawdata.begin() ;
       it != packedstrawdata.end(); ++it) {
    const TRTCond::ExpandedIdentifier& calid = it->first ;
    // get the end point corrections. if not the right type, store zeros.
    float dx1=m_dxcontainer->getDx1(calid) ; 
    float dx2=m_dxcontainer->getDx2(calid) ; 
    float dxerr=m_dxcontainer->getDxErr(calid) ;
    outfile << calid << " " 
	    << std::setprecision(5)
	    << std::setw(12) << dx1 << " " 
	    << std::setw(12) << dx2 << " " 
	    << std::setw(12) << dxerr << std::endl ; 
  }
  outfile.close() ;
  return StatusCode::SUCCESS ;
}
 
 

StatusCode TRT_StrawAlignDbSvc::readTextFile(const std::string& filename) 
{
  msg(MSG::INFO) << "Reading straw alignment data from text file " << filename << endmsg ;

  StrawDxContainer* dxcontainer = getDxContainer() ;
  if(!dxcontainer) {
    msg(MSG::WARNING) << " Could not find the container " << endmsg;
    return StatusCode::FAILURE;
  }
  dxcontainer->clear() ;
  std::ifstream infile(filename.c_str()) ;
  TRTCond::ExpandedIdentifier id ;
  float dx1,dx2,dxerr;
  int nentries(0) ;
  while ((infile >> id >> dx1 >> dx2 >> dxerr ) ) {
    setDx(id,dx1,dx2,dxerr) ;
    if(msgLvl(MSG::DEBUG)) msg() << " read from file: dx1 " << dx1 << " dx2 " << dx2 << " dxerr " << dxerr << endmsg;
    ++nentries ;
  }
  size_t dxfootprint = dxcontainer->footprint()  ;
  msg(MSG::INFO) << " read " << nentries << " from file. " << endmsg ; 
  msg(MSG::INFO) << " dx footprints " << dxfootprint << endmsg;
  msg(MSG::INFO) << " (no compression) " << endmsg ;

  // force a call back in the geometry
  int i(0);
  std::list<std::string> keys ;
  (const_cast<InDetDD::TRT_DetectorManager*>(m_trtman))->align(i,keys) ;

  return StatusCode::SUCCESS ;
}


StatusCode TRT_StrawAlignDbSvc::streamOutObjects() const
{
  msg(MSG::INFO) << "entering streamOutObjects "  << endmsg;
  StatusCode sc;
  
  // Get Output Stream tool for writing
  sc = m_streamer.retrieve();
  
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to find AthenaOutputStreamTool" << endmsg;
    return StatusCode::FAILURE;
    }
  
  IAthenaOutputStreamTool*  streamer=const_cast<IAthenaOutputStreamTool*>(&(*m_streamer));
    
  sc = streamer->connectOutput();
  if (sc.isFailure()) {
    msg(MSG::ERROR) <<"Could not connect stream to output" <<endmsg;
    return( StatusCode::FAILURE);
  }
  
  IAthenaOutputStreamTool::TypeKeyPairs typeKeys;
  typeKeys.push_back( IAthenaOutputStreamTool::TypeKeyPair(StrawDxContainer::classname(),m_par_dxcontainerkey)) ;
  getDxContainer()->crunch() ;
  
  sc = streamer->streamObjects(typeKeys);
  if (sc.isFailure()) {
    msg(MSG::ERROR) <<"Could not stream out containers " <<endmsg;
    return( StatusCode::FAILURE);
  }
  
  sc = streamer->commitOutput();
  if (sc.isFailure()) {
    msg(MSG::ERROR) <<"Could not commit output stream" <<endmsg;
    return( StatusCode::FAILURE);
  }
  
  msg(MSG::INFO) << "   Streamed out and committed "  << typeKeys.size() << " objects " << endmsg;
  return StatusCode::SUCCESS;
}


StatusCode TRT_StrawAlignDbSvc::registerObjects(std::string tag, int run1, int event1, int run2, int event2) const 
{
  msg(MSG::INFO) << "registerObjects with IOV " << endmsg;
  msg(MSG::INFO) << "Run/evt1 [" << run1 << "," << event1 << "]" << endmsg;
  msg(MSG::INFO) << "Run/evt2 [" << run2 << "," << event2 << "]" << endmsg;
  
  // get pointer to registration svc
  IIOVRegistrationSvc* regsvc;
  if (StatusCode::SUCCESS!=service("IOVRegistrationSvc",regsvc)) {
    msg(MSG::FATAL) << "IOVRegistrationSvc not found" << endmsg;
    return( StatusCode::FAILURE);
  }
  
  if (StatusCode::SUCCESS==regsvc->registerIOV(StrawDxContainer::classname(),
					       m_par_dxcontainerkey,tag,run1,run2,event1,event2))
    msg(MSG::INFO) << "Registered StrawDxContainer object with key " << m_par_dxcontainerkey << endmsg ;
  else 
    msg(MSG::ERROR) << "Could not register StrawDxContainer object with key " << m_par_dxcontainerkey << endmsg ;
  
  return( StatusCode::SUCCESS);
}


StatusCode TRT_StrawAlignDbSvc::IOVCallBack(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  for (std::list<std::string>::const_iterator 
	 itr=keys.begin(); itr!=keys.end(); ++itr) 
    msg(MSG::INFO) << " IOVCALLBACK for key " << *itr << " number " << I << endmsg;
  
  // if constants need to be read from textfile, we sue the call back routine to refill the IOV objects
  if(!m_par_strawtextfile.empty()) return readTextFile( m_par_strawtextfile ) ;
  
  return StatusCode::SUCCESS;
}

