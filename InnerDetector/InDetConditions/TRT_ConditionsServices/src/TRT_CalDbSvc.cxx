/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file TRT_CalDbSvc.cxx
 *  @brief AlgTool to manage TRT Conditions data
 *  @author Peter Hansen <phansen@nbi.dk>, Wouter Hulsberger <whulsber@cern.ch>
 **/

#include "TRT_CalDbSvc.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "RegistrationServices/IIOVRegistrationSvc.h"

#include "TRT_ConditionsData/BasicRtRelation.h"
#include "TRT_ConditionsData/DinesRtRelation.h"
#include "TRT_ConditionsData/BinnedRtRelation.h"
#include "TRT_ConditionsData/RtRelationFactory.h"

unsigned int TRT_CalDbSvc::s_numberOfInstances = 0;

TRT_CalDbSvc::TRT_CalDbSvc( const std::string& name, ISvcLocator* pSvcLocator )
  : AthService(name,pSvcLocator),
    par_rtcontainerkey("/TRT/Calib/RT"),
    par_errcontainerkey("/TRT/Calib/errors2d"),
    par_slopecontainerkey("/TRT/Calib/slopes"),
    par_t0containerkey("/TRT/Calib/T0"),
    par_caltextfile(""),
    m_detstore("DetectorStore",name),
    m_trtid(0),
    m_ownsrtcontainer(false),
    m_ownserrcontainer(false),
    m_ownsslopecontainer(false),
    m_existserrcontainer(false),
    m_existsslopecontainer(false),
    m_ownst0container(false),
    m_streamer("AthenaPoolOutputStreamTool/CondStream1")
{
  declareProperty("StreamTool",m_streamer);
  declareProperty("calibTextFile",par_caltextfile);
  declareProperty("DetectorStore",m_detstore);
  declareProperty("RtFolderName",par_rtcontainerkey);
  declareProperty("T0FolderName",par_t0containerkey);
  declareProperty("ErrorSlopeFolderName",par_slopecontainerkey);
  declareProperty("ErrorFolderName",par_errcontainerkey);

  ++s_numberOfInstances;
}


TRT_CalDbSvc::~TRT_CalDbSvc()
{
  // no need to delete. storegate will do that for us.
  // if(m_ownsrtcontainer) delete getRtContainer() ;
  // if(m_ownst0container) delete getT0Container() ;
}


