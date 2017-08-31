/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>  /* distance */
#include <cstdlib>    /* getenv */
#include <fstream>
#include <iostream>
#include <iterator>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/replace.hpp>

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/ModuleIncident.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/MsgStream.h"
#include "SGTools/CLIDRegistry.h"
#include "AthenaKernel/errorcheck.h"

#include "ClassIDSvc.h"
using namespace std;

namespace {
  inline 
  void massage (string& s) {
    boost::trim(s); 
    boost::replace_all(s, string(";"), string());
  }

// HACK LIFTED FROM AthenaBaseComps/AthMsgStreamMacros.h to remove dep loop
#define ATH_MSG_LVL(lvl, x) \
   do {                                      \
     if (msg().level() <= lvl) {		     \
       msg() << lvl << x  << endmsg;		     \
     }                                       \
   } while (0)
 
#define ATH_MSG_VERBOSE(x) ATH_MSG_LVL(MSG::VERBOSE, x)
#define ATH_MSG_DEBUG(x)   ATH_MSG_LVL(MSG::DEBUG, x)
#define ATH_MSG_INFO(x)    ATH_MSG_LVL(MSG::INFO, x)
#define ATH_MSG_ERROR(x)   ATH_MSG_LVL(MSG::ERROR, x)
#define ATH_MSG_FATAL(x)   ATH_MSG_LVL(MSG::FATAL, x)

}


/// Standard Constructor
ClassIDSvc::ClassIDSvc(const std::string& name,ISvcLocator* svc)
  : Service(name,svc), m_outputFileName("NULL"),
    m_clidDBPath(System::getEnv("DATAPATH")),
    m_msg (msgSvc(), name),
    m_regMutex()
{
  // Property Default values
  m_DBFiles.push_back("clid.db");
    
  // Get user's input	
  declareProperty("CLIDDBFiles",  m_DBFiles, 
		  "list of db files with (CLID, class_name) entries. Loaded at init in svc maps. Files are looked up in DATAPATH");
  declareProperty("OutputFileName",  m_outputFileName,
		  "path to clid.db file in which write at finalize entries in m_clidMap. Default ('NULL') is not to write output clid.db");

}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!

StatusCode 
ClassIDSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
    if ( IClassIDSvc::interfaceID().versionMatch(riid) )    {
        *ppvInterface = (IClassIDSvc*)this;
    }
    else  {
	// Interface is not directly available: try out a base class
	return Service::queryInterface(riid, ppvInterface);
    }
    addRef();
    return StatusCode::SUCCESS;
}

StatusCode 
ClassIDSvc::initialize()
{
  ATH_MSG_VERBOSE( "Initializing " << name() ) ;

  CHECK( Service::initialize() );

  // set up the incident service:
  IIncidentSvc* pIncSvc(0);
  const bool CREATEIF(true);
  CHECK( service("IncidentSvc", pIncSvc, CREATEIF) );
  assert( 0 != pIncSvc );

  const int PRIORITY = 100;
  pIncSvc->addListener(this, ModuleLoadedIncident::TYPE(), PRIORITY);
  pIncSvc->release();

  return fillDB();
}

bool ClassIDSvc::getRegistryEntries(const std::string& moduleName) {

  std::lock_guard<std::recursive_mutex> lock(m_regMutex);

  //not only this is fast, but is necessary to prevent recursion
  if (!CLIDRegistry::hasNewEntries()) return true;

  bool allOK(true);
  size_t nE = 0;
  //to speed up processing we only take entries added to CLIDRegistry
  //since last call (thanks Niels!)
  for (const CLIDRegistry::tuple_t& ent : CLIDRegistry::newEntries()) {
    const CLID& clid                   = std::get<0>(ent);
    const std::string& typeName        = std::get<1>(ent);
    const Athena::PackageInfo& pkgInfo = std::get<2>(ent);
    const std::string& typeInfoName    = std::get<3>(ent);
#ifndef NDEBUG		
    ATH_MSG_VERBOSE(
		    "reading [" 
		    << clid << ", "
		    << typeName << ", "
		    << pkgInfo << ", "
		    << typeInfoName << "]");
#endif
    if (setTypePackageForID (clid, 
                             typeName,
                             pkgInfo, 
                             typeInfoName).isSuccess())
    {
      ++nE;
    }
    else {
      allOK = false;
    }
  }
  
  if (allOK) {
    ATH_MSG_INFO( " getRegistryEntries: read " << nE 
		  << " CLIDRegistry entries for module " << moduleName );
  } else {
    ATH_MSG_ERROR(" getRegistryEntries: can not read  CLIDRegistry entries for module " 
                  << moduleName);
  }

  return allOK;
}


