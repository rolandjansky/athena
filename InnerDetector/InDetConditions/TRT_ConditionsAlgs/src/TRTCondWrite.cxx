/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>
#include <iostream>
#include <string>
#include "TRT_ConditionsAlgs/TRTCondWrite.h"
#include "TRT_ConditionsData/BasicRtRelation.h"
#include "TRT_ConditionsData/DinesRtRelation.h"
#include "TRT_ConditionsData/BinnedRtRelation.h"
#include "TRT_ConditionsData/RtRelationFactory.h"
#include "StoreGate/ReadCondHandle.h"


/**  @file TRTCondWrite.cxx
 *   Algoritm for publishing TRT Calibration constants to CondStore
 *
 *   If constants are supplied by text-files these are read
 *   and filled into Storegate
 *
 *   If an output text file is specified, the contents of Storegate are dumped.
 *
 * @author Peter Hansen <phansen@nbi.dk>
**/
TRTCondWrite::TRTCondWrite(const std::string& name, ISvcLocator* pSvcLocator)
  :AthAlgorithm   (name, pSvcLocator),
   m_par_rtcontainerkey("/TRT/Calib/RT"),
   m_par_errcontainerkey("/TRT/Calib/errors2d"),
   m_par_slopecontainerkey("/TRT/Calib/slopes"),
   m_par_t0containerkey("/TRT/Calib/T0"),
   m_setup(false),
   m_par_caltextfile(""),
   m_par_caloutputfile(""), //set this to "calibout_n,txt", where n=0,1,2,3 refers to the format
   m_trtid(nullptr),
   m_streamer("AthenaOutputStreamTool/CondStream1"),
   m_detstore("DetectorStore",name),
   m_condSvc("CondSvc",name)
{
  // declare algorithm parameters
  declareProperty("StreamTool",m_streamer);
  declareProperty("CalibInputFile",m_par_caltextfile);
  declareProperty("DetectorStore",m_detstore);
  declareProperty("RtFolderName",m_par_rtcontainerkey);
  declareProperty("T0FolderName",m_par_t0containerkey);
  declareProperty("ErrorSlopeFolderName",m_par_slopecontainerkey);
  declareProperty("ErrorFolderName",m_par_errcontainerkey);
  declareProperty("CalibOutputFile",m_par_caloutputfile);
}

TRTCondWrite::~TRTCondWrite(void)
{}

StatusCode TRTCondWrite::initialize() {

  ATH_MSG_DEBUG("TRTCondWrite::initialize() called");

  //
  // Get ID helper
  StatusCode sc = detStore()->retrieve(m_trtid,"TRT_ID");
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "Could not retrieve TRT ID helper." );
    return sc;
  }


  int format=0;
  if( !m_par_caltextfile.empty() ) {
    ATH_MSG_INFO( " input text file supplied " << m_par_caltextfile);
    if(StatusCode::SUCCESS!=checkTextFile(m_par_caltextfile, format)) {
      ATH_MSG_FATAL( "Could not read format of text file" << m_par_caltextfile);
      return StatusCode::FAILURE ;
    }
    ATH_MSG_INFO( " Found format " << format << " in text file " << m_par_caltextfile);
  } else {
    ATH_MSG_INFO( " No input text file supplied ");
  }

  // Read keys
  if(format==0) ATH_CHECK( m_rtReadKey.initialize() );
  if(format<3)  ATH_CHECK( m_errReadKey.initialize() );
  if(format<4)  ATH_CHECK( m_slopeReadKey.initialize() );
  if(format==0) ATH_CHECK( m_t0ReadKey.initialize() );

  // Reading from or writing to the DB?  
  if( !m_par_caltextfile.empty() ) {
    ATH_MSG_INFO( " Read calibration constants from text file " << m_par_caltextfile);
    if(StatusCode::SUCCESS!=readTextFile(m_par_caltextfile, format)) {
      ATH_MSG_FATAL( "Could not read calibration objects from text file " << m_par_caltextfile);
      return StatusCode::FAILURE ;
    }
  } else {
    ATH_MSG_INFO( "TRTCondWrite::initialize for standard running " );
  }
 
  return StatusCode::SUCCESS;
}

