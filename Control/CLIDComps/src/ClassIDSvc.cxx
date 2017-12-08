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
#include "GaudiKernel/ServiceHandle.h"
#include "SGTools/CLIDRegistry.h"
#include "AthenaKernel/errorcheck.h"
#include "CxxUtils/checker_macros.h"

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

#define ATH_CONST_MSG_VERBOSE(x)                  \
  do {                                            \
    if (msg().level() < MSG::VERBOSE) {           \
      MsgStream log (msgSvc(), name());           \
      log << MSG::VERBOSE << x << endmsg;         \
    }                                             \
  } while (0)

}


/// get next available CLID 
/// @throws std::runtime_error if no CLID can be allocated
CLID 
ClassIDSvc::nextAvailableID() const {
  lock_t lock (m_mutex);
  maybeRescan();
  CLID valid(CLIDdetail::MINCLID);
  while (valid <= CLIDdetail::MAXCLID &&
         m_clidMap.find(valid) != m_clidMap.end())
  {
    ++valid;
  }

  if (valid > CLIDdetail::MAXCLID) {
    throw runtime_error("ClassIDSvc::nextAvailableID: none in range");
  }

  return valid;
}


bool
ClassIDSvc::isIDInUse(const CLID& id ) const {
  lock_t lock (m_mutex);
  maybeRescan();
  return m_clidMap.find(id) != m_clidMap.end();
}


bool
ClassIDSvc::isNameInUse(const string& name ) const {
  lock_t lock (m_mutex);
  maybeRescan();
  return m_nameMap.find(name) != m_nameMap.end();
}


/// get type name associated with clID (if any)
StatusCode 
ClassIDSvc::getTypeNameOfID(const CLID& id, std::string& typeName) const
{
  lock_t lock (m_mutex);
  maybeRescan();
  return getTypeNameOfIDInternal (id, typeName);
}


/// get type name associated with clID (if any)
StatusCode 
ClassIDSvc::getTypeInfoNameOfID(const CLID& id, 
				std::string& typeInfoName) const
{
  lock_t lock (m_mutex);
  maybeRescan();
  StatusCode sc(StatusCode::FAILURE);
  CLIDMap::const_iterator iID = m_clidMap.find(id);
  if (iID != m_clidMap.end()) {
    typeInfoName = iID->second.second;
    ATH_CONST_MSG_VERBOSE( "getTypeInfoNameOfID(" << id <<
                           ") type-info name is " << typeInfoName );
    sc = StatusCode::SUCCESS;
  }
  else {
    ATH_CONST_MSG_VERBOSE( "getTypeInfoNameOfID(" << id <<
                           ") no associated type-info name found " );
  }
  return sc;
}


/// get id associated with type name (if any)
StatusCode 
ClassIDSvc::getIDOfTypeName(const std::string& typeName, CLID& id) const
{
  lock_t lock (m_mutex);
  maybeRescan();
  return getIDOfTypeNameInternal (typeName, id);
}


/// get id associated with type-info name (if any)
StatusCode 
ClassIDSvc::getIDOfTypeInfoName(const std::string& typeInfoName,
				CLID& id) const
{
  lock_t lock (m_mutex);
  maybeRescan();
  StatusCode sc(StatusCode::FAILURE);
  NameMap::const_iterator iID = m_tiNameMap.find(typeInfoName);
  if (iID != m_tiNameMap.end()) {
    id = iID->second;
    ATH_CONST_MSG_VERBOSE( "getIDOfTypeInfoName(" << typeInfoName << ") CLID is " << id);
    sc = StatusCode::SUCCESS;
  }
  else {
    ATH_CONST_MSG_VERBOSE( "getIDOfTypeInfoName(" << typeInfoName << ") no associated CLID found ");
  }
  return sc;
}

/// get PackageInfo associated with clID (if any)
StatusCode 
ClassIDSvc::getPackageInfoForID(const CLID& id, Athena::PackageInfo& info) const
{
  lock_t lock (m_mutex);
  maybeRescan();
  return getPackageInfoForIDInternal (id, info);
}


