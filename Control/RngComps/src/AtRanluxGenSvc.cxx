/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/DataIncident.h"
#include "GaudiKernel/ServiceHandle.h"

#include "CLHEP/Random/Ranlux64Engine.h"
#include "CLHEP/Random/RandGauss.h"

#include "interpretSeeds.h"
#include "AtRanluxGenSvc.h"
#include "crc_combine.h" 
#include <cassert>
#include <iostream>
#include <algorithm>

/*FIXME temporarily needed for old seeding scheme*/
#include "StoreGate/tools/hash_functions.h" 

using namespace std;



/// Standard Constructor
AtRanluxGenSvc::AtRanluxGenSvc(const std::string& name,ISvcLocator* svc)
  : AthService(name,svc), 
    m_read_from_file(false),
    m_file_to_read(name + ".out"),    
    m_save_to_file(true),
    m_file_to_write(name + ".out"),
    m_useOldBrokenSeeding(false),
    m_eventReseed(true),
    m_reseedStreamNames(),
    m_reseedingOffsets(),
    m_engines(), m_engines_copy(),
    m_defaultLuxLevel(1) ///depends on Ranlux64 implementation. 1 is default at least up until 2010
{
    // Get user's input
    declareProperty("Seeds", m_streams_seeds,
		    "seeds for the engines, a string of the form ['SequenceName [LUXURY luxLevel (range 0:2 default 1)()] [OFFSET num] Seed1 Seed2', ...] where OFFSET is an optional integer that allows to change the sequence of randoms for a given run/event no and SequenceName combination. Notice that Seed1/Seed2 are dummy when EventReseeding is used");
    declareProperty("ReadFromFile", m_read_from_file,
		    "set/restore the status of the engine from file");
    declareProperty("FileToRead",   m_file_to_read,
		    "name of a ASCII file, usually produced by AtRanLuxGenSvc itself at the end of a job, containing the information to fully set/restore the status of Ranlux64");
    declareProperty("SaveToFile", m_save_to_file,
		    "save the status of the engine to file");
    declareProperty("FileToWrite",   m_file_to_write,
		    "name of an ASCII file which will be produced on finalize, containing the information to fully set/restore the status of Ranlux64");
    declareProperty("UseOldBrokenSeeding",   m_useOldBrokenSeeding,
		    "use old seeding mechanism. This is broken in that the same seeds generate different sequences for 32 and 64 bit architectures");
    declareProperty("EventReseeding", m_eventReseed, "reseed every event using a hash of run and event numbers");
    declareProperty("ReseedStreamNames", m_reseedStreamNames, "the streams we are going to set the seeds of (default: all streams)");

    // Set Default values
    m_default_seed1		=	3591;
    m_default_seed2		=	2309736;
    m_PYTHIA_default_seed1	=	93453591;
    m_PYTHIA_default_seed2	=	73436;
    m_HERWIG_default_seed1	=	355391;
    m_HERWIG_default_seed2	=	97336;
}


/// Standard Destructor
AtRanluxGenSvc::~AtRanluxGenSvc()  
{
  engineIter i(m_engines.begin()), e(m_engines.end());
  while (i != e) delete (i++)->second;
}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode 
AtRanluxGenSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
    if ( IAtRndmGenSvc::interfaceID().versionMatch(riid) )    {
        *ppvInterface = (IAtRndmGenSvc*)this;
    }
    else  {
	// Interface is not directly available: try out a base class
	return AthService::queryInterface(riid, ppvInterface);
    }
    addRef();
    return StatusCode::SUCCESS;
}

