/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCollQuery/TrigCollQuery.h"

#include "boost/tokenizer.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <memory>
#include <set>
#include <iomanip>


const std::string TriggerPassed( "TriggerPassed" );
const std::string TriggerNotPassed( "TriggerNotPassed" );
const std::string TriggerPassedLowbits( "TriggerPassedInclusive" );
const std::string TriggerNotPassedLowbits( "TriggerNotPassedInclusive" );

const std::string L1prefix( "L1_" );
const std::string L2prefix( "L2_" );
const std::string EFprefix( "EF_" );

const std::string L1word_old( "CTPWord" );
const std::string L1word( "L1PassedTrigMaskTAV" );
const std::string L1wordAP( "L1PassedTrigMaskTAP" );
const std::string L1wordBP( "L1PassedTrigMaskTBP" );
const std::string L2word( "L2PassedTrigMask" );
const std::string EFword( "EFPassedTrigMask" );

const std::string root_bitAndStr( " & " );
const std::string root_logicAndStr( " && " );
const std::string root_logicOrStr( " || " );
const std::string root_equalsStr( "==" );
const std::string root_notEqualsStr( "!=" );

const std::string sql_bitAndStr( " bitand(" );
const std::string sql_logicAndStr( " AND " );
const std::string sql_logicOrStr( " OR " );
const std::string sql_equalsStr( " = " );
const std::string sql_notEqualsStr( " != " );

const std::string XMLDir( "TriggerMenuXML" );
const std::string MapFileName( "ChainTagMap" );
const std::string RunNumberAttribute( "RunNumber" );


class SyntaxError : public std::runtime_error {
public:
  SyntaxError() : std::runtime_error("Syntax error in the trigger condition")
  {}
};

class UnknownTrigger : public std::runtime_error {
public:
  UnknownTrigger( const std::string& msg )
        :  std::runtime_error( std::string("Unknown trigger name: ") + msg )
  {}
};



TrigCollQuery::TrigCollQuery() :
      m_runNumberColumnName( RunNumberAttribute )
      , m_xmlInitialized( false )
      , m_usingWebService( false )
      , m_outputLevel( 0 )
{
   useCTPWord(false);
   triggerWebService("https://atlas-tagservices.cern.ch/tagservices/triggerDecoder/getDecodedTriggerInfo.php");
}



void TrigCollQuery::useCTPWord( bool use ) {
   m_useCTPWord = use;
}

std::string
TrigCollQuery::triggerWebService( const std::string& url ) {
   if( url.length() ) {
      m_triggerWebService = url;
      m_usingWebService = true;
   } 
   return m_triggerWebService;
}

std::string
TrigCollQuery::XMLConfigDir( const std::string& url ) {
   if( url.length() ) {
      m_triggerMapDir = url;
      m_usingWebService = false;
   }
   return m_triggerMapDir;
}

std::string  
TrigCollQuery::defaultXMLConfigDir() {
   return "/afs/cern.ch/user/a/attrgcnf/TagChainMaps/";
}


void
TrigCollQuery::setCertPath( const std::string& certpath, const std::string& keypath ) {
   m_certpath = certpath;
   m_keypath = keypath;
}


   
typedef boost::tokenizer<boost::char_separator<char> > Tizer;