/// associate type name with clID
StatusCode 
ClassIDSvc::setTypePackageForID(const CLID& id, 
				const std::string& typeName,
				const Athena::PackageInfo& info,
				const std::string& typeInfoName)
{
  lock_t lock (m_mutex);
  if (id < CLIDdetail::MINCLID || id > CLIDdetail::MAXCLID) {
    ATH_MSG_FATAL( "setTypeNameForID: input id " << id 
                   << " is out of allowed range " << CLIDdetail::MINCLID 
                   << " : " << CLIDdetail::MAXCLID );
    return StatusCode::FAILURE;
  }
  return uncheckedSetTypePackageForID(id, typeName, info, typeInfoName);
}


void 
ClassIDSvc::dump() const
{
  lock_t lock (m_mutex);
  MsgStream log (msgSvc(), name());
  log << MSG::INFO << "dump: in memory" << endmsg;

  for (CLID clid : sortedIDs()) {
    const std::string& typeName = m_clidMap.find (clid)->second.first;
    log << MSG::INFO 
        << "CLID: "<< clid
        << " - type name: " << typeName;
    Athena::PackageInfo info;
    if (getPackageInfoForIDInternal (clid, info).isSuccess()) {
      log << MSG::INFO 
          << "- Package "<< info; 
    }
    log << MSG::INFO << '\n';
  }
  log << "------------------------------" << endmsg;
}


StatusCode 
ClassIDSvc::initialize()
{
  ATH_MSG_VERBOSE( "Initializing " << name() ) ;

  CHECK( Service::initialize() );

  // set up the incident service:
  ServiceHandle<IIncidentSvc> pIncSvc ("IncidentSvc", name());
  CHECK( pIncSvc.retrieve() );

  const int PRIORITY = 100;
  pIncSvc->addListener(this, ModuleLoadedIncident::TYPE(), PRIORITY);
  pIncSvc->release();

  CHECK( fillDB() );
  return StatusCode::SUCCESS;
}


StatusCode
ClassIDSvc::reinitialize() {
  ATH_MSG_INFO("RE-initializing " << name() ) ;  
  CHECK( fillDB() );
  return StatusCode::SUCCESS;
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
      for (CLID clid : sortedIDs()) {
	const std::string& typeName = m_clidMap[clid].first;
	const std::string& tiName   = m_clidMap[clid].second;
	outfile << clid << "; " << typeName;
        Athena::PackageInfo info;
        if (getPackageInfoForIDInternal (clid, info).isSuccess()) {
	  outfile << "; " << info;
	  outfile << "; " << tiName;
	}
	outfile	<< endl;
      }
      ATH_MSG_INFO( "finalize: wrote " << m_clidMap.size()  <<
		    " entries to output CLIDDB file: " << m_outputFileName );
    }
    outfile.close();
  } //outputfilename != NULL
  return Service::finalize();
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


void ClassIDSvc::handle(const Incident &inc)
{
  lock_t lock (m_mutex);
  const ModuleLoadedIncident& modInc(dynamic_cast<const ModuleLoadedIncident&>(inc));

  getRegistryEntries(modInc.module());
}


/// Standard Constructor
ClassIDSvc::ClassIDSvc(const std::string& name,ISvcLocator* svc)
  : Service(name,svc),
    m_outputFileName("NULL"),
    m_clidDBPath(System::getEnv("DATAPATH")),
    m_msg (msgSvc(), name)
{
  // Property Default values
  m_DBFiles.push_back("clid.db");
    
  // Get user's input	
  declareProperty("CLIDDBFiles",  m_DBFiles, 
		  "list of db files with (CLID, class_name) entries. Loaded at init in svc maps. Files are looked up in DATAPATH");
  declareProperty("OutputFileName",  m_outputFileName,
		  "path to clid.db file in which write at finalize entries in m_clidMap. Default ('NULL') is not to write output clid.db");

}


