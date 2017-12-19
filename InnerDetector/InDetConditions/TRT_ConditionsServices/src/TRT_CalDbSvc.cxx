/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file TRT_CalDbSvc.cxx
 *  @brief Service to manage TRT Conditions data
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
#include "StoreGate/ReadCondHandle.h"

unsigned int TRT_CalDbSvc::s_numberOfInstances = 0;

TRT_CalDbSvc::TRT_CalDbSvc( const std::string& name, ISvcLocator* pSvcLocator )
  : AthService(name,pSvcLocator),
    m_par_rtcontainerkey("/TRT/Calib/RT"),
    m_par_errcontainerkey("/TRT/Calib/errors2d"),
    m_par_slopecontainerkey("/TRT/Calib/slopes"),
    m_par_t0containerkey("/TRT/Calib/T0"),
    m_par_caltextfile(""),
    m_trtid(0),
    m_streamer("AthenaPoolOutputStreamTool/CondStream1"),
    m_detstore("DetectorStore",name),
    m_condSvc("CondSvc",name),
    m_rtReadKey("/TRT/Calib/RT"),
    m_errReadKey("/TRT/Calib/errors2d"),
    m_slopeReadKey("/TRT/Calib/slopes"),
    m_t0ReadKey("/TRT/Calib/T0"),
    m_useCachedPtr(false)
{
  declareProperty("StreamTool",m_streamer);
  declareProperty("calibTextFile",m_par_caltextfile);
  declareProperty("DetectorStore",m_detstore);
  declareProperty("RtFolderName",m_par_rtcontainerkey);
  declareProperty("T0FolderName",m_par_t0containerkey);
  declareProperty("ErrorSlopeFolderName",m_par_slopecontainerkey);
  declareProperty("ErrorFolderName",m_par_errcontainerkey);

  ++s_numberOfInstances;
}


TRT_CalDbSvc::~TRT_CalDbSvc()
{
  // no need to delete. storegate will do that for us.
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

  // Read keys
  ATH_CHECK( m_rtReadKey.initialize() );
  ATH_CHECK( m_errReadKey.initialize() );
  ATH_CHECK( m_slopeReadKey.initialize() );
  ATH_CHECK( m_t0ReadKey.initialize() );

  return StatusCode::SUCCESS;
}


StatusCode TRT_CalDbSvc::finalize()
{
  ATH_MSG_DEBUG("TRT_CalDbSvc finalize method called");
  return StatusCode::SUCCESS;
}


TRT_CalDbSvc::RtRelationContainer* TRT_CalDbSvc::getRtContainer() const {
  if(!m_useCachedPtr) {
    SG::ReadCondHandle<RtRelationContainer> rtc(m_rtReadKey);
    return const_cast<RtRelationContainer*>(*rtc);
  }
  return m_rtContainer.get();
}


TRT_CalDbSvc::RtRelationContainer* TRT_CalDbSvc::getErrContainer() const {
  if(!m_useCachedPtr) {
    SG::ReadCondHandle<RtRelationContainer> erc(m_errReadKey);
    return const_cast<RtRelationContainer*>(*erc);
  }
  return m_errContainer.get();
}

TRT_CalDbSvc::RtRelationContainer* TRT_CalDbSvc::getSlopeContainer() const {
  if(!m_useCachedPtr) {
    SG::ReadCondHandle<RtRelationContainer> slc(m_slopeReadKey);
    return const_cast<RtRelationContainer*>(*slc);
  }
  return m_slopeContainer.get();
}

TRT_CalDbSvc::StrawT0Container* TRT_CalDbSvc::getT0Container() const {
  if(!m_useCachedPtr) {
    SG::ReadCondHandle<StrawT0Container> t0c(m_t0ReadKey);
    return const_cast<StrawT0Container*>(*t0c);
  }
  return m_t0Container.get();
}