std::string
TrigCollQuery::triggerQueryRemap( const std::string& query, const std::string& tech )
{
  std::string mappedQuery;
   boost::char_separator<char> sep("", " |&!(,)");
   Tizer tizer( query, sep );

   for( Tizer::iterator token = tizer.begin(); token != tizer.end(); ++token ) { 
      if( *token == TriggerPassed || *token == TriggerNotPassed
	  || *token == TriggerPassedLowbits || *token == TriggerNotPassedLowbits )
      {
         bool trigCond = ( *token == TriggerPassed || *token == TriggerPassedLowbits );
         bool useHighBits = ( *token != TriggerPassedLowbits && *token != TriggerNotPassedLowbits );
         if( ++token == tizer.end() || *token != "(" ) throw SyntaxError();

         std::vector<std::string> triggers_vec;
         std::string         triggers_str;
         std::string         trigsep;
         // collect trigger names used in this operator
	 do {
            if( ++token == tizer.end() ) throw SyntaxError();
            if( *token == ")" ) break;
            triggers_vec.push_back( *token );
            triggers_str += trigsep + *token;
            trigsep = ",";
            if( ++token == tizer.end() ) throw SyntaxError();
	 } while( *token == "," );

         if( m_usingWebService ) {
            WSQueryTriggerPositions( triggers_str, useHighBits );
         }
	 mappedQuery += " (";
         bool   firstPass = true;
         for( std::vector<std::string>::const_iterator trigger = triggers_vec.begin(), end = triggers_vec.end();
              trigger != end; ++trigger )
         {
            TrigValRangeVect trigValRangeVect;
            if( m_usingWebService ) {
               TriggerRangesMap::const_iterator mi = m_triggerRangesMap.find( *trigger );
               if( mi != m_triggerRangesMap.end() )
                  trigValRangeVect = *(mi->second);
            } else
               trigValRangeVect = makeTrigValRangeVect( *trigger, useHighBits );

            if( !trigValRangeVect.size() ) {
               // add a FALSE statement because the trigger was not active
               // using condition '0 & 1 != 0' for TriggerPassed
               // note:  this will result in TRUE for NotPassed
               trigValRangeVect.push_back( TrigValRange(0,99999999,0, "0") );
            }
            std::sort( trigValRangeVect.begin(), trigValRangeVect.end() );
            int firstValRange = 1;   // open bracket
            int valRangeGrouping = 0;
            for( size_t r = 0; r < trigValRangeVect.size(); r++ ) {
               if( r+1 < trigValRangeVect.size()
                   && trigValRangeVect[r].bitPosition == trigValRangeVect[r+1].bitPosition
                   && trigValRangeVect[r].attributeName == trigValRangeVect[r+1].attributeName ) {
                  // group run ranges with the same bit position
                  valRangeGrouping = (valRangeGrouping? 2 : 1);  // 1 == first, >1 == next
               } else {
                  valRangeGrouping = (valRangeGrouping? 3 : 0);  // 3 means end of group
               }
               mappedQuery += addTriggerCondition( trigValRangeVect[r],
                                                   trigCond, firstPass, tech,
                                                   firstValRange, valRangeGrouping );
               if( valRangeGrouping == 3 )
                  valRangeGrouping = 0;  // reset group after its end
               firstValRange = 2;    // add  "or" in the next pass
            }
            mappedQuery += ")";   // closing braket (to match the one added by firstValRange==1?)
            firstPass = false;                                             
	 }
         // end of TriggerPassed(), close the parens
         mappedQuery += ")";

      } else {
         mappedQuery += *token;
      }
   }
   
   return mappedQuery;
}


TrigCollQuery::TrigValRangeVect
TrigCollQuery::makeTrigValRangeVect( const std::string& trigger, bool useHighBits )
{
   bool discontinuous = false;
   TrigValRangeVect trigValRangeVect;
   // check all trigger maps for this trigger position
   for( size_t m=0; m < m_triggerMapVect.size(); m++ ) {
      TriggerRunMap  *map = m_triggerMapVect[m];
      TriggerMap::const_iterator ti = map->triggerPos.find( trigger );
      if( ti == map->triggerPos.end() )  {
         // trigger not defined in this map - this is a "hole"
         discontinuous = true;
         continue;
      }
      // bit position for the trigger
      unsigned    bitPos = ti->second;
      if( trigValRangeVect.size()) {
         TrigValRange& lastRange = trigValRangeVect[ trigValRangeVect.size()-1 ];
         if( lastRange.bitPosition == bitPos && !discontinuous ) {
            //extend the last range (only if there was no hole)
            lastRange.endrun = map->endrun;
            continue;
         }
         if( lastRange.bitPosition != bitPos && lastRange.endrun >= map->startrun ) {
            //truncate the last range
            lastRange.endrun = map->startrun - 1;
         }
      }
      discontinuous = false;
      trigValRangeVect.push_back( TrigValRange(map->startrun, map->endrun, bitPos) );
   }
   if( !trigValRangeVect.size() )			   
      throw UnknownTrigger(trigger);
   if( !discontinuous ) {
      // extend last range to infinity
      trigValRangeVect[ trigValRangeVect.size()-1 ].endrun = 99999999;
   }
	    
   std::string wordName;
   if( trigger.compare(0, 3, L1prefix) == 0 ) {
      wordName = (m_useCTPWord? L1word_old : L1word );
      useHighBits = false;
   }
   if( trigger.compare(0, 3, L2prefix) == 0 ) {
      wordName = L2word;
      useHighBits = false;
   }
   if( trigger.compare(0, 3, EFprefix) == 0 ) {
      wordName = EFword;
   }

   // convert (triggerName, bit position) to (attributeName, bitpos) 
   for( TrigValRangeVect::iterator it = trigValRangeVect.begin(), end = trigValRangeVect.end();
        it != end; ++it ) {
      std::ostringstream stream;
      stream << wordName << unsigned(it->bitPosition/32);
      it->attributeName = stream.str();
      it->bitPosition = (it->bitPosition % 32) + (useHighBits? 32 : 0);
   }

   return trigValRangeVect;  // inefficient but easy
}




