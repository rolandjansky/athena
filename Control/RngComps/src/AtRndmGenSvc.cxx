/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/DataIncident.h"
#include "GaudiKernel/ServiceHandle.h"

#include "CLHEP/Random/RanecuEngine.h"
#include "CLHEP/Random/RandGauss.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "interpretSeeds.h"
#include "AtRndmGenSvc.h"
#include "crc_combine.h"

#include <cassert>
#include <iostream>

/*FIXME temporarily needed for old seeding scheme*/
#include "StoreGate/tools/hash_functions.h" 

using namespace std;

/// Standard Constructor
AtRndmGenSvc::AtRndmGenSvc(const std::string& name,ISvcLocator* svc)
  : AthService(name,svc), 
    m_read_from_file(false),
    m_file_to_read(name + ".out"),    
    m_save_to_file(true),
    m_file_to_write(name + ".out"),
    m_useOldBrokenSeeding(false),
    m_eventReseed(true),
    m_reseedStreamNames(),
    m_reseedingOffsets(),
    m_engines(), m_engines_copy()
{
    // Get user's input
    declareProperty("Seeds", m_streams_seeds,
		    "seeds for the engines, this is a vector of strings of the form ['SequenceName [OFFSET num] Seed1 Seed2', ...] where OFFSET is an optional integer that allows to change the sequence of randoms for a given run/event no and SequenceName combination. Notice that Seed1/Seed2 are dummy when EventReseeding is used");
    declareProperty("ReadFromFile", m_read_from_file,
		    "set/restore the status of the engine from file");
    declareProperty("FileToRead",   m_file_to_read,
		    "name of a ASCII file, usually produced by AtRndmGenSvc itself at the end of a job, containing the information to fully set/restore the status of Ranecu");
    declareProperty("SaveToFile", m_save_to_file,
		    "save the status of the engine to file");
    declareProperty("FileToWrite",   m_file_to_write,
		    "name of an ASCII file which will be produced on finalize, containing the information to fully set/restore the status of Ranecu");
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
AtRndmGenSvc::~AtRndmGenSvc()  
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
AtRndmGenSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
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
AtRndmGenSvc::initialize()
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
      pIncSvc->addListener(this, "BeginEvent");
      pIncSvc->addListener(this, "ReseedIncident", PRIORITY);
  }
  pIncSvc.release().ignore();

  if (m_read_from_file)
    {
      // Read from a file
      ifstream	infile( m_file_to_read.c_str() );
      if ( !infile ) {
	ATH_MSG_ERROR (" Unable to open: " << m_file_to_read);
	return StatusCode::FAILURE;
      } else {
	std::string buffer;
	while (std::getline(infile, buffer)) {
	  string stream; 
	  uint32_t seed1, seed2;
	  if (interpretSeeds(buffer, stream, seed1, seed2)) {
	    ATH_MSG_DEBUG 
	      (" INITIALISING " << stream << " stream with seeds "
	       << seed1 << "  " << seed2 
	       << " read from file " << m_file_to_read);
	    CreateStream(seed1, seed2, stream);
	  } else {		
		ATH_MSG_ERROR
		  ("bad line\n" << buffer 
		   << "\n in input file " << m_file_to_read);
		return StatusCode::FAILURE;
	  }		
	}
	
      }
    }

    // Create the various streams according to user's request
    for (VStrings::const_iterator i = m_streams_seeds.begin(); i != m_streams_seeds.end(); ++i) {
      string stream; 
      uint32_t seed1, seed2, offset(0);
      short dummy;
      if (interpretSeeds(*i, stream, seed1, seed2, dummy, offset)) {
	ATH_MSG_VERBOSE
	  ("Seeds property: stream " << stream 
	   << " seeds " << seed1 << ' ' << seed2
	   << ", reseeding offset " << offset);
      } else {
	ATH_MSG_ERROR ("bad Seeds property\n" << (*i));
	return StatusCode::FAILURE;
      }		
      
      // Check if stream already generated (e.g. from reading a file)
      bool				not_found	=	true;
      if ( number_of_streams() != 0 ) {
	AtRndmGenSvc::engineConstIter sf 		= 	begin();
	do {
	  if ((*sf).first == stream) not_found = false;
	  ++sf;
	} while (sf != end() && not_found);
      }
	
      if (not_found) {
	ATH_MSG_DEBUG
	  (" INITIALISING " << stream << " stream with seeds "
	   << seed1 << "  " << seed2);
	CreateStream(seed1, seed2, stream);
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
AtRndmGenSvc::handle(const Incident &inc) {

  if ( inc.type() == "EndEvent" ||
       inc.type() == "AfterReseedIncident" ) 
  {
    ATH_MSG_DEBUG (" Handle EndEvent ");
    m_engines_copy.clear();
    for (engineConstIter i = begin(); i != end(); ++i)
    {
      CLHEP::HepRandomEngine* engine = GetEngine((*i).first);
      const long*	    s =	engine->getSeeds();
      std::vector<long int> tseeds;
      tseeds.push_back(s[0]);
      tseeds.push_back(s[1]);
      m_engines_copy.insert( 
	std::map<std::string, std::vector<long int> >::value_type( (*i).first,
						                   tseeds ) );
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
AtRndmGenSvc::finalize()
{
  ATH_MSG_INFO (" FINALISING ");

  if (m_save_to_file) {
    // Write the status of the Service into a file
    std::ofstream outfile( m_file_to_write.c_str() );
    if ( !outfile ) {
      ATH_MSG_ERROR ("error: unable to open: " << m_file_to_write);
    } else {
      for  (std::map<std::string, std::vector<long int> >::const_iterator i = m_engines_copy.begin();
	    i != m_engines_copy.end();
	    ++i) { 
	outfile << (*i).first << " " << (*i).second[0] << " " << (*i).second[1] << "\n";
      }
    ATH_MSG_DEBUG (" wrote seeds to " << m_file_to_write );
    }
  }
  return AthService::finalize();
}

CLHEP::HepRandomEngine*
AtRndmGenSvc::GetEngine	( const std::string& streamName )
{
    engineConstIter citer = m_engines.find(streamName);
    if ( citer == m_engines.end() )
    {
        m_engines.insert( engineValType( streamName, new CLHEP::RanecuEngine() ) );
	SetStreamSeeds	( streamName );
    }
    
    engineIter	iter	=	m_engines.find(streamName);
    return	(CLHEP::HepRandomEngine*)(*iter).second;
}

void
AtRndmGenSvc::CreateStream(uint32_t seed1, uint32_t seed2, 
			   const std::string& streamName )
{
    engineConstIter citer = m_engines.find(streamName);
    if (citer == m_engines.end())
      m_engines.insert(engineValType(streamName, new CLHEP::RanecuEngine()));
    engineIter iter = m_engines.find(streamName);
    long seeds[3] = { (long)seed1, (long) seed2, 0 };
    ((*iter).second)->setSeeds( seeds, -1 );
}

void
AtRndmGenSvc::SetStreamSeeds	( const std::string& streamName )
{
    long seed1;
    long seed2;
    if (streamName == "PYTHIA")
    {
	seed1 = m_PYTHIA_default_seed1;
	seed2 = m_PYTHIA_default_seed2;
    }
    else if (streamName == "HERWIG")
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
      (" INITIALISING " << streamName << " stream with DEFAULT seeds "
       << seed1 << "  " << seed2);
    
    long seeds[3] = { seed1, seed2, 0 };
    const long* s = seeds;
    engineIter	iter	=	m_engines.find(streamName);
    ((*iter).second)->setSeeds( s, -1 );
}

void
AtRndmGenSvc::print	( const std::string& streamName )
{
    engineConstIter citer = m_engines.find(streamName);
    if ( citer == m_engines.end() )
    {
      ATH_MSG_WARNING (" Stream =  " << streamName << " NOT FOUND");
    }
    else
    {
	const long* s	=	((*citer).second)->getSeeds();
	ATH_MSG_INFO (" Stream =  " << streamName << ", Seed1 =  "
		      << s[0] << ", Seed2 = " << s[1]);
    }
}

void
AtRndmGenSvc::print	( void )
{
    for (engineConstIter i = m_engines.begin(); i != m_engines.end(); ++i)
	print( (*i).first );
}

CLHEP::HepRandomEngine*
AtRndmGenSvc::setOnDefinedSeeds(uint32_t eventNumber, uint32_t runNumber,
				const std::string& streamName)
{
  if (m_useOldBrokenSeeding) 
    return oldSetOnDefinedSeeds(eventNumber, runNumber, streamName);
  //do it properly
  uint32_t theHash(eventNumber);
  map<string, uint32_t>::const_iterator citer(m_reseedingOffsets.find(streamName));
  bool hasOffset(citer != m_reseedingOffsets.end() && 0 != citer->second);
  if (hasOffset) theHash=crc_combine(theHash, citer->second);

  theHash=crc_combine(theHash, runNumber);
  ATH_MSG_VERBOSE( "Reseeding stream " << streamName 
		   << " with eventNumber " << eventNumber 
		   << " runNumber " << runNumber);
  if (hasOffset) ATH_MSG_VERBOSE("Applied offset " << citer->second);
  return this->setOnDefinedSeeds(theHash, streamName);
}

CLHEP::HepRandomEngine*
AtRndmGenSvc::setOnDefinedSeeds(uint32_t theSeed, 
				const std::string& streamName){
  if (m_useOldBrokenSeeding) 
    return oldSetOnDefinedSeeds(theSeed, streamName);
  //do it properly
  engineConstIter citer = m_engines.find(streamName);
  ///create engine if not found. FIXME this may not be a good idea
  if ( citer == m_engines.end() ) 
    m_engines.insert(engineValType(streamName, 
				   new CLHEP::RanecuEngine() ) );
  engineIter iter = m_engines.find(streamName);
  theSeed=crc_combine(theSeed, streamName);
  ATH_MSG_DEBUG("Reseeding stream " << streamName << " with " << theSeed);
  //Ranecu takes a long as seed and makes a test on the sign of the seed
  //so let's make sure that our seed is presented to Ranecu as a 32 bit
  //signed int
  ((*iter).second)->setSeed( (int32_t)theSeed, 0);
  return (CLHEP::HepRandomEngine*)(*iter).second;
}

CLHEP::HepRandomEngine*
AtRndmGenSvc::oldSetOnDefinedSeeds(uint32_t eventNumber, uint32_t runNumber,
				   const std::string& streamName)
{
  engineConstIter citer = m_engines.find(streamName); 
  if ( citer == m_engines.end() ) 
    m_engines.insert( engineValType(streamName, new CLHEP::RanecuEngine() ) ); 
  engineIter iter = m_engines.find(streamName); 
  int hashedStream(SG::simpleStringHash(streamName)); 
  long seeds[3] = { (long)(1000*runNumber + hashedStream),
		    (long)eventNumber, 0 }; 
  assert( seeds[0] > 0 ); 
  assert( seeds[1] > 0 ); 
  const long* s = seeds; 
  ((*iter).second)->setSeeds( s, -1 ); 
  return (CLHEP::HepRandomEngine*)(*iter).second;
}

CLHEP::HepRandomEngine*
AtRndmGenSvc::oldSetOnDefinedSeeds(uint32_t theSeed, 
				const std::string& streamName){
  engineConstIter citer = m_engines.find(streamName); 
  if ( citer == m_engines.end() ) 
    m_engines.insert( engineValType(streamName, new CLHEP::RanecuEngine() ) ); 
  engineIter iter = m_engines.find(streamName); 
  int hashedStream(SG::simpleStringHash(streamName)); 
  long seeds[3] = { (long)(hashedStream % (theSeed+13)), (long)theSeed, 0 }; 
  assert( seeds[0] > 0 ); 
  assert( seeds[1] > 0 ); 
  const long* s = seeds; 
  ((*iter).second)->setSeeds( s, -1 ); 
  return (CLHEP::HepRandomEngine*)(*iter).second; 
}

bool
AtRndmGenSvc::setAllOnDefinedSeeds(uint32_t eventNumber, uint32_t runNumber)
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
AtRndmGenSvc::setAllOnDefinedSeeds(uint32_t theSeed){
  bool allOK(true);
  engineIter i(m_engines.begin()), e(m_engines.end());
  while (i!=e && 
         (allOK=(0 != this->setOnDefinedSeeds(theSeed, (*i++).first)))) {
    /* empty */
    }
  return allOK;
}