StatusCode TRT_CalDbSvc::initialize() 
{
  ATH_MSG_DEBUG( " in initialize " );

  if ( s_numberOfInstances > 1 ) {
    ATH_MSG_WARNING( "More than one instance of a TRT_CalDbSvc has been loaded/configured!" );
    ATH_MSG_WARNING( "If the configurations conflict, the job could crash." );
  }

  // Get StoreGate access to DetectorStore
  if (StatusCode::SUCCESS!=m_detstore.retrieve()) {
    msg(MSG::FATAL) << "Unable to retrieve " << m_detstore.name() << endmsg;
    return StatusCode::FAILURE;
  }

  // Get the TRT ID helper
  StatusCode sc = m_detstore->retrieve(m_trtid,"TRT_ID");
  if(sc.isFailure()) {
    msg(MSG::FATAL) << "Problem retrieving TRTID helper" << endmsg;
    return StatusCode::FAILURE;
  }

  // Register an IOV callback for the RT constants
  bool rtcontainerexists = m_detstore->StoreGateSvc::contains<RtRelationContainer>(par_rtcontainerkey) ;
  if( rtcontainerexists ) {
    // register call back
    if( (m_detstore->regFcn(&TRT_CalDbSvc::IOVCallBack,this,m_rtcontainer,par_rtcontainerkey)).isFailure()) {
      msg(MSG::ERROR) << "Could not register IOV callback for key: " << par_rtcontainerkey << endmsg ;
    }
  } else {
    // create, record and update data handle
    msg(MSG::INFO) << "Creating new rt-relation container" << endmsg ;
    m_ownsrtcontainer = true ;
    const RtRelationContainer* rtcontainer = new RtRelationContainer() ;
    if( (m_detstore->record(rtcontainer,par_rtcontainerkey))!=StatusCode::SUCCESS ) {
      msg(MSG::ERROR) << "Could not record RtRelationContainer for key " << par_rtcontainerkey << endmsg;
    }
    if(StatusCode::SUCCESS!=m_detstore->retrieve(m_rtcontainer,par_rtcontainerkey)) {
      msg(MSG::FATAL) << "Could not retrieve data handle for RtRelationContainer " << endmsg;
      return StatusCode::FAILURE ;
    }
  }

  // Register an IOV callback for the RT Errors
  bool errcontainerexists = m_detstore->StoreGateSvc::contains<RtRelationContainer>(par_errcontainerkey) ;
  if( errcontainerexists ) {
    m_existserrcontainer=true;
    // register call back
    if( (m_detstore->regFcn(&TRT_CalDbSvc::IOVCallBack,this,m_errcontainer,par_errcontainerkey)).isFailure()) {
      msg(MSG::ERROR) << "Could not register IOV callback for key: " << par_errcontainerkey << endmsg;
    }
  } else if( !par_caltextfile.empty() ) {
    // create, record and update data handle
    msg(MSG::INFO) << "Creating new error container" << endmsg ;
    m_ownserrcontainer = true ;
    const RtRelationContainer* errcontainer = new RtRelationContainer() ;
    if( (m_detstore->record(errcontainer,par_errcontainerkey))!=StatusCode::SUCCESS ) {
      msg(MSG::ERROR) << "Could not record RtRelationContainer for key " << par_errcontainerkey << endmsg;
    }
    if(StatusCode::SUCCESS!=m_detstore->retrieve(m_errcontainer,par_errcontainerkey)) {
      msg(MSG::FATAL) << "Could not retrieve data handle for Rt error container " << endmsg;
      return StatusCode::FAILURE ;
    }
  }

  // Register an IOV callback for the RT Slopes
  bool slopecontainerexists = m_detstore->StoreGateSvc::contains<RtRelationContainer>(par_slopecontainerkey) ;
  if( slopecontainerexists ) {
    m_existsslopecontainer=true;
    // register call back
    if( (m_detstore->regFcn(&TRT_CalDbSvc::IOVCallBack,this,m_slopecontainer,par_slopecontainerkey)).isFailure()) {
      msg(MSG::ERROR) << "Could not register IOV callback for key: " << par_slopecontainerkey << endmsg;
    }
  } else {
    // create, record and update data handle
    msg(MSG::INFO) << "Creating new slope container" << endmsg ;
    m_ownsslopecontainer = true ;
    const RtRelationContainer* slopecontainer = new RtRelationContainer() ;
    if( (m_detstore->record(slopecontainer,par_slopecontainerkey))!=StatusCode::SUCCESS ) {
      msg(MSG::ERROR) << "Could not record RtRelationContainer for key " << par_slopecontainerkey << endmsg;
    }
    if(StatusCode::SUCCESS!=m_detstore->retrieve(m_slopecontainer,par_slopecontainerkey)) {
      msg(MSG::FATAL) << "Could not retrieve data handle for Rt slope container " << endmsg;
      return StatusCode::FAILURE ;
    }
  }


  // Register an IOV callback for the t0
  bool t0containerexists = m_detstore->StoreGateSvc::contains<StrawT0Container>(par_t0containerkey) ;
  if( t0containerexists ) {
    if( (m_detstore->regFcn(&TRT_CalDbSvc::IOVCallBack,this,m_t0container,par_t0containerkey)).isFailure()) 
      msg(MSG::ERROR) << "Could not register IOV callback for key: " << par_t0containerkey << endmsg ;
  } else {
    // create, record and update data handle
    msg(MSG::INFO) << "Creating new t0 container" << endmsg ;
    m_ownst0container = true ;
    const StrawT0Container* t0container = new StrawT0Container() ;
    if( (m_detstore->record(t0container,par_t0containerkey))!=StatusCode::SUCCESS ) {
      msg(MSG::ERROR) << "Could not record StrawT0Container for key " << par_t0containerkey << endmsg;
    }
    if(StatusCode::SUCCESS!=m_detstore->retrieve(m_t0container,par_t0containerkey)) {
      msg(MSG::FATAL) << "Could not retrieve data handle for StrawT0Container " << endmsg;
      return StatusCode::FAILURE ;
    }
  }
  
  if( !par_caltextfile.empty() ) {
    if(StatusCode::SUCCESS!=this->readTextFile(par_caltextfile)) {
      msg(MSG::FATAL) << "Could not read calibration objects from text error file" << par_caltextfile << endmsg;
      return StatusCode::FAILURE ;
    }
    m_existserrcontainer=true;
  }
  

  return StatusCode::SUCCESS;
}


