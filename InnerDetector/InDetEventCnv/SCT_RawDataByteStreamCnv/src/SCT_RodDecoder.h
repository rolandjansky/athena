// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETRAWDATABYTESTREAM_SCT_RODDECODER_H 
#define INDETRAWDATABYTESTREAM_SCT_RODDECODER_H

#include "SCT_RawDataByteStreamCnv/ISCT_RodDecoder.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "InDetByteStreamErrors/IDCInDetBSErrContainer.h"
#include "InDetIdentifier/SCT_ID.h"
#include "SCT_ConditionsData/SCT_ByteStreamErrors.h"
#include "SCT_Cabling/ISCT_CablingTool.h"
#include "SCT_ConditionsTools/ISCT_ByteStreamErrorsTool.h"
#include "SCT_ConditionsTools/ISCT_ConfigurationConditionsTool.h"
#include "Identifier/IdContext.h"

#include "GaudiKernel/ToolHandle.h"

#include <atomic>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class IdentifierHash;


/**
 * @brief allows to accumulate errors in one fillColection call
 *
 * Errors information is scattered across this code.
 * To be sure that all of the errors are saved this helper class provides add method allowing to update/accumulate erorr.
 * The IDC, for a very good reasons (MT safety) do not allow for that.
 **/
class SCT_RodDecoderErrorsHelper {
public:
  SCT_RodDecoderErrorsHelper(IDCInDetBSErrContainer& idcContainer)
    : errorsIDC{ idcContainer } {}
  ~SCT_RodDecoderErrorsHelper() {
    for (auto [id, err]: accumulatedErrors) {
      errorsIDC.setOrDrop(id, err);
    }
  }
  void noerror(const IdentifierHash id) {
    accumulatedErrors[id]; // this adds 0 (no error) for an ID
  }

  void add(const IdentifierHash id, SCT_ByteStreamErrors::ErrorType etype) {
    SCT_ByteStreamErrors::addError(accumulatedErrors[id], etype);
  }

  void removeIfEmpty(const IdentifierHash id) {
    if (accumulatedErrors[id]==0) {
      accumulatedErrors.erase(id);
    } 
  }

