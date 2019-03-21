/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
 
#ifndef SCT_RAWDATABYTESTREAMCNV_SCT_RODENCODER_H
#define SCT_RAWDATABYTESTREAMCNV_SCT_RODENCODER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_RawDataByteStreamCnv/ISCT_RodEncoder.h"

#include "InDetRawData/SCT_RDO_Container.h"
#include "SCT_ConditionsTools/ISCT_ByteStreamErrorsTool.h"

#include "GaudiKernel/ToolHandle.h"

#include <set>

class ISCT_CablingTool;
class SCT_ID;
class SCT_RDORawData;
class Identifier;
class IdentifierHash;

/** 
 * @class SCT_RodEncoder
 *
 * @brief Athena Algorithm Tool that provides conversion from SCT RDO to ROD format Bytestream.
 *
 * This class inherits from AthAlgTool and ISCT_RodEncoder.
 *
 * Note from Jan 09: Make SCT_RodEncoder an AlgTool again, of which 
 * there is one instance per job, rather than the lightweight class of 
 * which there was one instance per ROD.  This change is motivated
 * by the changes to write ByteStream errors back into the bytestream,
 * as we don't want to access the ByteStreamErrorsSvc 90 times per event
 * if we can avoid it.. 
 */
class SCT_RodEncoder : public extends<AthAlgTool, ISCT_RodEncoder>
{ 
 public:
 
  /** Constructor */
  SCT_RodEncoder(const std::string& type, const std::string& name, const IInterface* parent);

  /** Destructor */
  virtual ~SCT_RodEncoder() = default;

  /** Initialize */
  virtual StatusCode initialize() override;

  /** Finalize */
  virtual StatusCode finalize() override;

  /**
   * @brief Main Convert method
   *
   * Converts SCT RDO to a vector of 32 bit words. Starts by retrieving and collecting
   * errors, then loops through RDO's and decode them to 16 bit words, and then
   * finally it packs the 16 bit word into 32 bit words vector.
   *
   * @param vec32Data Vector of 32 bit words to be filled with encoded RDOs from the SCT.
   * @param robID ID of the current readout buffer (ROB).
   * @param vecRDOs Vector containing the RDOs to be coverted to vector of 32 bit words.
   */
  virtual void fillROD(std::vector<uint32_t>& vec32Data, const uint32_t& robID, 
                       const std::vector<const SCT_RDORawData*>& vecRDOs) const override;

 private:
  
  /** Definitions of enum error words to be retrived from SCT_ByteStreamErrorSvc in fillROD(...) method */
  enum ErrorWords{TIMEOUT_ERR=(1<<11),
                  L1_ERR=(1<<10),
                  BCID_ERR=(1<<9),
                  PREAMBLE_ERR=(1<<12),
                  FORMATTER_ERR=12,
                  TRAILER_ERR=(1<<12),
                  NULL_HEADER_ERR=0,
                  HEADER_TRAILER_ERR=(1<<11),
                  TRAILER_OVFLW_ERR=(1<<10),
                  ABCD_ERR=0,
                  RAWDATA_ERR=(3<<13),
                  NULL_TRAILER_ERR=0}; 

  /**
   * @brief Method to encode RDO data to vector of 16 bin words.
   *
   * Methods used by main convert methods fillROD(...).
   *
   * @param vecTimeBins Vector of time bins for RDOs.
   * @param vec16Words Vector of 16 bit words to filled from encoded RDO data.
   * @param rdo RDO raw data object to be encoded.
   * @param groupSize Group size info from the RDO.
   * @param strip Strip number info from the RDO.
   */
  void encodeData(const std::vector<int>& vecTimeBins, std::vector<uint16_t>& vec16Words, 
                  const SCT_RDORawData* rdo, const int& groupSize, const int& strip) const;
  
  /**
   * @brief Method to pack vector of 16 bit words intto a vector of 32 bit words.
   *
   * Method us used by private method encodeData(...).
   *
   * @param vec16Words Vector containing 16 bit words.
   * @param vec32Words Vector for 32 bit words to be packed.
   */
  void packFragments(std::vector<uint16_t>& vec16Words, std::vector<uint32_t>& vec32Words) const;

