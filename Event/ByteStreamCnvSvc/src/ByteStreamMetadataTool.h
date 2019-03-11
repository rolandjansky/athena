/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMMETADATATOOL_H
#define BYTESTREAMMETADATATOOL_H

/** @file ByteStreamMetadataTool.h
 *  @brief This class is an implementation of the GenericMetadataToolNoAux
 *  for the ByteStreamMetadataContainer.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: $
 **/

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/SourceID.h"
#include "AthenaKernel/GenericMetadataToolNoAux.h"
#include "ByteStreamData/ByteStreamMetadataContainer.h"

#include <string>

class StoreGateSvc;

/** @class ByteStreamMetadataTool
 *  @brief This class provides the MetaDataTool for ByteStreamMetadata objects
 **/
class ByteStreamMetadataTool : public GenericMetadataToolNoAux <ByteStreamMetadataContainer> 
{
public: 
   /// Standard Service Constructor
   ByteStreamMetadataTool(const std::string& type, const std::string& name, const IInterface* parent);
   /// Destructor
   virtual ~ByteStreamMetadataTool();

   /// Gaudi Service Interface method implementations:
   StatusCode initialize() override;
   StatusCode finalize() override;
   /// Virtual method to update a container with information from another one
   virtual StatusCode updateContainer(ByteStreamMetadataContainer* bsmdc_out,
                                const ByteStreamMetadataContainer* bsmdc_in );
};

#endif
