/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_RAWDATABYTESTREAMCNV_TRTRAWDATAPROVIDERTOOL_H
#define TRT_RAWDATABYTESTREAMCNV_TRTRAWDATAPROVIDERTOOL_H

#include "TRT_RawDataByteStreamCnv/ITRTRawDataProviderTool.h"

#include "AthenaKernel/SlotSpecificObj.h"
#include "ByteStreamData/RawEvent.h" 
#include "InDetRawData/InDetTimeCollection.h"
#include "TRT_RawDataByteStreamCnv/ITRT_RodDecoder.h"
#include "StoreGate/WriteHandleKey.h"

#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include <set>
#include <string>

// the tool to decode a ROB frament

class TRT_BSErrorContainer;

class TRTRawDataProviderTool : virtual public ITRTRawDataProviderTool, 
                                public AthAlgTool
{

 public:
   
  //! AlgTool InterfaceID
  static const InterfaceID& interfaceID( ) ;
  
  //! constructor
  TRTRawDataProviderTool( const std::string& type, const std::string& name,
			  const IInterface* parent ) ;

  //! destructor 
  virtual ~TRTRawDataProviderTool() ;

  //! initialize
  virtual StatusCode initialize() override;

  //! finalize
  virtual StatusCode finalize() override;
  
  //! this is the main decoding method
  virtual StatusCode convert(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs,
			     TRT_RDO_Container* rdoIdc,
			     TRT_BSErrContainer* bsErrCont
			     ) override;

private: 
  TRTRawDataProviderTool( ); //Not implemented
  
  ToolHandle<ITRT_RodDecoder>  m_decoder;   

  // bookkeeping if we have decoded a ROB already
  mutable std::mutex m_mutex;
  struct CacheEntry {
    EventContext::ContextEvt_t m_evt{EventContext::INVALID_CONTEXT_EVT};
    uint32_t m_LastLvl1ID{0xffffffff};
  };
  mutable SG::SlotSpecificObj<CacheEntry> m_cache ATLAS_THREAD_SAFE; // Guarded by m_mutex
  

  SG::WriteHandleKey<InDetTimeCollection> m_lvl1idkey{this,"LVL1IDKey","TRT_LVL1ID","TRT_LVL1ID out-key"};
  SG::WriteHandleKey<InDetTimeCollection> m_bcidkey{this,"BCIDKey","TRT_BCID","TRT_BCID out-key"};
  bool  m_storeInDetTimeColls;
  bool  m_doEventCheck;
};

#endif



