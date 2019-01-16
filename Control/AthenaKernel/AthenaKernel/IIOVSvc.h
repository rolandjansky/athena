/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IIOVSVC_H
#define ATHENAKERNEL_IIOVSVC_H

#include "GaudiKernel/IService.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ClassID.h"

#include <string>
#include <vector>
#include <set>

#include "AthenaKernel/IOVSvcDefs.h"

class IOVRange;
class IOVTime;
class IIOVSvcTool;
class CallBackID;
class IOpaqueAddress;
class CondContBase;
class DataObjID;
class EventIDBase;

namespace SG {
  class DataProxy;
  class TransientAddress;
}

/** @class IIOVSvc
 * @brief Abstract interface for IOVSvc. This is used (usually via 
 * StoreGateSvc regFcn and regHandle methods) to setup access to time-varying
 * data, such as calibration. See http://annwm.lbl.gov/~leggett/Atlas/IOVSvc/main.shtml for details
 * @author Charles Leggett
 */  

class IIOVSvc : virtual public IService {

public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID();

  virtual StatusCode createIOVTool( const std::string& storeName ) = 0;
  virtual std::vector<std::string> getStoreNames() const = 0;

  /// @name interface to the IOVSvc CBTree
  //@{ 

  /**
   * @brief register callback function
   * @param[in] dp       trigger callback after data in dp changes
   * @param[in] c        a unique identifier of the callback (see CallBackID)
   * @param[in] fcn      a boost::function object holding the callback function and the object it operates on
   * @param[in] trigger  request immediate callback of fcn  (default false)
   */
  virtual StatusCode regFcn(SG::DataProxy *dp, const CallBackID c, 
			    const IOVSvcCallBackFcn& fcn, 
			    bool trigger=false) = 0;

  /**
   * @brief register callback function
   * @param[in] c1       call back fcn2 after callback with CallBackID c1 is triggered
   * @param[in] c2       a unique identifier of this callback 
   * @param[in] fcn2     a boost::function object holding the callback function and the object it operates on
   * @param[in] trigger  request immediate callback of fcn  (default false)
   */
  virtual StatusCode regFcn(const CallBackID c1, const CallBackID c2, 
			    const IOVSvcCallBackFcn& fcn2, 
			    bool trigger=false ) = 0;

  /**
   * @brief register callback function
   * @param[in] toolName call back fcn2 after the tool named toolName (registered as a callback) is triggered
   * @param[in] c2       a unique identifier of the callback (see CallBackID)
   * @param[in] fcn2     a boost::function object holding the callback function and the object it operates on
   * @param[in] trigger  request immediate callback of fcn  (default false)
   */
  virtual StatusCode regFcn(const std::string &toolName, const CallBackID c2, 
  			    const IOVSvcCallBackFcn& fcn2, 
  			    bool trigger=false) = 0;

  /// Subscribe method for DataProxy. key StoreGate key
  virtual StatusCode regProxy( const SG::DataProxy *proxy, 
			       const std::string& key,
			       const std::string& storeName="StoreGateSvc") = 0;
  /// replace a registered proxy with a new version
  virtual StatusCode replaceProxy( const SG::DataProxy *pOld, 
				   const SG::DataProxy *pNew, 
				   const std::string& storeName="StoreGateSvc") = 0;

  /// Another way to subscribe
  virtual StatusCode regProxy( const CLID& clid, const std::string& key,
			       const std::string& storeName="StoreGateSvc" ) = 0;
  //@}

  virtual StatusCode deregProxy( const SG::DataProxy *proxy ) = 0;
  virtual StatusCode deregProxy( const CLID& clid, const std::string& key ) = 0;



  /// @name IOVRange accessors
  //@{ 
  virtual StatusCode setRange(const CLID& clid, const std::string& key, 
			      IOVRange&) = 0;
  virtual StatusCode setRange(const CLID& clid, const std::string& key, 
			      IOVRange&,
			      const std::string& storeName) = 0;

  virtual StatusCode getRange(const CLID& clid, const std::string& key, 
			      IOVRange& iov ) const = 0;

  /// Get IOVRange from db for current event
  virtual StatusCode getRangeFromDB(const CLID& clid, const std::string& key, 
				    IOVRange& range, std::string &tag,
                                    IOpaqueAddress*& ioa) const =0;

  /// Get IOVRange from db for a particular event
  virtual StatusCode getRangeFromDB(const CLID& clid, const std::string& key, 
				    const IOVTime& time,
				    IOVRange& range, std::string &tag,
                                    IOpaqueAddress*& ioa) const=0;

  /// Set a particular IOVRange in db (and memory)
  virtual StatusCode setRangeInDB(const CLID& clid, const std::string& key, 
				  const IOVRange& range, 
				  const std::string &tag) = 0;
  //@}


  /// pre-load transient Addresses
  virtual StatusCode preLoadTAD( const SG::TransientAddress *,
				 const std::string& storeName="StoreGateSvc" ) = 0;

  // supply a list of TADs whose data will be preloaded via a 'partial preload' flag
  virtual StatusCode preLoadDataTAD( const SG::TransientAddress *,
				     const std::string& storeName="StoreGateSvc" ) = 0;

  /// get the names of the tools that have been triggered
  virtual StatusCode getTriggeredTools(const std::string& key,
  				       std::set<std::string>& tools,
				       const std::string& storeName="StoreGateSvc") = 0;

  /// reset all proxies known to IOVSvc
  virtual void resetAllProxies() = 0;

  virtual void ignoreProxy(const CLID& clid, const std::string& key,
                           const std::string& storeName="StoreGateSvc") = 0;

  virtual StatusCode createCondObj(CondContBase*, const DataObjID&, 
                                   const EventIDBase&) = 0;

};

inline
const InterfaceID& 
IIOVSvc::interfaceID() {
    static const InterfaceID IID("IIOVSvc", 1, 0);
    return IID;
}

#endif
