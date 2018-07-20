/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGOUTPUTHANDLING_HLTResultCreatorCreatorByteStream_H
#define TRIGOUTPUTHANDLING_HLTResultCreatorCreatorByteStream_H

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// OutputHandling includes
#include "TrigOutputHandling/IHLTOutputTool.h"
#include "TrigSteeringEvent/HLTResult.h"
#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/IAthenaSerializeSvc.h"
#include "AthenaKernel/IDictLoaderSvc.h"

/**
 * @class $(klass)s
 * @brief 
 **/

class HLTResultCreatorByteStream: public extends<AthAlgTool, IHLTOutputTool>
{ 
  
 public: 

  HLTResultCreatorByteStream( const std::string& type,
	     const std::string& name, 
	     const IInterface* parent );

  virtual ~HLTResultCreatorByteStream(); 
  virtual StatusCode createOutput(const EventContext& context) const override;

  virtual StatusCode  initialize() override;
  virtual StatusCode  finalize() override;

 private: 
  Gaudi::Property<std::vector<std::string>> m_collectionsToSerialize{this, "CollectionsToSerialize", {}, "TYPE#SG key of collections to be streamed (like in StreamAOD)" };

  SG::WriteHandleKey<HLT::HLTResult> m_hltResultKey{this, "HLTResult", "HLTResult", "Name of the HLTResult"};
  
  // internal structure to keep configuration organised conveniently
  struct Address {
    std::string type;
    CLID clid;
    std::string key;    
  };
  
  std::vector< Address > m_toSerialize; // postprocessed configuration info
  
  ServiceHandle<IClassIDSvc> m_clidSvc{ this, "ClassIDSvc", "ClassIDSvc", "Service to translate class name to CLID" };
  ServiceHandle<IAthenaSerializeSvc> m_serializerSvc{ this, "Serializer", "AthenaRootSerializeSvc", "Service that translates transient to persistent respresenation" };
  ServiceHandle<IDictLoaderSvc> m_dictLoaderSvc{ this, "DictLoaderSvc", "AthDictLoaderSvc", "Service managing ROOT dictionaries for EDM classes" };

  /**
   * Given the ID if the collection (in address arg) insert basic streaming info into the buffer.
   */
  void makeHeader( const HLTResultCreatorByteStream::Address& address, std::vector<uint32_t>& buffer  ) const;

  /**
   * For copy bytest from the memory into the buffer converting from char[] to uint32_t[]
   * This function is candidate to be made global function at some point
   * and we will need also readPayload function
   */  
  void fillPayload( void* data, size_t sz, std::vector<uint32_t>& buffer ) const ;

  
}; 


#endif //> !TRIGOUTPUTHANDLING_HLTResultCreatorCreatorByteStream_H