StatusCode TRTCondWrite::execute(){

  StatusCode sc = StatusCode::SUCCESS;
  //
  // at first event:
  if (!m_setup) {

    m_setup=true;


    //Write text file.
    if (!m_par_caloutputfile.empty()) {

      std::ofstream outfile(m_par_caloutputfile.c_str());

      if (m_par_caloutputfile=="calibout_1.txt") {
        outfile << "# Fileformat=" << 1 << std::endl ;
        sc = writeTextFile_Format1(outfile);
        outfile.close();
      } else if (m_par_caloutputfile=="calibout_2.txt") {
        outfile << "# Fileformat=" << 2 << std::endl ;
        sc = writeTextFile_Format2(outfile);
        outfile.close();
      } else if (m_par_caloutputfile=="calibout_3.txt") {
        outfile << "# Fileformat=" << 3 << std::endl ;
	sc = writeTextFile_Format3(outfile);
        outfile.close();
      } else {
        ATH_MSG_ERROR( " Output calibration text file must have name of the form calibout_n.txt, n=0,1,2,3 ");
        outfile.close();
        return StatusCode::FAILURE;
      }

      if(sc!=StatusCode::SUCCESS) {
        ATH_MSG_ERROR(" Could not write TRT Calibration to " << m_par_caloutputfile );
        outfile.close();
        return sc;
      }

    }

  }
  
  return sc;
}

StatusCode TRTCondWrite::finalize() {
  return StatusCode::SUCCESS;
}
StatusCode TRTCondWrite::checkTextFile(const std::string& filename, int& format ) 
{

  StatusCode sc=StatusCode::SUCCESS ;
  std::ifstream infile(filename.c_str()) ;
  if(!infile) {
    ATH_MSG_ERROR( "Cannot find input file " << filename ) ;
    sc=StatusCode::FAILURE;
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
      ATH_MSG_WARNING( "Input file has no Fileformat identifier. Assuming format=0.");
      // 'rewind' the file

      infile.close() ;
      infile.open(filename.c_str()) ;
    }
  }
  infile.close() ;
  return sc ;
}

StatusCode TRTCondWrite::readTextFile(const std::string& filename, int& format ) 
{

  StatusCode sc=StatusCode::SUCCESS ;
  std::ifstream infile(filename.c_str()) ;
  if(!infile) {
    ATH_MSG_ERROR( "Cannot find input file " << filename ) ;
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
      ATH_MSG_WARNING( "Input file has no Fileformat identifier. Assuming format=0.");
      // 'rewind' the file

      infile.close() ;
      infile.open(filename.c_str()) ;
    }
    ATH_MSG_INFO( "Reading calibration data from text file " << filename << " format " << format ) ;
    switch(format) {
    case 1: sc=readTextFile_Format1(infile) ; break ;
    case 2: sc=readTextFile_Format2(infile) ; break ;
    case 3: sc=readTextFile_Format3(infile) ; break ;
    default :  sc=readTextFile_Format1(infile) ; break ;
    }
  }
  infile.close() ;
  return sc ;
}


StatusCode TRTCondWrite::writeTextFile_Format1(std::ostream& outfile) const
{

  const RtRelationContainer* rtContainer = getRtContainer() ;
  const StrawT0Container* t0Container = getT0Container() ;


  // first store rtrelations
  outfile << "# RtRelation" << std::endl ;
  RtRelationContainer::FlatContainer rtrelations ;
  rtContainer->getall( rtrelations ) ;
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
  t0Container->getall( packedstrawdata ) ;
  float t0(0), t0err(0);
  for( TRTCond::StrawT0Container::FlatContainer::iterator it = packedstrawdata.begin() ;
       it != packedstrawdata.end(); ++it) {
    const TRTCond::ExpandedIdentifier& calid = it->first ;
    t0Container->unpack(calid,*it->second,t0,t0err) ; 
    outfile << calid << " : " << t0 << " " << t0err << std::endl ;
  }
  return StatusCode::SUCCESS ;
}