void TRT_CalDbSvc::TRT_CalDbSvc::setRtContainer(RtRelationContainer* rc) {
  m_rtContainer=rc;
}
void TRT_CalDbSvc::TRT_CalDbSvc::setErrContainer(RtRelationContainer* rc) {
  m_errContainer=rc;
}
void TRT_CalDbSvc::TRT_CalDbSvc::setSlopeContainer(RtRelationContainer* rc) {
  m_slopeContainer=rc;
}
void TRT_CalDbSvc::TRT_CalDbSvc::setT0Container(StrawT0Container* rc) {
  m_t0Container=rc;
}

StatusCode TRT_CalDbSvc::readTextFile(const std::string& filename, int& format ) 
{

  StatusCode sc=StatusCode::SUCCESS ;
  std::ifstream infile(filename.c_str()) ;
  if(!infile) {
    msg(MSG::ERROR) << "Cannot find input file " << filename << endmsg ;
  } else {
    // read the format tag. if none, default to 0
    format = 0 ;
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
    default :  sc=readTextFile_Format1(infile) ; break ;
    }
  }
  infile.close() ;
  return sc ;
}

StatusCode TRT_CalDbSvc::writeTextFile_Format0(std::ostream& outfile) const
{


  RtRelationContainer* rtcontainer = getRtContainer() ;
  StrawT0Container* t0container = getT0Container() ;

  if(!rtcontainer || !t0container) return StatusCode::FAILURE;

  StrawT0Container::FlatContainer packedstrawdata ;

  t0container->getall( packedstrawdata ) ;
  
  float t0(0),t0err(0),d0(0);
  const float* a ;
  const float defaulta[] = {0,0,0,0} ;
  for( StrawT0Container::FlatContainer::iterator it = packedstrawdata.begin() ;
       it != packedstrawdata.end(); ++it) {
    const TRTCond::ExpandedIdentifier& calid = it->first ;
    // unpack the t0
    t0container->unpack(calid,*it->second,t0,t0err) ; 
    
    // get the rt-relation. if not the right type, store zeros.
    const TRTCond::BasicRtRelation* rtr = dynamic_cast<const TRTCond::BasicRtRelation*>(rtcontainer->get(calid)) ;
    a = rtr ? rtr->cal() : defaulta ;
    outfile << calid << " "
	    << t0 << " " << t0err << " " << d0 << " " 
	    << a[0] << " " << a[1] << " " << a[2] << " " << a[3] << std::endl ; 
  }
  return StatusCode::SUCCESS ;
}
 

StatusCode TRT_CalDbSvc::readTextFile_Format0(std::istream& infile) 
{

  //If you supply a text file in format 0 remember to block the RT and T0 folders

 
  m_rtContainer = new RtRelationContainer() ;
  m_t0Container = new StrawT0Container() ;

 
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
  size_t t0footprint = m_t0Container->footprint()  ;
  size_t rtfootprint = m_rtContainer->footprint()  ;
  //t0container->crunch() ;
  //rtcontainer->crunch() ;
  
  msg(MSG::INFO) << "read " << nentries << " from file. " 
	<< " t0/rt footprints before " << t0footprint << " / " << rtfootprint << " and after "
	<< m_t0Container->footprint() << " / " << m_rtContainer->footprint() << " compression." << endmsg ;
  if(ninvalid>0) msg(MSG::WARNING) << "read " << ninvalid << " invalid lines from input file" << endmsg ;

  return StatusCode::SUCCESS ;
}


StatusCode TRT_CalDbSvc::writeTextFile_Format1(std::ostream& outfile) const
{

  // first store rtrelations
  outfile << "# RtRelation" << std::endl ;
  RtRelationContainer::FlatContainer rtrelations ;
  m_rtContainer->getall( rtrelations ) ;
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
  m_t0Container->getall( packedstrawdata ) ;
  float t0(0), t0err(0);
  for( TRTCond::StrawT0Container::FlatContainer::iterator it = packedstrawdata.begin() ;
       it != packedstrawdata.end(); ++it) {
    const TRTCond::ExpandedIdentifier& calid = it->first ;
    m_t0Container->unpack(calid,*it->second,t0,t0err) ; 
    outfile << calid << " : " << t0 << " " << t0err << std::endl ;
  }
  return StatusCode::SUCCESS ;
}