StatusCode TRT_CalDbSvc::finalize()
{
  ATH_MSG_DEBUG("TRT_CalDbSvc finalize method called");
  return StatusCode::SUCCESS;
}

StatusCode TRT_CalDbSvc::writeTextFile(const std::string& filename, int format) const
{
  msg(MSG::INFO) << " Write calibration data to text file " << filename << endmsg ;
  StatusCode sc=StatusCode::SUCCESS ;
  std::ofstream outfile(filename.c_str());
  outfile << "# Fileformat=" << format << std::endl ;
  switch(format) {
  case 0: sc=writeTextFile_Format0(outfile) ; break ;
  case 1: sc=writeTextFile_Format1(outfile) ; break ;
  case 2: sc=writeTextFile_Format2(outfile) ; break ;
  case 3: sc=writeTextFile_Format3(outfile) ; break ;
  default: msg(MSG::ERROR) << " Don't know how to write file in format = " << format << endmsg ;
  }
  outfile.close() ;
  return sc ;
}

StatusCode TRT_CalDbSvc::readTextFile(const std::string& filename) 
{

  StatusCode sc=StatusCode::SUCCESS ;
  std::ifstream infile(filename.c_str()) ;
  if(!infile) {
    msg(MSG::ERROR) << "Cannot find input file " << filename << endmsg ;
  } else {
    // read the format tag. if none, default to 0
    // very poor combination of c and C++ string handling
    int format = 0 ;
    char line[512] ;
    infile.getline(line,512) ;
    std::string linestring(line) ;
    size_t pos = linestring.find("Fileformat") ;
    if(pos != std::string::npos) {
      sscanf(line,"# Fileformat=%d",&format) ;
    } else {
      msg(MSG::WARNING) << "Input file has no Fileformat identifier. Assuming format=0." << endmsg;
      // 'rewind' the file
      infile.close() ;
      infile.open(filename.c_str()) ;
    }
    msg(MSG::INFO) << "Reading calibration data from text file " << filename << " format " << format << endmsg ;
    switch(format) {
    case 0: sc=readTextFile_Format0(infile) ; break ;
    case 1: sc=readTextFile_Format1(infile) ; break ;
    case 2: sc=readTextFile_Format2(infile) ; break ;
    case 3: sc=readTextFile_Format3(infile) ; break ;
    }
  }
  infile.close() ;
  return sc ;
}

StatusCode TRT_CalDbSvc::writeTextFile_Format0(std::ostream& outfile) const
{
  // firstreduce the containers as much as possible
  //getRtContainer()->crunch() ;
  //getT0Container()->crunch() ;

  // now we write only entries from the t0 container, but retrieve the
  // relevant rt-relations. we might want to change this at some
  // point.

  StrawT0Container::FlatContainer packedstrawdata ;
  m_t0container->getall( packedstrawdata ) ;
  
  // now, we store the StrawT0Container entries, finding the
  // relevant rt-relation on the way. after that, do we still store
  // any rt-relations?
  
  float t0(0),t0err(0),d0(0);
  const float* a ;
  const float defaulta[] = {0,0,0,0} ;
  for( TRTCond::StrawT0Container::FlatContainer::iterator it = packedstrawdata.begin() ;
       it != packedstrawdata.end(); ++it) {
    const TRTCond::ExpandedIdentifier& calid = it->first ;
    // unpack the t0
    //m_t0container->unpack(*it->second,t0,t0err,t0status) ; 
    m_t0container->unpack(calid,*it->second,t0,t0err) ; 
    
    // get the rt-relation. if not the right type, store zeros.
    const TRTCond::BasicRtRelation* rtr = dynamic_cast<const TRTCond::BasicRtRelation*>(m_rtcontainer->get(calid)) ;
    a = rtr ? rtr->cal() : defaulta ;
    outfile << calid << " "
	    << t0 << " " << t0err << " " << d0 << " " 
	    << a[0] << " " << a[1] << " " << a[2] << " " << a[3] << std::endl ; 
  }
  return StatusCode::SUCCESS ;
}
 

