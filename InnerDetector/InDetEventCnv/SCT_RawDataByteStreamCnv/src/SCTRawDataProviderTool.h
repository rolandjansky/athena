// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RAWDATABYTESTREAMCNV_SCTRAWDATAPROVIDERTOOL_H
#define SCT_RAWDATABYTESTREAMCNV_SCTRAWDATAPROVIDERTOOL_H

#include "SCT_RawDataByteStreamCnv/ISCTRawDataProviderTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "AthenaKernel/SlotSpecificObj.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "ByteStreamData/RawEvent.h"

#include "GaudiKernel/ToolHandle.h"

#include <atomic>
#include <mutex>
#include <unordered_set>

class ISCT_RodDecoder;

/**
 * @class SCTRawDataProviderTool
 *
 * @brief Athena Algorithm Tool to fill Collections of SCT RDO Containers.
 *
 * The class inherits from AthAlgTool and ISCTRawDataProviderTool.
 *
 * Contains a convert method that fills the SCT RDO Collection.
 */
class SCTRawDataProviderTool : public extends<AthAlgTool, ISCTRawDataProviderTool>
{
 public:
   
  /** Constructor */
  SCTRawDataProviderTool(const std::string& type, const std::string& name, const IInterface* parent);

  /** Destructor */
  virtual ~SCTRawDataProviderTool() = default;

  /** Initialize */
  virtual StatusCode initialize() override;

  /**
   * @brief Old-style decoding method.
   *
   * Retrieve EventContext and call the convert method with EventContext
   *
   * @param vecROBFrags Vector containing ROB framgents.
   * @param rdoIDCont RDO ID Container to be filled.
   * @param errs Byte stream error container.
   * @param bsFracCont Byte stream fraction container.
   *  */
  virtual StatusCode convert(std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecROBFrags,
                             ISCT_RDO_Container& rdoIDCont,
                             InDetBSErrContainer* errs,
                             SCT_ByteStreamFractionContainer* bsFracCont) const override;
  /**
   * @brief Main decoding method.
   *
   * Loops over ROB fragments, get ROB/ROD ID, then decode if not allready decoded.
   *
   * @param vecROBFrags Vector containing ROB framgents.
   * @param rdoIDCont RDO ID Container to be filled.
   * @param errs Byte stream error container.
   * @param bsFracCont Byte stream fraction container.
   * @param ctx EventContext of the event
   *  */
  virtual StatusCode convert(std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecROBFrags,
                             ISCT_RDO_Container& rdoIDCont,
                             InDetBSErrContainer* errs,
                             SCT_ByteStreamFractionContainer* bsFracCont,
                             const EventContext& ctx) const override;

 private: 

  /** Algorithm Tool to decode ROD byte stream into RDO. */
  ToolHandle<ISCT_RodDecoder> m_decoder{this, "Decoder", "SCT_RodDecoder", "Decoder"};
  
  /** For bookkeeping of decoded ROBs */
  struct CacheEntry {
    EventContext::ContextEvt_t m_evt{EventContext::INVALID_CONTEXT_EVT};
    std::unordered_set<uint32_t> m_robIDSet;
  };
  mutable SG::SlotSpecificObj<CacheEntry> m_cache ATLAS_THREAD_SAFE; // Guarded by m_mutex

  /** Number of decode errors encountered in decoding. 
      Turning off error message after 100 errors are counted */
  mutable std::atomic_int m_decodeErrCount{0};

  mutable std::mutex m_mutex{};
};

#endif // SCT_RAWDATABYTESTREAMCNV_SCTRAWDATAPROVIDERTOOL_H
