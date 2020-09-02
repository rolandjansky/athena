/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IOVSVC_IIOVSVCTOOL_H
#define IOVSVC_IIOVSVCTOOL_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

#include <string>
#include <memory>

#include "GaudiKernel/ClassID.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "SGTools/CallBackID.h"

class IOVRange;
class IOVTime;
class IOpaqueAddress;

namespace SG {
  class TransientAddress;
  class DataProxy;
}

class IIOVSvcTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(IIOVSvcTool, 1, 0);

  virtual void setStoreName(const std::string& storeName) = 0;
  virtual const std::string& getStoreName() const = 0;

  // register callback functions
  virtual StatusCode regFcn(SG::DataProxy *dp, const CallBackID& c, 
                            const IOVSvcCallBackFcn& fcn, bool trigger) = 0;

  virtual StatusCode regFcn(const CallBackID& c1,
                            const CallBackID& c2, 
                            const IOVSvcCallBackFcn& fcn2, 
                            bool trigger) = 0;
  
  virtual StatusCode regFcn(const IAlgTool* ia,
                            const CallBackID& c2, const IOVSvcCallBackFcn& fcn2,
                            bool trigger) = 0;
  
  // Update Range from dB
  virtual StatusCode setRange(const CLID& clid, const std::string& key, 
                              IOVRange&) = 0;

  virtual StatusCode getRange(const CLID& clid, const std::string& key, 
                              IOVRange& iov) const = 0;

  // Subscribe method for DataProxy. key StoreGate key
  virtual StatusCode regProxy(       SG::DataProxy *proxy, 
                               const std::string& key ) = 0;

  virtual StatusCode deregProxy( SG::DataProxy *proxy ) = 0;
  virtual StatusCode deregProxy( const CLID& clid, const std::string& key ) = 0;


  // Replace a registered proxy with a new version
  virtual StatusCode replaceProxy( SG::DataProxy *pOld,
                                   SG::DataProxy *pNew ) = 0;

  // Another way to subscribe
  virtual StatusCode regProxy( const CLID& clid, const std::string& key ) = 0;

  // Get IOVRange from db for current event
  virtual StatusCode getRangeFromDB(const CLID& clid, const std::string& key, 
                                    IOVRange& range, std::string &tag,
                                    std::unique_ptr<IOpaqueAddress>& ioa) const = 0;

  // Get IOVRange from db for a particular event
  virtual StatusCode getRangeFromDB(const CLID& clid, const std::string& key, 
                                    const IOVTime& time,
                                    IOVRange& range, std::string &tag,
                                    std::unique_ptr<IOpaqueAddress>& ioa) const = 0;

  // Set a particular IOVRange in db (and memory)
  virtual StatusCode setRangeInDB(const CLID& clid, const std::string& key, 
                                  const IOVRange& range, 
                                  const std::string &tag) = 0;
  
  // supply a list of TADs whose proxies will be preloaded
  virtual StatusCode preLoadTAD( const SG::TransientAddress * ) = 0;

  // supply a list of TADs whose data will be preloaded
  virtual StatusCode preLoadDataTAD( const SG::TransientAddress * ) = 0;

  // return list of tools (or functions) that have been triggered by key
  // will return FAILURE if no tools found, or no key found
  virtual StatusCode getTriggeredTools(const std::string& key,
                                       std::set<std::string>& tools) = 0;

  virtual bool holdsProxy( SG::DataProxy* proxy ) const = 0;
  virtual bool holdsProxy( const CLID& clid, const std::string& key ) const = 0;
  virtual bool holdsCallback( const CallBackID& ) const = 0;
  virtual bool holdsAlgTool( const IAlgTool* ia ) const = 0;

  virtual void resetAllProxies() = 0;

  virtual void ignoreProxy(SG::DataProxy* proxy)  = 0;
  virtual void ignoreProxy( const CLID& clid, const std::string& key)  = 0;

};

#endif