void	TrigCollQuery::queryRuns( const std::string& runs )
{
   // cout << endl << ">>>    TrigCollQuery QueryRuns:" << runs << endl << endl;
   m_runsStr = runs;
   
   boost::char_separator<char> sep(",");
   Tizer tizer( runs, sep );

   for( Tizer::iterator token = tizer.begin(); token != tizer.end(); ++token ) { 
      size_t  dash_idx =  token->find('-');
      if( dash_idx != std::string::npos ) {
	 int startrun = atoi(token->c_str());
	 int endrun = ( dash_idx+1 >= token->size() ? 99999999 : atoi( token->substr( dash_idx+1 ).c_str() ) );
	 m_runs.push_back( std::pair<int,int>(startrun, endrun) );
      } else {
	 int run = atoi(token->c_str());
	 m_runs.push_back( std::pair<int,int>(run, run) );
      }
      // cout<< "** Run range: " << m_runs.back().first << ", " << m_runs.back().second <<  endl;
   }
}


/// set (get) the name of the RunNumber column
std::string	TrigCollQuery::runNumberColumn( const std::string& column )
{
   if( column.size() ) {
      m_runNumberColumnName = column;
   }
   return  m_runNumberColumnName;
}


#include <boost/regex.hpp>
    
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

void TrigCollQuery::readTriggerMap( std::string filename )
{
   const std::string logName = "TrigCollQuery";
   if( m_runs.empty() ) queryRuns("1-99999999"); 

   std::string xmlpath;
   if( !m_triggerMapDir.empty() ) {
      // Athena property, if set, overrides environment
      xmlpath = m_triggerMapDir;
   } else {
      const char *pathenv_name = "XMLPATH";
      char* xmlpath_env = getenv( pathenv_name );
      if( xmlpath_env )
	 xmlpath = xmlpath_env;
      xmlpath += ":.";
   }

   // cout << endl << ">>>    TrigCollQuery searching XMLPATH:" << xmlpath << endl << endl;
   Tizer paths( xmlpath, boost::char_separator<char>(":") );
   Tizer::iterator path;
   boost::regex const 	xmlmap_filenameRE( ".*"+MapFileName+"_(\\d{8})-(\\d{8})\\.xml$");
   for( path = paths.begin(); path != paths.end(); ++path ) {
      std::set<std::string>  files;
      // Fill "files" with filenames in the given element of XMLPATH
      boost::cmatch what;
      if( *path == "http" ) {
	 ++path;
	 if( path == paths.end() ) break;
	 readWebDir( std::string("http:")+*path, files );
      } else {
	 const std::string  dirpath( *path + "/" + XMLDir );
	 DIR *d = opendir( dirpath.c_str() );
	 if( !d ) {
	    // cout << logName << " Can't open XMLPATH component: " << dirpath << endl;
	    continue;
	 } else {
	    // Reading all files from directory into a set (to sort them)
	    // cout << logName << " Searching XMLPATH path: " << dirpath << endl;
	    struct dirent *de;
	    while( (de = readdir(d)) != 0 ) {
               // cout << " found file " <<  de->d_name << endl;
	       files.insert( dirpath + "/" + de->d_name );
	    }
	    closedir( d );
	 }
      }
      
      std::set<std::string>::const_iterator fi = files.begin();
      while( fi != files.end() ) {
	 if( boost::regex_match(fi->c_str(), what, xmlmap_filenameRE) ) {
	    TriggerRunMap *trm = new TriggerRunMap;
	    trm->startrun =  atoi(what[1].first);
	    trm->endrun =  atoi(what[2].first);
	    for( RunsVect::const_iterator ri = m_runs.begin(); ri != m_runs.end(); ++ri ) {
	       // cout << "====  Comparing " <<  ri->first << " <= " << trm->endrun << " && " << ri->second << " >= " << trm->startrun << endl;
	       if( ri->first <= trm->endrun && ri->second >= trm->startrun ) {
		  // cout << " parsing:  "  << *fi << ":  RUNS: " << trm->startrun << "-" << trm->endrun << endl;
		  if( parseXML( *fi, trm->triggerPos ) ) {
		     std::cerr << logName << " Failed reading trigger map file " << *fi << std::endl;
		     delete trm;  trm = 0;
		  } else {
		     m_triggerMapVect.push_back( trm );
		     continue;
		  }
	       }
	    }
	 }
	 ++fi;
      }
      if( files.size() )   break;  // read only one location
   }
   if( m_triggerMapVect.empty() ) {
     std::cout << logName << " Warning: Failed to retrieve run-dependent trigger mapping information for the given run range" << std::endl
               << " Looking for the old (static and possibly obsolete) trigger map file" << std::endl;

      if( !filename.size() )
	 filename =  XMLDir + "/" + MapFileName + ".xml";
      struct stat  statbuf;
      std::string       fullpath;
      for( path = paths.begin(); path != paths.end(); ++path ) {
	 fullpath = *path + "/" + filename;
	 if( 0 == stat( fullpath.c_str(), &statbuf ) ) {
	    // cout << logName << " Found trigger mappings file: " << fullpath << endl;
	    break;
	 }
      }
      if( path == paths.end() ) {
         std::cerr << logName << " Warning: File " << filename << " not found in " << xmlpath << std::endl;
	 std::cerr << logName << " No trigger mapping information available - trigger queries will not work" << std::endl;
	 return;	
      }
      TriggerRunMap *trm = new TriggerRunMap;
      trm->startrun =  1;
      trm->endrun = 99999999;

      if( !parseXML( fullpath, trm->triggerPos ) ) {
	 std::cout << logName << " XML trigger map " << fullpath << " read in " << std::endl;
	 m_triggerMapVect.push_back( trm );
      } else {
	 delete trm;
	 std::cerr << logName << " Error reading XML trigger map! " << fullpath << std::endl;
      }
   }
}





