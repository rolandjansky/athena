/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IOVSVC_IOVSVCTOOL_H
#define IOVSVC_IOVSVCTOOL_H 1


/*****************************************************************************
 *
 *  IOVSvcTool.h
 *  IOVSvc
 *
 *  Author: Charles Leggett
 *  $Id: IOVSvcTool.h,v 1.7 2008-06-04 23:35:03 leggett Exp $
 *
 *  Provides automatic updating and callbacks for time dependent data
 *  This AlgTool does the real work.
 *
 *****************************************************************************/


#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AthenaKernel/IOVTime.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "SGTools/DataProxy.h"
#include "SGTools/CallBackID.h"
#include "IOVSvc/IIOVSvcTool.h"
#include "IOVSvc/IOVEntry.h"

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


class IOVSvcTool: virtual public IIOVSvcTool,
                  virtual public IIncidentListener,
                  virtual public AthAlgTool {

public:

  IOVSvcTool(const std::string& type, const std::string& name,
             const IInterface* parent);


  virtual StatusCode initialize();
  virtual StatusCode reinitialize();
  virtual StatusCode finalize();


  /////////////////////////////////////////////////////////////////////////

  // Incident handler
  void handle(const Incident&);

  void setStoreName(const std::string& storeName) {
    m_storeName = storeName;    
  }
  const std::string& getStoreName() const { return m_storeName; }

  // register callback functions
  StatusCode regFcn(SG::DataProxy *dp, const CallBackID c, 
                    const IOVSvcCallBackFcn& fcn, bool trigger = false);

  StatusCode regFcn(const CallBackID c1,
                    const CallBackID c2, const IOVSvcCallBackFcn& fcn2, 
                    bool trigger = false);

  StatusCode regFcn(const IAlgTool* ia,
                    const CallBackID c2, const IOVSvcCallBackFcn& fcn2,
                    bool trigger = false);

  // Update Range from dB
  virtual StatusCode setRange(const CLID& clid, const std::string& key, 
                              IOVRange&);

  virtual StatusCode getRange(const CLID& clid, const std::string& key, 
                              IOVRange& iov) const;

  // Subscribe method for DataProxy. key StoreGate key
  virtual StatusCode regProxy( const SG::DataProxy *proxy, 
                               const std::string& key );
  // Another way to subscribe
  virtual StatusCode regProxy( const CLID& clid, const std::string& key );

  virtual StatusCode deregProxy( const SG::DataProxy *proxy );
  virtual StatusCode deregProxy( const CLID& clid, const std::string& key );

  // replace method for DataProxy, to be used when an update is necessary
  virtual StatusCode replaceProxy( const SG::DataProxy *pOld,
                                   const SG::DataProxy *pNew);

  // Get IOVRange from db for current event
  virtual StatusCode getRangeFromDB(const CLID& clid, const std::string& key, 
                                    IOVRange& range, std::string &tag,
                                    IOpaqueAddress*& ioa) const;

  // Get IOVRange from db for a particular event
  virtual StatusCode getRangeFromDB(const CLID& clid, const std::string& key, 
                                    const IOVTime& time,
                                    IOVRange& range, std::string &tag,
                                    IOpaqueAddress*& ioa) const;

  // Set a particular IOVRange in db (and memory)
  virtual StatusCode setRangeInDB(const CLID& clid, const std::string& key, 
                                  const IOVRange& range, 
                                  const std::string &tag);
  
  // supply a list of TADs whose proxies will be preloaded
  virtual StatusCode preLoadTAD( const SG::TransientAddress * );

  // supply a list of TADs whose data will be preloaded
  virtual StatusCode preLoadDataTAD( const SG::TransientAddress * );

  // return list of tools (or functions) that have been triggered by key
  // will return FAILURE if no tools found, or no key found
  virtual StatusCode getTriggeredTools(const std::string& key,
                                       std::set<std::string>& tools);

  bool holdsProxy( const SG::DataProxy* proxy ) const;
  bool holdsProxy( const CLID& clid, const std::string& key ) const;
  bool holdsCallback( const CallBackID& ) const;
  bool holdsAlgTool( const IAlgTool* ia ) const;

  virtual void resetAllProxies();

  void ignoreProxy( const CLID& clid, const std::string& key ) {
    m_ignoredProxyNames.insert( std::make_pair(clid,key) );
  }
  void ignoreProxy(const SG::DataProxy* proxy) {
    m_ignoredProxies.insert(proxy);
  }

protected:

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
  void setRange_impl (const SG::DataProxy* proxy, IOVRange& iovr);

  std::string m_storeName;

  mutable MsgStream m_log;

  StoreGateSvc* p_sgSvc;
  ServiceHandle<StoreGateSvc> p_cndSvc;
  //  ServiceHandle<IProxyDict> p_cndSvc;
  ServiceHandle<IIncidentSvc> p_incSvc;
  ServiceHandle<IProxyProviderSvc> p_PPSvc;
  ServiceHandle<IClassIDSvc> p_CLIDSvc;
  ServiceHandle<IToolSvc> p_toolSvc;

  IOVTime m_curTime;

  typedef IOVSvcCallBackFcn BFCN;
  typedef std::multimap<const SG::DataProxy*, BFCN*>::iterator pmITR;
  typedef std::multimap<BFCN*, const SG::DataProxy*>::iterator fnITR;
  typedef std::multiset<IOVEntry*, IOVEntry::IOVEntryStartCritereon> startSet;
  typedef std::multiset<IOVEntry*, IOVEntry::IOVEntryStopCritereon> stopSet;
  typedef startSet::iterator startITR;
  typedef stopSet::iterator  stopITR;

  std::map< const SG::DataProxy*, std::string> m_names;

  std::set< const SG::DataProxy*, SortDPptr > m_proxies;
  std::multimap< const SG::DataProxy*, BFCN* > m_proxyMap;
  std::multimap< BFCN*, const SG::DataProxy* > m_bfcnMap;

  std::set<const SG::DataProxy*> m_ignoredProxies;
  std::set< std::pair<CLID, std::string> > m_ignoredProxyNames;

  std::map<BFCN*, CallBackID> m_fcnMap;
  std::map<CallBackID, BFCN*> m_cbidMap;

  typedef std::map<const void*, std::set<CallBackID>* > ObjMap;
  ObjMap m_objMap;

  std::map< const SG::DataProxy*, IOVEntry*> m_entries;

  startSet *p_startSet;
  stopSet  *p_stopSet;

  startSet m_startSet_Clock, m_startSet_RE;
  stopSet  m_stopSet_Clock, m_stopSet_RE;


  CBTree* m_trigTree;

  std::set< const SG::TransientAddress*, SortTADptr > m_preLoad, m_partPreLoad;

  bool m_first;
  BooleanProperty m_preLoadRanges, m_preLoadData, m_partialPreLoadData;
  BooleanProperty m_sortKeys, m_forceReset;
  StringProperty  m_updateInterval;
  bool m_checkOnce;
  bool m_triggered;
  bool m_resetAllCallbacks;
  std::string m_checkTrigger;

  void scanStartSet(startSet &pSet, const std::string &type,
                    std::set<const SG::DataProxy*, SortDPptr> &proxiesToReset);
  void scanStopSet(stopSet &pSet, const std::string &type,
                   std::set<const SG::DataProxy*, SortDPptr> &proxiesToReset);

  void PrintStartSet();
  void PrintStopSet();
  void PrintProxyMap();
  void PrintProxyMap(const SG::DataProxy*);
  

private:
  IOVSvcTool (const IOVSvcTool&);
  IOVSvcTool& operator= (const IOVSvcTool&);
};

#endif