inline
void ClassIDSvc::maybeRescan() const
{
  const_cast<ClassIDSvc*>(this)->getRegistryEntries ("ALL");
}


void ClassIDSvc::handle(const Incident &inc) {
  const ModuleLoadedIncident& modInc(dynamic_cast<const ModuleLoadedIncident&>(inc));

  getRegistryEntries(modInc.module());
}


StatusCode 
ClassIDSvc::finalize()
{
  if (m_outputFileName != "NULL") {
    ofstream outfile( m_outputFileName.c_str());
    if ( !outfile ) {
      msg() << MSG::ERROR << "unable to open output CLIDDB file: " 
	    << m_outputFileName << endmsg;
      return StatusCode::RECOVERABLE;
    } else {
      //    ostream_iterator< pair<CLID, string> > os(outfile, ':');
      //    copy(m_clidMap.begin(), m_clidMap,end(), os);
      CLIDMap::const_iterator i(m_clidMap.begin()), iE(m_clidMap.end());
      while (i != iE) {
	const CLID clid = i->first;
	const std::string& typeName = i->second.first;
	const std::string& tiName   = i->second.second;
	outfile << clid << "; " << typeName;
	Athena::PackageInfo existInfo;
	if (getPackageInfoForID(i->first, existInfo).isSuccess()) {
	  outfile << "; " << existInfo;
	  outfile << "; " << tiName;
	}
	outfile	<< endl;
	++i;
      }
      ATH_MSG_INFO( "finalize: wrote " << m_clidMap.size()  <<
		    " entries to output CLIDDB file: " << m_outputFileName );
    }
    outfile.close();
  } //outputfilename != NULL
  return Service::finalize();
}

/// get next available CLID 
/// @throws std::runtime_error if no CLID can be allocated
CLID 
ClassIDSvc::nextAvailableID() const {
  maybeRescan();
  CLID valid(CLIDdetail::MINCLID);
  while (valid <= CLIDdetail::MAXCLID && isIDInUse(valid)) ++valid;
  if (valid > CLIDdetail::MAXCLID) throw runtime_error("ClassIDSvc::nextAvailableID: none in range");
  return valid;
}

bool
ClassIDSvc::isIDInUse(const CLID& id ) const {
  maybeRescan();
  return 0 != m_clidMap.count(id);
}

bool
ClassIDSvc::isNameInUse(const string& name ) const {
  maybeRescan();
  return 0 != m_nameMap.count(name);
}

/// get type name associated with clID (if any)
StatusCode 
ClassIDSvc::getTypeNameOfID(const CLID& id, std::string& typeName) const {
  maybeRescan();
  StatusCode sc(StatusCode::FAILURE);
  CLIDMap::const_iterator iID = m_clidMap.find(id);
  if (iID != m_clidMap.end()) {
    typeName = iID->second.first;
#ifndef NDEBUG		
    ATH_MSG_VERBOSE( "getTypeNameOfID(" << id << ") type name is " << typeName);
#endif			
    sc = StatusCode::SUCCESS;
  } else {
#ifndef NDEBUG		
    ATH_MSG_VERBOSE( "getTypeNameOfID(" << id << ") no associated type name found ");
#endif			
  }
  return sc;
}

/// get type name associated with clID (if any)
StatusCode 
ClassIDSvc::getTypeInfoNameOfID(const CLID& id, 
				std::string& typeInfoName) const {
  maybeRescan();
  StatusCode sc(StatusCode::FAILURE);
  CLIDMap::const_iterator iID = m_clidMap.find(id);
  if (iID != m_clidMap.end()) {
    typeInfoName = iID->second.second;
#ifndef NDEBUG		
    ATH_MSG_VERBOSE( "getTypeInfoNameOfID(" << id << ") type-info name is " << typeInfoName);
#endif			
    sc = StatusCode::SUCCESS;
  } else {
#ifndef NDEBUG		
    ATH_MSG_VERBOSE( "getTypeInfoNameOfID(" << id << ") no associated type-info name found ");
#endif			
  }
  return sc;
}

