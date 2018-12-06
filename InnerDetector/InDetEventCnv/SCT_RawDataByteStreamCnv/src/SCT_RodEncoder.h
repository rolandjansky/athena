/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 *      @header file for SCT_RodEncoder Class
 *      This class provides conversion from SCT RDO to ROD format ByteStream.
 *      @date 21/06/07:  SCT_RodEncoder turned into An AlgTool 
 *      @author: Nick Barlow nbarlow@mail.cern.ch, Kondo.Gnanvo@cern.ch
 *  
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

/** Jan 09: make SCT_RodEncoder an AlgTool again, of which there
 * is one instance per job, rather than the lightweight class of 
 * which there was one instance per ROD.  This change is motivated
 * by the changes to write ByteStream errors back into the bytestream,
 * as we don't want to access the ByteStreamErrorsSvc 90 times per event
 * if we can avoid it.. */
 
class SCT_RodEncoder : public extends<AthAlgTool, ISCT_RodEncoder>
{ 
 public:
 
  /** constructor  */

  SCT_RodEncoder(const std::string& type, const std::string& name, const IInterface* parent);

  /** destructor  */
  virtual ~SCT_RodEncoder() = default;

  /** AlgTool initialize */
  virtual StatusCode initialize() override;

  /** AlgTool finalize*/
  virtual StatusCode finalize() override;

  /// convert all collections of RDO's in the current  list to vector of 32bit words   
  virtual void fillROD(std::vector<uint32_t>& vec32Data, const uint32_t& robID, const std::vector<const SCT_RDORawData*>& vecRDOs) const override;

 private:

  /// Encode rdo into the data: called by fillROD(..) 
  void encodeData(const std::vector<int>& vecTimeBins, std::vector<uint16_t>& vec16Words, const SCT_RDORawData* rdo, const int& groupSize, const int& strip) const;
  
  /// pack 32 bit word:  called by  encodeData(..) 
  void packFragments(std::vector<uint16_t>& vec16Words, std::vector<uint32_t>& vec32Words) const;

  /// from 16 bits array to 32 bit array   
  uint32_t set32Bits(const unsigned short int* arr16Words, const unsigned short int* position, const unsigned short int& numWords) const;

  /// Get the side info from the RDO 
  int side(const SCT_RDORawData* rdo) const;
  
  /// Get the time bin info from the RDO
  int getTimeBin(const SCT_RDORawData* rdo) const;
  
  /// Get the strip number info from the RDO  
  int getStrip(const SCT_RDORawData* rdo) const;
 
  /// Get the offline Identifirer from the RDO  
  Identifier offlineID(const SCT_RDORawData* rdo) const;

  /// Get the online id from the RDO  
  uint32_t onlineID(const SCT_RDORawData* rdo) const;

  /// Get the ROD link info from the RDO 
  int getRODLink(const SCT_RDORawData* rdo) const;

  /// Get the 16-bit word for a header for a hit 
  uint16_t getHeaderUsingRDO(const SCT_RDORawData* rdo) const;
  
  /// Get the 16-bit word for a header for a link with a ByteStream error 
  uint16_t getHeaderUsingHash(const IdentifierHash& linkHash, const int& errorWord) const;
  
  /// Get the 16-bit word for a trailer, with or without ByteStream errors 
  uint16_t getTrailer(const int& errorWord) const;

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
  void addHeadersWithErrors(const uint32_t& robID, const std::set<IdentifierHash>* errors, 
                            const ErrorWords& errType, std::vector<uint16_t>& vec16Data) const;
  void addTrailersWithErrors(const uint32_t& robID, const std::set<IdentifierHash>* errors, 
                             const ErrorWords& errType, std::vector<uint16_t>& vec16dDta) const;
  void addSpecificErrors(const uint32_t& robID, const std::set<IdentifierHash>* errors, 
                         const ErrorWords& errType, std::vector<uint16_t>& vec16Data) const;
  ToolHandle<ISCT_ByteStreamErrorsTool> m_bsErrTool{this, "SCT_ByteStreamErrorsTool", "SCT_ByteStreamErrorsTool", "Tool to retrieve SCT ByteStream Errors"};
  ToolHandle<ISCT_CablingTool> m_cabling{this, "SCT_CablingTool", "SCT_CablingTool", "Tool to retrieve SCT Cabling"};
  const SCT_ID* m_sctID;
  BooleanProperty m_condensed{this, "CondensedMode", false, "Condensed mode (true) or Expanded mode (false)"};
  std::set<Identifier> m_swapModuleID; // Set by SCTRawContByteStreamTool
};

#endif // SCT_RAWDATABYTESTREAMCNV_SCT_RODENCODER_H