  std::map<IdentifierHash, IDCInDetBSErrContainer::ErrorCode> accumulatedErrors;
  IDCInDetBSErrContainer& errorsIDC;
};


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
  struct CacheHelper {
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
   * rdoIdc, and errs are updated based on robFrag and vecHash.
   *
   * @param robFrag ROB fragment.
   * @param rdoIDCont RDO ID Container to be filled.
   * @param errs Byte stream error container.
   * @param vecHash Vector of hashes.
   */
  virtual StatusCode fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag,
                                    SCT_RDO_Container& rdoIDCont,
                                    IDCInDetBSErrContainer& errs,
                                    const std::vector<IdentifierHash>* vecHash = nullptr) const override;

 private:

  /**
   * @enum SCT_DecoderNumbers
   * @brief Define frequently used magic numbers
   */
  enum SCT_DecoderNumbers { N_SIDES = 2,
                            N_CHIPS_PER_SIDE = 6,
                            N_STRIPS_PER_CHIP = 128,
                            N_STRIPS_PER_SIDE = N_CHIPS_PER_SIDE*N_STRIPS_PER_CHIP,
  };

  /** Struct to hold data shared in methods used in fillCollection method */
  struct SharedData {
    bool condensedMode{true}; // Condensed mode or Expanded mode for each link if superCondensedMode is false

    // Variables necessary for makeRDO
    int strip{0};
    int groupSize{0};
    int timeBin{0};
    IdentifierHash linkIDHash; // Determined from header and changed for links using Rx redundancy (waferHash)
    Identifier collID; // Determined from linkIDHash (waferID)
    int errors{0}; // Encodes the errors on the header (bit 4: error in condensed mode 1st hit, bit 5: error in condensed mode 2nd hit)
    CacheHelper cache; // For the trigger
    std::vector<int> errorHit;

    int side{0};
    int oldSide{-1};
    int oldStrip{-1};
    int linkNumber{0}; // Determined from header and may be changed for links using Rx redundancy

    std::array<bool, N_STRIPS_PER_SIDE*N_SIDES> saved;

    // For MissingLinkHeaderError
    bool foundMissingLinkHeaderError{false};
    std::unordered_set<IdentifierHash> foundHashes;

    std::unordered_map<IdentifierHash, std::unique_ptr<SCT_RDO_Collection>> rdoCollMap; // If SCT_RDO_Collection* is nullptr, it means the collection is already present in the container.
    std::unordered_map<IdentifierHash, SCT_RDO_Container::IDC_WriteHandle> writeHandleMap;

    bool foundHeader{false};

    void reset() {
      strip = 0;
      oldStrip = -1;
      oldSide = -1;
      groupSize = 0;
      errors = 0;
      saved.fill(false);
      errorHit.clear();
    };
    void setOld() {
      oldStrip = strip;
      oldSide = side;
      groupSize = 0;
    }
    void setSaved(const bool isOld, const int code) {
      if (isOld) {
        saved[oldSide*N_STRIPS_PER_SIDE + oldStrip] = code;
      }
      else {
        saved[   side*N_STRIPS_PER_SIDE +    strip] = code;
      }
    }
    bool isSaved(const bool isOld) {
      if (isOld) {
        return saved[oldSide*N_STRIPS_PER_SIDE + oldStrip];
      }
      else {
        return saved[   side*N_STRIPS_PER_SIDE +    strip];
      }
    }
    void setCollection(const SCT_ID* sctID,
                       const IdentifierHash& waferHash,
                       SCT_RDO_Container& rdoIDCont,
                       SCT_RodDecoderErrorsHelper& errs) {
      linkIDHash = waferHash;
      collID = sctID->wafer_id(linkIDHash);
      foundHashes.insert(linkIDHash);
      if (rdoCollMap.count(linkIDHash)==0) { // The collection is not in the local map.
        writeHandleMap.insert(std::pair<IdentifierHash, SCT_RDO_Container::IDC_WriteHandle>(linkIDHash, rdoIDCont.getWriteHandle(linkIDHash)));
        if (writeHandleMap[linkIDHash].alreadyPresent()) { // The collection is already in the container.
          rdoCollMap[linkIDHash] = nullptr;
          writeHandleMap.erase(linkIDHash); // lock is released
        }
        else { // Create a new collection for linkIDHash
          std::unique_ptr<SCT_RDO_Collection> rdoColl{std::make_unique<SCT_RDO_Collection>(linkIDHash)};
          rdoColl->setIdentifier(collID);
          rdoCollMap[linkIDHash] = std::move(rdoColl);
          errs.noerror(linkIDHash); // make sure the error information is filled for this hash
        }
      }
    }
  };

  /**
   * @brief Builds RawData RDO and adds to RDO container
   *
   * Method that builds the RawData RDO and add it to the collection rdoIdc and cache are 
   * updated based on other arguments. 
   * @return Method has 3 possible return values:
   *   1 if RDO was successfully created
   *   0 if collection was deliberately skipped (for trigger)
   *  -1 if there was an error in the decoding - will be passed on as StatusCode::RECOVERABLE by fillCollection()
   *
   * @param isOld if true use data.oldStrip, otherwise use data.strip.
   * @param data Struct to hold data shared in methods used in fillCollection method
   * @param cache Cache.
   */
  int makeRDO(const bool isOld,
              SharedData& data,
              CacheHelper& cache) const;

  /**
   * @brief Add an error for each wafer in the problematic ROD
   *
   * @param rodID Identifer of ROD.
   * @param errorType Error type info.
   * @param errs Byte stream error container.
   * @param foundHashes FE-links whose headers are found. Used only for MissingLinkHeaderError.
   */
  StatusCode addRODError(uint32_t rodID, SCT_ByteStreamErrors::ErrorType error,
                         SCT_RodDecoderErrorsHelper& errs,
                         const std::unordered_set<IdentifierHash>* foundHashes=nullptr) const;
  /**
   * @brief Add single eror
   *
   * @param hashID Identifier for hash.
   * @param bsErrorType Byte Stream error type info.
   * @param errs Byte stream error container.
   */
  StatusCode addSingleError(const IdentifierHash& hashID,
                            SCT_ByteStreamErrors::ErrorType error,
                            SCT_RodDecoderErrorsHelper& errs) const;

  /**
   * @brief Set first temporarily masked chip information from byte stream trailer
   *
   * @param hashID Identifier for hash.
   * @param firstTempMaskedChip  Firt temporarily masked chip info.
   * @param errs Byte stream error container.
   */
  StatusCode setFirstTempMaskedChip(const IdentifierHash& hashID, 
                                    unsigned int firstTempMaskedChip, 
                                    SCT_RodDecoderErrorsHelper& errs) const;

  /**
   * @brief Process header word
   *
   * @param inData input 16 bit data word for header
   * @param robID ROB ID
   * @param data Struct to hold data shared in methods used in fillCollection method
   * @param rdoIDCont RDO ID Container to be filled.
   * @param cache Cache.
   * @param errs SCT_RodDecoderErrorsHelper to fill IDCInDetBSErrContainer
   * @param hasError false means no error, true means at least one error
   * @param breakNow to tell if need to break after this method execution.
   */
  StatusCode processHeader(const uint16_t inData,
                           const uint32_t robID,
                           SharedData& data,
                           SCT_RDO_Container& rdoIDCont,
                           CacheHelper& cache,
                           SCT_RodDecoderErrorsHelper& errs,
                           bool& hasError,
                           bool& breakNow) const;

  /**
   * @brief Process hit word in Super-Condensed mode
   *
   * @param inData input 16 bit data word for header
   * @param robID ROB ID
   * @param data Struct to hold data shared in methods used in fillCollection method
   * @param rdoIDCont RDO ID Container to be filled.
   * @param cache Cache.
   * @param errs SCT_RodDecoderErrorsHelper to fill IDCInDetBSErrContainer
   * @param hasError false means no error, true means at least one error
   */
  StatusCode processSuperCondensedHit(const uint16_t inData,
                                      const uint32_t robID,
                                      SharedData& data,
                                      SCT_RDO_Container& rdoIDCont,
                                      CacheHelper& cache,
                                      SCT_RodDecoderErrorsHelper& errs,
                                      bool& hasError) const;

  /**
   * @brief Process hit word in Condensed mode
   *
   * @param inData input 16 bit data word for header
   * @param robID ROB ID
   * @param data Struct to hold data shared in methods used in fillCollection method
   * @param rdoIDCont RDO ID Container to be filled.
   * @param cache Cache.
   * @param errs SCT_RodDecoderErrorsHelper to fill IDCInDetBSErrContainer
   * @param hasError false means no error, true means at least one error
   */
  StatusCode processCondensedHit(const uint16_t inData,
                                 const uint32_t robID,
                                 SharedData& data,
                                 SCT_RDO_Container& rdoIDCont,
                                 CacheHelper& cache,
                                 SCT_RodDecoderErrorsHelper& errs,
                                 bool& hasError) const;

  /**
   * @brief Process hit word in Expanded mode
   *
   * @param inData input 16 bit data word for header
   * @param robID ROB ID
   * @param data Struct to hold data shared in methods used in fillCollection method
   * @param rdoIDCont RDO ID Container to be filled.
   * @param cache Cache.
   * @param errs SCT_RodDecoderErrorsHelper to fill IDCInDetBSErrContainer
   * @param hasError false means no error, true means at least one error
   */
  StatusCode processExpandedHit(const uint16_t inData,
                                const uint32_t robID,
                                SharedData& data,
                                SCT_RDO_Container& rdoIDCont,
                                CacheHelper& cache,
                                SCT_RodDecoderErrorsHelper& errs,
                                bool& hasError) const;

  /**
   * @brief Process ABCD error
   *
   * @param inData input 16 bit data word for header
   * @param robID ROB ID
   * @param data Struct to hold data shared in methods used in fillCollection method
   * @param errs SCT_RodDecoderErrorsHelper to fill IDCInDetBSErrContainer
   * @param hasError false means no error, true means at least one error
   */
  StatusCode processABCDError(const uint16_t inData,
                              const uint32_t robID,
                              SharedData& data,
                              SCT_RodDecoderErrorsHelper& errs,
                              bool& hasError) const;

  /**
   * @brief Process raw data word
   *
   * @param inData input 16 bit data word for header
   * @param robID ROB ID
   * @param data Struct to hold data shared in methods used in fillCollection method
   * @param errs SCT_RodDecoderErrorsHelper to fill IDCInDetBSErrContainer
   * @param hasError false means no error, true means at least one error
   */
  StatusCode processRawData(const uint16_t inData,
                            const uint32_t robID,
                            SharedData& data,
                            SCT_RodDecoderErrorsHelper& errs,
                            bool& hasError) const;

  /**
   * @brief Process trailer word
   *
   * @param inData input 16 bit data word for header
   * @param robID ROB ID
   * @param data Struct to hold data shared in methods used in fillCollection method
   * @param errs SCT_RodDecoderErrorsHelper to fill IDCInDetBSErrContainer
   * @param hasError false means no error, true means at least one error
   */
  StatusCode processTrailer(const uint16_t inData,
                            const uint32_t robID,
                            SharedData& data,
                            SCT_RodDecoderErrorsHelper& errs,
                            bool& hasError) const;

  /** Identifier helper class for the SCT subdetector that creates compact Identifier objects and 
      IdentifierHash or hash IDs. Also allows decoding of these IDs. */
  const SCT_ID* m_sctID{nullptr};

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
  mutable std::atomic_uint m_singleCondHitNumber{0};

  /** Total number of paired strips with hit decoded in condensed mode */
  mutable std::atomic_uint m_pairedCondHitNumber{0};
  
  /** Total number of first strips with hit decoded in expanded mode */
  mutable std::atomic_uint m_firstExpHitNumber{0};
  
  /** Total number of consecutive paired strips with hit decoded in expanded mode */
  mutable std::atomic_uint m_evenExpHitNumber{0};
  
  /** Total number of last consecutive strips with hit decoded in expanded mode */
  mutable std::atomic_uint m_lastExpHitNumber{0};
  
  /** Total number of decoded header data */
  mutable std::atomic_uint m_headNumber{0};
  
  /** Total number of decoded trailer data */
  mutable std::atomic_uint m_trailerNumber{0};
  
  /** Total number of BCID errors in the header data */
  mutable std::atomic_uint m_headErrorBCID{0};
  
  /** Total number of Lvl1ID errors in the header data */
  mutable std::atomic_uint m_headErrorLvl1ID{0};
  
  /** Total number of timeout errors in the header data */
  mutable std::atomic_uint m_headErrorTimeout{0};
  
  /** Total number of formatter errors in the header data */
  mutable std::atomic_uint m_headErrorFormatter{0};
  
  /** Total number of preamble errors in the header data */
  mutable std::atomic_uint m_headErrorPreamble{0};
  
  /** Total number of overflow errors in the trailer data */
  mutable std::atomic_uint m_trailerErrorOverflow{0};
  
  /** Total number of header trailer limit errors in the trailer data */
  mutable std::atomic_uint m_trailerErrorLimit{0};
  
  /** Total number of trailer bit errors */
  mutable std::atomic_uint m_trailerErrorBit{0};
  
  /** Total number of configuration data for raw data */
  mutable std::atomic_uint m_configDataBit{0};
  
  /** Total number of flag error data */
  mutable std::atomic_uint m_flagErrorBit{0};
  
  /** Total number of first hit data errors */
  mutable std::atomic_uint m_condHit1Error{0};
  
  /** Total number second hit data errors */
  mutable std::atomic_uint m_condHit2Error{0};
  
  /** Total number of chip number errors */
  mutable std::atomic_uint m_chipNumberError{0};
  
  /** Total number of unknown data formats */
  mutable std::atomic_uint m_unknownDataFormat{0};
  
  /** Total number of SCT hits in ByteStream */
  mutable std::atomic_uint m_nHits{0};
  
  /** Total number of SCT RDOs created */
  mutable std::atomic_uint m_nRDOs{0};
  
  /** Total number of masked links in the header data */
  mutable std::atomic_uint m_maskedLinkNumber{0};
  
  /** Total number of masked RDOs */
  mutable std::atomic_uint m_maskedRODNumber{0};
  
  /** Total number of ROD clock errors */
  mutable std::atomic_uint m_rodClockErrorNumber{0};
  
  /** Total number of truncated ROBFragments */
  mutable std::atomic_uint m_truncatedRODNumber{0};
  
  /** Total number of missing link headers */
  mutable std::atomic_uint m_numMissingLinkHeader{0};
  
  /** Swap phi readout direction */
  std::vector<bool> m_swapPhiReadoutDirection{};
};

#endif //SCT_RAWDATABYTESTREAM_SCT_RODDECODER_H
