//Dear emacs, this is -*-c++-*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATDSFMTGENSVC_H
#define ATDSFMTGENSVC_H
/** @file AtDSFMTGenSvc.h
 *  @brief A random number engine manager, based on dSFMT. 
 *  http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/SFMT/
 *  author (of wrapper) Michael Duehrssen
 *  
 *  $Id: AtDSFMTGenSvc.h 729159 2016-03-11 12:58:15Z krasznaa $
 */

#include <map>

//base classes
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaBaseComps/AthService.h"

#include "CLHEP/Random/RandomEngine.h"
#include "Gaudi/Property.h"

class IAlgorithm; 
class ISvcLocator;
class IIncident;


/** @class AtDSFMTGenSvc
 *  @brief A random number engine manager, based on dSFMT. 
 *
 *  @details this service mantains a number of named, independent random
 *  number sequences. Each sequence is initialized by an entry of the form
 *  "SequenceName Seed1 Seed2" in the Seeds property. For example
 *  @code
 *   Seeds = [ "PYTHIA 4789899 989240512", "PYTHIA_INIT 820021 2347532",
 *             "JIMMY 390020611 821000366", "JIMMY_INIT 820021 2347532",
 *             "HERWIG 390020611 821000366", "HERWIG_INIT 820021 2347532" ]
 *
 *  @endcode
 *  At the end of the job  in AtDSFMTGenSvc::finalize(), the status of the
 *  engine is dumped as an array of unsigned long to the ASCII file
 *  "AtDSFMTGenSvc.out".  This file can be used to restore the status of the
 *  engine in another job by setting the properties
 *  @code
 *   ReadFromFile = true
 *   FileToRead = path_to_ascii_file
 *  @endcode
 */
class AtDSFMTGenSvc : virtual public IAtRndmGenSvc,
		     virtual public IIncidentListener,
		     public AthService
{
public:
    /// @name Interface to the CLHEP engine
    //@{
    CLHEP::HepRandomEngine*	GetEngine	( const std::string& streamName );
    void		CreateStream	( uint32_t seed1, uint32_t seed2, const std::string& streamName );
    bool  CreateStream (const std::vector<uint32_t>& seeds, const std::string& streamName);
    //@}

    /// CLHEP engines typedefs:
    typedef	std::map<std::string, CLHEP::HepRandomEngine*>	engineMap;
    typedef	engineMap::iterator 			engineIter;
    typedef	engineMap::const_iterator		engineConstIter;  
    typedef	engineMap::value_type			engineValType;

    /// @name Stream access
    //@{
    engineConstIter	begin			(void)	const;
    engineConstIter	end			(void)	const;
    unsigned int	number_of_streams	(void)	const;    
    void		print		( const std::string& streamName );
    void		print		( void );
    //@}

    virtual CLHEP::HepRandomEngine* setOnDefinedSeeds (uint32_t theSeed,const std::string& streamName);
    virtual CLHEP::HepRandomEngine* setOnDefinedSeeds (uint32_t eventNumber, uint32_t runNumber, const std::string& streamName);
    ///seed all streams we manage, combining theSeed and the stream names
    virtual bool setAllOnDefinedSeeds (uint32_t theSeed); 
    ///seed all streams, combining eventNumber, runNumber and the stream names
    virtual bool setAllOnDefinedSeeds (uint32_t eventNumber, uint32_t runNumber);  
    /// @name Gaudi Service Implementation
    //@{
    StatusCode initialize();
    StatusCode finalize();
    virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
    //@}

    /// IIncidentListener implementation. Handles EndEvent incident
    void handle(const Incident&);

private:
    /// @name Properties
    //@{
    /// seeds for the engines, this is a vector of strings of the form "EnginName Seed1 Seed2"
   StringArrayProperty m_streams_seeds{this,"Seeds",{},
					 "seeds for the engines, this is a vector of strings of the form "\
					   "['SequenceName [OFFSET num] Seed1 Seed2', ...] where OFFSET is an "\
					   "optional integer that allows to change the sequence of randoms for a "\
					   "given run/event no and SequenceName combination. Notice that "\
					   "Seed1/Seed2 are dummy when EventReseeding is used",
					   "OrderedSet<std::string>"};


    Gaudi::Property<bool> m_read_from_file{this,"ReadFromFile",false,"set/restore the status of the engine from file"};  ///< read engine status from file
    Gaudi::Property<std::string>  m_file_to_read{this,"FileToRead",this->name()+".out",
	"name of a ASCII file, usually produced by AtDSFMTGenSvc itself at the end of a job, containing the information to fully set/restore the status"
	}; ///< name of the file to read the engine status from
    Gaudi::Property<bool> m_save_to_file{this,"SaveToFile", true,"save the status of the engine to file"}; ///< should current engine status be saved to file ?
    Gaudi::Property<std::string> m_file_to_write{this,"FileToWrite",this->name()+".out",
	"name of an ASCII file which will be produced on finalize, containing the information to fully set/restore the status"}; ///< name of the file to save the engine status to.

    Gaudi::Property<bool> m_eventReseed{this,"EventReseeding",true,"reseed every event using a hash of run and event numbers"}; ///< reseed for every event
    StringArrayProperty m_reseedStreamNames{this,"ReseedStreamNames",{}, "the streams we are going to set the seeds of (default: all streams)"};
///< streams to be reseeded for every event   
    //@}

    /// optional offsets to combine to run/evt no when reseeding.
    /// Set using OFFSET keyword of the Seeds property
    std::map<std::string, uint32_t> m_reseedingOffsets;

    engineMap	m_engines;
    /// Random engine copy (for output to a file)
    std::map<std::string, std::vector<uint32_t> >	m_engines_copy;


    /// @name Default seed values
    //@{
    long m_default_seed1;
    long m_default_seed2;
    long m_PYTHIA_default_seed1;
    long m_PYTHIA_default_seed2;
    long m_HERWIG_default_seed1;
    long m_HERWIG_default_seed2;
    //@}

    void SetStreamSeeds( const std::string& StreamName );
    
public:
    
    // Standard Constructor
    AtDSFMTGenSvc(const std::string& name, ISvcLocator* svc);
        
    // Standard Destructor
    virtual ~AtDSFMTGenSvc();

};
 
inline	AtDSFMTGenSvc::engineConstIter
AtDSFMTGenSvc::begin			(void)	const
{ return m_engines.begin(); }

inline	AtDSFMTGenSvc::engineConstIter
AtDSFMTGenSvc::end			(void)	const
{ return m_engines.end(); }

inline	unsigned int
AtDSFMTGenSvc::number_of_streams		(void)	const
{ return m_engines.size(); }

#endif // ATDSFMTGENSVC_H