StatusCode TRT_CalDbSvc::readTextFile_Format0(std::istream& infile) 
{
  RtRelationContainer* rtcontainer = getRtContainer() ;
  StrawT0Container* t0container = getT0Container() ;
  t0container->clear() ;
  rtcontainer->clear() ;
  TRTCond::ExpandedIdentifier calid ;
  float t0,t0err,d0,a[4];
  int nentries(0),ninvalid(0) ; 
  char line[512] ;
  while( infile.getline(line,512) )
    if(line[0] != '#') {
      std::istringstream linestream(line) ;
      if( linestream >> calid >> t0 >> t0err >> d0 >> a[0] >> a[1] >> a[2] >> a[3] ) {


	setT0(calid,t0,t0err) ;
	TRTCond::BasicRtRelation rtr( a ) ;

	setRtRelation(calid,&rtr) ;
	++nentries ;
      } else ++ninvalid ;
    }
  size_t t0footprint = t0container->footprint()  ;
  size_t rtfootprint = rtcontainer->footprint()  ;
  //t0container->crunch() ;
  //rtcontainer->crunch() ;
  
  msg(MSG::INFO) << "read " << nentries << " from file. " 
	<< " t0/rt footprints before " << t0footprint << " / " << rtfootprint << " and after "
	<< t0container->footprint() << " / " << rtcontainer->footprint() << " compression." << endmsg ;
  if(ninvalid>0) msg(MSG::WARNING) << "read " << ninvalid << " invalid lines from input file" << endmsg ;

  return StatusCode::SUCCESS ;
}


StatusCode TRT_CalDbSvc::writeTextFile_Format1(std::ostream& outfile) const
{
  // first store rtrelations
  outfile << "# RtRelation" << std::endl ;
  RtRelationContainer::FlatContainer rtrelations ;
  m_rtcontainer->getall( rtrelations ) ;
  for( RtRelationContainer::FlatContainer::iterator it = rtrelations.begin() ;
       it != rtrelations.end(); ++it) {
    // write the identifier
    outfile << it->first << " : " ;
    // write the rt-relation via the factory
    TRTCond::RtRelationFactory::writeToFile(outfile,**(it->second)) ;
    outfile << std::endl ;
  }

  // now store the t0s
  outfile << "# StrawT0" << std::endl ;
  StrawT0Container::FlatContainer packedstrawdata ;
  m_t0container->getall( packedstrawdata ) ;
  float t0(0), t0err(0);
  for( TRTCond::StrawT0Container::FlatContainer::iterator it = packedstrawdata.begin() ;
       it != packedstrawdata.end(); ++it) {
    const TRTCond::ExpandedIdentifier& calid = it->first ;
    m_t0container->unpack(calid,*it->second,t0,t0err) ; 
    outfile << calid << " : " << t0 << " " << t0err << std::endl ;
  }
  return StatusCode::SUCCESS ;
}