StatusCode TRT_CalDbSvc::readTextFile_Format1(std::istream& infile) 
{

  //If you supply a text file in format 1 remember to block the RT and T0 folders

  m_rtContainer = new RtRelationContainer() ;
  m_t0Container = new StrawT0Container() ;

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
  size_t t0footprint = m_t0Container->footprint()  ;
  size_t rtfootprint = m_rtContainer->footprint()  ;
  //t0container->crunch() ;
  //rtcontainer->crunch() ;
  
  msg(MSG::INFO) << "read " << nstrawt0 << " t0 and " << nrtrelations << " rt from file. " 
	<< " t0/rt footprints before " << t0footprint << " / " << rtfootprint << " and after "
	<< m_t0Container->footprint() << " / " << m_rtContainer->footprint() << " compression." << endmsg ;

  return StatusCode::SUCCESS ;
}
StatusCode TRT_CalDbSvc::writeTextFile_Format2(std::ostream& outfile) const
{
  // first store rtrelations
  outfile << "# RtRelation" << std::endl ;
  RtRelationContainer::FlatContainer rtrelations ;
  m_rtContainer->getall( rtrelations ) ;
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
  m_errContainer->getall( errors ) ;
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
  m_t0Container->getall( packedstrawdata ) ;
  float t0(0), t0err(0);
  for( TRTCond::StrawT0Container::FlatContainer::iterator it = packedstrawdata.begin() ;
       it != packedstrawdata.end(); ++it) {
    const TRTCond::ExpandedIdentifier& calid = it->first ;
    m_t0Container->unpack(calid,*it->second,t0,t0err) ; 
    outfile << calid << " : " << t0 << " " << t0err << std::endl ;
  }
  return StatusCode::SUCCESS ;
}

