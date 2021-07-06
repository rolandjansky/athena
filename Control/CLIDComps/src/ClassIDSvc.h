/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CLIDCOMPS_CLASSIDSVC_H
# define CLIDCOMPS_CLASSIDSVC_H
/** @file ClassIDSvc.h
 * @brief a service to manage and verify CLID assignments in athena

 * @author Paolo Calafiura <pcalafiura@lbl.gov> - ATLAS Collaboration
 */

#include <unordered_map>
#include <mutex>
#include <string>
#include <vector>
#include <utility> // for std::pair

#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IClassIDSvc.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/DirSearchPath.h"

#include "AthenaKernel/CLIDRegistry.h"


/** @class ClassIDSvc 
 * @brief a service to manage and verify CLID assignments in athena.
 *
 * @details (clid, class_name) entries are loaded at init from the list of
 * files specifies in "CLIDDBFiles", and from the CLID_Registry of every
 * library. Optionally the resulting m_clidMap can be dumped to 
 * "OutputFileName" at finalize time.
 */

class ClassIDSvc : public extends<Service, IClassIDSvc, IIncidentListener>
{
private:
  typedef std::pair<std::string, std::string> TypeName; //typename+typeinfoname
  typedef std::unordered_map<CLID, TypeName> CLIDMap; 
  typedef std::unordered_map<std::string, CLID> NameMap; 

public:
  // Locking convention: public methods (except for gaudi state-change methods)
  // take the lock.  Protected/private methods do not take the lock.
  // Public methods do not call each other.

  //========================================================================
  /** @name IClassIDSvc interfaces. */

  /// get next available CLID 
  /// @throws std::runtime_error if no CLID can be allocated  virtual CLID nextAvailableID() const;
  virtual CLID nextAvailableID() const override;
  /// check if id is used
  virtual bool isIDInUse(const CLID& id) const override;
  /// check if id is used
  virtual bool isNameInUse(const std::string& name) const override;
  /// get type name associated with clID (if any)
  virtual StatusCode getTypeNameOfID(const CLID& id, std::string& typeName) const  override;
  /// get user assigned type-info name associated with clID
  virtual StatusCode getTypeInfoNameOfID(const CLID& id, std::string& typeInfoName) const override;
  /// get id associated with type name (if any)
  virtual StatusCode getIDOfTypeName(const std::string& typeName, CLID& id) const  override;
  /// get id associated with type-info name (if any)
  virtual StatusCode getIDOfTypeInfoName(const std::string& typeInfoName, CLID& id) const override;
  /// associate type name, package info and type-info name with clID
  virtual StatusCode setTypeForID(const CLID& id,
                                  const std::string& typeName,
                                  const std::string& typeInfoName = "") override;

  //========================================================================
  /** @name Debugging methods. */

  ///dump to MsgStream contents of in memory DB
  void dump() const;

  
  //========================================================================
  /** @name Gaudi methods. */

  /// Gaudi Service Implementation
  //@{
  virtual StatusCode initialize() override;
  virtual StatusCode reinitialize() override;
  ///dump CLIDmap to outputFileName;
  virtual StatusCode finalize() override;
  //@}

  ///implement IIncidentListener
  void handle(const Incident &inc) override;

  // Standard Constructor
  ClassIDSvc(const std::string& name, ISvcLocator* svc);

private:
  /// Return all registered IDs in sorted order.
  std::vector<CLID> sortedIDs() const;

  /// get clids from CLIDDB and from registry entries
  StatusCode fillDB();
  /// load clid/names from a "db" file
  bool processCLIDDB(const std::string& fileName);
  /// load clid/names from a DLL registry
  bool getRegistryEntries(const std::string& moduleName);

  /// associate type name with clID w/o checking CLID range
  StatusCode 
  uncheckedSetTypePackageForID(const CLID& id, 
                               const std::string& typeName,
                               const std::string& typeInfoName);

  /// Test to see if anything new has been added to the registry.
  bool maybeRescan() const;

  /// @name Properties
  //@{
  Gaudi::Property<std::vector<std::string>> m_DBFiles{this, "CLIDDBFiles", {"clid.db"},
    "List of db files with (CLID, class_name) entries. Loaded at init in svc maps. Files are looked up in DATAPATH"};

  Gaudi::Property<std::string> m_outputFileName{this, "OutputFileName", "NULL",
    "Path to clid.db file for writing. By default ('NULL') to not create the file."};
  //@}
  CLIDMap m_clidMap;
  NameMap m_nameMap;
  NameMap m_tiNameMap;

  /// The path is which clid db files are to be searched (DATAPATH)
  DirSearchPath m_clidDBPath;

  // Mutex to protect internal structures.
  typedef std::mutex mutex_t;
  typedef std::lock_guard<mutex_t> lock_t;
  mutable mutex_t m_mutex;
};


#endif // CLIDCOMPS_CLASSIDSVC_H
