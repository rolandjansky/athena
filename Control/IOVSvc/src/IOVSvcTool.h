/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IOVSVC_IOVSVCTOOL_H
#define IOVSVC_IOVSVCTOOL_H 1


/*****************************************************************************
 *
 *  IOVSvcTool.h
 *  IOVSvc
 *
 *  Author: Charles Leggett
 *
 *  Provides automatic updating and callbacks for time dependent data
 *  This AlgTool does the real work.
 *
 *****************************************************************************/


#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AthenaKernel/IOVTime.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "SGTools/DataProxy.h"
#include "SGTools/CallBackID.h"
#include "IOVSvc/IIOVSvcTool.h"
#include "IOVEntry.h"

#include <string>
#include <set>
#include <map>
#include <list>
#include <utility>


class StoreGateSvc;
class IIncidentSvc;
class Incident;
class IProxyProviderSvc;
class IClassIDSvc;
class IProxyDict;
class IToolSvc;
class CallBackID;
class CBTree;

namespace SG {
  class TransientAddress;
  class DataProxy;
}

class SortTADptr {
public:
  bool operator() ( const SG::TransientAddress*, 
                    const SG::TransientAddress* ) const;
};

class SortDPptr {
public:
  bool operator() ( const SG::DataProxy*, 
                    const SG::DataProxy* ) const;
};


class IOVSvcTool: public extends<AthAlgTool, IIOVSvcTool, IIncidentListener> {

public:

  IOVSvcTool(const std::string& type, const std::string& name,
             const IInterface* parent);


  virtual StatusCode initialize() override;
  virtual StatusCode reinitialize() override;


  /////////////////////////////////////////////////////////////////////////

  // Incident handler
  virtual void handle(const Incident&) override;

  virtual
  void setStoreName(const std::string& storeName) override {
    m_storeName = storeName;    
  }
  virtual const std::string& getStoreName() const override { return m_storeName; }

  // register callback functions
  virtual StatusCode regFcn(SG::DataProxy *dp, const CallBackID& c,
                            const IOVSvcCallBackFcn& fcn, bool trigger = false) override;

  virtual StatusCode regFcn(const CallBackID& c1,
                            const CallBackID& c2, const IOVSvcCallBackFcn& fcn2,
                            bool trigger = false) override;

  virtual StatusCode regFcn(const IAlgTool* ia,
                            const CallBackID& c2, const IOVSvcCallBackFcn& fcn2,
                            bool trigger = false) override;

  // Update Range from dB
  virtual StatusCode setRange(const CLID& clid, const std::string& key, 
                              IOVRange&) override;

  virtual StatusCode getRange(const CLID& clid, const std::string& key, 
                              IOVRange& iov) const override;

  // Subscribe method for DataProxy. key StoreGate key
  virtual StatusCode regProxy( SG::DataProxy *proxy, 
                               const std::string& key ) override;
  // Another way to subscribe
  virtual StatusCode regProxy( const CLID& clid, const std::string& key ) override;

  virtual StatusCode deregProxy( SG::DataProxy *proxy ) override;
  virtual StatusCode deregProxy( const CLID& clid, const std::string& key ) override;

  // replace method for DataProxy, to be used when an update is necessary
  virtual StatusCode replaceProxy( SG::DataProxy *pOld,
                                   SG::DataProxy *pNew) override;

  // Get IOVRange from db for current event
  virtual StatusCode getRangeFromDB(const CLID& clid, const std::string& key, 
                                    IOVRange& range, std::string &tag,
                                    std::unique_ptr<IOpaqueAddress>& ioa) const override;

  // Get IOVRange from db for a particular event
  virtual StatusCode getRangeFromDB(const CLID& clid, const std::string& key, 
                                    const IOVTime& time,
                                    IOVRange& range, std::string &tag,
                                    std::unique_ptr<IOpaqueAddress>& ioa) const override;

  // Set a particular IOVRange in db (and memory)
  virtual StatusCode setRangeInDB(const CLID& clid, const std::string& key, 
                                  const IOVRange& range, 
                                  const std::string &tag) override;
  
  // supply a list of TADs whose proxies will be preloaded
  virtual StatusCode preLoadTAD( const SG::TransientAddress * ) override;

  // supply a list of TADs whose data will be preloaded
  virtual StatusCode preLoadDataTAD( const SG::TransientAddress * ) override;

  // return list of tools (or functions) that have been triggered by key
  // will return FAILURE if no tools found, or no key found
  virtual StatusCode getTriggeredTools(const std::string& key,
                                       std::set<std::string>& tools) override;

  virtual bool holdsProxy( SG::DataProxy* proxy ) const override;
  virtual bool holdsProxy( const CLID& clid, const std::string& key ) const override;
  virtual bool holdsCallback( const CallBackID& ) const override;
  virtual bool holdsAlgTool( const IAlgTool* ia ) const override;