StatusCode 
AtRanluxGenSvc::initialize()
{
  ATH_CHECK( AthService::initialize() );
  ATH_MSG_INFO
    ("Initializing " << name()
     << " - package version " << PACKAGE_VERSION 
     << "\n INITIALISING RANDOM NUMBER STREAMS. ");

  /// Incident Service
  ServiceHandle<IIncidentSvc> pIncSvc("IncidentSvc", name());

  // set up the incident service:
  if (!(pIncSvc.retrieve()).isSuccess()) {
    ATH_MSG_ERROR ("Could not locate IncidentSvc ");
    return StatusCode::FAILURE;
  }
  
  //start listening to "EndEvent"
  static const int PRIORITY = 100;
  pIncSvc->addListener(this, "EndEvent", PRIORITY);
  pIncSvc->addListener(this, "AfterReseedIncident", PRIORITY);

  //and to BeginEvent if we are reseeding
  if (m_eventReseed) {
      ATH_MSG_INFO ("will be reseeded for every event");
      pIncSvc->addListener(this, "BeginEvent", PRIORITY);
      pIncSvc->addListener(this, "ReseedIncident", PRIORITY);
  }
  pIncSvc.release().ignore();


  //3 is not valid for ranlux, but whatever, that's what we had before...
  if (m_useOldBrokenSeeding) m_defaultLuxLevel = 3; 

  if (m_read_from_file) {
    // Read from a file
    ifstream	infile( m_file_to_read.c_str() );
    if ( !infile ) {
      ATH_MSG_ERROR (" Unable to open: " << m_file_to_read);
      return StatusCode::FAILURE;
    } else {
      std::string buffer;
      while (std::getline(infile, buffer)) {
	string stream; 
	std::vector<uint32_t> seeds;
	//split the space-separated string in 3 words:
	if (interpretSeeds(buffer, stream, seeds)) {
	  msg (MSG::DEBUG) 
	    << " INITIALISING " << stream << " stream with seeds ";
	  for (std::vector<uint32_t>::const_iterator i = seeds.begin(); i != seeds.end(); ++i){
            // The state returned is a set of 32-bit numbers.
            // On 64-bit platforms, though, the vector holds 64-bit ints.
            // For the first word, one gets garbage in the high 32 bits.
            // (This is because the crc32 routine used in clhep
            // to hash the engine names doesn't mask down to 32 bits.)
            // So mask off the garbage so that we get consistent results
            // across platforms.
            msg() << ((*i) & 0xffffffffu) << " ";
	  }
	  msg() << " read from file " << m_file_to_read << endmsg;
	  if (CreateStream(seeds, stream)) {
	    msg(MSG::DEBUG)
	      << stream << " stream initialized succesfully" <<endmsg;
	  } else {
	    msg(MSG::ERROR)
	      << stream << " stream FAILED to initialize" <<endmsg;
	    return StatusCode::FAILURE;
	  }
	} else {		
	  msg(MSG::ERROR)
	    << "bad line\n" << buffer 
	    << "\n in input file " << m_file_to_read << endmsg;
	  return StatusCode::FAILURE;
	}		
      }
      
    }
  }
  // Create the various streams according to user's request
  for (VStrings::const_iterator i = m_streams_seeds.begin(); i != m_streams_seeds.end(); ++i) {
    string stream; 
    uint32_t seed1, seed2, offset(0);
    //parse the stream property string
    short ll(m_defaultLuxLevel); // temp copy so we don't overwrite default
    if (interpretSeeds(*i, stream, seed1, seed2, ll, offset)) {
      ATH_MSG_VERBOSE("Seeds property: stream " << stream 
		      << " seeds " << seed1 << ' ' << seed2 
		      << ", luxury level " << ll
		      << ", reseeding offset " << offset);
    } else {
      ATH_MSG_ERROR("bad Seeds property\n" << (*i));
      return StatusCode::FAILURE;
    }		
    	
    // Check if stream already generated (e.g. from reading a file)
    bool not_found(true);
    if ( number_of_streams() != 0 ) {
      AtRanluxGenSvc::engineConstIter sf(begin());
      while (sf != end() && (not_found=((*sf).first != stream))) ++sf;
    }
	
    if (not_found) {
      ATH_MSG_DEBUG
	(" INITIALISING " << stream << " stream with seeds "
	 << seed1 << "  " << seed2);
      createStream(seed1, seed2, stream, ll);
      if (m_eventReseed) {
	m_reseedingOffsets.insert(std::make_pair(stream, offset));
	// apply the offset we just inserted
	ATH_MSG_DEBUG("Applying reseeding  offset " << offset << 
		      " to stream " << stream);
	this->setOnDefinedSeeds(seed1, seed2, stream);
      }
    }
    
  }
  return StatusCode::SUCCESS;
}

