/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATRANLUXGENSVC_H
#define ATRANLUXGENSVC_H
/** @file AtRanluxGenSvc.h
 *  @brief The default ATLAS random number engine manager, based on Ranlux64
 *  @author Paolo Calafiura
 *  @author George Stavropoulous
 *  
 *  $Id: AtRanluxGenSvc.h 729159 2016-03-11 12:58:15Z krasznaa $
 */

#include <map>

//base classes
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaBaseComps/AthService.h"
#include "Gaudi/Property.h"

#include "CLHEP/Random/Ranlux64Engine.h"

/** @class AtRanluxGenSvc
 *  @brief The default ATLAS random number engine manager, based on Ranlux64
 *
 *  @details this service mantains a number of named, independent random
 *  number sequences. Each sequence is initialized by an entry of the form
 *  "SequenceName [LUXURY luxLevel (range 0:2 default 1) ] Seed1 Seed2" 
 *  in the Seeds property. For example
 *  @code
 *   Seeds = [ "PYTHIA LUXURY 2 4789899 989240512", 
 *             "PYTHIA_INIT LUXURY 0 820021 2347532",
 *             "JIMMY 390020611 821000366", 
 *             "JIMMY_INIT LUXURY 0 820021 2347532",
 *             "HERWIG 390020611 821000366", "HERWIG_INIT 820021 2347532" ]
 *
 *  @endcode
 *  At the end of the job  in AtRanluxGenSvc::finalize(), the status of the
 *  engine is dumped as an array of uint32_t to the ASCII file
 *  "AtRanluxGenSvc.out":
 *  @code
 *   PITHIA 4010409557 1071463497 2862960128 1068652744 145815808 1072305199 2363435712 1072215177 2274024032 1071848040 1607052768 1070928280 1688486400 1071840599 2068481888 1067315347 62450688 1072023539 312153120 1070857822 2535009472 1070427845 3698875904 1070616419 3726185024 0 0 11 1 202 
 *   PITHIA_INIT 4010409557 1057836705 3838836736 1072443439 2098440704 1066892778 3689119744 1072151794 3818888768 1071853913 3574320864 1071982408 3603063712 1071718264 2168786016 1072391676 2208235040 1072000044 3037871232 1072480358 3292613248 1071901297 579757952 1065720911 3788918784 0 0 11 1 202 
 * @endcode
 *  This file can be used to restore the status of the
 *  engine in another job by setting the properties
 *  @code
 *   ReadFromFile = true
 *   FileToRead = path_to_ascii_file
 *  @endcode
 * 
 */