StatusCode TRT_CalDbSvc::readTextFile_Format2(std::istream& infile) 
{

  //If you supply a text file in format 2 remember to block the RT, T0 and errors2 folders

  enum ReadMode { ReadingRtRelation, ReadingErrors, ReadingStrawT0, ReadingGarbage } ;
  ReadMode readmode =ReadingGarbage ;
  char line[512] ;
  int nrtrelations(0), nerrors(0), nstrawt0(0) ;
  m_rtContainer = new RtRelationContainer() ;
  m_errContainer = new RtRelationContainer() ;
  m_t0Container = new StrawT0Container() ;
  while( infile.getline(line,512) ) {
    if(line[0] == '#') {
      // line with tag
      std::string linestring(line) ;
      if(     linestring.find("RtRelation") != std::string::npos) {
	readmode = ReadingRtRelation ;
        m_rtContainer->clear() ;
      } else if(linestring.find("StrawT0") != std::string::npos) {
	readmode = ReadingStrawT0 ;
        m_t0Container->clear() ;
      } else if(linestring.find("RtErrors") != std::string::npos) {
	readmode = ReadingErrors ; 
        m_errContainer->clear() ;
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

  //If you supply a text file in format 3 remember to block the RT, T0, errors2 and slopes folders

  enum ReadMode { ReadingRtRelation, ReadingErrors, ReadingSlopes, ReadingStrawT0, ReadingGarbage } ;
  ReadMode readmode =ReadingGarbage ;
  char line[512] ;
  int nrtrelations(0), nerrors(0), nslopes(0), nstrawt0(0) ;
  m_rtContainer = new RtRelationContainer() ;
  m_errContainer = new RtRelationContainer() ;
  m_slopeContainer = new RtRelationContainer() ;
  m_t0Container = new StrawT0Container() ;
  while( infile.getline(line,512) ) {
    if(line[0] == '#') {
      // line with tag
      std::string linestring(line) ;
      if(     linestring.find("RtRelation") != std::string::npos) {
        readmode = ReadingRtRelation ;
        m_rtContainer->clear() ;
      } else if(linestring.find("RtErrors") != std::string::npos) {
        readmode = ReadingErrors ;
        m_errContainer->clear() ;
      } else if(linestring.find("RtSlopes") != std::string::npos) {
        readmode = ReadingSlopes ;
        m_slopeContainer->clear() ;
      } else if(linestring.find("StrawT0") != std::string::npos) {
        readmode = ReadingStrawT0 ;
        m_t0Container->clear() ;
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
  getRtContainer()->getall( rtrelations ) ;
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
  getErrContainer()->getall( errors ) ;
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
  getSlopeContainer()->getall( slopes ) ;
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
  getT0Container()->getall( packedstrawdata ) ;
  float t0(0), t0err(0);
  for( TRTCond::StrawT0Container::FlatContainer::iterator it = packedstrawdata.begin() ;
       it != packedstrawdata.end(); ++it) {
    const TRTCond::ExpandedIdentifier& calid = it->first ;
    getT0Container()->unpack(calid,*it->second,t0,t0err) ;
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
  typeKeys.push_back( IAthenaOutputStreamTool::TypeKeyPair(StrawT0Container::classname(),m_par_t0containerkey)) ;
  typeKeys.push_back( IAthenaOutputStreamTool::TypeKeyPair(RtRelationContainer::classname(),m_par_rtcontainerkey)) ;
  typeKeys.push_back( IAthenaOutputStreamTool::TypeKeyPair(RtRelationContainer::classname(),m_par_errcontainerkey)) ;
  typeKeys.push_back( IAthenaOutputStreamTool::TypeKeyPair(RtRelationContainer::classname(),m_par_slopecontainerkey)) ;
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


StatusCode TRT_CalDbSvc::registerCalibObjects(std::string tag, unsigned int run1, unsigned int event1, unsigned int run2, unsigned int event2) const 
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
					       m_par_rtcontainerkey,tag,run1,run2,event1,event2))
    msg(MSG::INFO) << "Registered RtRelationContainer object with key " << m_par_rtcontainerkey << endmsg ;
  else 
    msg(MSG::ERROR) << "Could not register RtRelationContainer object with key " << m_par_rtcontainerkey << endmsg ;

  if (StatusCode::SUCCESS==regsvc->registerIOV(RtRelationContainer::classname(),
					       m_par_errcontainerkey,tag,run1,run2,event1,event2))
    msg(MSG::INFO) << "Registered RtRelationContainer object with key " << m_par_errcontainerkey << endmsg ;
  else 
    msg(MSG::ERROR) << "Could not register RtRelationContainer object with key " << m_par_errcontainerkey << endmsg ;
  
  if (StatusCode::SUCCESS==regsvc->registerIOV(RtRelationContainer::classname(),
                                               m_par_slopecontainerkey,tag,run1,run2,event1,event2))
    msg(MSG::INFO) << "Registered RtRelationContainer object with key " << m_par_slopecontainerkey << endmsg ;
  else
    msg(MSG::ERROR) << "Could not register RtRelationContainer object with key " << m_par_slopecontainerkey << endmsg ;

  if (StatusCode::SUCCESS==regsvc->registerIOV(StrawT0Container::classname(),
					       m_par_t0containerkey,tag,run1,run2,event1,event2))
    msg(MSG::INFO) << "Registered StrawT0Container object with key " << m_par_t0containerkey << endmsg ;
  else 
    msg(MSG::ERROR) << "Could not register StrawT0Container object with key " << m_par_t0containerkey << endmsg ;
  
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
  const TRTCond::RtRelation* rtr = getErrors(ident) ;
  double error=0.;
  if(rtr) {
    error = rtr->radius( time );
  } else {
    found=false;
    return 0;
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
  const TRTCond::RtRelation* rtr = getSlopes(ident) ;
  double slope=0.;
  if(rtr) {
    slope = rtr->radius( time );
  } else {
    found=false;
    return 0;
  }
  if(msgLvl(MSG::VERBOSE)) msg() << " time " << time
                                 << " slope on radius " << slope << endmsg;
  return slope;
}




