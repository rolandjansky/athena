/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELRAWDATABYTESTREAMCNV_PIXELRAWDATAPROVIDERTOOL_H
#define PIXELRAWDATABYTESTREAMCNV_PIXELRAWDATAPROVIDERTOOL_H

#include "PixelRawDataByteStreamCnv/IPixelRawDataProviderTool.h"
#include "PixelRawDataByteStreamCnv/IPixelRodDecoder.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/SlotSpecificObj.h"
#include "StoreGate/WriteHandleKey.h"
#include "InDetRawData/InDetTimeCollection.h"

#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ToolHandle.h"

#include <atomic>

// the tool to decode a ROB frament
class PixelRawDataProviderTool : virtual public IPixelRawDataProviderTool, public AthAlgTool
{

 public:
   
  //! constructor
  PixelRawDataProviderTool( const std::string& type, const std::string& name,
			    const IInterface* parent ) ;

  //! destructor 
  ~PixelRawDataProviderTool() ;

  //! initialize
  StatusCode initialize() override;

  //! finalize
  StatusCode finalize() override;
  
  //! this is the main decoding method
  StatusCode convert( std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs,
		      IPixelRDO_Container* rdoIdc,
		      IDCInDetBSErrContainer& decodingErrors) const final;

  //! Size of IDCInDetBSErrContainer
  int SizeOfIDCInDetBSErrContainer() const final;

private: 
  
  ToolHandle<IPixelRodDecoder>  m_decoder
  {this, "Decoder", "PixelRodDecoder", "Tool for PixelRodDecoder"};


  SG::WriteHandleKey<InDetTimeCollection> m_LVL1CollectionKey{this, "LVL1CollectionName", "PixelLVL1ID"};
  SG::WriteHandleKey<InDetTimeCollection> m_BCIDCollectionKey{this, "BCIDCollectionName", "PixelBCID"};

  mutable std::atomic_int m_DecodeErrCount;
  bool m_checkLVL1ID;

  mutable std::mutex m_mutex;
  struct CacheEntry {
    EventContext::ContextEvt_t m_evt{EventContext::INVALID_CONTEXT_EVT};
    uint32_t m_LastLvl1ID{0xffffffff};
  };
  mutable SG::SlotSpecificObj<CacheEntry> m_cache ATLAS_THREAD_SAFE; // Guarded by m_mutex

};

#endif