void 
AtRanluxGenSvc::handle(const Incident &inc) {
  ATH_MSG_DEBUG (" Handle EndEvent ");

  if ( inc.type() == "EndEvent" ||
       inc.type() == "AfterReseedIncident" ) 
  {

    m_engines_copy.clear();
    engineConstIter iE(begin()), eE(end());
    while(iE != eE) {
      CLHEP::HepRandomEngine* engine = GetEngine(iE->first);
      std::vector<unsigned long> v = engine->put();
      std::vector<uint32_t> tseeds(v.size());
      for (size_t i=0; i<v.size(); ++i) {
        // The state returned is a set of 32-bit numbers.
        // On 64-bit platforms, though, the vector holds 64-bit ints.
        // For the first word, one gets garbage in the high 32 bits.
        // (This is because the crc32 routine used in clhep
        // to hash the engine names doesn't mask down to 32 bits.)
        // Mask off the garbage to get consistent results
        // across platforms.
	tseeds[i] = (v[i] & 0xffffffffu);
      }
      m_engines_copy.insert(std::make_pair(iE->first, tseeds));
      ++iE;
    }
    
    print();    
  } else if (inc.type() == "BeginEvent") {
    ATH_MSG_DEBUG (" Handle BeginEvent ");
    EventContext context = inc.context();
    const EventIDBase& ei = context.eventID();
    //clear static RandGauss cache (generates two numbers per call to shoot()
    CLHEP::RandGauss::setFlag(false);
    //loop over generator streams, combining the stream name to the hash
    vector<string>::const_iterator i(m_reseedStreamNames.begin());
    vector<string>::const_iterator e(m_reseedStreamNames.end());
    //by default (when no streams are specified in streamNames, seed all
    //streams
    if (i == e) {
      if (!(this->setAllOnDefinedSeeds(ei.event_number(), 
				       ei.run_number())))
	throw GaudiException("can not reseed all streams ", name(), StatusCode::FAILURE);
    } else {
      while (i != e) {
	const string& strName(*i++);
	if (0 == this->setOnDefinedSeeds(ei.event_number(), 
					 ei.run_number(),
					 strName)) { 
	  throw GaudiException(string("can not reseed stream ") + strName,  
			       name(), StatusCode::FAILURE);
	} else {
	  msg() << MSG::VERBOSE << "Reseeded stream " << strName 
		<< " for random service " << endmsg;
	}
      }
    }
  }
  else if (inc.type() == "ReseedIncident") {
    typedef ContextIncident<std::pair<unsigned,unsigned> > Ctxt;
    const Ctxt* incident = dynamic_cast<const Ctxt*>(&inc);
    const std::pair<unsigned,unsigned>& data = incident->tag();
    //clear static RandGauss cache (generates two numbers per call to shoot()
    CLHEP::RandGauss::setFlag(false);
    //loop over generator streams, combining the stream name to the hash
    vector<string>::const_iterator i(m_reseedStreamNames.begin());
    vector<string>::const_iterator e(m_reseedStreamNames.end());
    //by default (when no streams are specified in streamNames, seed all
    //streams
    if (i == e) {
      if (!(this->setAllOnDefinedSeeds(data.first,
                                       data.second)))
        throw GaudiException("can not reseed all streams ", name(), StatusCode::FAILURE);
    } else {
      while (i != e) {
        const string& strName(*i++);
        if (0 == this->setOnDefinedSeeds(data.first,
                                         data.second,
                                         strName)) {
          throw GaudiException(string("can not reseed stream ") + strName,
                               name(), StatusCode::FAILURE);
        } else {
          msg() << MSG::VERBOSE << "Reseeded stream " << strName
                << " for random service " << endmsg;
        }
      }
    }
  }
}

StatusCode 
AtRanluxGenSvc::finalize()
{
  ATH_MSG_INFO (" FINALISING ");

  if (m_save_to_file) {
    // Write the status of the Service to file
    std::ofstream outfile( m_file_to_write.c_str() );
    if ( !outfile ) {
      ATH_MSG_ERROR ("error: unable to open: " << m_file_to_write);
    } else {
      for (std::map<std::string, std::vector<uint32_t> >::const_iterator i = m_engines_copy.begin();
	   i != m_engines_copy.end();
	   ++i) {
	outfile << (*i).first << " ";
	for (std::vector<uint32_t>::const_iterator j = (*i).second.begin(); j !=(*i).second.end(); ++j){
	  outfile << (*j) << " ";
	}
	outfile << endl;
      }
      ATH_MSG_DEBUG (" wrote seeds to " << m_file_to_write );
      
    }
  }
  return AthService::finalize();
}