StatusCode TRT_CalDbSvc::readTextFile_Format1(std::istream& infile) 
{
  RtRelationContainer* rtcontainer = getRtContainer() ;
  StrawT0Container* t0container = getT0Container() ;
  t0container->clear() ;
  rtcontainer->clear() ;
  enum ReadMode { ReadingRtRelation, ReadingStrawT0, ReadingGarbage } ;
  ReadMode readmode =ReadingGarbage ;
  char line[512] ;
  int nrtrelations(0), nstrawt0(0) ;
  while( infile.getline(line,512) ) {
    if(line[0] == '#') {
      // line with tag
      std::string linestring(line) ;
      if(     linestring.find("RtRelation") != std::string::npos) 
	readmode = ReadingRtRelation ;
      else if(linestring.find("StrawT0") != std::string::npos) 
	readmode = ReadingStrawT0 ;
      else readmode = ReadingGarbage ;
    } else if( readmode != ReadingGarbage) {
      std::istringstream is(line) ;
      // read the id
      TRTCond::ExpandedIdentifier id ;
      is >> id ;
      // read the semicolon that end the id
      char dummy ;
      is >> dummy ;
      // read the object
      if( readmode == ReadingRtRelation ) {


 	TRTCond::RtRelation* rt = TRTCond::RtRelationFactory::readFromFile(is) ;
	setRtRelation(id,rt) ;
	delete rt ;
	++nrtrelations ;
      } else if( readmode == ReadingStrawT0 ) {

	float t0(0), t0err(0) ;
	is >> t0 >> t0err ;
	setT0(id,t0,t0err) ;
	++nstrawt0 ;
      }
    }
  }
  size_t t0footprint = t0container->footprint()  ;
  size_t rtfootprint = rtcontainer->footprint()  ;
  //t0container->crunch() ;
  //rtcontainer->crunch() ;
  
  msg(MSG::INFO) << "read " << nstrawt0 << " t0 and " << nrtrelations << " rt from file. " 
	<< " t0/rt footprints before " << t0footprint << " / " << rtfootprint << " and after "
	<< t0container->footprint() << " / " << rtcontainer->footprint() << " compression." << endmsg ;

  return StatusCode::SUCCESS ;
}
StatusCode TRT_CalDbSvc::writeTextFile_Format2(std::ostream& outfile) const
{
  // first store rtrelations
  outfile << "# RtRelation" << std::endl ;
  RtRelationContainer::FlatContainer rtrelations ;
  m_rtcontainer->getall( rtrelations ) ;
  for( RtRelationContainer::FlatContainer::iterator it = rtrelations.begin() ;
       it != rtrelations.end(); ++it) {
    // write the identifier
    outfile << it->first << " : " ;
    // write the rt-relation via the factory
    TRTCond::RtRelationFactory::writeToFile(outfile,**(it->second)) ;
    outfile << std::endl ;
  }

  // then store errors
  outfile << "# RtErrors" << std::endl ;
  RtRelationContainer::FlatContainer errors ;
  m_errcontainer->getall( errors ) ;
  for( RtRelationContainer::FlatContainer::iterator it = errors.begin() ;
       it != errors.end(); ++it) {
    // write the identifier
    outfile << it->first << " : " ;
    // write the errors via the factory
    TRTCond::RtRelationFactory::writeToFile(outfile,**(it->second)) ;
    outfile << std::endl ;
  }

  // now store the t0s
  outfile << "# StrawT0" << std::endl ;
  StrawT0Container::FlatContainer packedstrawdata ;
  m_t0container->getall( packedstrawdata ) ;
  float t0(0), t0err(0);
  for( TRTCond::StrawT0Container::FlatContainer::iterator it = packedstrawdata.begin() ;
       it != packedstrawdata.end(); ++it) {
    const TRTCond::ExpandedIdentifier& calid = it->first ;
    m_t0container->unpack(calid,*it->second,t0,t0err) ; 
    outfile << calid << " : " << t0 << " " << t0err << std::endl ;
  }
  return StatusCode::SUCCESS ;
}

StatusCode TRT_CalDbSvc::readTextFile_Format2(std::istream& infile) 
{


  enum ReadMode { ReadingRtRelation, ReadingErrors, ReadingStrawT0, ReadingGarbage } ;
  ReadMode readmode =ReadingGarbage ;
  char line[512] ;
  int nrtrelations(0), nerrors(0), nstrawt0(0) ;
  RtRelationContainer* rtcontainer = getRtContainer() ;
  RtRelationContainer* errcontainer = getErrContainer() ;
  StrawT0Container* t0container = getT0Container() ;
  while( infile.getline(line,512) ) {
    if(line[0] == '#') {
      // line with tag
      std::string linestring(line) ;
      if(     linestring.find("RtRelation") != std::string::npos) {
	readmode = ReadingRtRelation ;
        rtcontainer->clear() ;
      } else if(linestring.find("StrawT0") != std::string::npos) {
	readmode = ReadingStrawT0 ;
        t0container->clear() ;
      } else if(linestring.find("RtErrors") != std::string::npos) {
	readmode = ReadingErrors ; 
        errcontainer->clear() ;
      } else { readmode = ReadingGarbage ; }
    } else if( readmode != ReadingGarbage) {
      std::istringstream is(line) ;
      // read the id
      TRTCond::ExpandedIdentifier id ;
      is >> id ;
      // read the semicolon that end the id
      char dummy ;
      is >> dummy ;
      // read the object
      if( readmode == ReadingRtRelation ) {

 	TRTCond::RtRelation* rt = TRTCond::RtRelationFactory::readFromFile(is) ;
	setRtRelation(id,rt) ;
	delete rt ;
	++nrtrelations ;

      } else if( readmode == ReadingErrors ) {

 	TRTCond::RtRelation* err = TRTCond::RtRelationFactory::readFromFile(is) ;
	setRtErrors(id,err) ;
	delete err ;
	++nerrors ;

      } else if( readmode == ReadingStrawT0 ) {

	float t0(0), t0err(0) ;
	is >> t0 >> t0err ;
	setT0(id,t0,t0err) ;
	++nstrawt0 ;
      }
    }
  }
  
  msg(MSG::INFO) << "read " << nstrawt0 << " t0 and " << nerrors << " errors and " << nrtrelations << " rt from file. " << endmsg;

  return StatusCode::SUCCESS ;
}