// Return all registered IDs in sorted order.
std::vector<CLID> ClassIDSvc::sortedIDs() const
{
  std::vector<CLID> ids;
  ids.reserve (m_clidMap.size());
  for (const auto& p : m_clidMap) {
    ids.push_back (p.first);
  }
  std::sort (ids.begin(), ids.end());
  return ids;
}


/// get PackageInfo associated with clID (if any)
StatusCode 
ClassIDSvc::getPackageInfoForIDInternal(const CLID& id,
                                        Athena::PackageInfo& info) const
{
  StatusCode sc(StatusCode::FAILURE);
  PackageMap::const_iterator iID = m_packageMap.find(id);
  if (iID != m_packageMap.end()) {
    info = iID->second;
    ATH_CONST_MSG_VERBOSE("getPackageInfoForID(" << id << 
                          ") package name is " << info.name() << 
                          " package version is " << info.version());
    sc = StatusCode::SUCCESS;
  }
  else {
    ATH_CONST_MSG_VERBOSE( "getPackageInfoForID(" << id <<
                           ") no associated type name found ");
  }
  return sc;
}


/// get id associated with type name (if any)
StatusCode 
ClassIDSvc::getIDOfTypeNameInternal(const std::string& typeName, CLID& id) const
{
  StatusCode sc(StatusCode::FAILURE);
  NameMap::const_iterator iID = m_nameMap.find(typeName);
  if (iID != m_nameMap.end()) {
    id = iID->second;
    ATH_CONST_MSG_VERBOSE( "getIDOfTypeName(" << typeName << ") CLID is " << id);
    sc = StatusCode::SUCCESS;
  }
  else {
    ATH_CONST_MSG_VERBOSE( "getIDOfTypeName(" << typeName << ") no associated CLID found " );
  }
  return sc;
}


/// get type name associated with clID (if any)
StatusCode 
ClassIDSvc::getTypeNameOfIDInternal(const CLID& id, std::string& typeName) const
{
  StatusCode sc(StatusCode::FAILURE);
  CLIDMap::const_iterator iID = m_clidMap.find(id);
  if (iID != m_clidMap.end()) {
    typeName = iID->second.first;
    ATH_CONST_MSG_VERBOSE( "getTypeNameOfID(" << id << ") type name is " <<
                           typeName );
    sc = StatusCode::SUCCESS;
  }
  else {
    ATH_CONST_MSG_VERBOSE( "getTypeNameOfID(" << id <<
                           ") no associated type name found " );
  }
  return sc;
}


StatusCode
ClassIDSvc::fillDB() {
  // Process the various clid dbs according to user's request
  bool allOK(true);
  for (const std::string& DBFile : m_DBFiles) {
    DirSearchPath::path clidDB(DBFile.c_str());
    const char* clidDBFileName(clidDB.c_str());
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
	while (p!=pe) allOK &= processCLIDDB((*p++).c_str());
      }
    }
  }
  
  maybeRescan(); //scan registry if we had no CLIDDB to process
  return allOK ?  
    StatusCode::SUCCESS :
    StatusCode::FAILURE;
}


bool
ClassIDSvc::processCLIDDB(const char* fileName)
{
  maybeRescan();
  bool allOK(true);
  ifstream ifile(fileName);
  if (!ifile) {
    msg() << MSG::WARNING << "processCLIDDB: unable to open " << fileName <<endmsg;
  } else {
    unsigned int newEntries(0);
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
	    ATH_MSG_VERBOSE( "processCLIDDB(" << fileName << 
			     ")\n    added entry for CLID <" << id << 
			     "> type name <" << typeName << '>' );
	    ++newEntries;
	  }
	}
      }
    } //while records
    if (!allOK) {
      msg() << MSG::ERROR << "processCLIDDB: processing record " << line 
	    << " 	from CLIDDB file: " << fileName << endmsg;
    } else {
      ATH_MSG_DEBUG( "processCLIDDB: read " << newEntries << 
		     " entries from CLIDDB file: " << fileName);
    }
    ifile.close();
  } //input file open
      
  return allOK;
}


