/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RAWDATABYTESTREAMCNV_SCTRAWCONTBYTESTREAMTOOL_H
#define SCT_RAWDATABYTESTREAMCNV_SCTRAWCONTBYTESTREAMTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_RawDataByteStreamCnv/ISCTRawContByteStreamTool.h"

#include "ByteStreamCnvSvcBase/FullEventAssembler.h"

#include "GaudiKernel/ToolHandle.h"

#include <mutex>

class ISCT_RodEncoder;
class ISCT_CablingTool;
class SCT_ID;
class SrcIdMap;

/** 
 * @class SCTRawContByteStreamTool
 * 
 * @brief Athena Algorithm Tool to provide conversion from SCT RDO container to ByteStream.
 *
 * Conversion from SCT RDO container to ByteStream, and fill it in RawEvent.
 *
 * The class inherits from AthAlgTool and ISCTRawContByteStreamTool.
 *
 * Contains convert method that maps ROD ID's to vectors of RDOs in those RODs, then
 * loops through the map, using RodEncoder to fill data for each ROD in turn.
 */
class SCTRawContByteStreamTool : public extends<AthAlgTool, ISCTRawContByteStreamTool> 
{
 public:

  /** Constructor */
  SCTRawContByteStreamTool(const std::string& type, const std::string& name, const IInterface* parent);
  
  /** Destructor */
  virtual ~SCTRawContByteStreamTool() = default;

  /** Initialize */
  virtual StatusCode initialize() override;

  /** Execute */
  virtual StatusCode finalize() override;

  /** 
   * @brief Main Convert method 
   *
   * Maps ROD ID's to vectors of RDOs in those RODs, then loops through the map, 
   * using RodEncoder to fill data for each ROD in turn.
   *
   * @param sctRDOCont SCT RDO Container of Raw Data Collections.
   * @param rawEvtWrite Data type for writing raw event.
   * @param log Object used to transmit messages and log errors.
   * */
  virtual StatusCode convert(const SCT_RDO_Container* sctRDOCont, 
                             RawEventWrite* rawEvtWrite, MsgStream& log) const override;
  
 private: 

  /** Algorithm Tool to decode ROB bytestream data into RDO. */ 
  ToolHandle<ISCT_RodEncoder> m_encoder{this, "Encoder", "SCT_RodEncoder", "SCT ROD Encoder for RDO to BS conversion"};

  /** Providing mappings of online and offline identifiers and also serial numbers. */ 
  ToolHandle<ISCT_CablingTool> m_cabling{this, "SCT_CablingTool", "SCT_CablingTool", "Tool to retrieve SCT Cabling"};

  /** Identifier helper class for the SCT subdetector that creates compact Identifier objects and 
      IdentifierHash or hash IDs. Also allows decoding of these IDs. */ 
  const SCT_ID* m_sctIDHelper;

  unsigned short m_rodBlockVersion;

  /** Conversion between Lower level Source ID to higher level source ID, used to assemble
      fragments from ROD fragments to assemble full ATLAS raw events. */ 
  mutable FullEventAssembler<SrcIdMap> m_fullEventAssembler ATLAS_THREAD_SAFE;

  mutable std::mutex m_mutex;
};

#endif // SCT_RAWDATABYTESTREAMCNV_SCTRAWCONTBYTESTREAMTOOL_H