StatusCode TRT_CalDbSvc::readTextFile_Format3(std::istream& infile)
{


  enum ReadMode { ReadingRtRelation, ReadingErrors, ReadingSlopes, ReadingStrawT0, ReadingGarbage } ;
  ReadMode readmode =ReadingGarbage ;
  char line[512] ;
  int nrtrelations(0), nerrors(0), nslopes(0), nstrawt0(0) ;
  RtRelationContainer* rtcontainer = getRtContainer() ;
  RtRelationContainer* errcontainer = getErrContainer() ;
  RtRelationContainer* slopecontainer = getSlopeContainer() ;
  StrawT0Container* t0container = getT0Container() ;
  while( infile.getline(line,512) ) {
    if(line[0] == '#') {
      // line with tag
      std::string linestring(line) ;
      if(     linestring.find("RtRelation") != std::string::npos) {
        readmode = ReadingRtRelation ;
        rtcontainer->clear() ;
      } else if(linestring.find("RtErrors") != std::string::npos) {
        readmode = ReadingErrors ;
        errcontainer->clear() ;
      } else if(linestring.find("RtSlopes") != std::string::npos) {
        readmode = ReadingSlopes ;
        slopecontainer->clear() ;
      } else if(linestring.find("StrawT0") != std::string::npos) {
        readmode = ReadingStrawT0 ;
        t0container->clear() ;
      } else { readmode = ReadingGarbage ; }
    } else if( readmode != ReadingGarbage) {
      std::istringstream is(line) ;
      // read the id
      TRTCond::ExpandedIdentifier id ;
      is >> id ;
      // read the semicolon that end the id
      char dummy ;
      is >> dummy ;
      // read the object
      if( readmode == ReadingRtRelation ) {

        TRTCond::RtRelation* rt = TRTCond::RtRelationFactory::readFromFile(is) ;
        setRtRelation(id,rt) ;
        delete rt ;
        ++nrtrelations ;

      } else if( readmode == ReadingErrors ) {

        TRTCond::RtRelation* err = TRTCond::RtRelationFactory::readFromFile(is) ;
        setRtErrors(id,err) ;
        delete err ;
        ++nerrors ;

      } else if( readmode == ReadingSlopes ) {

        TRTCond::RtRelation* slope = TRTCond::RtRelationFactory::readFromFile(is) ;
        setRtSlopes(id,slope) ;
        delete slope ;
        ++nslopes ;
	
      } else if( readmode == ReadingStrawT0 ) {

        float t0(0), t0err(0) ;
        is >> t0 >> t0err ;
        setT0(id,t0,t0err) ;
        ++nstrawt0 ;
      }
    }
  }

  msg(MSG::INFO) << "read " << nstrawt0 << " t0 and " << nerrors << " errors and " << nslopes << " slopes and " << nrtrelations << " rt from file. " << endmsg;

  return StatusCode::SUCCESS ;
}