bool ClassIDSvc::getRegistryEntries(const std::string& moduleName)
{
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
    ATH_MSG_VERBOSE(
		    "reading [" 
		    << clid << ", "
		    << typeName << ", "
		    << pkgInfo << ", "
		    << typeInfoName << "]");
    if (uncheckedSetTypePackageForID (clid,
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


StatusCode 
ClassIDSvc::uncheckedSetTypePackageForID(const CLID& id, 
					 const std::string& typeName,
					 const Athena::PackageInfo& info,
					 const std::string& typeInfoName)
{
  StatusCode sc(StatusCode::SUCCESS);
  //process "raw" typeName
  string procName(typeName);
  massage(procName);
  //first the id->name map
  string knownName("_____++++");
  if (getTypeNameOfIDInternal(id, knownName).isSuccess() && procName != knownName) {
    msg() << MSG::FATAL << "uncheckedSetTypePackageForID: " << info <<
      " can not set type name <" << procName << "> for CLID " <<
      id << ": Known name for this ID <" << knownName << '>';
    Athena::PackageInfo existInfo;
    if (getPackageInfoForIDInternal(id, existInfo).isSuccess()) {
      msg() << MSG::FATAL 
	    << " It was set by " << existInfo;
    }
    msg() << MSG::ERROR << endmsg;
    sc = StatusCode::FAILURE;
  } else if (procName == knownName) {
    if (msg().level() <= MSG::VERBOSE) {
      ATH_MSG_VERBOSE("uncheckedSetTypePackageForID: type name <" << procName <<
                      "> already set for CLID " << id);
      Athena::PackageInfo existInfo;
      if (getPackageInfoForIDInternal(id, existInfo).isSuccess()) {
        ATH_MSG_VERBOSE( " It was set by " << existInfo );
      }
    }
  } 
  if (!sc.isSuccess()) return StatusCode::FAILURE;

  //now the name->id map
  CLID knownID(0);
  if (getIDOfTypeNameInternal(procName, knownID).isSuccess() && id != knownID) {
    msg() << MSG::ERROR << "uncheckedSetTypePackageForID: " << info << 
      " can not set CLID <" << id << "> for type name " <<
      procName << ": Known CLID for this name <" << knownID << '>' ;
    Athena::PackageInfo existInfo;
    if (getPackageInfoForIDInternal(knownID, existInfo).isSuccess()) {
      msg() << MSG::ERROR 
	    << " It was set by " << existInfo; 
    }
    msg() << MSG::ERROR << endmsg;
    sc = StatusCode::FAILURE;
  } else if (id == knownID) {
    if (msg().level() <= MSG::VERBOSE) {
      ATH_MSG_VERBOSE( "uncheckedSetTypePackageForID: CLID <" << id <<
                       "> already set for type name " << procName );
      Athena::PackageInfo existInfo;
      if (getPackageInfoForIDInternal(id, existInfo).isSuccess()) {
        ATH_MSG_VERBOSE( " It was set by " << existInfo);
      }
    }
  } 
  const std::string procTiName = typeInfoName.empty() 
    ? procName
    : typeInfoName;
  m_clidMap[id] = std::make_pair(procName, procTiName);
  m_nameMap[procName] = id;
  // FIXME: should we also check for ti-name<=>clid duplicates ?
  m_tiNameMap[procTiName] = id;
  m_packageMap[id] = info;
  ATH_MSG_VERBOSE("uncheckedSetTypePackageForID: set type name <" <<
                  procName << "> for CLID " << id);
  return sc;
}


void ClassIDSvc::maybeRescan() const
{
  ClassIDSvc* nc ATLAS_THREAD_SAFE = const_cast<ClassIDSvc*>(this);
  nc->getRegistryEntries ("ALL");
}