class AtRanluxGenSvc : virtual public IAtRndmGenSvc,
                       virtual public IIncidentListener,
		       public AthService
{
public:
    /// @name Interface to the CLHEP engine
    //@{
    virtual CLHEP::HepRandomEngine* GetEngine(const std::string& streamName);
    virtual void CreateStream(uint32_t seed1, uint32_t seed2,
			      const std::string& streamName);
    virtual bool CreateStream(const std::vector<uint32_t>& seeds, 
			      const std::string& streamName);
    //@}

    /// @name CLHEP engines typedefs:
    //@{
    typedef std::map<std::string, CLHEP::Ranlux64Engine*> engineMap;
    typedef engineMap::iterator 		   engineIter;
    typedef engineMap::const_iterator              engineConstIter;  
    typedef engineMap::value_type		   engineValType;
    //@}

    /// @name Stream access
    //@{
    engineConstIter	begin			(void)	const;
    engineConstIter	end			(void)	const;
    unsigned int	number_of_streams	(void)	const;    
    void		print		( const std::string& streamName );
    void		print		( void );
    /// allows to specify luxLevel
    void createStream(uint32_t seed1, uint32_t seed2,
		      const std::string& streamName, short luxLevel);
    //@}

    virtual CLHEP::HepRandomEngine* setOnDefinedSeeds (uint32_t theSeed, 
						const std::string& streamName);
    virtual CLHEP::HepRandomEngine* setOnDefinedSeeds (uint32_t eventNumber, 
						uint32_t runNumber,
						const std::string& streamName);
    ///broken, temporarily keep for backward compatibility
    CLHEP::HepRandomEngine* oldSetOnDefinedSeeds (uint32_t theSeed, 
					   const std::string& streamName);
    ///broken, temporarily keep for backward compatibility
    CLHEP::HepRandomEngine* oldSetOnDefinedSeeds (uint32_t eventNumber, 
					   uint32_t runNumber,
					   const std::string& streamName);
    ///seed all streams we manage, combining theSeed and the stream names
    virtual bool setAllOnDefinedSeeds (uint32_t theSeed); 
    ///seed all streams, combining eventNumber, runNumber and the stream names
    virtual bool setAllOnDefinedSeeds (uint32_t eventNumber, uint32_t runNumber);
  
    /// @name Gaudi Service Implementation
    //@{
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode queryInterface( const InterfaceID& riid, 
				       void** ppvInterface );
    //@}

    /// IIncidentListener implementation. Handles EndEvent incident
    virtual void handle(const Incident&);


private:
    /// @name Properties
    //@{
    /// seeds for the engines, this is a vector of strings of the form "EnginName Seed1 Seed2"  
    StringArrayProperty m_streams_seeds{this,"Seeds",{},
	"seeds for the engines, a string of the form ['SequenceName [LUXURY luxLevel (range 0:2 default 1)()] "	\
	  "[OFFSET num] Seed1 Seed2', ...] where OFFSET is an optional integer that allows to change the " \
	  "sequence of randoms for a given run/event no and SequenceName combination. Notice that " \
	  "Seed1/Seed2 are dummy when EventReseeding is used",
	  "OrderedSet<std::string>"};


    Gaudi::Property<bool> m_read_from_file{this,"ReadFromFile",false,"set/restore the status of the engine from file"};  ///< read engine status from file
    Gaudi::Property<std::string>  m_file_to_read{this,"FileToRead",this->name()+".out",
	"name of a ASCII file, usually produced by AtRanLuxGenSvc itself at the end of a job, "	\
	"containing the information to fully set/restore the status of Ranlux64"
	}; ///< name of the file to read the engine status from
    Gaudi::Property<bool> m_save_to_file{this,"SaveToFile", true,"save the status of the engine to file"}; ///< should current engine status be saved to file ?
    Gaudi::Property<std::string> m_file_to_write{this,"FileToWrite",this->name()+".out",
	"name of an ASCII file which will be produced on finalize, containing the information to fully set/restore the status"}; ///< name of the file to save the engine status to.

    Gaudi::Property<bool> m_eventReseed{this,"EventReseeding",true,"reseed every event using a hash of run and event numbers"}; ///< reseed for every event
    StringArrayProperty m_reseedStreamNames{this,"ReseedStreamNames",{}, "the streams we are going to set the seeds of (default: all streams)"};
    ///< streams to be reseeded for every event   

    Gaudi::Property<bool> m_useOldBrokenSeeding{this,"UseOldBrokenSeeding",false}; ///< backward compatibility only, broken 32/64 bits
    //@}
    
    /// optional offsets to combine to run/evt no when reseeding.
    /// Set using OFFSET keyword of the Seeds property
    std::map<std::string, uint32_t> m_reseedingOffsets;

    engineMap	m_engines;
    /// Random engine copy (for output to a file)
    std::map<std::string, std::vector<uint32_t> >	m_engines_copy;
    short m_defaultLuxLevel; ///< Ranlux luxury level to be used by default


    /// @name Default seed values
    //@{
    uint32_t m_default_seed1;
    uint32_t m_default_seed2;
    uint32_t m_PYTHIA_default_seed1;
    uint32_t m_PYTHIA_default_seed2;
    uint32_t m_HERWIG_default_seed1;
    uint32_t m_HERWIG_default_seed2;
    //@}

    void SetStreamSeeds( const std::string& streamName );
    
public:
    
    /// Standard Gaudi Constructor
    AtRanluxGenSvc(const std::string& name, ISvcLocator* svc);
        
    virtual ~AtRanluxGenSvc();

};
 
inline	AtRanluxGenSvc::engineConstIter
AtRanluxGenSvc::begin			(void)	const
{ return m_engines.begin(); }

inline	AtRanluxGenSvc::engineConstIter
AtRanluxGenSvc::end			(void)	const
{ return m_engines.end(); }

inline	unsigned int
AtRanluxGenSvc::number_of_streams		(void)	const
{ return m_engines.size(); }

#endif // ATRANLUXGENSVC_H