StatusCode TRT_CalDbSvc::writeTextFile_Format3(std::ostream& outfile) const
{
  // first store rtrelations
  outfile << "# Rtrelation" << std::endl ;
  RtRelationContainer::FlatContainer rtrelations ;
  m_rtcontainer->getall( rtrelations ) ;
  for( RtRelationContainer::FlatContainer::iterator it = rtrelations.begin() ;
       it != rtrelations.end(); ++it) {
    // write the identifier
    outfile << it->first << " : " ;
    // write the rt-relation via the factory
    TRTCond::RtRelationFactory::writeToFile(outfile,**(it->second)) ;
    outfile << std::endl ;
  }

  // then store errors
  outfile << "# RtErrors" << std::endl ;
  RtRelationContainer::FlatContainer errors ;
  m_errcontainer->getall( errors ) ;
  for( RtRelationContainer::FlatContainer::iterator it = errors.begin() ;
       it != errors.end(); ++it) {
    // write the identifier
    outfile << it->first << " : " ;
    // write the errors via the factory
    TRTCond::RtRelationFactory::writeToFile(outfile,**(it->second)) ;
    outfile << std::endl ;
  }

  // then store slopes
  outfile << "# RtSlopes" << std::endl ;
  RtRelationContainer::FlatContainer slopes ;
  m_slopecontainer->getall( slopes ) ;
  for( RtRelationContainer::FlatContainer::iterator it = slopes.begin() ;
       it != slopes.end(); ++it) {
    // write the identifier
    outfile << it->first << " : " ;
    // write the slopes via the factory
    TRTCond::RtRelationFactory::writeToFile(outfile,**(it->second)) ;
    outfile << std::endl ;
  }

  // now store the t0s
  outfile << "# StrawT0" << std::endl ;
  StrawT0Container::FlatContainer packedstrawdata ;
  m_t0container->getall( packedstrawdata ) ;
  float t0(0), t0err(0);
  for( TRTCond::StrawT0Container::FlatContainer::iterator it = packedstrawdata.begin() ;
       it != packedstrawdata.end(); ++it) {
    const TRTCond::ExpandedIdentifier& calid = it->first ;
    m_t0container->unpack(calid,*it->second,t0,t0err) ;
    outfile << calid << " : " << t0 << " " << t0err << std::endl ;
  }
  return StatusCode::SUCCESS ;
}