/// get PackageInfo associated with clID (if any)
StatusCode 
ClassIDSvc::getPackageInfoForID(const CLID& id, Athena::PackageInfo& info) const {
  maybeRescan();
  StatusCode sc(StatusCode::FAILURE);
  PackageMap::const_iterator iID = m_packageMap.find(id);
  if (iID != m_packageMap.end()) {
    info = iID->second;
#ifndef NDEBUG		
    ATH_MSG_VERBOSE("getPackageInfoForID(" << id << 
		    ") package name is " << info.name() << 
		    " package version is " << info.version());
#endif			
    sc = StatusCode::SUCCESS;
  } else {
#ifndef NDEBUG		
    ATH_MSG_VERBOSE( "getPackageInfoForID(" << id <<
		     ") no associated type name found ");
#endif			
  }
  return sc;
}
/// get id associated with type name (if any)
StatusCode 
ClassIDSvc::getIDOfTypeName(const std::string& typeName, CLID& id) const {
  maybeRescan();
  StatusCode sc(StatusCode::FAILURE);
  NameMap::const_iterator iID = m_nameMap.find(typeName);
  if (iID != m_nameMap.end()) {
    id = iID->second;
#ifndef NDEBUG		
    ATH_MSG_VERBOSE( "getIDOfTypeName(" << typeName << ") CLID is " << id);
#endif			
    sc = StatusCode::SUCCESS;
  } else {
#ifndef NDEBUG		
    ATH_MSG_VERBOSE( "getIDOfTypeName(" << typeName << ") no associated CLID found " );
#endif			
  }
  return sc;
}

/// get id associated with type-info name (if any)
StatusCode 
ClassIDSvc::getIDOfTypeInfoName(const std::string& typeInfoName,
				CLID& id) const {
  maybeRescan();
  StatusCode sc(StatusCode::FAILURE);
  NameMap::const_iterator iID = m_tiNameMap.find(typeInfoName);
  if (iID != m_tiNameMap.end()) {
    id = iID->second;
#ifndef NDEBUG		
    ATH_MSG_VERBOSE( "getIDOfTypeInfoName(" << typeInfoName << ") CLID is " << id);
#endif			
    sc = StatusCode::SUCCESS;
  } else {
#ifndef NDEBUG		
    ATH_MSG_VERBOSE( "getIDOfTypeInfoName(" << typeInfoName << ") no associated CLID found ");
#endif			
  }
  return sc;
}

/// associate type name with clID
StatusCode 
ClassIDSvc::setTypePackageForID(const CLID& id, 
				const std::string& typeName,
				const Athena::PackageInfo& info,
				const std::string& typeInfoName) {
  if (id < CLIDdetail::MINCLID || id > CLIDdetail::MAXCLID) {
    ATH_MSG_FATAL( "setTypeNameForID: input id " << id 
                   << " is out of allowed range " << CLIDdetail::MINCLID 
                   << " : " << CLIDdetail::MAXCLID );
    return StatusCode::FAILURE;
  }
  return uncheckedSetTypePackageForID(id, typeName, info, typeInfoName);
}

