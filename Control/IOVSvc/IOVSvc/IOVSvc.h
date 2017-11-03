/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IOVSVC_IOVSVC_H
#define IOVSVC_IOVSVC_H

/*****************************************************************************
 *
 *  IOVSvc.h
 *  IOVSvc
 *
 *  Author: Charles Leggett
 *  $Id: IOVSvc.h,v 1.39 2008-05-15 18:04:12 leggett Exp $
 *
 *  Provides automatic updating and callbacks for time dependent data
 *
 *****************************************************************************/

#include "AthenaBaseComps/AthService.h"
#ifndef KERNEL_STATUSCODES_H
#include "GaudiKernel/StatusCode.h"
#endif
#ifndef GAUDIKERNEL_CLASSID_H
#include "GaudiKernel/ClassID.h"
#endif
#ifndef GAUDIKERNEL_MSGSTREAM_H
#include "GaudiKernel/MsgStream.h"
#endif
#ifndef SGTOOLS_DATAPROXY_H
#include "SGTools/DataProxy.h"
#endif

#ifndef  ATHENAKERNEL_IIOVSVC_H
#include "AthenaKernel/IIOVSvc.h"
#endif

#ifndef IOVSVC_IOVENTRY_H
#include "IOVSvc/IOVEntry.h"
#endif
#ifndef SGTOOLS_CALLBACKID_H
#include "SGTools/CallBackID.h"
#endif
#ifndef ATHENAKERNEL_IOVTIME_H
#include "AthenaKernel/IOVTime.h"
#endif

#ifndef ATHENAKERNEL_IOVSVCDEFS_H
#include "AthenaKernel/IOVSvcDefs.h"
#endif

#include "IOVSvc/IIOVSvcTool.h"

#ifndef _CPP_SET
#include <set>
#endif
#ifndef _CPP_MAP
#include <map>
#endif
#ifndef _CPP_LIST
#include <list>
#endif
#ifndef _CPP_STRING
#include <string>
#endif
#include <mutex>

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ICondSvc.h"



class StoreGateSvc;
class IClassIDSvc;
class IProxyDict;
class IToolSvc;
class CallBackID;
class CBTree;

namespace SG {
  class TransientAddress;
  class DataProxy;
}

class IOVSvc: virtual public AthService, 
              virtual public IIOVSvc {

public:

  IOVSvc( const std::string& name, ISvcLocator* svc );
  virtual ~IOVSvc();

  virtual StatusCode initialize();
  virtual StatusCode reinitialize();
  virtual StatusCode finalize();

  /// Query the interfaces.
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  ///  static const InterfaceID& interfaceID();

  StatusCode createIOVTool( const std::string& storeName );
  std::vector<std::string> getStoreNames() const;

  /// register callback functions
  StatusCode regFcn(SG::DataProxy *dp, const CallBackID c, 
                    const IOVSvcCallBackFcn& fcn, bool trigger=false);

  StatusCode regFcn(const CallBackID c1,
                    const CallBackID c2, const IOVSvcCallBackFcn& fcn2, 
                    bool trigger);

  StatusCode regFcn(const std::string& toolName,
                    const CallBackID c2, const IOVSvcCallBackFcn& fcn2,
                    bool trigger);

  /// Update Range from dB
  virtual StatusCode setRange(const CLID& clid, const std::string& key, 
                              IOVRange&);
  virtual StatusCode setRange(const CLID& clid, const std::string& key, 
                              IOVRange& io,
                              const std::string& storeName);

  virtual StatusCode getRange(const CLID& clid, const std::string& key, 
                              IOVRange& io) const;

  /// Subscribe method for DataProxy. key StoreGate key
  virtual StatusCode regProxy( const SG::DataProxy *proxy, 
                               const std::string& key,
                               const std::string& storeName);

  /// replace a registered DataProxy with a new version
  virtual StatusCode replaceProxy( const SG::DataProxy *pOld,
                                   const SG::DataProxy *pNew,
                                   const std::string& storeName);


  /// Another way to subscribe
  virtual StatusCode regProxy( const CLID& clid, const std::string& key,
                               const std::string& storeName );


  virtual StatusCode deregProxy( const SG::DataProxy *proxy );
  virtual StatusCode deregProxy( const CLID& clid, const std::string& key );



  /// Get IOVRange from db for current event
  virtual StatusCode getRangeFromDB(const CLID& clid, const std::string& key, 
                                    IOVRange& range, std::string &tag,
                                    IOpaqueAddress*& ioa) const;

  /// Get IOVRange from db for a particular event
  virtual StatusCode getRangeFromDB(const CLID& clid, const std::string& key, 
                                    const IOVTime& time,
                                    IOVRange& range, std::string &tag,
                                    IOpaqueAddress*& ioa) const;

  /// Set a particular IOVRange in db (and memory)
  virtual StatusCode setRangeInDB(const CLID& clid, const std::string& key, 
                                  const IOVRange& range, 
                                  const std::string &tag);
  
  /// supply a list of TADs whose proxies will be preloaded
  virtual StatusCode preLoadTAD( const SG::TransientAddress *,
                                 const std::string& storeName );

  /// supply a list of TADs whose data will be preloaded
  virtual StatusCode preLoadDataTAD( const SG::TransientAddress *,
                                     const std::string& storeName );

  /// return list of tools (or functions) that have been triggered by key
  /// will return FAILURE if no tools found, or no key found
  virtual StatusCode getTriggeredTools(const std::string& key,
                                       std::set<std::string>& tools,
                                       const std::string& storeName);

  virtual void resetAllProxies();

  virtual void ignoreProxy(const CLID& clid, const std::string& key,
                           const std::string& storeName);

  virtual StatusCode createCondObj(CondContBase*, const DataObjID&, 
                                   const EventIDBase&);


private:

  bool createIOVTool(const std::string& storeName, IIOVSvcTool*& tool) const;
  IIOVSvcTool* getTool( const std::string& storeName, 
                        bool createIF=true ) const;
  IIOVSvcTool* getTool( const CLID& clid, const std::string& key) const;
  IIOVSvcTool* getTool( const SG::DataProxy* proxy ) const;
  IIOVSvcTool* getTool( const CallBackID& c1 ) const;
  IIOVSvcTool* getTool( const IAlgTool* ia ) const;
  std::string fullProxyName( const SG::DataProxy* proxy ) const;
  std::string fullProxyName( const CLID& clid, const std::string& key ) const;
  

  typedef std::map< std::string, IIOVSvcTool* > toolMap;

  mutable toolMap m_toolMap;


  BooleanProperty m_preLoadRanges, m_preLoadData, m_partialPreLoadData;
  BooleanProperty m_sortKeys, m_forceReset;
  std::string m_updateInterval;

  ServiceHandle<IToolSvc> p_toolSvc;
  ServiceHandle<IClassIDSvc> p_CLIDSvc;
  ServiceHandle<StoreGateSvc> p_sgs, p_detStore;
  ServiceHandle<ICondSvc> p_condSvc;

  mutable std::mutex m_lock;

};

#endif
