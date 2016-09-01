/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IOVSVC_CONDSVC_H
#define IOVSVC_CONDSVC_H 1

#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/EventIDBase.h"
#include "GaudiKernel/EventIDRange.h"
#include "AthenaBaseComps/AthService.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaBaseComps/AthService.h"

#include <map>
#include <set>
#include <vector>
#include <mutex>

class CondSvc: public extends1<AthService, ICondSvc> {
public:

  CondSvc(const std::string& name, ISvcLocator* svc);
  ~CondSvc();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  // from ICondSvc
public:
  virtual StatusCode regHandle(IAlgorithm* alg, const Gaudi::DataHandle& id, 
                               const std::string& key);

  virtual bool getInvalidIDs(const EventContext*, DataObjIDColl& ids);
  virtual bool getValidIDs(const EventContext*, DataObjIDColl& ids);
  virtual bool getIDValidity(const EventContext*, DataObjIDColl& validIDs,
                             DataObjIDColl& invalidIDs);

  virtual std::set<std::string> getUnchangedAlgs( const DataObjIDColl& );

  virtual StatusCode getRange(const std::string&, const EventContext*, EventIDRange&, 
                              ICondSvc::dbData_t&) const;

  virtual const std::set<IAlgorithm*>& condAlgs() const { return m_condAlgs; }

  virtual void dump() const;

private:

  class iAlgHasher {
  public:
    std::size_t operator()(const IAlgorithm * a) const {
      return ( iAlgHasher()(*a));
    }
    std::size_t operator()(const IAlgorithm& a) const {
      return (a.nameKey().__hash__());
    }
    
    bool operator()(const IAlgorithm* lhs, const IAlgorithm* rhs) const {
      return ( iAlgHasher()(*lhs,*rhs));
    }
    bool operator()(const IAlgorithm& lhs, const IAlgorithm& rhs) const {
      return ( lhs.nameKey() < rhs.nameKey() );
    }
    
  };

  bool parse(EventIDRange& t, const std::string& s);
  bool parse(IOVEntryT<ICondSvc::dbData_t>& t, const std::string& s);

  StatusCode readDbFile(const std::string&);

  ServiceHandle<StoreGateSvc> m_sgs;

  std::string m_file;

  typedef std::set<IAlgorithm*, iAlgHasher> IAlgHashSet;
  typedef std::map<DataObjID,   IAlgHashSet> id_map_t;
  typedef std::map<IAlgorithm*, DataObjIDColl, iAlgHasher> alg_map_t;
  id_map_t  m_idMap;
  alg_map_t m_algMap;

  std::map<std::string, DataObjID> m_keyMap;
  std::set<IAlgorithm*> m_condAlgs;

  typedef std::map<std::string, std::vector<IOVEntryT<ICondSvc::dbData_t>>> registry_t;
  registry_t m_registry;

  mutable std::recursive_mutex m_lock;

};

#endif