const std::string      DocType = "TAGMAP";
const std::string      LevelElementName = "LEVEL";
const std::string      LevelAttributeName = "level";

const std::string      TriggerElementName = "MAPELEMENT";
const std::string      TriggerAttributeName = "chainname";
const std::string      BitAttributeName = "tagindex";



#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/URLInputSource.hpp>
#include <xercesc/util/BinInputStream.hpp>
#include <xercesc/util/BinFileInputStream.hpp>

#include <boost/scoped_array.hpp>

XERCES_CPP_NAMESPACE_USE

typedef std::basic_string<XMLCh> XercesString;

// Converts from a narrow-character string to a wide-character string.
inline XercesString fromNative(const char* str)
{
    boost::scoped_array<XMLCh> ptr( XMLString::transcode(str) );
    return XercesString(ptr.get( ));
}

// Converts from a narrow-character string to a wide-charactr string.
inline XercesString fromNative(const std::string& str)
{
    return fromNative(str.c_str( ));
}

// Converts from a wide-character string to a narrow-character string.
inline std::string toNative(const XMLCh* str)
{
    boost::scoped_array<char> ptr( XMLString::transcode(str) );
    return std::string(ptr.get( ));
}

// Converts from a wide-character string to a narrow-character string.
inline std::string toNative(const XercesString& str)
{
    return toNative(str.c_str( ));
}