StatusCode TRT_CalDbSvc::streamOutCalibObjects() const
{
  msg(MSG::INFO) << "entering streamOutCalibObjects "  << endmsg;
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
  typeKeys.push_back( IAthenaOutputStreamTool::TypeKeyPair(StrawT0Container::classname(),par_t0containerkey)) ;
  typeKeys.push_back( IAthenaOutputStreamTool::TypeKeyPair(RtRelationContainer::classname(),par_rtcontainerkey)) ;
  typeKeys.push_back( IAthenaOutputStreamTool::TypeKeyPair(RtRelationContainer::classname(),par_errcontainerkey)) ;
  typeKeys.push_back( IAthenaOutputStreamTool::TypeKeyPair(RtRelationContainer::classname(),par_slopecontainerkey)) ;
  //getRtContainer()->crunch() ;
  //getT0Container()->crunch() ;
  
  sc = streamer->streamObjects(typeKeys);
  if (sc.isFailure()) {
    msg(MSG::ERROR) <<"Could not stream out Containers " <<endmsg;
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


StatusCode TRT_CalDbSvc::registerCalibObjects(std::string tag, int run1, int event1, int run2, int event2) const 
{
  msg(MSG::INFO) << "registerCalibObjects with IOV " << endmsg;
  msg(MSG::INFO) << "Run/evt1 [" << run1 << "," << event1 << "]" << endmsg;
  msg(MSG::INFO) << "Run/evt2 [" << run2 << "," << event2 << "]" << endmsg;
  
  // get pointer to registration svc
  IIOVRegistrationSvc* regsvc;
  if (StatusCode::SUCCESS!=service("IOVRegistrationSvc",regsvc)) {
    msg(MSG::FATAL) << "IOVRegistrationSvc not found" << endmsg;
    return( StatusCode::FAILURE);
  }
  
  if (StatusCode::SUCCESS==regsvc->registerIOV(RtRelationContainer::classname(),
					       par_rtcontainerkey,tag,run1,run2,event1,event2))
    msg(MSG::INFO) << "Registered RtRelationContainer object with key " << par_rtcontainerkey << endmsg ;
  else 
    msg(MSG::ERROR) << "Could not register RtRelationContainer object with key " << par_rtcontainerkey << endmsg ;

  if (StatusCode::SUCCESS==regsvc->registerIOV(RtRelationContainer::classname(),
					       par_errcontainerkey,tag,run1,run2,event1,event2))
    msg(MSG::INFO) << "Registered RtRelationContainer object with key " << par_errcontainerkey << endmsg ;
  else 
    msg(MSG::ERROR) << "Could not register RtRelationContainer object with key " << par_errcontainerkey << endmsg ;
  
  if (StatusCode::SUCCESS==regsvc->registerIOV(RtRelationContainer::classname(),
                                               par_slopecontainerkey,tag,run1,run2,event1,event2))
    msg(MSG::INFO) << "Registered RtRelationContainer object with key " << par_slopecontainerkey << endmsg ;
  else
    msg(MSG::ERROR) << "Could not register RtRelationContainer object with key " << par_slopecontainerkey << endmsg ;

  if (StatusCode::SUCCESS==regsvc->registerIOV(StrawT0Container::classname(),
					       par_t0containerkey,tag,run1,run2,event1,event2))
    msg(MSG::INFO) << "Registered StrawT0Container object with key " << par_t0containerkey << endmsg ;
  else 
    msg(MSG::ERROR) << "Could not register StrawT0Container object with key " << par_t0containerkey << endmsg ;
  
  return( StatusCode::SUCCESS);
}


double TRT_CalDbSvc::driftRadius(const double& time, float& t0, const Identifier& ident,bool& found) const
{
  // Returns a drift radius, a t0 and a success indicator,
  // given an identifier and a time given by (leading_edge_bin+0.5)*3.125ns
  found=true;
  t0 = this->getT0(ident);
  const TRTCond::RtRelation* rtr = getRtRelation(ident) ;
  double radius = 0;
  if (rtr != 0)
     radius = rtr->radius( time - t0 );
  else
     msg(MSG::FATAL) << " cannot find an rt-relation for TRT layer_or_wheel " <<  m_trtid->layer_or_wheel(ident) << " Please check IOV ranges " << endmsg ;
  
  if(msgLvl(MSG::VERBOSE)) msg() << " time " << time << " t0 " << t0 << " t " << time-t0
                                 << " radius " << radius << endmsg;
  //
  if(      radius<0 ) radius=0 ;
  else if( radius>2.) radius=2.;

  // add protection for the turnover:
  if (time - t0 > 55){
    		if(msgLvl(MSG::VERBOSE)) msg() << " time " << time << " t0 " << t0 << " t " << time-t0  << " > 55, check Rt derivative" << endmsg;
		// Check Second Derivative.
		if (rtr != 0){
			if (rtr->drdt( time - t0 ) < 0 ){
	    		if(msgLvl(MSG::VERBOSE)) msg() << " time " << time << " t0 " << t0 << " t " << time-t0  << " and rt derivative: " <<  rtr->drdt( time - t0 )   << endmsg;
			radius=2.;
			}
		}
  }
  return radius;
}

double TRT_CalDbSvc::driftError( const double& time, const Identifier& ident,bool& found) const
{
  // Returns an error on drift radius and a success indicator,
  // given an identifier and a drift-time in ns
  found=true;
  if(!m_existserrcontainer) {
    found=false;
    return 0.;
  }
  const TRTCond::RtRelation* rtr = getErrors(ident) ;
  double error=0.;
  if(rtr) {
    error = rtr->radius( time );
  } else {
    found=false;
  }
  if(msgLvl(MSG::VERBOSE)) msg() << " time " << time  
				 << " error on radius " << error << endmsg;
  return error;
}

double TRT_CalDbSvc::driftSlope( const double& time, const Identifier& ident,bool& found) const
{
  // Returns an error on drift radius and a success indicator,
  // given an identifier and a drift-time in ns
  found=true;
  if(!m_existserrcontainer) {
    found=false;
    return 0.;
  }
  const TRTCond::RtRelation* rtr = getSlopes(ident) ;
  double slope=0.;
  if(rtr) {
    slope = rtr->radius( time );
  } else {
    found=false;
  }
  if(msgLvl(MSG::VERBOSE)) msg() << " time " << time
                                 << " slope on radius " << slope << endmsg;
  return slope;
}


StatusCode TRT_CalDbSvc::IOVCallBack(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  for (std::list<std::string>::const_iterator 
	 itr=keys.begin(); itr!=keys.end(); ++itr) 
    msg(MSG::INFO) << " IOVCALLBACK for key " << *itr << " number " << I << endmsg;
  
  // if constants need to be read from textfile, we sue the call back routine to refill the IOV objects
  if(!par_caltextfile.empty()) return readTextFile( par_caltextfile ) ;
  return StatusCode::SUCCESS;
}