StatusCode TRTCondWrite::readTextFile_Format1(std::istream& infile) 
{


  enum ReadMode { ReadingRtRelation, ReadingStrawT0, ReadingGarbage } ;
  ReadMode readmode =ReadingGarbage ;

  ATH_MSG_INFO( "Creating new T0 container" ) ;
  StrawT0Container*  t0Container = new TRTCond::StrawT0MultChanContainer() ; 
  ATH_MSG_INFO("Creating new Rt container") ;
  RtRelationContainer* rtContainer = new TRTCond::RtRelationMultChanContainer() ; 


  if( rtContainer==nullptr || t0Container==nullptr) {
     ATH_MSG_ERROR(" Cannot create  Rt and T0 contianers ");
     return StatusCode::FAILURE;
  } 

  rtContainer->clear();
  t0Container->clear();

  char line[512] ;
  int nrtrelations(0), nstrawt0(0) ;
  while( infile.getline(line,512) ) {
    if(line[0] == '#') {
      // line with tag
      std::string linestring(line) ;
      if(linestring.find("RtRelation") != std::string::npos) {
	readmode = ReadingRtRelation ;
        ATH_MSG_VERBOSE(" Found line with Rt tag ");

      } else if (linestring.find("StrawT0") != std::string::npos) { 
	readmode = ReadingStrawT0 ;
        ATH_MSG_VERBOSE(" Found line with T0 tag ");

      }else readmode = ReadingGarbage ;
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
        rtContainer->set( id,rt); 
	delete rt ;
	++nrtrelations ;
      } else if( readmode == ReadingStrawT0 ) {

	float t0(0), t0err(0) ;
	is >> t0 >> t0err; 
        t0Container->setT0( id, t0, t0err );
	++nstrawt0 ;
      }
    }
  }
  size_t t0footprint = t0Container->footprint()  ;
  size_t rtfootprint = rtContainer->footprint()  ;

  
  ATH_MSG_INFO( "read " << nstrawt0 << " t0 and " << nrtrelations << " rt from file. " 
	<< " t0/rt footprints before " << t0footprint << " / " << rtfootprint << " and after "
		<< t0Container->footprint() << " / " << rtContainer->footprint() << " compression." ) ;

   ATH_MSG_INFO(" Recording T0 container ");
   if( (m_detstore->record(t0Container,m_par_t0containerkey))!=StatusCode::SUCCESS ) {
      ATH_MSG_ERROR("Could not record T0 Container for key " << m_par_t0containerkey );
      return StatusCode::FAILURE;
    }



  return StatusCode::SUCCESS ;
}
StatusCode TRTCondWrite::writeTextFile_Format2(std::ostream& outfile) const
{

  const RtRelationContainer* rtContainer = getRtContainer() ;
  const StrawT0Container* t0Container = getT0Container() ;
  const RtRelationContainer* errContainer = getErrContainer() ;

  // first store rtrelations
  outfile << "# RtRelation" << std::endl ;
  RtRelationContainer::FlatContainer rtrelations ;
  rtContainer->getall( rtrelations ) ;
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
  errContainer->getall( errors ) ;
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
  t0Container->getall( packedstrawdata ) ;
  float t0(0), t0err(0);
  for( TRTCond::StrawT0Container::FlatContainer::iterator it = packedstrawdata.begin() ;
       it != packedstrawdata.end(); ++it) {
    const TRTCond::ExpandedIdentifier& calid = it->first ;
    t0Container->unpack(calid,*it->second,t0,t0err) ; 
    outfile << calid << " : " << t0 << " " << t0err << std::endl ;
  }


  return StatusCode::SUCCESS ;
}

