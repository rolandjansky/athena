// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RAWDATABYTESTREAMCNV_SCTRAWDATAPROVIDERTOOL_H
#define SCT_RAWDATABYTESTREAMCNV_SCTRAWDATAPROVIDERTOOL_H

#include "SCT_RawDataByteStreamCnv/ISCTRawDataProviderTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ToolHandle.h"

#include <atomic>


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
   * @brief Main decoding method.
   *
   * Loops over ROB fragments, get ROB/ROD ID, then decode if not allready decoded.
   *
   * @param vecROBFrags Vector containing ROB framgents.
   * @param rdoIDCont RDO ID Container to be filled.
   * @param errs Byte stream error container.
   * @param ctx EventContext of the event
   *  */
  virtual StatusCode convert(std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecROBFrags,
                             SCT_RDO_Container& rdoIDCont,
                             IDCInDetBSErrContainer& errs,
                             const EventContext& ctx) const override;

 private: 

  /** Algorithm Tool to decode ROD byte stream into RDO. */
  ToolHandle<ISCT_RodDecoder> m_decoder{this, "Decoder", "SCT_RodDecoder", "Decoder"};

  /** Number of decode errors encountered in decoding. 
      Turning off error message after 100 errors are counted */
  mutable std::atomic_int m_decodeErrCount{0};

};

#endif // SCT_RAWDATABYTESTREAMCNV_SCTRAWDATAPROVIDERTOOL_H
