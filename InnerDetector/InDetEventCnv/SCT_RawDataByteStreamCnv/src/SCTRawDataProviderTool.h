/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCTRawDataProviderTool.h
//   Header file for class SCTRawDataProviderTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef SCT_RAWDATABYTESTREAMCNV_SCTRAWDATAPROVIDERTOOL_H
#define SCT_RAWDATABYTESTREAMCNV_SCTRAWDATAPROVIDERTOOL_H

#include "SCT_RawDataByteStreamCnv/ISCTRawDataProviderTool.h"

#include "SCT_RawDataByteStreamCnv/ISCT_RodDecoder.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "ByteStreamData/RawEvent.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ToolHandle.h"

#include <set>

/** @class SCTRawDataProviderTool
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
  SCTRawDataProviderTool(const std::string& type, const std::string& name,
                         const IInterface* parent);

  /** Destructor */
  virtual ~SCTRawDataProviderTool() = default;

  /** Initialize */
  virtual StatusCode initialize() override;

  // finalize is empty, unnecessary to override
  
  /** @brief Main decoding method.
   *
   * Loops over ROB fragments, get ROB/ROD ID, then decode if not allready decoded.
   *
   * @param vecRobs Vector containing ROB framgents.
   * @param rdoIdc RDO ID Container to be filled.
   * @param errs Byte stream error container.
   * @param bsFracCont Byte stream fraction container.
   *  */
  virtual StatusCode convert(std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs,
                             ISCT_RDO_Container& rdoIdc,
                             InDetBSErrContainer* errs,
                             SCT_ByteStreamFractionContainer* bsFracCont) override;

  /** Reset list of known ROB IDs */
  virtual void beginNewEvent() override;

 private: 

  /** Algorithm Tool to decode ROD byte stream into RDO. */
  ToolHandle<ISCT_RodDecoder> m_decoder{this, "Decoder", "SCT_RodDecoder", "Decoder"};
  
  /** For bookkeeping of decoded ROBs */
  std::set<uint32_t> m_robIdSet;

  /** Number of decode errors encountered in decoding. 
   * Turning off error message after 100 errors are counted */
  int m_decodeErrCount;
};

#endif // SCT_RAWDATABYTESTREAMCNV_SCTRAWDATAPROVIDERTOOL_H