StatusCode TRTCondWrite::readTextFile_Format2(std::istream& infile) 
{


  enum ReadMode { ReadingRtRelation, ReadingErrors, ReadingStrawT0, ReadingGarbage } ;
  ReadMode readmode =ReadingGarbage ;
  char line[512] ;
  int nrtrelations(0), nerrors(0), nstrawt0(0) ;

  ATH_MSG_INFO( "Creating new T0 container" ) ;
  StrawT0Container*  t0Container = new TRTCond::StrawT0MultChanContainer() ; 
  ATH_MSG_INFO("Creating new Rt container") ;
  RtRelationContainer* rtContainer = new TRTCond::RtRelationMultChanContainer() ; 
  ATH_MSG_INFO("Creating new error container") ;
  RtRelationContainer* errContainer = new TRTCond::RtRelationMultChanContainer() ; 

  if( rtContainer==nullptr || t0Container==nullptr || errContainer==nullptr) {
     ATH_MSG_ERROR(" Cannot create  Rt, error and T0 containers ");
     return StatusCode::FAILURE;
  } 

  rtContainer->clear();
  t0Container->clear();
  errContainer->clear();

  while( infile.getline(line,512) ) {
    if(line[0] == '#') {
      // line with tag
      std::string linestring(line) ;
      if(     linestring.find("RtRelation") != std::string::npos) {
	readmode = ReadingRtRelation ;
        rtContainer->clear() ;
      } else if(linestring.find("StrawT0") != std::string::npos) {
	readmode = ReadingStrawT0 ;
        t0Container->clear() ;
      } else if(linestring.find("RtErrors") != std::string::npos) {
	readmode = ReadingErrors ; 
        errContainer->clear() ;
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
        rtContainer->set( id,rt); 
	delete rt ;
	++nrtrelations ;

      } else if( readmode == ReadingErrors ) {

 	TRTCond::RtRelation* err = TRTCond::RtRelationFactory::readFromFile(is) ;
        errContainer->set( id,err);
	delete err ;
	++nerrors ;

      } else if( readmode == ReadingStrawT0 ) {

	float t0(0), t0err(0) ;
	is >> t0 >> t0err ;
        t0Container->setT0( id, t0, t0err );
	++nstrawt0 ;
      }
    }
  }
  
  ATH_MSG_INFO( "read " << nstrawt0 << " t0 and " << nerrors << " errors and " << nrtrelations << " rt from file. " );

   ATH_MSG_INFO(" Recording T0 container ");
   if( (m_detstore->record(t0Container,m_par_t0containerkey))!=StatusCode::SUCCESS ) {
      ATH_MSG_ERROR("Could not record T0 Container for key " << m_par_t0containerkey );
      return StatusCode::FAILURE;
    }

   ATH_MSG_INFO(" Recording RT container ");
   if( (m_detstore->record(rtContainer,m_par_rtcontainerkey))!=StatusCode::SUCCESS ) {
      ATH_MSG_ERROR("Could not record RT Container for key " << m_par_rtcontainerkey );
      return StatusCode::FAILURE;
    }

   ATH_MSG_INFO(" Recording Error container ");
   if( (m_detstore->record(errContainer,m_par_errcontainerkey))!=StatusCode::SUCCESS ) {
      ATH_MSG_ERROR("Could not record err Container for key " << m_par_errcontainerkey );
      return StatusCode::FAILURE;
    }



  return StatusCode::SUCCESS ;
}


StatusCode TRTCondWrite::readTextFile_Format3(std::istream& infile)
{


  ATH_MSG_INFO( "Creating new T0 container" ) ;
  StrawT0Container*  t0Container = new TRTCond::StrawT0MultChanContainer() ; 
  ATH_MSG_INFO("Creating new Rt container") ;
  RtRelationContainer* rtContainer = new TRTCond::RtRelationMultChanContainer() ; 
  ATH_MSG_INFO("Creating new error container") ;
  RtRelationContainer* errContainer = new TRTCond::RtRelationMultChanContainer() ; 
  ATH_MSG_INFO("Creating new slope container") ;
  RtRelationContainer* slopeContainer = new TRTCond::RtRelationMultChanContainer() ; 

  if( rtContainer==nullptr || t0Container==nullptr || errContainer==nullptr || slopeContainer==nullptr) {
     ATH_MSG_ERROR(" Cannot create  Rt, error, slope and T0 containers ");
     return StatusCode::FAILURE;
  } 

  t0Container->clear();
  rtContainer->clear();
  errContainer->clear();
  slopeContainer->clear();

  enum ReadMode { ReadingRtRelation, ReadingErrors, ReadingSlopes, ReadingStrawT0, ReadingGarbage } ;
  ReadMode readmode =ReadingGarbage ;
  char line[512] ;
  int nrtrelations(0), nerrors(0), nslopes(0), nstrawt0(0) ;

  while( infile.getline(line,512) ) {
    if(line[0] == '#') {
      // line with tag
      std::string linestring(line) ;
      if(     linestring.find("RtRelation") != std::string::npos) {
        readmode = ReadingRtRelation ;
        rtContainer->clear() ;
      } else if(linestring.find("RtErrors") != std::string::npos) {
        readmode = ReadingErrors ;
        errContainer->clear() ;
      } else if(linestring.find("RtSlopes") != std::string::npos) {
        readmode = ReadingSlopes ;
        slopeContainer->clear() ;
      } else if(linestring.find("StrawT0") != std::string::npos) {
        readmode = ReadingStrawT0 ;
        t0Container->clear() ;
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
        rtContainer->set( id,rt); 
        delete rt ;
        ++nrtrelations ;

      } else if( readmode == ReadingErrors ) {

        TRTCond::RtRelation* err = TRTCond::RtRelationFactory::readFromFile(is) ;
        errContainer->set( id,err);
        delete err ;
        ++nerrors ;

      } else if( readmode == ReadingSlopes ) {

        TRTCond::RtRelation* slope = TRTCond::RtRelationFactory::readFromFile(is) ;
        slopeContainer->set( id,slope);
        delete slope ;
        ++nslopes ;
	
      } else if( readmode == ReadingStrawT0 ) {

        float t0(0), t0err(0) ;
        is >> t0 >> t0err ;
        t0Container->setT0( id, t0, t0err );
        ++nstrawt0 ;
      }
    }
  }

  ATH_MSG_INFO( "read " << nstrawt0 << " t0 and " << nerrors << " errors and " << nslopes << " slopes and " << nrtrelations << " rt from file. " );

   ATH_MSG_INFO(" Recording T0 container ");
   if( (m_detstore->record(t0Container,m_par_t0containerkey))!=StatusCode::SUCCESS ) {
      ATH_MSG_ERROR("Could not record T0 Container for key " << m_par_t0containerkey );
      return StatusCode::FAILURE;
    }

   ATH_MSG_INFO(" Recording RT container ");
   if( (m_detstore->record(rtContainer,m_par_rtcontainerkey))!=StatusCode::SUCCESS ) {
      ATH_MSG_ERROR("Could not record RT Container for key " << m_par_rtcontainerkey );
      return StatusCode::FAILURE;
    }

   ATH_MSG_INFO(" Recording Error container ");
   if( (m_detstore->record(errContainer,m_par_errcontainerkey))!=StatusCode::SUCCESS ) {
      ATH_MSG_ERROR("Could not record err Container for key " << m_par_errcontainerkey );
      return StatusCode::FAILURE;
    }

   ATH_MSG_INFO(" Recording Slope container ");
   if( (m_detstore->record(slopeContainer,m_par_slopecontainerkey))!=StatusCode::SUCCESS ) {
      ATH_MSG_ERROR("Could not record slope Container for key " << m_par_slopecontainerkey );
      return StatusCode::FAILURE;
    }



  return StatusCode::SUCCESS ;
}