  /**
   * @breif Method to set pairs of 16 bit words to a 32 bit word.
   *
   * Function used by the packFragments(...) method.
   *
   * @param arr16Words Pointer to array containing a pair of 16 bit words.
   * @param position Pointer to an array that gives the 32 bit starting positions of the 16 bit words and corresponding to arr16Words.
   * @param numWords Number of word to be set to a 32 bit word.
   */
  uint32_t set32Bits(const unsigned short int* arr16Words, 
                     const unsigned short int* position, 
                     const unsigned short int& numWords) const;

  /** Get the side info from the RDO. */
  int side(const SCT_RDORawData* rdo) const;
  
  /** Get the time bin info from the RDO. */
  int getTimeBin(const SCT_RDORawData* rdo) const;
  
  /** Get the strip number info from the RDO. */
  int getStrip(const SCT_RDORawData* rdo) const;
 
  /** Get the offline Identifier from the RDO.  */
  Identifier offlineID(const SCT_RDORawData* rdo) const;

  /** Get the online Identifier from the RDO. */
  uint32_t onlineID(const SCT_RDORawData* rdo) const;

  /** Get the ROD link number info in the RDO header data. */
  int getRODLink(const SCT_RDORawData* rdo) const;

  /** Get the 16-bit word for a header for a hit. */
  uint16_t getHeaderUsingRDO(const SCT_RDORawData* rdo) const;
  
  /** Get the 16-bit word for a header for a link with a ByteStream error. */
  uint16_t getHeaderUsingHash(const IdentifierHash& linkHash, const int& errorWord) const;
  
  /** Get the 16-bit word for a trailer, with or without ByteStream errors. */
  uint16_t getTrailer(const int& errorWord) const;

  /** Add header with errors for ROB to the vector of 16 bit words. */
  void addHeadersWithErrors(const uint32_t& robID, const std::set<IdentifierHash>* errors, 
                            const ErrorWords& errType, std::vector<uint16_t>& vec16Data) const;

  /** Add trailers with errors for ROB to the vector of 16 bit words. */
  void addTrailersWithErrors(const uint32_t& robID, const std::set<IdentifierHash>* errors, 
                             const ErrorWords& errType, std::vector<uint16_t>& vec16Data) const;

  /** Add specific errors for ROB to the vector of 16 bit words. */
  void addSpecificErrors(const uint32_t& robID, const std::set<IdentifierHash>* errors, 
                         const ErrorWords& errType, std::vector<uint16_t>& vec16Data) const;

  /** Tool that keeps track of modules that give rise to errors in the bytestream. */
  ToolHandle<ISCT_ByteStreamErrorsTool> m_bsErrTool{this, 
                                                    "SCT_ByteStreamErrorsTool", 
                                                    "SCT_ByteStreamErrorsTool", 
                                                    "Tool to retrieve SCT ByteStream Errors"};
  
  /** Providing mappings of online and offline identifiers and also serial numbers. */
  ToolHandle<ISCT_CablingTool> m_cabling{this, 
                                         "SCT_CablingTool", 
                                         "SCT_CablingTool", 
                                         "Tool to retrieve SCT Cabling"};

  /** Identifier helper class for the SCT subdetector that creates compact Identifier objects and 
      IdentifierHash or hash IDs. Also allows decoding of these IDs. */
  const SCT_ID* m_sctID;

  /** Boolean used to determine if fillROD(...) should use Condensed or Expanded mode when decoding. */
  BooleanProperty m_condensed{this, 
                              "CondensedMode", 
                              false, 
                              "Condensed mode (true) or Expanded mode (false)"};

  /** Swap Module identifier, set by SCTRawContByteStreamTool. */
  std::set<Identifier> m_swapModuleID;
};

#endif // SCT_RAWDATABYTESTREAMCNV_SCT_RODENCODER_H