CLHEP::HepRandomEngine*
AtRanluxGenSvc::GetEngine	( const std::string& StreamName )
{
    engineConstIter citer = m_engines.find(StreamName);
    if ( citer == m_engines.end() )
    {
	m_engines.insert( engineValType( StreamName, new CLHEP::Ranlux64Engine() ) );
	SetStreamSeeds	( StreamName );
    }
    
    engineIter	iter	=	m_engines.find(StreamName);
    return	(CLHEP::HepRandomEngine*)(*iter).second;
}

void
AtRanluxGenSvc::CreateStream(uint32_t seed1, uint32_t seed2, 
			     const std::string& streamName) {
  createStream(seed1, seed2, streamName, m_defaultLuxLevel);
}

void
AtRanluxGenSvc::createStream(uint32_t seed1, uint32_t seed2, 
			     const std::string& streamName, short luxLevel )
{
    uint32_t seeds[2] = { seed1, seed2 };
    const uint32_t* s = seeds;
    engineConstIter citer = m_engines.find(streamName);
    if ( citer == m_engines.end() ) 
      m_engines.insert(engineValType(streamName, 
				     new CLHEP::Ranlux64Engine(s[0], luxLevel)));
    engineIter iter = m_engines.find(streamName);
    ((*iter).second)->setSeed(s[0], luxLevel);
}

bool
AtRanluxGenSvc::CreateStream(const std::vector<uint32_t>& seeds, 
			     const std::string& streamName)
{
    engineConstIter citer = m_engines.find(streamName);
    if ( citer == m_engines.end() ) 
      m_engines.insert(engineValType(streamName, 
				     new CLHEP::Ranlux64Engine() ) );
    engineIter	iter = m_engines.find(streamName);
    std::vector<unsigned long> longSeeds(seeds.size());
    for (size_t i=0; i<seeds.size(); ++i) longSeeds[i]=seeds[i];
    return (((*iter).second)->getState( longSeeds ));
}

void
AtRanluxGenSvc::SetStreamSeeds	( const std::string& StreamName )
{
    uint32_t seed1;
    uint32_t seed2;
    if (StreamName == "PYTHIA")
    {
	seed1 = m_PYTHIA_default_seed1;
	seed2 = m_PYTHIA_default_seed2;
    }
    else if (StreamName == "HERWIG")
    {
	seed1 = m_HERWIG_default_seed1;
	seed2 = m_HERWIG_default_seed2;
    }
    else
    {
	seed1 = m_default_seed1;
	seed2 = m_default_seed2;
    }
    ATH_MSG_WARNING
      (" INITIALISING " << StreamName << " stream with DEFAULT seeds "
       << seed1 << "  " << seed2);
    
    uint32_t seeds[2] = { seed1, seed2 };
    const uint32_t* s = seeds;
    engineIter iter = m_engines.find(StreamName);
    ((*iter).second)->setSeed(s[0]); //FIXME no need to set luxLevel?
}

void
AtRanluxGenSvc::print(const std::string& StreamName )
{
    engineConstIter citer = m_engines.find(StreamName);
    if ( citer == m_engines.end() ) {
      ATH_MSG_WARNING (" Stream =  " << StreamName << " NOT FOUND");
    } else {
      std::vector<unsigned long> v = ((*citer).second)->put();
      msg(MSG::INFO) << " Stream =  " << StreamName << " ";
      for (std::vector<unsigned long>::const_iterator i = v.begin(); i != v.end(); ++i){
        // The state returned is a set of 32-bit numbers.
        // On 64-bit platforms, though, the vector holds 64-bit ints.
        // For the first word, one gets garbage in the high 32 bits.
        // (This is because the crc32 routine used in clhep
        // to hash the engine names doesn't mask down to 32 bits.)
        // So mask off the garbage so that we get consistent results
        // across platforms.
	msg() << ((*i) & 0xffffffffu) << " ";
      }
      msg() << endmsg;
    }
}

void
AtRanluxGenSvc::print	( void )
{
    for (engineConstIter i = m_engines.begin(); i != m_engines.end(); ++i)
	print( (*i).first );
}