  virtual void resetAllProxies() override;

  virtual
  void ignoreProxy( const CLID& clid, const std::string& key ) override{
    m_ignoredProxyNames.insert( std::make_pair(clid,key) );
  }
  virtual
  void ignoreProxy(SG::DataProxy* proxy) override {
    m_ignoredProxies.insert(proxy);
  }

  // Destructor.
  virtual ~IOVSvcTool();

private:

  StatusCode preLoadProxies();
  StatusCode preLoadData();
  StatusCode triggerCallback( IOVSvcCallBackFcn*, const std::string& key );
  StatusCode triggerCallback( const SG::DataProxy*, const std::string& key );
  std::string fullProxyName( const SG::TransientAddress* ) const;
  std::string fullProxyName( const SG::DataProxy* ) const;
  std::string fullProxyName( const CLID&, const std::string& ) const;
  void setRange_impl (SG::DataProxy* proxy, IOVRange& iovr);

  std::string m_storeName;

  ServiceHandle<StoreGateSvc> p_cndSvc;
  ServiceHandle<IIncidentSvc> p_incSvc;
  ServiceHandle<IProxyProviderSvc> p_PPSvc;
  ServiceHandle<IClassIDSvc> p_CLIDSvc;
  ServiceHandle<IToolSvc> p_toolSvc;

  IOVTime m_curTime{0};

  typedef IOVSvcCallBackFcn BFCN;
  typedef std::multimap<const SG::DataProxy*, BFCN*>::iterator pmITR;
  typedef std::multimap<BFCN*, const SG::DataProxy*>::iterator fnITR;
  typedef std::multiset<IOVEntry*, IOVEntry::IOVEntryStartCritereon> startSet;
  typedef std::multiset<IOVEntry*, IOVEntry::IOVEntryStopCritereon> stopSet;
  typedef startSet::iterator startITR;
  typedef stopSet::iterator  stopITR;

  std::map< const SG::DataProxy*, std::string> m_names;

  std::set< SG::DataProxy*, SortDPptr > m_proxies;
  std::multimap< const SG::DataProxy*, BFCN* > m_proxyMap;
  std::multimap< BFCN*, const SG::DataProxy* > m_bfcnMap;

  std::set<SG::DataProxy*> m_ignoredProxies;
  std::set< std::pair<CLID, std::string> > m_ignoredProxyNames;

  std::map<BFCN*, CallBackID> m_fcnMap;
  std::map<CallBackID, BFCN*> m_cbidMap;

  typedef std::map<const void*, std::set<CallBackID>* > ObjMap;
  ObjMap m_objMap;

  std::map< const SG::DataProxy*, IOVEntry*> m_entries;

  startSet *p_startSet{nullptr};
  stopSet  *p_stopSet{nullptr};

  startSet m_startSet_Clock, m_startSet_RE;
  stopSet  m_stopSet_Clock, m_stopSet_RE;

  CBTree* m_trigTree{nullptr};

  std::set< const SG::TransientAddress*, SortTADptr > m_preLoad;

  typedef std::tuple <CLID, std::string> TADkey_t;
  TADkey_t TADkey (const SG::DataProxy& p)
  { return TADkey_t (p.clID(), p.name()); }
  TADkey_t TADkey (const SG::TransientAddress& t)
  { return TADkey_t (t.clID(), t.name()); }
  std::set< TADkey_t > m_partPreLoad;

  bool m_first{true};
  bool m_checkOnce{false};
  bool m_triggered{false};
  bool m_firstEventOfRun{false};
  bool m_resetAllCallbacks{false};
  std::string m_checkTrigger;

  Gaudi::Property<bool> m_preLoadRanges{this, "preLoadRanges", false};
  Gaudi::Property<bool> m_preLoadData{this, "preLoadData", false};
  Gaudi::Property<bool> m_partialPreLoadData{this, "partialPreLoadData", true};
  Gaudi::Property<bool> m_preLoadExtensibleFolders{this, "preLoadExtensibleFolders", true};
  Gaudi::Property<bool> m_sortKeys{this, "sortKeys", true};
  Gaudi::Property<bool> m_forceReset{this, "forceResetAtBeginRun", false};
  Gaudi::Property<std::string> m_updateInterval{this, "updateInterval", "Event"};


  void scanStartSet(startSet &pSet, const std::string &type,
                    std::set<SG::DataProxy*, SortDPptr> &proxiesToReset);
  void scanStopSet(stopSet &pSet, const std::string &type,
                   std::set<SG::DataProxy*, SortDPptr> &proxiesToReset);

  void PrintStartSet();
  void PrintStopSet();
  void PrintProxyMap();
  void PrintProxyMap(const SG::DataProxy*);

};

#endif
