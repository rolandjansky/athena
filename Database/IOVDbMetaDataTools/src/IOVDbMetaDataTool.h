/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IOVDBMETADATATOOLS_IOVDBMETADATATOOL_H
#define IOVDBMETADATATOOLS_IOVDBMETADATATOOL_H

/**
 * @file   IOVDbMetaDataTool.h
 * 
 * @brief This is a tool used to manage the IOV Meta Data for a given
 * object into the Meta Data Store.
 * 
 * @author Antoine Pérus <perus@lal.in2p3.fr>
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * @date   April 2007
 *
 */

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "IOVDbMetaDataTools/IIOVDbMetaDataTool.h"

#include <string>  
#include <shared_mutex>

class StoreGateSvc;
class IOVMetaDataContainer;

/**
 * @class IOVDbMetaDataTool
 *
 * @brief This is an interface to a tool used to manage the IOV Meta Data
 * for a given object in the Meta Data Store.
 *
 **/

class IOVDbMetaDataTool : virtual public AthAlgTool,
                          virtual public IIncidentListener,
                          virtual public IIOVDbMetaDataTool
{
public:    
    // Constructor
    IOVDbMetaDataTool(const std::string& type, 
                      const std::string& name, 
                      const IInterface* parent); 
    // Standard Destructor
    virtual ~IOVDbMetaDataTool();

    /// Initialize AlgTool
    virtual StatusCode initialize() override;

    /// Finalize AlgTool
    virtual StatusCode finalize() override;

    /// Function called when a new input file is opened
    virtual StatusCode beginInputFile(const SG::SourceID&) override;

    /// Function called when the currently open input file got completely processed
    virtual StatusCode endInputFile(const SG::SourceID&) override;

    /// Function called when the tool should write out its metadata
    virtual StatusCode metaDataStop() override;

    /// Incident service handle listening for BeginInputFile and EndInputFile.
    virtual void handle(const Incident& incident) override;

    /// Register folder in the IOV Db MetaData - done once at initialize
    virtual StatusCode  registerFolder(const std::string& folderName, 
                                       const std::string& folderDescription) const override;
    
    /// Register folder in the IOV Db MetaData without folder description -
    ///   uses default folder description for CondAttrListCollection
    virtual StatusCode registerFolder(const std::string& folderName) const override;

    /// Add an IOV and Payload for a particular folder - replaces
    /// payloads if there is an IOV overlap
    virtual StatusCode addPayload(const std::string& folderName,
				   CondAttrListCollection* payload) const override;

    /// Explicit call to process IOV meta data from the input meta
    /// data store, transferring it to the main meta data
    /// store. Normally, this method is called at the
    /// BeginInputFile incident. However, this explicit call allows the
    /// transfer to occur during the initialization phase, which
    /// occurs before BegininputFile incident.
    virtual StatusCode processInputFileMetaData(const std::string& fileName) override;

    virtual IOVMetaDataContainer* findMetaDataContainer(const std::string& folderName) const override final;

    // ILockableTool API implementation
    virtual void lock_shared() const override final { m_mutex.lock_shared(); }
    virtual void unlock_shared() const override final { m_mutex.unlock_shared(); }

private:

    /// return meta data container from the meta data store
    IOVMetaDataContainer* getMetaDataContainer(const std::string& folderName, 
                                               const std::string& folderDescription) const;

    /// check if we should override the run number in the incoming meta data
    void        checkOverrideRunNumber();

    /// override IOV with new run number
    StatusCode  overrideIOV (CondAttrListCollection*& coll) const;

    /// Modify a Payload for a particular folder - replaces one of the
    /// internal attributes
    StatusCode  modifyPayload ATLAS_NOT_THREAD_SAFE(const std::string& folderName, 
                                                    CondAttrListCollection*& payload) const;

    /// Add input objects to MetaCont
    StatusCode fillMetaCont(const std::string& sid
			    , const IOVMetaDataContainer* iovCont);

    /// Dump the contents of MetaCont<IOVMetaDataContainer> objects in MetaDataStore
    StatusCode dumpMetaConts();

    /// Handles to the meta data stores
    typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
    StoreGateSvc_t   m_metaDataStore;
    StoreGateSvc_t   m_inputStore;

    // Has the FirstInputFileIncident fired? Used to skip the first
    // BeginInputFile incident
    bool             m_processedFirstInputFileIncident;

    // Flag to check whether we need to override run number for MC
    // events in incoming file meta data. This is needed for example
    // for incoming simulation or digitization parameters.
    bool             m_overrideRunNumber;
    bool             m_overrideMinMaxRunNumber;

    // Values for changing run number
    unsigned int     m_newRunNumber;
    unsigned int     m_oldRunNumber;

    unsigned int     m_minRunNumber;
    unsigned int     m_maxRunNumber;

    // Files already processed, maintain a set
    std::set<std::string> m_filesProcessed;

    // Min/max run numbers for overriding meta data IOV  
    UnsignedIntegerArrayProperty m_minMaxRunNumbers;

    // Folders and attributes to be deleted
    StringArrayProperty  m_foldersToBeModified;
    StringArrayProperty  m_attributesToBeRemoved;
    bool                 m_modifyFolders;

    // mutex for R/W locking of the entire tool (and supposedly all metadata objects it works with)
    mutable std::shared_mutex    m_mutex  ATLAS_THREAD_SAFE;
};

#endif // IOVDBMETADATATOOLS_IOVDBMETADATATOOL_H


