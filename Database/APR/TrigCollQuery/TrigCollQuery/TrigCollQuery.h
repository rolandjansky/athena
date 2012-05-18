/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOLLQUERY_TRIGCOLLQUERY_H
#define TRIGCOLLQUERY_TRIGCOLLQUERY_H

#include <map>
#include <string>
#include <vector>
#include <set>

class TrigCollQuery
{
public:
  TrigCollQuery();
  ~TrigCollQuery();

  /// read in the map of trigger bits
  void          readTriggerMap( std::string filename = "" );

   /// replace trigger names in a collection query with bitwise operands
  std::string   triggerQueryRemap( const std::string& query,
                                   const std::string& tech );

  /// use "CTPWord" attribute name for L1 trigger
  /// - for backward compatibility wotj early 2008 tags (FDR2)
  void          useCTPWord( bool use=true );

  /// restrict trigger query to given runs
  void		queryRuns( const std::string& runs );

  /// set (get) the name of the RunNumber column
  std::string	runNumberColumn( const std::string& column = "" );

  /// set/get the URL for the trigger decoding web service
  std::string   triggerWebService( const std::string& url = "" );

  /// set/get the directory URL where trigger XML configuration files are
  std::string   XMLConfigDir( const std::string& url = "" );

  std::string   defaultXMLConfigDir();

  /// set paths to certificate when using trigger web decoding
  void          setCertPath( const std::string& certpath, const std::string& keypath );

  /// set  MC project
  void          setDataPeriod( const std::string& period )   { m_dataPeriod = period; } 
  /// set AMI tag
  void          setAMITag( const std::string& tag )          { m_AMITag = tag; }

  void          setDebug()                                   { m_outputLevel = 1; }
 
protected:
  typedef  std::map<std::string, unsigned>    TriggerMap;

  /// parse the XML file with trigger bit positions
  int           parseXML(  const std::string& filepath, TriggerMap& trigMap );

  /// read Web server directory looking for configuration files
  int		readWebDir( const std::string& _url, std::set<std::string>& files );

  /// initialize Xersecs
  bool		initXML();


  
  struct TrigValRange {
    TrigValRange( unsigned sr, unsigned er, unsigned bitpos, const std::string& attribute="" )
          : startrun(sr), endrun(er), bitPosition(bitpos),
            attributeName( attribute )
    { }

    bool operator<(const TrigValRange& rhs) const
    {
       return bitPosition < rhs.bitPosition;
    }
  
    unsigned              startrun, endrun, bitPosition;
    std::string           attributeName;
  };

  typedef std::vector< TrigValRange >   TrigValRangeVect;

  /// Query the web service for trigger bit mappings
  /// trigger : coma separated list of trigger names
  /// useHighBits : when true use the upper half of the word for 64 but EF trigger attributes,
  void          WSQueryTriggerPositions( const std::string& triggers, bool useHighBits );

 /// Build list of trigger bit positions for query run ranges
  /// trigger  : trigger name
  /// highBits : use to query upper half of a 64bit trigger word attribute (currently EF only)
  TrigValRangeVect      makeTrigValRangeVect( const std::string& trigger, bool highBits );

  /*
    tvRange : run range with trigger bit position constant (contains bit position and attribute name)
    condidtion : true for testing bit value of 1, false for 0
    tech : POOL Collection technology (ROOT or Relational)
  */
  /// Produce trigger condition query fragment for a given run-range
  std::string   addTriggerCondition( const TrigValRange& tvRange,
				     bool condition,
				     bool firstPass,
				     const std::string& tech,
				     int firstRange,
				     int valRangeGrouping);

  typedef  std::vector< std::pair<unsigned,unsigned> >  RunsVect;
  
  // properties - filled in by athena joboptions
  // (declared here so TrigCollQueryTool can be considered an interface)

  /// location of the trigger map XML configuration files. can be directory or URL
  std::string           m_triggerMapDir;

  /// the name of the attribute wher RunNumber is kept in the collection
  std::string		m_runNumberColumnName;
  
  /// list of run ranges to query (empty = query all runs)
  std::string		m_queryRuns;

  /// compatibility flag for the old name of CTPWord attribute
  bool                  m_useCTPWord;

  bool			m_xmlInitialized;
  
  /// runs option string (e.g. as specified on the command line)
  std::string		m_runsStr;
  /// decoded runs from m_runsStr
  RunsVect		m_runs;

  /// URL of the trigger decoding web service
  std::string           m_triggerWebService;

  /// true (defualt) - use web service, false - use XML config files
  bool                  m_usingWebService;

  /// paths to certificate files
  std::string           m_certpath;
  std::string           m_keypath;

  /// data period used by the trigger decoding web service to identify MC project
  std::string           m_dataPeriod;
  /// AMI tag used by the trigger decoding web service for MC projects
  std::string           m_AMITag;

  /// output level setting, so far only non-zero recognized
  int                   m_outputLevel;
  
  struct TriggerRunMap {
    unsigned	startrun, endrun;
    TriggerMap	triggerPos;
  };

  class TriggerMapVect : public  std::vector< TriggerRunMap* >  {
    public:
    ~TriggerMapVect() {
       for( size_t i = 0; i<this->size(); i++ )   delete (*this)[i];
    }
  };

  TriggerMapVect        m_triggerMapVect;

  typedef std::map< std::string, TrigValRangeVect* >   TriggerRangesMap;
  TriggerRangesMap      m_triggerRangesMap;

};


#endif


