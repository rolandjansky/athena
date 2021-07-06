/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>
#include <iostream>
#include <iterator>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/ModuleIncident.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/CLIDRegistry.h"
#include "AthenaKernel/errorcheck.h"
#include "CxxUtils/checker_macros.h"

#include "ClassIDSvc.h"

namespace {
  bool tryNumeric (const std::string& s, CLID& clid)
  {
    clid = CLID_NULL;
    char* endptr = nullptr;
    clid = strtol (s.c_str(), &endptr, 10);
    return clid != CLID_NULL && endptr == (s.c_str() + s.size());
  }
  

// HACK LIFTED FROM AthenaBaseComps/AthMsgStreamMacros.h to remove dep loop
#define ATH_MSG_LVL(lvl, x) \
   do {                                      \
     if (msgLevel(lvl)) {                    \
       msgStream(lvl) << x  << endmsg;	     \
     }                                       \
   } while (0)
 
#define ATH_MSG_VERBOSE(x) ATH_MSG_LVL(MSG::VERBOSE, x)
#define ATH_MSG_DEBUG(x)   ATH_MSG_LVL(MSG::DEBUG, x)
#define ATH_MSG_INFO(x)    ATH_MSG_LVL(MSG::INFO, x)
#define ATH_MSG_WARNING(x) ATH_MSG_LVL(MSG::WARNING, x)
#define ATH_MSG_ERROR(x)   ATH_MSG_LVL(MSG::ERROR, x)

#define ATH_CONST_MSG_VERBOSE(x)                  \
  do {                                            \
    if (msgLevel(MSG::VERBOSE)) {                 \
      verbose() << x << endmsg;                   \
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
    throw std::runtime_error("ClassIDSvc::nextAvailableID: none in range");
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
ClassIDSvc::isNameInUse(const std::string& name ) const {
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

  CLIDMap::const_iterator iID = m_clidMap.find(id);
  if (iID != m_clidMap.end()) {
    typeName = iID->second.first;
    ATH_CONST_MSG_VERBOSE( "getTypeNameOfID(" << id << ") type name is " << typeName );
    return StatusCode::SUCCESS;
  }
  else {
    ATH_CONST_MSG_VERBOSE( "getTypeNameOfID(" << id << ") no associated type name found" );
    return StatusCode::FAILURE;
  }
}


/// get type name associated with clID (if any)
StatusCode 
ClassIDSvc::getTypeInfoNameOfID(const CLID& id, std::string& typeInfoName) const
{
  lock_t lock (m_mutex);
  maybeRescan();

  CLIDMap::const_iterator iID = m_clidMap.find(id);
  if (iID != m_clidMap.end()) {
    typeInfoName = iID->second.second;
    ATH_CONST_MSG_VERBOSE( "getTypeInfoNameOfID(" << id <<
                           ") type-info name is " << typeInfoName );
    return StatusCode::SUCCESS;
  }
  else {
    ATH_CONST_MSG_VERBOSE( "getTypeInfoNameOfID(" << id <<
                           ") no associated type-info name found" );
    return StatusCode::FAILURE;
  }
}


/// get id associated with type name (if any)
StatusCode 
ClassIDSvc::getIDOfTypeName(const std::string& typeName, CLID& id) const
{
  lock_t lock (m_mutex);
  maybeRescan();

  NameMap::const_iterator iID = m_nameMap.find(typeName);
  if (iID != m_nameMap.end()) {
    id = iID->second;
    ATH_CONST_MSG_VERBOSE( "getIDOfTypeName(" << typeName << ") CLID is " << id );
    return StatusCode::SUCCESS;
  }
  else if (tryNumeric (typeName, id)) {
    ATH_CONST_MSG_VERBOSE( "getIDOfTypeName(" << typeName << ") is a numeric CLID" );
    return StatusCode::SUCCESS;
  }
  else {
    ATH_CONST_MSG_VERBOSE( "getIDOfTypeName(" << typeName << ") no associated CLID found" );
    return StatusCode::FAILURE;
  }
}


/// get id associated with type-info name (if any)
StatusCode 
ClassIDSvc::getIDOfTypeInfoName(const std::string& typeInfoName,
				CLID& id) const
{
  lock_t lock (m_mutex);
  maybeRescan();
  NameMap::const_iterator iID = m_tiNameMap.find(typeInfoName);
  if (iID != m_tiNameMap.end()) {
    id = iID->second;
    ATH_CONST_MSG_VERBOSE( "getIDOfTypeInfoName(" << typeInfoName << ") CLID is " << id );
    return StatusCode::SUCCESS;
  }
  else {
    ATH_CONST_MSG_VERBOSE( "getIDOfTypeInfoName(" << typeInfoName << ") no associated CLID found" );
    return StatusCode::FAILURE;
  }
}


/// associate type name with clID
StatusCode 
ClassIDSvc::setTypeForID(const CLID& id,
                         const std::string& typeName,
                         const std::string& typeInfoName)
{
  lock_t lock (m_mutex);
  if (id < CLIDdetail::MINCLID || id > CLIDdetail::MAXCLID) {
    ATH_MSG_ERROR( "setTypeNameForID: input id " << id
                   << " is out of allowed range " << CLIDdetail::MINCLID 
                   << " : " << CLIDdetail::MAXCLID );
    return StatusCode::FAILURE;
  }
  return uncheckedSetTypePackageForID(id, typeName, typeInfoName);
}


void 
ClassIDSvc::dump() const
{
  lock_t lock (m_mutex);
  ATH_MSG_INFO( "dump: in memory" );

  for (CLID clid : sortedIDs()) {
    const std::string& typeName = m_clidMap.find (clid)->second.first;
    ATH_MSG_INFO( "CLID: "<< clid << " - type name: " << typeName );
  }
  ATH_MSG_INFO( "------------------------------" );
}


StatusCode 
ClassIDSvc::initialize()
{
  ATH_MSG_VERBOSE( "Initializing " << name() ) ;

  CHECK( Service::initialize() );

  // set up the incident service:
  ServiceHandle<IIncidentSvc> pIncSvc ("IncidentSvc", name());
  CHECK( pIncSvc.retrieve() );

  pIncSvc->addListener(this, ModuleLoadedIncident::TYPE(), /*priority*/ 100);
  pIncSvc->release();

  CHECK( maybeRescan() );  // calls fillDB() if not already done

  return StatusCode::SUCCESS;
}


StatusCode
ClassIDSvc::reinitialize() {
  ATH_MSG_INFO( "RE-initializing " << name() ) ;
  CHECK( fillDB() );
  return StatusCode::SUCCESS;
}


StatusCode 
ClassIDSvc::finalize()
{
  if (m_outputFileName != "NULL") {
    std::ofstream outfile( m_outputFileName );
    if ( !outfile ) {
      ATH_MSG_ERROR( "unable to open output CLIDDB file: " << m_outputFileName );
      return StatusCode::RECOVERABLE;
    } else {
      for (CLID clid : sortedIDs()) {
        const std::string& typeName = m_clidMap[clid].first;
        const std::string& tiName   = m_clidMap[clid].second;
        outfile << clid << "; " << typeName << "; " << tiName << std::endl;
      }
      ATH_MSG_INFO( "finalize: wrote " << m_clidMap.size()  <<
                    " entries to output CLIDDB file: " << m_outputFileName );
    }
  }
  return Service::finalize();
}


void ClassIDSvc::handle(const Incident &inc)
{
  lock_t lock (m_mutex);
  const ModuleLoadedIncident& modInc(dynamic_cast<const ModuleLoadedIncident&>(inc));

  getRegistryEntries(modInc.module());
}


/// Standard Constructor
ClassIDSvc::ClassIDSvc(const std::string& name,ISvcLocator* svc)
  : base_class(name,svc),
    m_clidDBPath(System::getEnv("DATAPATH"))
{
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


/// Process the various clid dbs according to user's request
StatusCode
ClassIDSvc::fillDB() {
  bool allOK(true);
  for (const std::string& DBFile : m_DBFiles) {
    DirSearchPath::path clidDB(DBFile);

    if (clidDB.is_complete()) {
      allOK = processCLIDDB(clidDB.string());
    } else {
      std::list<DirSearchPath::path> paths(m_clidDBPath.find_all(clidDB));
      if (paths.empty()) {
        ATH_MSG_WARNING( "Could not resolve clid DB path " << clidDB
                         << " using DATAPATH. Skipping it." );
      } else {
        for (const auto& p : paths) {
          allOK &= processCLIDDB(p.string());
        }
      }
    }
  }
  return StatusCode(allOK);
}


bool
ClassIDSvc::processCLIDDB(const std::string& fileName)
{
  std::ifstream ifile(fileName);
  if (!ifile) {
    ATH_MSG_WARNING( "processCLIDDB: unable to open " << fileName );
    return true;
  }

  bool allOK(true);
  unsigned int newEntries(0);
  std::string line;
  // Format: CLID;typeName[;typeInfoName]
  while (allOK && std::getline(ifile, line)) {
    std::vector<std::string> columns;
    boost::split(columns, line, boost::is_any_of(";"));
    long id(-1);
    if (columns.size()>=2) {
      boost::trim(columns[0]);
      try {
        id = std::stol(columns[0]);
      } catch (const std::logic_error& e) {
        ATH_MSG_ERROR( "processCLIDDB: Can't cast ["<< columns[0] << "] to long (clid): " << e.what() );
        allOK = false;
      }

      if (uncheckedSetTypePackageForID(id, columns[1],
                                       columns.size()>2 ? columns[2] : "")) {
        ++newEntries;
      }
    }
  }

  if (!allOK) {
    ATH_MSG_ERROR( "processCLIDDB: processing record '" << line
                   << "' from CLIDDB file: " << fileName );
  } else {
    ATH_MSG_DEBUG( "processCLIDDB: read " << newEntries <<
                   " entries from CLIDDB file: " << fileName );
  }

  return allOK;
}


bool ClassIDSvc::getRegistryEntries(const std::string& moduleName)
{
  //not only this is fast, but is necessary to prevent recursion
  if (!CLIDRegistry::hasNewEntries()) return true;

  bool allOK(true);
  size_t newEntries(0);
  //to speed up processing we only take entries added to CLIDRegistry
  //since last call (thanks Niels!)
  for (const auto& [clid, typeName, typeInfoName] : CLIDRegistry::newEntries()) {
    ATH_MSG_VERBOSE( "reading [" << clid << ", " << typeName << ", " << typeInfoName << "]" );
    if (uncheckedSetTypePackageForID(clid, typeName, typeInfoName)) {
      ++newEntries;
    }
    else {
      allOK = false;
    }
  }
  
  if (allOK) {
    ATH_MSG_INFO( "getRegistryEntries: read " << newEntries
                  << " CLIDRegistry entries for module " << moduleName );
  } else {
    ATH_MSG_ERROR("getRegistryEntries: can not read  CLIDRegistry entries for module "
                  << moduleName );
  }

  return allOK;
}


StatusCode 
ClassIDSvc::uncheckedSetTypePackageForID(const CLID& id, 
                                         const std::string& typeName,
                                         const std::string& typeInfoName)
{
  // process "raw" typeName
  std::string procName(typeName);
  boost::trim(procName);

  if (procName.empty()) {
    ATH_MSG_ERROR( "Empty type name for CLID " << id );
    return StatusCode::FAILURE;
  }

  // assign typeInfoName
  std::string procTiName;
  if (typeInfoName.empty()) {
    procTiName = procName;
  }
  else {
    procTiName = typeInfoName;
    boost::trim(procTiName);
  }

  // insert into CLID map
  const auto& [clid_it, clid_success] = m_clidMap.try_emplace(id, procName, procTiName);
  if (!clid_success && clid_it->second!=std::make_pair(procName,procTiName)) {
    ATH_MSG_ERROR( "Cannot set type " << std::make_pair(procName,procTiName) << " for CLID " <<
                   id << ": known type for this ID " << clid_it->second );
    return StatusCode::FAILURE;
  }

  // insert into type name map
  const auto& [name_it, name_success] = m_nameMap.try_emplace(procName, id);
  if (!name_success && name_it->second!=id) {
    ATH_MSG_ERROR( "Cannot set CLID " << id << " for type name '"
                   << procName << "': known CLID for this name " << name_it->second );
    return StatusCode::FAILURE;
  }

  // insert into typeInfo map
  const auto& [info_it, info_success] = m_tiNameMap.try_emplace(procTiName, id);
  if (!info_success && info_it->second!=id) {
    ATH_MSG_ERROR( "Cannot set CLID " << id << " for type-info name '"
                   << procTiName << "' and type '" << procName
                   << "': known CLID for this type-info name " << info_it->second );
    return StatusCode::FAILURE;
  }

  ATH_MSG_VERBOSE( "Set type name '" << procName << "' for CLID " << id <<
                   " with type-info name '" << procTiName << "'" );

  return StatusCode::SUCCESS;
}


bool ClassIDSvc::maybeRescan() const
{
  // thread-safe because calls are protected by mutex or during initialize
  ClassIDSvc* nc ATLAS_THREAD_SAFE = const_cast<ClassIDSvc*>(this);

  // read CLID database in case initialize() was not called yet (ATR-23634)
  [[maybe_unused]] static bool fillDB ATLAS_THREAD_SAFE = nc->fillDB().isSuccess();

  // make sure registry is up-to-date
  bool status = nc->getRegistryEntries ("ALL");

  return status && fillDB;
}