std::string  TrigCollQuery::addTriggerCondition( const TrigValRange& trigValRange,
						 bool condition,
						 bool firstPass,
						 const std::string& tech,
						 int firstRange,
						 int valRangeGrouping )
{
   //cout << "Adding condition for " << trigValRange.attributeName << " bit " << trigValRange.bitPosition << endl;
   uint64_t 	mask = (1ULL << trigValRange.bitPosition);
   
   std::string nextOper, logicAnd, logicOr, logicEquals, comparison, bitAndOperation;
   if( tech == "RelationalCollection" ) {
      logicAnd = sql_logicAndStr;
      logicOr  = sql_logicOrStr;
      logicEquals = sql_equalsStr;
      comparison = condition? sql_notEqualsStr : sql_equalsStr;
      bitAndOperation = sql_bitAndStr + trigValRange.attributeName + ", ";
   } else { 
      logicAnd = root_logicAndStr;
      logicOr  = root_logicOrStr;
      logicEquals = root_equalsStr;
      comparison = condition? root_notEqualsStr : root_equalsStr;
      bitAndOperation =  "(" + trigValRange.attributeName + root_bitAndStr;
   }

   std::ostringstream out;
   // add AND between different triggers
   if( !firstPass && firstRange < 2 )     out << logicAnd;
   // add OR between run ranges for the same trigger
   if( firstRange == 1 )       out << "( ";  // closing bracket must be added by calling method
   if( firstRange == 2 )       out << logicOr;
   // opening bracket for the start of run range (0 means it's not compressed)
   if( valRangeGrouping == 0 ) out << "( ";
   // extra bracket at the start of a commpresed run range
   if( valRangeGrouping == 1 ) out << "( ( "; 

   // add run range
   if( trigValRange.startrun == trigValRange.endrun ) {
      out << m_runNumberColumnName << logicEquals << trigValRange.startrun;
      nextOper = logicAnd;
   } else {
      if( trigValRange.startrun > 1 ) {
	 out << m_runNumberColumnName << ">=" << trigValRange.startrun;
	 nextOper = logicAnd;
      }
      if( trigValRange.endrun < 99999999 ) {
	 out << nextOper << m_runNumberColumnName << "<=" << trigValRange.endrun;
	 nextOper = logicAnd;
      }
   }
   // finish here if just creating compressed run range for the same bit
   if( valRangeGrouping == 1 || valRangeGrouping == 2 ) 
      return out.str();

   // close compressed run range for same bit pos
   if( valRangeGrouping == 3)     out << ")";

   // Finally the real bitwise AND operation for the trigger
   out << nextOper << bitAndOperation;
   // give the mask in hex to avoid sign issues in ROOT
   if( tech == "RootCollection" ) out << std::hex << "0x";
   out << mask << ")" << std::dec << comparison << " 0) ";

   // else throw std::runtime_error(string("Unknown collection technology: ") + tech); 
   return out.str();                            
}

   


