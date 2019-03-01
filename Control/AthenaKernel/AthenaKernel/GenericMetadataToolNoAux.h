/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERICMETADATATOOLNOAUX_H
#define GENERICMETADATATOOLNOAUX_H

/** @file GenericMetadataToolNoAux.h
 *  @brief This file contains the class definition for the GenericMetadataToolNoAux class.
 *  @author Jack Cranshaw <cranshaw@anl.gov>
 *  $Id: $
 **/

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IMetaDataTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"

#include <string>

/** @class GenericMetadataToolNoAux
 *  @brief This class provides the basic functionality for processing metadata in the athena framework. The derived class must simply implement merge functionality in the updateContainer method.
 **/

template <typename T>
class GenericMetadataToolNoAux : public AthAlgTool, public virtual ::IMetaDataTool
{
public: // Constructor and Destructor
   /// Standard Service Constructor
   GenericMetadataToolNoAux(const std::string& type, 
                  const std::string& name,
                  const IInterface*  parent);
   /// Destructor
   virtual ~GenericMetadataToolNoAux();

public:
   virtual StatusCode metaDataStop() override;
   virtual StatusCode beginInputFile(const SG::SourceID& sid = "Serial") override;
   virtual StatusCode endInputFile(const SG::SourceID& sid = "Serial") override;
   virtual StatusCode initialize() override;
   virtual StatusCode finalize() override;

protected:
   ServiceHandle<StoreGateSvc> inputMetaStore() const;
   ServiceHandle<StoreGateSvc> outputMetaStore() const;

   /// Helper class to update a container with information from another one
   virtual StatusCode updateContainer( T* contToUpdate,
                                 const T* otherCont ) = 0;

   StatusCode initOutputContainer(const std::string& sgkey);

   StatusCode buildAthenaInterface(const std::string& inputName,
                                   const std::string& outputName,
                                   const SG::SourceID& sid);

   /// Fill Cutflow information
   StatusCode addProcessMetadata();
 
   /// Pointer to cut flow svc 
   ServiceHandle<StoreGateSvc> m_inputMetaStore;
   ServiceHandle<StoreGateSvc> m_outputMetaStore;

   /// The name of the output Container
   std::string m_outputCollName;
   /// The name of the input Container
   std::string  m_inputCollName;
   /// The name of the process Container
   std::string m_procMetaName;

   bool m_processMetadataTaken;
   bool m_markIncomplete;

   /// List of source ids which have reached end file
   std::set<SG::SourceID> m_fullreads;
   std::set<SG::SourceID> m_read;
   std::set<SG::SourceID> m_written;

};

template <typename T>
inline ServiceHandle<StoreGateSvc> GenericMetadataToolNoAux<T>::inputMetaStore() const
{
   return m_inputMetaStore;
}

template <typename T>
inline ServiceHandle<StoreGateSvc> GenericMetadataToolNoAux<T>::outputMetaStore() const
{
   return m_outputMetaStore;
}

#include "GenericMetadataToolNoAux.icc"
#endif

