/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IIOVDBSVC_H
#define ATHENAKERNEL_IIOVDBSVC_H

/**
 *
 *  @file IIOVDbSvc.h
 *  @brief Abstract interface to IOVDbSvc to access IOVRange and 
 *  tag information
 *
 *  @author Charles Leggett, RD Schaffer
 *  $Id: IIOVDbSvc.h,v 1.11 2009-02-02 17:54:35 leggett Exp $
 *
 *
 *****************************************************************************/

#include "GaudiKernel/IInterface.h"
#include <string>
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ClassID.h"

// Declaration of the interface ID. 
static const InterfaceID IID_IOVDbSvc("IOVDbSvc", 1 , 0);   

class IOVRange;
class IOVTime;
class IOpaqueAddress;
class EventContext;

/**
 *
 *  @class IIOVDbSvc
 *  @brief Abstract interface to IOVDbSvc to access IOVRange and 
 *  tag information
 */
class IIOVDbSvc  : virtual public IInterface  {
public:
    /// Retrieve interface ID
    static const InterfaceID& interfaceID();

    /// Get range for a particular data object
    virtual StatusCode getRange      (const CLID& clid, 
				      const std::string& dbKey, 
				      const IOVTime& time,
				      IOVRange& range,
				      std::string& tag,
                                      IOpaqueAddress*& ioa) = 0;

    /// Set range for a particular data object
    virtual StatusCode setRange      (const CLID& clid,
				      const std::string& dbKey,
				      const IOVRange& range,
				      const std::string& tag ) = 0;

    /// Set time for begin run
    virtual StatusCode signalBeginRun(const IOVTime& beginRunTime,
                                      const EventContext& ctx) = 0;

    /// Signal that callback has been fired
    virtual void       signalEndProxyPreload() = 0;

    /// Register callback for TagInfo access
    virtual StatusCode registerTagInfoCallback() = 0;

    // return list of SG keys being provided by IOVDbSvc
    virtual std::vector<std::string> getKeyList() = 0;

    // return information about one SG key
    // - folder, tag, IOVRange and whether data has been retrieved
    // (if not, range and tag may not be meaningful)
    // return false if this key is not known to IOVDbSvc
    virtual bool getKeyInfo(const std::string& key, std::string& foldername,
                            std::string& tag, IOVRange& range, bool& retrieved,
                            unsigned long long& bytesRead, float& readTime) = 0;
    
    // drop an IOVDbSvc-managed object from Storegate, indicating we will
    // not read it again and can free up memory 
    // If resetCache=True, also drop the corresponding folder cache
    // so any subsequent reads will access the database again
    // returns False if key not known to IOVDbSvc
    virtual bool dropObject(const std::string& key,
			    const bool resetCache=false) = 0;
};


inline
const InterfaceID& 
IIOVDbSvc::interfaceID() {
    static const InterfaceID IID("IOVDbSvc", 1, 0);
    return IID;
}

#endif