int  TrigCollQuery::parseXML( const std::string& xmlfilename, TriggerMap& trigMap ) {

   if( !initXML() )     return 1;

   std::unique_ptr<XercesDOMParser> parser( new XercesDOMParser() );
   parser->setDoSchema( false );
   parser->setValidationScheme(XercesDOMParser::Val_Never);    
   std::unique_ptr<ErrorHandler> eh( new HandlerBase() );
   parser->setErrorHandler( eh.get() );

   try {
      parser->parse( xmlfilename.c_str() );
      DOMDocument *doc = parser->getDocument();

      DOMElement* elementRoot = doc->getDocumentElement();
      if( !elementRoot ) throw(std::runtime_error( "empty XML document" ));
      
      DOMNodeList *nodelist = elementRoot->getChildNodes();
      for( XMLSize_t xx = 0; xx < nodelist->getLength(); ++xx )
      {
         DOMNode* currentNode = nodelist->item(xx);
         if( currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) {
            // Found node which is an Element. Re-cast node as element
            // static_cast should be safe as we confirmed the NoneType (right?)
            DOMElement* currentElement = static_cast< xercesc::DOMElement* >( currentNode );            
            if( toNative(currentElement->getTagName()) == LevelElementName ) {
               // Already tested node as type element and of name "ApplicationSettings".
               // Read attributes of element "ApplicationSettings".
               std::string level = toNative( currentElement->getAttribute( fromNative(LevelAttributeName).c_str()) );

               DOMNodeList *triggerlist = currentNode->getChildNodes();
               for( XMLSize_t t = 0; t < triggerlist->getLength(); ++t ) {
                  DOMNode* currentNode = triggerlist->item(t);
                  if( currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) {
                     // Found node which is an Element. Re-cast node as element
                     DOMElement* currentElement = static_cast< xercesc::DOMElement* >( currentNode );
                     if( toNative(currentElement->getTagName()) == TriggerElementName )  {
                        std::string trigger = toNative( currentElement->getAttribute( fromNative(TriggerAttributeName).c_str() ) );
                        std::string bit = toNative( currentElement->getAttribute( fromNative(BitAttributeName).c_str() ) );
                        // cout << "Trigger: " << chainname << " bit=" << bit << endl;
                        std::istringstream bitnumber( bit );
                        bitnumber >> trigMap[ trigger ];
                     }
                  }
               }
            }
         }
      }      

      return 0;

   }
   catch (const XMLException& toCatch) {
      std::cout << "Error! Exception: " << toNative(toCatch.getMessage()) << std::endl;
   }
   catch (const DOMException& toCatch) {
      std::cout << "Error! Exception: " << toNative(toCatch.getMessage()) << std::endl;
   }
   catch( const SAXException &e ) {
      std::cout << "Error! Exception: " << toNative(e.getMessage()) << std::endl;
   }
   catch( std::exception &e ) {
      std::cout << "Exception cought: " << e.what() << std::endl;
   }
   catch(...) {
      std::cout << "Unexpected Exception " << std::endl ;
   }
   return -1;      
}
          



int  TrigCollQuery::readWebDir( const std::string& url_in, std::set<std::string>& files )
{
   if( !initXML() )     return 1;

   std::string url( url_in );
   if( url[ url.size()-1 ] != '/' )
      url += '/';
   try {
      URLInputSource	dir( url.c_str() );
      BinInputStream * 	dircont;
      try {
	 dircont = dir.makeStream ();
      }catch( const XMLException& e) {
	 std::cout << toNative(e.getMessage()) << std::endl;
	 return -1;
      }
      
      std::vector<XMLByte>	buff;
      int		iniBuffSize = 20000;
      buff.resize( iniBuffSize );

      int	total_read_bytes = 0;
      do{
	 int rb = dircont->readBytes( &buff[total_read_bytes], iniBuffSize);
	 total_read_bytes += rb;
	 // cout << "***  read " << rb << " bytes" <<  endl;
	 if( rb == 0 )
	    break;
	 if( buff.size() > 10*1024*1024 ) {
	    std::cerr << "Exceeded 10MB while reading from " << url << " Trigger configuration may be incomplete" << std::endl;
	    break;
	 }
	 buff.resize( buff.size() + rb );
      }while(true);

      boost::regex const 	xmlmap_filenameRE( "a href=\"("+MapFileName+"_\\d{8}-\\d{8}\\.xml)");
      boost::cmatch what;
      const char *start = (char*)&buff[0], *end = (char*)&buff[ buff.size() ];
      boost::match_flag_type flags = boost::match_default; 
      while( regex_search( start, end, what, xmlmap_filenameRE, flags))  {
	 std::string filename( what[1].first, what[1].second );
	 filename = url + filename;
	 files.insert( filename );
	 // cout << " > "  << "found:" << filename << endl;
	 // update search position: 
	 start = what[0].second; 
	 // update flags: 
	 flags |= boost::match_prev_avail; 
	 flags |= boost::match_not_bob; 
      }
      
      return 0;
   }
   catch (const XMLException& toCatch) {
      std::cout << "Error! Exception: " << toNative(toCatch.getMessage()) << std::endl;
   }
   catch (const DOMException& toCatch) {
      std::cout << "Error! Exception: " << toNative(toCatch.getMessage()) << std::endl;
   }
   catch( const SAXException &e ) {
      std::cout << "Error! Exception: " << toNative(e.getMessage()) << std::endl;
   }
   catch( std::exception &e ) {
      std::cout << "Exception cought: " << e.what() << std::endl;
   }
   catch(...) {
      std::cout << "Unexpected Exception " << std::endl ;
   }
   return -1;      
}
          