StatusCode TRTCondWrite::writeTextFile_Format3(std::ostream& outfile) const
{
  const RtRelationContainer* rtContainer = getRtContainer() ;
  const StrawT0Container* t0Container = getT0Container() ;
  const RtRelationContainer* errContainer = getErrContainer() ;
  const RtRelationContainer* slopeContainer = getSlopeContainer() ;


  // first store rtrelations
  outfile << "# Rtrelation" << std::endl ;
  RtRelationContainer::FlatContainer rtrelations ;
  rtContainer->getall( rtrelations ) ;
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
  errContainer->getall( errors ) ;
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
  slopeContainer->getall( slopes ) ;
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
  t0Container->getall( packedstrawdata ) ;
  float t0(0), t0err(0);
  for( TRTCond::StrawT0Container::FlatContainer::iterator it = packedstrawdata.begin() ;
       it != packedstrawdata.end(); ++it) {
    const TRTCond::ExpandedIdentifier& calid = it->first ;
    t0Container->unpack(calid,*it->second,t0,t0err) ;
    outfile << calid << " : " << t0 << " " << t0err << std::endl ;
  }
  return StatusCode::SUCCESS ;
}


const TRTCondWrite::RtRelationContainer* TRTCondWrite::getRtContainer() const {
    SG::ReadCondHandle<RtRelationContainer> rtc(m_rtReadKey);
    return *rtc;
}


const TRTCondWrite::RtRelationContainer* TRTCondWrite::getErrContainer() const {
    SG::ReadCondHandle<RtRelationContainer> erc(m_errReadKey);
    return *erc;
}

const TRTCondWrite::RtRelationContainer* TRTCondWrite::getSlopeContainer() const {
    SG::ReadCondHandle<RtRelationContainer> slc(m_slopeReadKey);
    return *slc;
}

const TRTCondWrite::StrawT0Container* TRTCondWrite::getT0Container() const {
    SG::ReadCondHandle<StrawT0Container> t0c(m_t0ReadKey);
    return *t0c;
}

TRTCond::ExpandedIdentifier TRTCondWrite::trtcondid( const Identifier& id, int level) const
{
  return { m_trtid->barrel_ec(id),m_trtid->layer_or_wheel(id),
				      m_trtid->phi_module(id),m_trtid->straw_layer(id),
				      m_trtid->straw(id),level } ;
}





