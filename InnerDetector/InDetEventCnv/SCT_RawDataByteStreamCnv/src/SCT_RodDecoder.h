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

#include "InDetByteStreamErrors/InDetBSErrContainer.h"
#include "SCT_ConditionsData/SCT_ByteStreamErrors.h"
#include "SCT_Cabling/ISCT_CablingTool.h"
#include "SCT_ConditionsTools/ISCT_ConfigurationConditionsTool.h"
#include "Identifier/IdContext.h"

#include "GaudiKernel/ToolHandle.h"

#include <atomic>
#include <cstdint>
#include <string>
#include <vector>

class IdentifierHash;
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
                                    ISCT_RDO_Container& rdoIDCont,
                                    InDetBSErrContainer* errs,
                                    SCT_ByteStreamFractionContainer* bsFracCont,
                                    const std::vector<IdentifierHash>* vecHash = nullptr) const override;

 private:
  /// method that builds the RawData RDO and add it to the collection 
  /// rdoIdc and cache are updated based on other arguments
  int makeRDO(int strip, int groupSize, int timeBin,
              uint32_t onlineID, int errors,
              ISCT_RDO_Container& rdoIDCont,
              CacheHelper& cache,
              const std::vector<int>& errorHit) const;

  /// add an error for each wafer in a problematic ROD.
  void addRODError(uint32_t rodID, int errorType,
                   InDetBSErrContainer* errs) const;
  bool addSingleError(const IdentifierHash& idHash,
                      int bsErrorType,
                      InDetBSErrContainer* errs) const;

  /** Set first temporarily masked chip information from byte stream trailer */
  void setFirstTempMaskedChip(const IdentifierHash& hashID, 
                              unsigned int firstTempMaskedChip, 
                              InDetBSErrContainer* errs) const;

  const SCT_ID* m_sctID;
  IdContext m_contextSCT;
  ToolHandle<ISCT_CablingTool> m_cabling{this, 
                                         "SCT_CablingTool", 
                                         "SCT_CablingTool", 
                                         "Tool to retrieve SCT Cabling"};
  ToolHandle<ISCT_ConfigurationConditionsTool> m_configTool{this, 
                                                            "ConfigTool",
                                                            "SCT_ConfigurationConditionsTool/InDetSCT_ConfigurationConditionsTool", 
                                                            "Tool to retrieve SCT Configuration Tool"};
  /** Summary of the decoding process */
  mutable std::atomic_uint m_singleCondHitNumber;   //!< Total number of single hit decoded in condensed mode
  mutable std::atomic_uint m_pairedCondHitNumber;   //!< Total number of paired hit decoded in condensed mode
  mutable std::atomic_uint m_firstExpHitNumber;     //!< Total number of first hit decoded in expanded mode
  mutable std::atomic_uint m_evenExpHitNumber;      //!< Total number of paired hit decoded in expanded mode
  mutable std::atomic_uint m_lastExpHitNumber;      //!< Total number of last hit decoded in expanded mode
  mutable std::atomic_uint m_headNumber;            //!< Total number of decoded header data
  mutable std::atomic_uint m_trailerNumber;         //!< Total number of decoded trailer data
  mutable std::atomic_uint m_headErrorBCID;         //!< Total number of bcid error in the header data
  mutable std::atomic_uint m_headErrorLvl1ID;       //!< Total number of lvl1id error in the header data
  mutable std::atomic_uint m_headErrorTimeout;      //!< Total number of timeout error in the header data
  mutable std::atomic_uint m_headErrorFormatter;    //!< Total number of formatter error in the header data
  mutable std::atomic_uint m_headErrorPreamble;     //!< Total number of preamble error in the header data
  mutable std::atomic_uint m_trailerErrorOverflow;  //!< Total number of overflow error in the trailer data
  mutable std::atomic_uint m_trailerErrorLimit;     //!< Total number of header trailer limit error in the trailer data
  mutable std::atomic_uint m_trailerErrorBit;       //!< Total number of trailer bit error
  mutable std::atomic_uint m_configDataBit;         //!< Total number of configuration data
  mutable std::atomic_uint m_flagErrorBit;          //!< Total number of flag error data
  mutable std::atomic_uint m_condHit1Error;         //!< Total number of first hit data error
  mutable std::atomic_uint m_condHit2Error;         //!< Total number second hit data errors
  mutable std::atomic_uint m_chipNumberError;       //!< Total number of chip number error
  mutable std::atomic_uint m_unknownDataFormat;     //!< Total number of unknown data format
  mutable std::atomic_uint m_nHits;
  mutable std::atomic_uint m_nRDOs;
  mutable std::atomic_uint m_maskedLinkNumber;
  mutable std::atomic_uint m_maskedRODNumber;
  mutable std::atomic_uint m_rodClockErrorNumber;
  mutable std::atomic_uint m_truncatedRODNumber;
  mutable std::atomic_uint m_numMissingLinkHeader;
  mutable std::atomic_uint m_numUnknownOfflineID;
  
  std::vector<bool> m_swapPhiReadoutDirection;
};

#endif //SCT_RAWDATABYTESTREAM_SCT_RODDECODER_H