#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <xercesc/framework/MemBufInputSource.hpp>


/// Read trigger bit mappings for triggers present in the query (only)
void TrigCollQuery::WSQueryTriggerPositions( const std::string& triggers, bool useHighBits )
{
   m_triggerRangesMap.clear();
   
   if( m_certpath.empty() ) {
      const char* proxy = getenv("X509_USER_PROXY");
      if( proxy ) {
         m_certpath = m_keypath = proxy;
      } else {
         // check the default proxy location
         std::ostringstream px;
         px << "/tmp/x509up_u" << getuid();
         if( access(px.str().c_str(), R_OK) )
            throw std::runtime_error( "Certificate path not specified and X509_USER_PROXY not set (run voms-proxy-init)" );
         m_certpath = m_keypath = px.str();
      }
   }
//   if( m_dataPeriod.empty() )
//      throw runtime_error( "Project (data period/MC) not set" );
   
   std::string command = std::string("curl -s -S --key ") + m_keypath + " --sslv3 --cert " + m_certpath
      + " -k --url \"" + m_triggerWebService
      + "?trignms=" + triggers
      + "&runrange=" + m_runsStr
      + "&phys_raw=" + (useHighBits? "phys" : "raw")
      + "&filename_tag=" + m_dataPeriod
      + "&ami_tag=" + m_AMITag
      + "\" 2>&1";

   if( m_outputLevel ) {
      std::cout << " --------  executing command:" << std::endl << command << std::endl;
   }

   FILE* fp = popen(command.c_str(), "r");
   if( !fp )  throw std::runtime_error("Failed to execute CURL command: " + command);
   std::string output;
   const size_t readsize(1000);
   char buffer[readsize+1];
   size_t readbytes = 0;
   do {
      readbytes = fread(buffer, 1, readsize, fp);
      buffer[readbytes] = 0; // string terminator
      output += buffer;
   } while( readbytes == readsize );
   
   if( ferror(fp) ) {
      throw std::runtime_error("reading CURL output: " + output);
   }
   int rc = pclose(fp);
   if( rc ) {
      std::cerr << "Error executing the CURL command: " << command << std::endl
                << "pclose() returned code 0x" << std::hex << rc << std::endl;    
      throw std::runtime_error("executing the CURL commnad: " + output);
   }
   // cout << "------------  CURL output:" << endl << output << endl;

   if( output.find("404 Not Found") != std::string::npos )
      throw std::runtime_error(output); 

   if( !initXML() )    throw std::runtime_error("XML init failed");
   MemBufInputSource    xmlSource((const XMLByte*)output.c_str(), output.length(), "id");

   std::unique_ptr<XercesDOMParser> parser( new XercesDOMParser() );
   parser->setDoSchema( false );
   parser->setValidationScheme(XercesDOMParser::Val_Never);    
   std::unique_ptr<ErrorHandler> eh( new HandlerBase() );
   parser->setErrorHandler( eh.get() );

   try {
      parser->parse( xmlSource );
      //parser->parse( "test.xml" );
      DOMDocument *doc = parser->getDocument();
      // cout << "=========  XML document parsed" << endl;
      
      DOMElement* elementRoot = doc->getDocumentElement();
      if( !elementRoot )
         throw std::runtime_error( "empty XML document" );
      std::string rootTagName = toNative(elementRoot->getTagName());
      if( rootTagName == "ERROR" )
         throw std::runtime_error( output );
      
      DOMNodeList *nodelist = elementRoot->getChildNodes();
      for( XMLSize_t xx = 0; xx < nodelist->getLength(); ++xx )
      {
         DOMNode* currentNode = nodelist->item(xx);
         if( currentNode->getNodeType() == DOMNode::ELEMENT_NODE )         {
            DOMElement* currentElement = static_cast< xercesc::DOMElement* >( currentNode );

            // cout << ">>  Tag Name:" << toNative(currentElement->getTagName()) << endl;
            if( toNative(currentElement->getTagName()) == "trigger" ) {
               // Read attributes of element 
               std::string triggerName = toNative( currentElement->getAttribute( fromNative("name").c_str()) );
               DOMNodeList *triggernodes = currentNode->getChildNodes();
               for( XMLSize_t t = 0; t < triggernodes->getLength(); ++t ) {
                  DOMNode* currentNode = triggernodes->item(t);
                  if( currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) {
                     DOMElement* currentElement = static_cast< xercesc::DOMElement* >( currentNode );
                     // cout << ">>     Tag Name:" << toNative(currentElement->getTagName()) << endl;
                     if( toNative(currentElement->getTagName()) == "run" )  {
                        std::istringstream runNumberStr( toNative( currentElement->getAttribute( fromNative("number").c_str()) ) );
                        unsigned long long runNumber;
                        runNumberStr >> runNumber;
                        DOMNodeList *runnodes = currentNode->getChildNodes();
                        for( XMLSize_t r = 0; r < runnodes->getLength(); ++r ) {
                           DOMNode* currentNode = runnodes->item(r); 
                           if( currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) {
                              DOMElement* currentElement = static_cast< xercesc::DOMElement* >( currentNode );
                              // cout << ">>        Tag Name:" << toNative(currentElement->getTagName()) << endl;
                              if( toNative(currentElement->getTagName()) == "decoded_trigger" )  {
                                 std::string word = toNative( currentElement->getAttribute( fromNative("word").c_str() ) );
                                 if( word == "undefined" )
                                    continue;
                                 if( word == "unknown" )
                                    throw std::runtime_error(std::string("Unknown trigger: ") + triggerName);
                                 std::istringstream bitstr( toNative( currentElement->getAttribute( fromNative("bit").c_str() ) ) ); 
                                 unsigned bitpos = 0;
                                 bitstr >> bitpos;
                                 // cout << "Trigger " << triggerName << " run=" << runNumber << "  word=" << word << " bit=" << bitpos << endl;
                                 if( m_triggerRangesMap.find(triggerName) == m_triggerRangesMap.end() ) {
                                    m_triggerRangesMap[triggerName] = new TrigValRangeVect;
                                 }
                                 TrigValRangeVect* tv = m_triggerRangesMap[triggerName];
                                 if( tv->size() && tv->back().endrun+1 == runNumber
                                     && tv->back().bitPosition == bitpos && tv->back().attributeName == word )
                                    // expand range of existing entry
                                    tv->back().endrun++;
                                 else 
                                    tv->push_back( TrigValRange(runNumber, runNumber, bitpos, word) );
                              }
                           }
                        }
                     }

                  }
               }
               
            }
         }
      }      

   }
   catch (const XMLException& toCatch) {
      throw std::runtime_error(std::string("XML Exception: ") + toNative(toCatch.getMessage()) );
   }
   catch (const DOMException& toCatch) {
      throw std::runtime_error(std::string("XML DOM Exception: ") + toNative(toCatch.getMessage()) );
   }
   catch( const SAXException &e ) {
      throw std::runtime_error(std::string("XML SAX Exception: ") + toNative(e.getMessage()) );
   }
}
   


      

bool TrigCollQuery::initXML()
{
   if( ! m_xmlInitialized ) {   
      try {
	 XMLPlatformUtils::Initialize();
      }
      catch (const XMLException& toCatch) {
	 std::cerr << "Error during initialization! :" << toNative(toCatch.getMessage()) << std::endl;
      }
      m_xmlInitialized = true;
   }
   return  m_xmlInitialized;
}


TrigCollQuery::~TrigCollQuery()
{
   if( m_xmlInitialized ) {   
      try {
         XMLPlatformUtils::Terminate(); 
      } catch (const XMLException& toCatch) {}
   }
   for( TriggerRangesMap::const_iterator it = m_triggerRangesMap.begin(), end = m_triggerRangesMap.end();
        it != end; ++it )
      delete it->second;
}
