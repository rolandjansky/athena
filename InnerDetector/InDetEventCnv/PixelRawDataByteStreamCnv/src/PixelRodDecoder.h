/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELRAWDATABYTESTREAM_PIXEL_RODDECODER_H
#define PIXELRAWDATABYTESTREAM_PIXEL_RODDECODER_H

#include "PixelRawDataByteStreamCnv/IPixelRodDecoder.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ServiceHandle.h"

#include "eformat/SourceIdentifier.h"

class IPixelCablingSvc;
class PixelID;
class IInDetConditionsSvc;
class IPixelByteStreamErrorsSvc;
class PixelCablingData;

class PixelRodDecoder : virtual public IPixelRodDecoder, public AthAlgTool
{

public: 

  // Collection type
  typedef Pixel1RawData RDO;
  typedef InDetRawDataCollection< PixelRDORawData > PixelRawCollection; 

  // constructor
  PixelRodDecoder(const std::string& type, const std::string& name,
                 const IInterface* parent ) ;

  // destructor 
  ~PixelRodDecoder(); 

  StatusCode initialize() override;
  StatusCode finalize() override;

  StatusCode fillCollection  (const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment *robFrag, IPixelRDO_Container* rdoIdc, 
			std::vector<IdentifierHash>* vecHash = NULL) override;

  inline void setDet( const eformat::SubDetector det );
  bool m_is_ibl_present;
  bool m_is_ibl_module;
  bool m_is_dbm_module;

  uint32_t getDataType(unsigned int rawDataWord, bool link_start);   // determine module word type
  
  ////// decode IBL module header word
  uint32_t decodeBCID_IBL(const uint32_t word); // decode IBL BCID from header word
  uint32_t decodeL1ID_IBL(const uint32_t word); // decode IBL L1ID from header word
  uint32_t decodeFeI4Bflag_IBL(const uint32_t word); // decode IBL FeI4 flag bit
  uint32_t decodeModule_IBL(const uint32_t word); // decode IBL module link number from header word
  