StatusCode 
ClassIDSvc::uncheckedSetTypePackageForID(const CLID& id, 
					 const std::string& typeName,
					 const Athena::PackageInfo& info,
					 const std::string& typeInfoName) {
  StatusCode sc(StatusCode::SUCCESS);
  //process "raw" typeName
  string procName(typeName);
  massage(procName);
  //first the id->name map
  string knownName("_____++++");
  if (getTypeNameOfID(id, knownName).isSuccess() && procName != knownName) {
    msg() << MSG::FATAL << "uncheckedSetTypePackageForID: " << info <<
      " can not set type name <" << procName << "> for CLID " <<
      id << ": Known name for this ID <" << knownName << '>';
    Athena::PackageInfo existInfo;
    if (getPackageInfoForID(id, existInfo).isSuccess()) {
      msg() << MSG::FATAL 
	    << " It was set by " << existInfo;
    }
    msg() << MSG::ERROR << endmsg;
    sc = StatusCode::FAILURE;
  } else if (procName == knownName) {
#ifndef NDEBUG		
    ATH_MSG_VERBOSE("uncheckedSetTypePackageForID: type name <" << procName <<
		    "> already set for CLID " << id);
    Athena::PackageInfo existInfo;
    if (getPackageInfoForID(id, existInfo).isSuccess()) {
      ATH_MSG_VERBOSE( " It was set by " << existInfo );
    }
#endif			
  } 
  if (!sc.isSuccess()) return StatusCode::FAILURE;

  //now the name->id map
  CLID knownID(0);
  if (getIDOfTypeName(procName, knownID).isSuccess() && id != knownID) {
    msg() << MSG::ERROR << "uncheckedSetTypePackageForID: " << info << 
      " can not set CLID <" << id << "> for type name " <<
      procName << ": Known CLID for this name <" << knownID << '>' ;
    Athena::PackageInfo existInfo;
    if (getPackageInfoForID(knownID, existInfo).isSuccess()) {
      msg() << MSG::ERROR 
	    << " It was set by " << existInfo; 
    }
    msg() << MSG::ERROR << endmsg;
    sc = StatusCode::FAILURE;
  } else if (id == knownID) {
#ifndef NDEBUG		
    ATH_MSG_VERBOSE( "uncheckedSetTypePackageForID: CLID <" << id <<
		     "> already set for type name " << procName );
    Athena::PackageInfo existInfo;
    if (getPackageInfoForID(id, existInfo).isSuccess()) {
      ATH_MSG_VERBOSE( " It was set by " << existInfo);
    }
#endif			
  } 
//   //finally the id->package map
//   Athena::PackageInfo knownInfo;
//   if (getPackageInfoForID(id, knownInfo).isSuccess() && info != knownInfo) {
//     msg() << MSG::ERROR 
// 	<< "uncheckedSetTypePackageForID: can not set package info <" << info
// 	<< "> for CLID " << id
// 	<< ": Known info for this CLID <" << knownInfo << '>' << endmsg;
//     sc = StatusCode::FAILURE;
//   } else if (info == knownInfo) {
// #ifndef NDEBUG		
//     msg() << MSG::VERBOSE
// 	<< "uncheckedSetTypePackageForID: package info <" << info
// 	<< "> already set for CLID " << id<< endmsg;
// #endif			
//   } else {
    const std::string procTiName = typeInfoName.empty() 
      ? procName
      : typeInfoName;
    m_clidMap[id] = std::make_pair(procName, procTiName);
    m_nameMap[procName] = id;
    // FIXME: should we also check for ti-name<=>clid duplicates ?
    m_tiNameMap[procTiName] = id;
    m_packageMap[id] = info;
#ifndef NDEBUG		
    ATH_MSG_VERBOSE("uncheckedSetTypePackageForID: set type name <" <<
		    procName << "> for CLID " << id);
#endif					
    //  }
  return sc;
}
bool
ClassIDSvc::processCLIDDB(const char* fileName) {
  maybeRescan();
  bool allOK(true);
  ifstream ifile(fileName);
  if (!ifile) {
    msg() << MSG::WARNING << "processCLIDDB: unable to open " << fileName <<endmsg;
  } else {
#ifndef NDEBUG
    unsigned int newEntries(0);
#endif
    string line;
    while (allOK && std::getline(ifile, line)) {
      //not yet if ("#" == line.substr(0,0)) continue; //skip comments
      //split the record in 2 fields:	
      //      cout << "record " << line << endl;
      typedef boost::tokenizer<boost::char_separator<char> > Tokenizer;
      Tokenizer tokens(line, boost::char_separator<char>(";"));
      Tokenizer::iterator iToken(tokens.begin()), tEnd(tokens.end());
      long id(-1);
      const std::size_t columns = distance (iToken, tEnd);
      if (columns == 2  || columns == 3 || columns == 4) {
	string massTok(*iToken++);
	massage(massTok);
	try {
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
	  id = boost::lexical_cast<long>(massTok);
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
	  //	  cout << "id " << id << endl;
	} catch (boost::bad_lexical_cast e) { 
	  msg() << MSG::ERROR << "processCLIDDB: Can't cast ["  
		<< massTok << "] to long (clid)" << endmsg;
	  allOK=false;
	  break;
	}
	string typeName(*iToken++);
	massage(typeName);
	//	cout << "typeName " << typeName << endl;

	
	string sinfo;
	if (columns>=3) { 
	  sinfo=*iToken++; 
	  massage(sinfo);
	} else { sinfo = "UNKNOWN-00-00-00"; }
	Athena::PackageInfo info(sinfo);

	//	cout << "info " << info << endl;

	string typeInfoName;
	if (columns>=4) { 
	  massage(typeInfoName = *iToken++);
	} else { typeInfoName = typeName; }
	  
	// cout << "typeInfo " << typeInfoName << endl;

	if ((allOK = !typeName.empty())) {
	  if (uncheckedSetTypePackageForID(id, 
					   typeName,
					   info,
					   typeInfoName).isSuccess()) {
#ifndef	 NDEBUG
	    ATH_MSG_VERBOSE( "processCLIDDB(" << fileName << 
			     ")\n    added entry for CLID <" << id << 
			     "> type name <" << typeName << '>' );
	    ++newEntries;
#endif	
	  }
	}
      }
    } //while records
    if (!allOK) {
      msg() << MSG::ERROR << "processCLIDDB: processing record " << line 
	    << " 	from CLIDDB file: " << fileName << endmsg;
    } else {
#ifndef NDEBUG
      ATH_MSG_DEBUG( "processCLIDDB: read " << newEntries << 
		     " entries from CLIDDB file: " << fileName);
#endif	
    }
    ifile.close();
  } //input file open
      
  return allOK;
}

