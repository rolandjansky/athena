/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

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
 * $Header: /build/atlas/cvs/atlas/offline/Database/IOVDbMetaDataTools/src/IOVDbMetaDataTool.h,v 1.5 2009-04-29 07:44:13 schaffer Exp $
 */

#ifndef IOVDBMETADATATOOLS_IOVDBMETADATATOOL_H
# define IOVDBMETADATATOOLS_IOVDBMETADATATOOL_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaKernel/IMetaDataTool.h"
#include "IOVDbMetaDataTools/IIOVDbMetaDataTool.h"

#include <string>  

//<<<<<< PUBLIC TYPES                                                   >>>>>>

class StoreGateSvc;
class IOVMetaDataContainer;

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/** 
 ** Class IOVDbMetaDataTool
 ** 
 **   @brief This is an interface to a tool used to manage the IOV Meta Data
 **   for a given object in the Meta Data Store.
 **  
 **    Properties:
 **
 **    - BeginRun:<pre>        Begin run number</pre>
 **    - EndRun:<pre>          End run number</pre>
 **    - BeginEvent:<pre>      Begin event number</pre>
 **    - EndEvent:<pre>        End event number</pre>
 **    - BeginTime:<pre>       Begin time</pre>
 **    - EndTime:<pre>         End time</pre>
 **/
class IOVDbMetaDataTool : virtual public AthAlgTool,
                          virtual public IMetaDataTool,
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
    virtual StatusCode initialize();

    /// Finalize AlgTool
    virtual StatusCode finalize();


    /// Function called when a new input file is opened
    StatusCode beginInputFile() {return StatusCode::SUCCESS;}

    /// Function called when the currently open input file got completely
    /// processed
    StatusCode endInputFile() {return StatusCode::SUCCESS;}

    /// Function called when the tool should write out its metadata
    StatusCode metaDataStop() {return StatusCode::SUCCESS;}

    /// Function called when a new input file is opened
    StatusCode beginInputFile(const SG::SourceID&) {return this->beginInputFile();}

    /// Function called when the currently open input file got completely
    /// processed
    StatusCode endInputFile(const SG::SourceID&) {return this->endInputFile();}

    /// Incident service handle listening for BeginInputFile and EndInputFile.
    void handle(const Incident& incident);

    /// Register folder in the IOV Db MetaData - done once at initialize
    virtual StatusCode  registerFolder(const std::string& folderName, 
                                       const std::string& folderDescription) const;
    
    /// Register folder in the IOV Db MetaData without folder description -
    ///   uses default folder description for CondAttrListCollection
    virtual StatusCode  registerFolder(const std::string& folderName) const;

    /// Add an IOV and Payload for a particular folder - replaces
    /// payloads if there is an IOV overlap
    StatusCode  addPayload    (const std::string& folderName, 
                               CondAttrListCollection* payload) const;

    /// Explicit call to process IOV meta data from the input meta
    /// data store, transferring it to the main meta data
    /// store. Normally, this method is called at the
    /// BeginFileIncident. However, this explicit call allows the
    /// transfer to occur during the initialization phase, which
    /// occurs before BeginFileIncident.
    StatusCode  processInputFileMetaData(const std::string& fileName);

private:

    /// return meta data containerr from the meta data store
    IOVMetaDataContainer* getMetaDataContainer(const std::string& folderName, 
                                               const std::string& folderDescription) const;

    /// check if we should override the run number in the incoming meta data
    void        checkOverrideRunNumber();

    /// override IOV with new run number
    void        overrideIOV(CondAttrListCollection*& coll) const;

    /// print out of CondAttrListCollection
    void        printOut(const CondAttrListCollection* coll) const;

    /// Modify a Payload for a particular folder - replaces one of the
    /// internal attributes
    StatusCode  modifyPayload (const std::string& folderName, 
                               CondAttrListCollection*& payload) const;

    /// Handles to the meta data stores
    typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
    StoreGateSvc_t   m_metaDataStore;
    StoreGateSvc_t   m_inputStore;

    // Has the FirstInputFileIncidence fired? Used to skip the first
    // BeginFileIncidence
    bool             m_processedFirstInputFileIncidence;

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

};

    

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // IOVDBMETADATATOOLS_IOVDBMETADATATOOL_H