CLHEP::HepRandomEngine*
AtRanluxGenSvc::setOnDefinedSeeds(uint32_t eventNumber, uint32_t runNumber,
				  const std::string& streamName)
{
  if (m_useOldBrokenSeeding) 
    return oldSetOnDefinedSeeds(eventNumber, runNumber, streamName);
  // do it properly
  uint32_t theHash(eventNumber);
  map<string, uint32_t>::const_iterator citer(m_reseedingOffsets.find(streamName));
  bool hasOffset(citer != m_reseedingOffsets.end() && 0 != citer->second);
  if (hasOffset) theHash=crc_combine(theHash, citer->second);

  theHash=crc_combine(theHash, runNumber);
  ATH_MSG_VERBOSE( "Reseeding stream " << streamName 
		   << " with eventNumber " << eventNumber 
		   << " runNumber " << runNumber);
  if (hasOffset) ATH_MSG_VERBOSE("Applied offset " <<  citer->second);
  return this->setOnDefinedSeeds(theHash, streamName);
}

CLHEP::HepRandomEngine*
AtRanluxGenSvc::setOnDefinedSeeds(uint32_t theSeed, 
				  const std::string& streamName){
  if (m_useOldBrokenSeeding) 
    return oldSetOnDefinedSeeds(theSeed, streamName);
  //do it propertly
  engineConstIter citer = m_engines.find(streamName);
  ///create engine if not found. FIXME this may not be a good idea
  if ( citer == m_engines.end() ) 
    m_engines.insert(engineValType(streamName, 
				   new CLHEP::Ranlux64Engine() ) );

  engineIter iter = m_engines.find(streamName);
  theSeed=crc_combine(theSeed, streamName);
  ATH_MSG_DEBUG("Reseeding stream " << streamName << " with " << theSeed);
  CLHEP::Ranlux64Engine* eng = (*iter).second;
  //Ranlux64 takes a long as seed and makes a test on the sign of the seed
  //so let's make sure that our seed is presented to Ranlux64 as a 32 bit
  //signed int
  eng->setSeed( (int32_t)theSeed, eng->getLuxury() );
  return (CLHEP::HepRandomEngine*)eng;
}


CLHEP::HepRandomEngine*
AtRanluxGenSvc::oldSetOnDefinedSeeds(uint32_t eventNumber, uint32_t runNumber,
				  const std::string& streamName)
{
  engineConstIter citer = m_engines.find(streamName); 
  if ( citer == m_engines.end() ) 
    m_engines.insert( engineValType(streamName, new CLHEP::Ranlux64Engine() ) ); 
  engineIter iter = m_engines.find(streamName); 
  int hashedStream(SG::simpleStringHash(streamName)); 
  long seeds[2] = { (long)(1000*runNumber + hashedStream),
		    (long)eventNumber }; 
  assert( seeds[0] > 0 ); 
  assert( seeds[1] > 0 ); 
  const long* s = seeds; 
  ((*iter).second)->setSeed( s[0], m_defaultLuxLevel); 
  return  (CLHEP::HepRandomEngine*)(*iter).second;   
}

CLHEP::HepRandomEngine*
AtRanluxGenSvc::oldSetOnDefinedSeeds(uint32_t theSeed, 
				     const std::string& streamName){
  engineConstIter citer = m_engines.find(streamName); 
  if ( citer == m_engines.end() ) 
    m_engines.insert( engineValType(streamName, new CLHEP::Ranlux64Engine() ) ); 
  engineIter iter =  m_engines.find(streamName); 
  int hashedStream(SG::simpleStringHash(streamName)); 
  long seeds[2] = { (long)(hashedStream % (theSeed+13)), 
		    (long)theSeed }; 
  assert( seeds[0] > 0 ); 
  assert( seeds[1] > 0 ); 
  const long* s = seeds; 
  ((*iter).second)->setSeed( s[0], m_defaultLuxLevel ); 
  return        (CLHEP::HepRandomEngine*)(*iter).second; 
}


bool
AtRanluxGenSvc::setAllOnDefinedSeeds(uint32_t eventNumber, uint32_t runNumber)
{
  bool allOK(true);
  engineIter i(m_engines.begin()), e(m_engines.end());
  while (i!=e && 
         (allOK=(0 != this->setOnDefinedSeeds(eventNumber,
                                              runNumber,
                                              (*i++).first)))) {
    /*empty*/
  }
  return allOK;
}

bool
AtRanluxGenSvc::setAllOnDefinedSeeds(uint32_t theSeed){
  bool allOK(true);
  engineIter i(m_engines.begin()), e(m_engines.end());
  while (i!=e && 
         (allOK=(0 != this->setOnDefinedSeeds(theSeed, (*i++).first)))) {
    /*empty*/
    }
  return allOK;
}
