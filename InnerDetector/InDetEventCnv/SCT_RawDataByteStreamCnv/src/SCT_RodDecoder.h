/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 *      @file header for SCT_RodDecoder Class
 *      AlgTool class to decode ROB bytestream data into RDO
 *      @author: Kondo.Gnanvo@cern.ch, Maria.Jose.Costa@cern.ch
 *      @current developer: Kondo Gnanvo, QMUL (London), august 2005
 */

#ifndef INDETRAWDATABYTESTREAM_SCT_RODDECODER_H 
#define INDETRAWDATABYTESTREAM_SCT_RODDECODER_H

#include "SCT_RawDataByteStreamCnv/ISCT_RodDecoder.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "Identifier/IdContext.h"
#include "Identifier/IdentifierHash.h"
#include "InDetByteStreamErrors/InDetBSErrContainer.h"
#include "SCT_Cabling/ISCT_CablingTool.h"
#include "SCT_ConditionsData/SCT_ByteStreamErrors.h"
#include "SCT_ConditionsTools/ISCT_ConfigurationConditionsTool.h"

#include "GaudiKernel/ToolHandle.h"

//STL
#include <atomic>
#include <cstdint>
#include <string>
#include <vector>

class SCT_ID;

//using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;
/** @class SCT_RodDecoder
 *  @brief AlgTool which decodes the SCT binary format to create RDOs
 *  also inserts them to the collection.
 *  This tool is used by the SCTRawCollByteStreamTool
 **/
class SCT_RodDecoder : public extends<AthAlgTool, ISCT_RodDecoder>
{

  struct CacheHelper{//temp object to help with trigger caching
    IdentifierHash skipHash, lastHash;
    const std::vector<IdentifierHash>* vecHash;
  };

 public: 
  //@name Usual AlgTool methods
  //@{
  /** constructor*/
  SCT_RodDecoder(const std::string& type, const std::string& name, const IInterface* parent);
  
  /** destructor  */
  virtual ~SCT_RodDecoder() = default;

  /** AlgTool initialize */
  virtual StatusCode initialize() override;

  /** AlgTool finalize*/
  virtual StatusCode finalize() override;
  
  /** @brief Decode the rob data fragment and fill the collection SCT_RDO_Collection 
   *  with the RDO built by the makeRDO(..) method
   * rdoIdc, errs, and bsFracCont are updated based on robFrag and vecHash.
   **/
  virtual StatusCode fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag,
                                    ISCT_RDO_Container& rdoIdc,
                                    InDetBSErrContainer* errs,
                                    SCT_ByteStreamFractionContainer* bsFracCont,
                                    const std::vector<IdentifierHash>* vecHash = nullptr) const override;

 private:
  /// method that builds the RawData RDO and add it to the collection 
  /// rdoIdc and cache are updated based on other arguments
  int makeRDO(int strip, int groupSize, int tbin,
              uint32_t onlineId, int ERRORS,
              ISCT_RDO_Container& rdoIdc,
              CacheHelper& cache,
              const std::vector<int>& errorHit) const;

  /// add an error for each wafer in a problematic ROD.
  void addRODError(uint32_t rodid, int errorType,
                   InDetBSErrContainer* errs) const;
  bool addSingleError(const IdentifierHash& idHash,
                      int bsErrorType,
                      InDetBSErrContainer* errs) const;

  /** Set first temporarily masked chip information from byte stream trailer */
  void setFirstTempMaskedChip(const IdentifierHash& hashId, unsigned int firstTempMaskedChip, InDetBSErrContainer* errs) const;

  const SCT_ID* m_sct_id;
  IdContext m_cntx_sct;
  ToolHandle<ISCT_CablingTool> m_cabling{this, "SCT_CablingTool", "SCT_CablingTool", "Tool to retrieve SCT Cabling"};
  ToolHandle<ISCT_ConfigurationConditionsTool> m_configTool{this, "ConfigTool",
      "SCT_ConfigurationConditionsTool/InDetSCT_ConfigurationConditionsTool", "Tool to retrieve SCT Configuration Tool"};
  /** Summary of the decoding process */
  mutable std::atomic_uint m_singleCondHitNumber;   //!< Total number of single hit decoded in condensed mode
  mutable std::atomic_uint m_pairedCondHitNumber;   //!< Total number of paired hit decoded in condensed mode
  mutable std::atomic_uint m_firstExpHitNumber;     //!< Total number of first hit decoded in expanded mode
  mutable std::atomic_uint m_evenExpHitNumber;      //!< Total number of paired hit decoded in expanded mode
  mutable std::atomic_uint m_lastExpHitNumber;      //!< Total number of last hit decoded in expanded mode
  mutable std::atomic_uint m_headnumber;            //!< Total number of decoded header data
  mutable std::atomic_uint m_trailnumber;           //!< Total number of decoded trailer data
  mutable std::atomic_uint m_head_error_bcid;       //!< Total number of bcid error in the header data
  mutable std::atomic_uint m_head_error_lvl1id;     //!< Total number of lvl1id error in the header data
  mutable std::atomic_uint m_head_error_timeout;    //!< Total number of timeout error in the header data
  mutable std::atomic_uint m_head_error_formatter;  //!< Total number of formatter error in the header data
  mutable std::atomic_uint m_head_error_preamb;     //!< Total number of preamble error in the header data
  mutable std::atomic_uint m_trail_error_overflow;  //!< Total number of overflow error in the trailer data
  mutable std::atomic_uint m_trail_error_limit;     //!< Total number of header trailer limit error in the trailer data
  mutable std::atomic_uint m_trail_error_bit;       //!< Total number of trailer bit error
  mutable std::atomic_uint m_config_data_bit;       //!< Total number of configuration data
  mutable std::atomic_uint m_flag_error_bit;        //!< Total number of flag error data
  mutable std::atomic_uint m_cond_hit1_error;       //!< Total number of first hit data error
  mutable std::atomic_uint m_cond_hit2_error;       //!< Total number second hit data errors
  mutable std::atomic_uint m_chip_number_error;     //!< Total number of chip number error
  mutable std::atomic_uint m_unknown_data_format;   //!< Total number of unknown data format
  mutable std::atomic_uint m_nHits;
  mutable std::atomic_uint m_nRDOs;
  mutable std::atomic_uint m_maskedLinkNumber;
  mutable std::atomic_uint m_maskedRODNumber;
  mutable std::atomic_uint m_RODClockErrorNumber;
  mutable std::atomic_uint m_truncatedRODNumber;
  mutable std::atomic_uint m_numMissingLinkHeader;
  mutable std::atomic_uint m_numUnknownOfflineId;
  
  std::vector<bool> m_swapPhiReadoutDirection;
};

#endif //SCT_RAWDATABYTESTREAM_SCT_RODDECODER_H