void 
ClassIDSvc::dump() const {
  ATH_MSG_INFO("dump: in memory");
  CLIDMap::const_iterator i(m_clidMap.begin()), iE(m_clidMap.end());
  while (i != iE) {
    const CLID clid = i->first;
    const std::string& typeName = i->second.first;
    msg() << MSG::INFO 
	  << "CLID: "<< clid
	  << " - type name: " << typeName;
    Athena::PackageInfo info;
    if (getPackageInfoForID(clid, info).isSuccess()) {
      msg() << MSG::INFO 
	    << "- Package "<< info; 
    }
    msg() << MSG::INFO << '\n';
    ++i;
  }
  ATH_MSG_INFO("------------------------------");
}

StatusCode
ClassIDSvc::fillDB() {
  std::lock_guard<std::recursive_mutex> lock(m_regMutex);
  // Process the various clid dbs according to user's request
  vector< string >::const_iterator f(m_DBFiles.begin()), fE(m_DBFiles.end());
  bool allOK(true);
  while (f != fE) {
#if BOOST_FILESYSTEM_VERSION == 3
    DirSearchPath::path clidDB((*f++).c_str());
    const char* clidDBFileName(clidDB.c_str());
#else
    DirSearchPath::path clidDB(*f++, boost::filesystem::no_check);
    const char* clidDBFileName(clidDB.native_file_string().c_str());
#endif
    if (clidDB.is_complete()) {
      allOK = processCLIDDB(clidDBFileName);
    } else {
      std::list<DirSearchPath::path> paths(m_clidDBPath.find_all(clidDBFileName));
      if (paths.empty()) {
	msg() << MSG::WARNING 
	      << "Could not resolve clid DB path " << clidDBFileName 
	      << " using DATAPATH [" << System::getEnv("DATAPATH") 
	      << "] ----- SKIPPING" << endmsg;
      } else {
	std::list<DirSearchPath::path>::const_iterator p(paths.begin()), pe(paths.end());
#if BOOST_FILESYSTEM_VERSION == 3
	while (p!=pe) allOK &= processCLIDDB((*p++).c_str());
#else
	while (p!=pe) allOK &= processCLIDDB((*p++).native_file_string().c_str());
#endif
      }
    }
  }
  
  maybeRescan(); //scan registry if we had no CLIDDB to process
  return allOK ?  
    StatusCode::SUCCESS :
    StatusCode::FAILURE;
}

StatusCode
ClassIDSvc::reinitialize() {
  ATH_MSG_INFO("RE-initializing " << name() ) ;  
  return fillDB();
}
