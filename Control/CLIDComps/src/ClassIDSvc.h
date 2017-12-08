///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CLIDCOMPS_CLASSIDSVC_H
# define CLIDCOMPS_CLASSIDSVC_H
/** @file ClassIDSvc.h
 * @brief a service to manage and verify CLID assignments in athena

 * @author Paolo Calafiura <pcalafiura@lbl.gov> - ATLAS Collaboration
 *$Id: ClassIDSvc.h,v 1.8 2009-01-15 19:08:43 binet Exp $
 */

#include <unordered_map>
#include <mutex>
#include <string>
#include <vector>
#include <utility> // for std::pair

#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/Service.h"

#include "GaudiKernel/DirSearchPath.h"

#include "AthenaKernel/IClassIDSvc.h"

#include "SGTools/CLIDRegistry.h"


/** @class ClassIDSvc 
 * @brief a service to manage and verify CLID assignments in athena.
 *
 * @details (clid, class_name) entries are loaded at init from the list of
 * files specifies in "CLIDDBFiles", and from the CLID_Registry of every
 * library. Optionally the resulting m_clidMap can be dumped to 
 * "OutputFileName" at finalize time.
 */


class ClassIDSvc : virtual public IClassIDSvc, 
                   virtual public IIncidentListener, 
                   public Service
{
private:
  typedef std::pair<std::string, std::string> TypeName; //typename+typeinfoname
  typedef std::unordered_map<CLID, TypeName> CLIDMap; 
  typedef std::unordered_map<std::string, CLID> NameMap; 
  typedef std::unordered_map<CLID, Athena::PackageInfo> PackageMap; 

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
  /// get type name associated with clID (if any)
  virtual StatusCode getPackageInfoForID(const CLID& id, Athena::PackageInfo& info) const override;
  /// associate type name, package info and type-info name with clID
  virtual StatusCode setTypePackageForID(const CLID& id, 
					 const std::string& typeName, 
					 const Athena::PackageInfo& info,
					 const std::string& typeInfoName) override;

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
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface ) override;
  //@}

  ///implement IIncidentListener
  void handle(const Incident &inc) override;

  
  // Standard Constructor
  ClassIDSvc(const std::string& name, ISvcLocator* svc);
        
  // Standard Destructor
  virtual ~ClassIDSvc() {};

private:
  MsgStream& msg() { return m_msg; }
  const MsgStream& msg() const { return m_msg; }

  // Return all registered IDs in sorted order.
  std::vector<CLID> sortedIDs() const;

  StatusCode
  getPackageInfoForIDInternal(const CLID& id, Athena::PackageInfo& info) const;
  /// get id associated with type name (if any)
  StatusCode getIDOfTypeNameInternal(const std::string& typeName,
                                     CLID& id) const;
  /// get type name associated with clID (if any)
  StatusCode getTypeNameOfIDInternal(const CLID& id,
                                     std::string& typeName) const;

  /// get clids from CLIDDB and from registry entries
  StatusCode fillDB();
  /// load clid/names from a "db" file
  bool processCLIDDB(const char* fileName);
  /// load clid/names from a DLL registry
  bool getRegistryEntries(const std::string& moduleName);

  /// associate type name with clID w/o checking CLID range
  StatusCode 
  uncheckedSetTypePackageForID(const CLID& id, 
			       const std::string& typeName,
			       const Athena::PackageInfo& info,
			       const std::string& typeInfoName);

  /// Test to see if anything new has been added to the registry.
  void maybeRescan() const;

  /// @name Properties
  //@{
  /// "CLIDDBFiles": list of db files with (CLID, class_name) entries. Loaded at init in svc maps
  std::vector<std::string> m_DBFiles;
  /// "OutputFileName": path to clid.db file in which write at finalize entries in m_clidMap. Default ("NULL") is not to write output clid.db".
  std::string m_outputFileName;
  //@}
  CLIDMap m_clidMap;
  NameMap m_nameMap;
  NameMap m_tiNameMap;
  PackageMap m_packageMap;

  /// The path is which clid db files are to be searched (DATAPATH)
  DirSearchPath m_clidDBPath;

  /// a local @c MsgStream object
  MsgStream m_msg;

  // Mutex to protect internal structures.
  typedef std::mutex mutex_t;
  typedef std::lock_guard<mutex_t> lock_t;
  mutable mutex_t m_mutex;
};


#endif // CLIDCOMPS_CLASSIDSVC_H
