/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERICMETADATATOOL_H
#define GENERICMETADATATOOL_H

/** @file GenericMetadataTool.h
 *  @brief This file contains the class definition for the GenericMetadataTool class.
 *  @author Jack Cranshaw <cranshaw@anl.gov>
 *  $Id: $
 **/

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IMetaDataTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"

#include <string>

/** @class GenericMetadataTool
 *  @brief This class provides the basic functionality for processing metadata in the athena framework. The derived class must simply implement merge functionality in the updateContainer method.
 **/

template <typename T, typename U>
class GenericMetadataTool : public AthAlgTool, public virtual ::IMetaDataTool
{
public: // Constructor and Destructor
   /// Standard Service Constructor
   GenericMetadataTool(const std::string& type, 
                  const std::string& name,
                  const IInterface*  parent);
   /// Destructor
   virtual ~GenericMetadataTool();

public:
   /// IMetaDataTool interface method
   virtual StatusCode metaDataStop() override;
   /// IMetaDataTool interface method
   virtual StatusCode beginInputFile(const SG::SourceID& sid = "Serial") override;
   /// IMetaDataTool interface method
   virtual StatusCode endInputFile(const SG::SourceID& sid = "Serial") override;

   /// AthAlgTool methods
   virtual StatusCode initialize() override;
   virtual StatusCode finalize() override;

protected:
   /// Method to retrieve handle to input metadata store
   //  Note that these tools operate outside an event context
   ServiceHandle<StoreGateSvc> inputMetaStore() const;
   /// Method to retrieve handle to output metadata store
   ServiceHandle<StoreGateSvc> outputMetaStore() const;

   // Utility methods to implement the general logic of a metadata tool
   /// Helper class to update a container with information from another one
   //  i.e. this is the 'merge' method
   virtual StatusCode updateContainer( T* contToUpdate,
                                 const T* otherCont ) = 0;

   // Utility methods to implement the general logic of a metadata tool
   // 
   // Initialize container in output store, link aux to interface
   StatusCode initOutputContainer(const std::string& sgkey);
   /// Put input metadata in a MetaCont[sid]
   //  so that it can be accessed by event context aware handles
   StatusCode buildAthenaInterface(const std::string& inputName,
                                   const std::string& outputName,
                                   const SG::SourceID& sid);

   /// Add any information which has been collected during processing
   //  Metadata to be added must be placed in the output metadata store 
   //     and the appropriate key declared to the tool. Otherwise the 
   //     additional metadata can be created however needed. 
   StatusCode addProcessMetadata();
 
   /// Cached handles to the metadata stores
   ServiceHandle<StoreGateSvc> m_inputMetaStore;
   ServiceHandle<StoreGateSvc> m_outputMetaStore;

   /// The name of the output Container
   std::string m_outputCollName;
   /// The name of the input Container
   std::string  m_inputCollName;
   /// The name of the process Container
   std::string m_procMetaName;

   /// Flag for internal use to avoid writing process metadata twice
   bool m_processMetadataTaken;
   /// Flag settable by property on whether to do Filekeeping
   bool m_markIncomplete;

   //  List of source ids for doing garbage collection 
   //
   /// List of source ids which have reached end file
   std::set<SG::SourceID> m_fullreads;
   /// List of source ids which have been opened
   std::set<SG::SourceID> m_read;
   /// List of source ids which have been written
   std::set<SG::SourceID> m_written;

};

template <typename T, typename U>
inline ServiceHandle<StoreGateSvc> GenericMetadataTool<T,U>::inputMetaStore() const
{
   return m_inputMetaStore;
}

template <typename T, typename U>
inline ServiceHandle<StoreGateSvc> GenericMetadataTool<T,U>::outputMetaStore() const
{
   return m_outputMetaStore;
}

#include "GenericMetadataTool.icc"
#endif

