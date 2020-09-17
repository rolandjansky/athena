/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   IIOVDbMetaDataTool.h
 * 
 * @brief This is an interface to a tool used to manage the IOV Meta Data
 * for a given object in the MetaData Store.
 * 
 * @author Antoine Pérus <perus@lal.in2p3.fr>
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Header: /build/atlas/cvs/atlas/offline/Database/IOVDbMetaDataTools/IOVDbMetaDataTools/IIOVDbMetaDataTool.h,v 1.4 2009-04-29 07:44:12 schaffer Exp $
 */

#ifndef IOVDBMETADATATOOL_IIOVDBMETADATATOOL_H
# define IOVDBMETADATATOOL_IIOVDBMETADATATOOL_H

// Gaudi
#include "AthenaKernel/IMetaDataTool.h"
#include <string>

//<<<<<< CLASS DECLARATIONS                                             >>>>>>
class CondAttrListCollection;
class IOVMetaDataContainer;

/** 
 ** @class IIOVDbMetaDataTool
 ** 
 **   @brief This is an interface to a tool used to manage the IOV Meta Data
 **   for a given object in the MetaData Store.
 **  
 **    Properties:
 **
 **    - BeginRun:<pre>        Begin run number</pre>
 **    - EndRun:<pre>          End run number</pre>
 **    - BeginEvent:<pre>      Begin event number</pre>
 **    - EndEvent:<pre>        End event number</pre>
 **/

class IIOVDbMetaDataTool : virtual public IMetaDataTool,
                           virtual public ILockableTool
{

public:
  
    /// Retrieve interface ID
    static const InterfaceID& interfaceID() { 
        static const InterfaceID IID_IIOVDbMetaDataTool("IIOVDbMetadataTool", 1 , 0); 
        return IID_IIOVDbMetaDataTool; }

    /// Register folder in the IOV Db MetaData
    virtual StatusCode  registerFolder(const std::string& folderName, 
                                       const std::string& folderDescription) const = 0;
    
    /// Register folder in the IOV Db MetaData without folder description -
    ///   uses default folder description for CondAttrListCollection
    virtual StatusCode  registerFolder(const std::string& folderName) const = 0;
    
    /// Add an IOV and Payload for a particular folder - replaces
    /// payloads if there is an IOV overlap
    virtual StatusCode  addPayload    (const std::string& folderName, 
                                       CondAttrListCollection* payload) const = 0;

    /// Explicit call to process IOV meta data from the input meta
    /// data store, transferring it to the main meta data
    /// store. Normally, this method is called at the
    /// BeginInputFile incident. However, this explicit call allows the
    /// transfer to occur during the initialization phase, which
    /// occurs before BeginInputFile incident.
    virtual StatusCode  processInputFileMetaData(const std::string& fileName) = 0;

    virtual IOVMetaDataContainer* findMetaDataContainer(const std::string& folderName) const = 0;
};


#endif // IOVDBMETADATATOOL_IIOVDBMETADATATOOL_H