  ////// decode Pixel module header word
  uint32_t decodeBCID(const uint32_t word);   // decode BCID from header word
  uint32_t decodeL1ID(const uint32_t word);   // decode L1ID from header word
  uint32_t decodeL1IDskip(const uint32_t word);   // decode L1ID skips from header word
  uint32_t decodeModule(const uint32_t word);   // decode module link number from header word 
  uint32_t decodeHeaderErrors(const uint32_t word);   // decode header errors from header word
  //   uint32_t decodeHeaderBitflips(const uint32_t word);   // search for bitflips in wordparts supposed to be empty
  
  
  ////// decode Pixel and IBL module hit word
  uint32_t decodeRow(const uint32_t word);   // decode row number from hit word
  uint32_t decodeColumn(const uint32_t word);   // decode column number from hit word
  uint32_t decodeToT(const uint32_t word);   // decode TimeOverThreshold value from hit word
  
  
  ////// decode IBL-only module (not-condensed) hit word
  uint32_t decodeLinkNumHit_IBL(const uint32_t word); // decode Link Number in the IBL not-condensed Hit word // At least temporarily not used, because IBL data format is not clear (Franconi, 17.06.2014)
  //   uint32_t decodeHitBitflips_IBL(const uint32_t word); // search for bitflips in wordparts supposed to be empty
  
  
  ////// decode Pixel-only module hit word
  uint32_t decodeFE(const uint32_t word);   // decode FE number from hit word
  //   uint32_t decodeFE2(const uint32_t word);   // decode FE number from flag type 1 word
  //   uint32_t decodeHitBitflips(const uint32_t word);   // search for bitflips in wordparts supposed to be empty
  
  
  ////// decode IBL module trailer word
  uint32_t decodeSkippedTrigTrailer_IBL(const uint32_t word); // decode the skipped trigger counter in the IBL trailer
  uint32_t decodeTrailerErrors_IBL (const uint32_t word); // decode IBL trailer errors, all together 
  uint32_t decodeLinkNumTrailer_IBL(const uint32_t word); // decode the link number in the IBL trailer // At least temporarily not used, because IBL data format is not clear (Franconi, 17.06.2014)
  //   uint32_t decodeTimeOutErrorBit_IBL(const uint32_t word); // decode the Time out error bit in the IBL trailer
  uint32_t decodeCondensedModeBit_IBL(const uint32_t word); // decode the Condensed mode bit in the IBL trailer
  //   uint32_t decodeTrailerBitflips_IBL(const uint32_t word); // decode IBL bitflips in the trailer word // At least temporarily not used, because IBL data format is not clear (Franconi, 17.06.2014)
  
  
  ////// decode Pixel module trailer word
  uint32_t decodeTrailerErrors(const uint32_t word);   // decode trailer errors from trailer word 
  uint32_t decodeTrailerBitflips(const uint32_t word);   // search for bitflips in wordparts supposed to be empty
  uint32_t decodeBcidTrailer_IBL(const uint32_t word);
  
  
  ////// decode Pixel module FE flags
  uint32_t decodeFEFlags2(const uint32_t word);   // decode FE flags from flag type 2 word
  uint32_t decodeMCCFlags(const uint32_t word);   // decode MCC flags from flag type 2 word
  //   uint32_t decodeFEFlags1(const uint32_t word);   // decode FE flags from flag type 1 word
  //   uint32_t decodeFEFlags1Bitflips(const uint32_t word);   // search for bitflips in wordparts supposed to be empty
  //   uint32_t decodeFEFlags2Bitflips(const uint32_t word);   // search for bitflips in wordparts supposed to be empty
  
  
  ////// decode IBL module FE flags
  uint32_t decodeServiceCodeCounter_IBL(const uint32_t word); // decode the Service Code Counter from the "non-short" IBL FE flag word
  uint32_t decodeServiceCode_IBL(const uint32_t word); // decode the Service code from the "non-short" IBL FE Flag word
  uint32_t decodeLinkNumFEFlag_IBL(const uint32_t word); // decode the Link Number present in the IBL FE Flag word // At least temporarily not used, because IBL data format is not clear (Franconi, 17.06.2014)
  //   uint32_t decodeFEFlagBitFlips_IBL(const uint32_t word); // search for bitflips in word partes supposed to be empty // At least temporarily not used, because IBL data format is not clear (Franconi, 17.06.2014)
  
  
  ////// decode Pixel RawData word
  uint32_t decodeRawData(const uint32_t word); // decode raw data word for IBL and Pixel
  
  uint32_t extractFefromLinkNum (const uint32_t linkNum);
  uint32_t extractSLinkfromLinkNum (const uint32_t linkNum);
  
  void addRODError(uint32_t rodid, uint32_t robStatus);
  
  bool CheckLinkStart(const bool Link_start, const int rodId, const unsigned int mLink, const unsigned int mBCID, const unsigned int mLVL1ID, const int LVL1A);

  uint32_t treatmentFEFlagInfo(unsigned int serviceCode, unsigned int serviceCodeCounter);
  void addToFlaggedErrorCounter(const unsigned int & serviceCodeCounter);

 private:
  int m_masked_errors;

  int m_numGenWarnings;
  int m_maxNumGenWarnings;     // Maximum number of general warnings to print
  int m_numBCIDWarnings;
  int m_maxNumBCIDWarnings;    // Maximum number of BCID and LVL1ID warnings to print


  ServiceHandle<IPixelCablingSvc>  m_pixelCabling;
  
  const PixelID*              m_pixel_id;   
  eformat::SubDetector        m_det;
  PixelCablingData*           m_cablingData;
  
  ServiceHandle<IInDetConditionsSvc>   m_condsummary;
  ServiceHandle<IPixelByteStreamErrorsSvc>   m_errors;
  
};

inline void PixelRodDecoder::setDet( const eformat::SubDetector det )
{
  m_det = det;
}

#endif
