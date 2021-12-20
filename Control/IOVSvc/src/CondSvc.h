/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IOVSVC_CONDSVC_H
#define IOVSVC_CONDSVC_H 1

#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/Service.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaBaseComps/AthService.h"
#include "ICondSvcSetupDone.h"

#include <unordered_map>
#include <map>
#include <set>
#include <vector>
#include <mutex>

class ConditionSlotFuture;
class ICondtionIOSvc;

class CondSvc: public extends<AthService, ICondSvc, ICondSvcSetupDone> {
public:

  CondSvc(const std::string& name, ISvcLocator* svc);
  ~CondSvc();

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode start() override;
  virtual StatusCode stop() override;

  // from ICondSvc
public:
  virtual StatusCode regHandle(IAlgorithm* alg, const Gaudi::DataHandle& id) override;

  virtual bool getInvalidIDs(const EventContext&, DataObjIDColl& ids);
  virtual bool getValidIDs(const EventContext&, DataObjIDColl& ids);
  virtual bool getIDValidity(const EventContext&, DataObjIDColl& validIDs,
                             DataObjIDColl& invalidIDs);
  virtual bool isValidID(const EventContext&, const DataObjID&) const override;

  virtual const std::set<IAlgorithm*>& condAlgs() const override { return m_condAlgs; }

  virtual bool isRegistered(const DataObjID&) const override;
  virtual bool isRegistered(IAlgorithm*) const override;
  
  virtual const DataObjIDColl& conditionIDs() const override;

  virtual StatusCode validRanges( std::vector<EventIDRange>& ranges,
                                  const DataObjID& id ) const override;

  //  virtual void dump() const;
  virtual void dump(std::ostream&) const override;

  /// To be called after changes to the set of conditions containers
  /// in the conditions store.
  /// May not be called concurrently with any other methods of this class.
  virtual StatusCode setupDone() override;


public:
  // unimplemented interfaces
  
  /// Asynchronously setup conditions
  virtual ConditionSlotFuture* startConditionSetup(const EventContext&) override {
    return nullptr;
  }

  /// register an IConditionIOSvc (alternative to Algorithm processing of 
  /// Conditions)
  virtual StatusCode registerConditionIOSvc(IConditionIOSvc*)  override {
    return StatusCode::FAILURE;
  }

  

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

  StatusCode regHandle_i(IAlgorithm* alg, const Gaudi::DataHandle& id);

  ServiceHandle<StoreGateSvc> m_sgs;

  typedef std::set<IAlgorithm*, iAlgHasher> IAlgHashSet;
  typedef std::map<DataObjID,   IAlgHashSet> id_map_t;
  typedef std::map<IAlgorithm*, DataObjIDColl, iAlgHasher> alg_map_t;
  id_map_t  m_idMap;
  alg_map_t m_algMap;

  //  std::map<std::string, DataObjID> m_keyMap;
  std::set<IAlgorithm*> m_condAlgs;

  DataObjIDColl m_condIDs;

  // Map from keys to CondContBase instances.
  // Populated by startConditionSetup().
  typedef std::unordered_map<std::string, const CondContBase*> CondContMap_t;
  CondContMap_t m_condConts;

  typedef std::mutex mutex_t;
  mutable mutex_t m_lock;

};

#endif
