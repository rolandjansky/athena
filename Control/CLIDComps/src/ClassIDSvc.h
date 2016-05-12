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

#include <map>
#include <mutex>
#include <string>
#include <vector>
#include <utility> // for std::pair

#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/Service.h"

#include "GaudiKernel/DirSearchPath.h"

#include "AthenaKernel/tools/AthenaPackageInfo.h"
#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/MsgStreamMember.h"

#include "SGTools/CLIDRegistry.h"

template <class TYPE> class SvcFactory;

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
                   public Service {
private:
  typedef std::pair<std::string, std::string> TypeName; //typename+typeinfoname
  typedef std::map<CLID, TypeName> CLIDMap; 
  typedef std::map<std::string, CLID> NameMap; 
  typedef std::map<CLID, Athena::PackageInfo> PackageMap; 
public:
  /// get next available CLID 
  /// @throws std::runtime_error if no CLID can be allocated  virtual CLID nextAvailableID() const;
  CLID nextAvailableID() const;
  /// check if id is used
  virtual bool isIDInUse(const CLID& id) const;
  /// check if id is used
  virtual bool isNameInUse(const std::string& name) const;
  /// get type name associated with clID (if any)
  virtual StatusCode getTypeNameOfID(const CLID& id, std::string& typeName) const ;
  /// get user assigned type-info name associated with clID
  virtual StatusCode getTypeInfoNameOfID(const CLID& id, std::string& typeInfoName) const ;
  /// get type name associated with clID (if any)
  virtual StatusCode getPackageInfoForID(const CLID& id, Athena::PackageInfo& info) const ;
  /// get id associated with type name (if any)
  virtual StatusCode getIDOfTypeName(const std::string& typeName, CLID& id) const ;
  /// get id associated with type-info name (if any)
  virtual StatusCode getIDOfTypeInfoName(const std::string& typeInfoName, CLID& id) const ;
  /// associate type name, package info and type-info name with clID
  virtual StatusCode setTypePackageForID(const CLID& id, 
					 const std::string& typeName, 
					 const Athena::PackageInfo& info,
					 const std::string& typeInfoName);

  ///dump to MsgStream contents of in memory DB
  void dump() const;

  /// Gaudi Service Implementation
  //@{
  virtual StatusCode initialize();
  virtual StatusCode reinitialize();
  ///dump CLIDmap to outputFileName;
  virtual StatusCode finalize(); 
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  //@}

  ///implement IIncidentListener
  void handle(const Incident &inc);

protected:
  friend class SvcFactory<ClassIDSvc>;

  // Standard Constructor
  ClassIDSvc(const std::string& name, ISvcLocator* svc);
        
  // Standard Destructor
  virtual ~ClassIDSvc() {};

private:
  MsgStream& msg() const { return m_msg.get(); }
  /// get clids from CLIDDB and from registry entries
  StatusCode fillDB();
  /// load clid/names from a "db" file
  bool processCLIDDB(const char* fileName);
  /// load clid/names from a DLL registry
  bool getRegistryEntries(const std::string& moduleName);

  /// associate type name with clID w/o checking CLID range
  virtual StatusCode 
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

  /// a local @c MsgStream -like object
  mutable Athena::MsgStreamMember m_msg;

  ///protect db failling
  std::recursive_mutex m_regMutex;
};

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // CLIDCOMPS_CLASSIDSVC_H
