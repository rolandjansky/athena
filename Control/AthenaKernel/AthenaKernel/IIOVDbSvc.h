/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
 *
 *
 *****************************************************************************/

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ClassID.h"

#include "AthenaKernel/IOVRange.h"

#include <string>
#include <memory>

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

    /// Filled by IIOVDbSvc::getKeyInfo
    struct KeyInfo {
        std::string folderName;
        std::string tag;
        IOVRange range;
        float readTime{0};
        unsigned long long bytesRead{0};
        bool retrieved{false};
        bool extensible{false};
    };

    /// Get range for a particular data object
    virtual StatusCode getRange(const CLID& clid,
                                const std::string& dbKey,
                                const IOVTime& time,
                                IOVRange& range,
                                std::string& tag,
                                std::unique_ptr<IOpaqueAddress>& ioa) = 0;

    /// Set range for a particular data object
    virtual StatusCode setRange(const CLID& clid,
                                const std::string& dbKey,
                                const IOVRange& range,
                                const std::string& tag ) = 0;

    /// Set time for begin run
    virtual StatusCode signalBeginRun(const IOVTime& beginRunTime,
                                      const EventContext& ctx) = 0;

    /// Signal that callback has been fired
    virtual void signalEndProxyPreload() = 0;

    /// May be called once conditions are loaded to let IOVDbSvc release resources
    virtual void       postConditionsLoad() = 0;

    /// Process TagInfo
    virtual StatusCode processTagInfo() = 0;

    /// return list of SG keys being provided by IOVDbSvc
    virtual std::vector<std::string> getKeyList() = 0;

    /// Return information about SG key
    /// return false if this key is not known to IOVDbSvc
    virtual bool getKeyInfo(const std::string& key, IIOVDbSvc::KeyInfo& info) = 0;

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
