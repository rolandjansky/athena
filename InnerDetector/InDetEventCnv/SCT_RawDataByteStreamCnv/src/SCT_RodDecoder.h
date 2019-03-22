/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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

/** 
 * @class SCT_RodDecoder
 *
 * @brief Athena Algorithm Tool to decode the SCT binary format to create RDOs and inserts them to the collection.
 *
 * This tool is used by the SCTRawCollByteStreamTool.
 */
class SCT_RodDecoder : public extends<AthAlgTool, ISCT_RodDecoder>
{
  /** Temp object to help with trigger caching. */
  struct CacheHelper{
    IdentifierHash skipHash, lastHash;
    const std::vector<IdentifierHash>* vecHash;
  };

 public: 

  /** Constructor */
  SCT_RodDecoder(const std::string& type, const std::string& name, const IInterface* parent);
  
  /** Destructor */
  virtual ~SCT_RodDecoder() = default;

  /** Initialize */
  virtual StatusCode initialize() override;

  /** Finalize */
  virtual StatusCode finalize() override;
 
  /** 
   * @brief Fill SCT RDO Collection with decoded ROB data
   *
   * Decode the rob data fragment and fill the collection SCT_RDO_Collection with the RDO built by the makeRDO(..) method.
   * rdoIdc, errs, and bsFracCont are updated based on robFrag and vecHash.
   *
   * @param robFrag ROB fragment.
   * @param rdoIDCont RDO ID Container to be filled.
   * @param errs Byte stream error container.
   * @param bsFracCont Byte stream fraction container.
   * @param vecHash Vector of hashes.
   */
  virtual StatusCode fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag,
                                    ISCT_RDO_Container& rdoIDCont,
                                    InDetBSErrContainer* errs,
                                    SCT_ByteStreamFractionContainer* bsFracCont,
                                    const std::vector<IdentifierHash>* vecHash = nullptr) const override;

 private:

  /**
   * @brief Builds RawData RDO and adds to RDO container
   *
   * Method that builds the RawData RDO and add it to the collection rdoIdc and cache are 
   * updated based on other arguments. 
   * Method has 3 possible return values:
   *   1 if RDO was successfully created
   *   0 if collection was deliberately skipped (for trigger)
   *  -1 if there was an error in the decoding - will be passed on as StatusCode::RECOVERABLE by fillCollection()
   *
   * @param strip Strip number info from the RDO.
   * @param groupSize Group size info from the RDO.
   * @param timeBin Time bin info for RDO.
   * @param onlineID Online Identifier from the RDO.
   * @param errors Error info.
   * @param rdoIDCont RDO ID Container to be filled.
   * @param cache Cache.
   * @param errorHit Hit error info.
   */
  int makeRDO(int strip, int groupSize, int timeBin,
              uint32_t onlineID, int errors,
              ISCT_RDO_Container& rdoIDCont,
              CacheHelper& cache,
              const std::vector<int>& errorHit) const;

  /**
   * @brief Add an error for each wafer in the problematic ROD
   *
   * @param rodID Identifer of ROD.
   * @param errorType Error type info.
   * @param errs Byte stream error container.
   */
  void addRODError(uint32_t rodID, int errorType,
                   InDetBSErrContainer* errs) const;
  /**
   * @brief Add single eror
   *
   * @param hashID Identifier for hash.
   * @param bsErrorType Byte Stream error type info.
   * @param errs Byte stream error container.
   */
  bool addSingleError(const IdentifierHash& hashID,
                      int bsErrorType,
                      InDetBSErrContainer* errs) const;

  /**
   * @brief Set first temporarily masked chip information from byte stream trailer
   *
   * @param hashID Identifier for hash.
   * @param firstTempMaskedChip  Firt temporarily masked chip info.
   * @param errs Byte stream error container.
   */
  void setFirstTempMaskedChip(const IdentifierHash& hashID, 
                              unsigned int firstTempMaskedChip, 
                              InDetBSErrContainer* errs) const;

  /** Identifier helper class for the SCT subdetector that creates compact Identifier objects and 
      IdentifierHash or hash IDs. Also allows decoding of these IDs. */
  const SCT_ID* m_sctID;

  /** "Context" of an expanded identifier (ExpandedIdentifier) for compact or 
      hash versions (Identifier32 or IdentifierHash) */
  IdContext m_contextSCT;

  /** Providing mappings of online and offline identifiers and also serial numbers. */
  ToolHandle<ISCT_CablingTool> m_cabling{this, 
                                         "SCT_CablingTool", 
                                         "SCT_CablingTool", 
                                         "Tool to retrieve SCT Cabling"};

  /** Service that keeps track of configuration conditions. */
  ToolHandle<ISCT_ConfigurationConditionsTool> m_configTool{this, 
                                                            "ConfigTool",
                                                            "SCT_ConfigurationConditionsTool/InDetSCT_ConfigurationConditionsTool", 
                                                            "Tool to retrieve SCT Configuration Tool"};

  /** Total number of single strips with hit decoded in condensed mode */
  mutable std::atomic_uint m_singleCondHitNumber;   

  /** Total number of paired strips with hit decoded in condensed mode */
  mutable std::atomic_uint m_pairedCondHitNumber;  
  
  /** Total number of first strips with hit decoded in expanded mode */
  mutable std::atomic_uint m_firstExpHitNumber;   
  
  /** Total number of consecutive paired strips with hit decoded in expanded mode */
  mutable std::atomic_uint m_evenExpHitNumber;   
  
  /** Total number of last consecutive strips with hit decoded in expanded mode */
  mutable std::atomic_uint m_lastExpHitNumber;  
  
  /** Total number of decoded header data */
  mutable std::atomic_uint m_headNumber;       
  
  /** Total number of decoded trailer data */
  mutable std::atomic_uint m_trailerNumber;   
  
  /** Total number of BCID errors in the header data */
  mutable std::atomic_uint m_headErrorBCID;  
  
  /** Total number of Lvl1ID errors in the header data */
  mutable std::atomic_uint m_headErrorLvl1ID; 
  
  /** Total number of timeout errors in the header data */
  mutable std::atomic_uint m_headErrorTimeout;    
  
  /** Total number of formatter errors in the header data */
  mutable std::atomic_uint m_headErrorFormatter;  
  
  /** Total number of preamble errors in the header data */
  mutable std::atomic_uint m_headErrorPreamble;  
  
  /** Total number of overflow errors in the trailer data */
  mutable std::atomic_uint m_trailerErrorOverflow; 
  
  /** Total number of header trailer limit errors in the trailer data */
  mutable std::atomic_uint m_trailerErrorLimit;   
  
  /** Total number of trailer bit errors */
  mutable std::atomic_uint m_trailerErrorBit;    
  
  /** Total number of configuration data for raw data */
  mutable std::atomic_uint m_configDataBit;     
  
  /** Total number of flag error data */
  mutable std::atomic_uint m_flagErrorBit;     
  
  /** Total number of first hit data errors */
  mutable std::atomic_uint m_condHit1Error;   
  
  /** Total number second hit data errors */
  mutable std::atomic_uint m_condHit2Error;  
  
  /** Total number of chip number errors */
  mutable std::atomic_uint m_chipNumberError;  
  
  /** Total number of unknown data formats */
  mutable std::atomic_uint m_unknownDataFormat; 
  
  /** Total number of SCT hits in ByteStream */
  mutable std::atomic_uint m_nHits;
  
  /** Total number of SCT RDOs created */
  mutable std::atomic_uint m_nRDOs;
  
  /** Total number of masked links in the header data */
  mutable std::atomic_uint m_maskedLinkNumber;
  
  /** Total number of masked RDOs */
  mutable std::atomic_uint m_maskedRODNumber;
  
  /** Total number of ROD clock errors */
  mutable std::atomic_uint m_rodClockErrorNumber;
  
  /** Total number of truncated ROBFragments */
  mutable std::atomic_uint m_truncatedRODNumber;
  
  /** Total number of missing link headers */
  mutable std::atomic_uint m_numMissingLinkHeader;
  
  /** Total number of SCT unknown online IDs */
  mutable std::atomic_uint m_numUnknownOfflineID;
  
  /** Swap phi readout direction */
  std::vector<bool> m_swapPhiReadoutDirection;
};

#endif //SCT_RAWDATABYTESTREAM_SCT_RODDECODER_H
