/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CNVTOOLS_TGCRODREADOUT_H
#define MUONTGC_CNVTOOLS_TGCRODREADOUT_H

#include <inttypes.h>
#include <vector>

#include "MuonRDO/TgcRdo.h"
#include "TGCcablingInterface/ITGCcablingSvc.h"
#include "ByteStreamData/RawEvent.h"

namespace Muon 
{
  class TgcSlbData;
  class TgcSlbDataHelper;


  /** @class TgcRODReadOut 
   * 
   *  TGC ROD decoder for conversion from ROB fragment to TgcRDO
   *
   *  @author Susumu Oda <Susumu.Oda@cern.ch>
   *  @author Hisaya Kurashige
   *
   *  This class was developed by Tadashi Maeno based on 
   *  MdtRODReadOut written by S. Rosati. Hisaya Kurashige  
   *  removed TGC ROD Encoder and updated decodeRodToRdo 
   *  on January 2008. Takashi Kubota migrated to 
   *  MuonTGC_CnvTools. 
   */
 
  class TgcRODReadOut
    {
    private:
      typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType ByteStream;

    public:
      /** Constructor */
      TgcRODReadOut();
      /** Destructor */
      virtual ~TgcRODReadOut();

      /** Get TgcRdo member */ 
      const TgcRdo * getRdo(void) const;
      /** Set TgcRdo member */ 
      StatusCode setRdo(TgcRdo * v_tgcRdo);

      /** Convert BS (ROB fragment) to RDO */ 
      StatusCode byteStream2Rdo(const ByteStream& bs, 
				TgcRdo& tgcRdo, 
				uint32_t source_id);
      /** Convert BS (ROB fragment) to RDO and compare decoded RDO container 
       *  and another RDO container decoded by other converter */ 
      StatusCode check(const ByteStream& bs, 
		       TgcRdo& tgcRdo, 
		       uint32_t source_id);
      /** Compare two RDO containers */ 
      StatusCode compare(TgcRdo* rdo, TgcRdo* newRdo); 
      /** Compare two RDOs */ 
      bool isMatched(const TgcRawData* rdo1,
		     const TgcRawData* rdo2) const;
      /** Decode BS to RDO container */
      StatusCode decodeRodToRdo(const ByteStream& vData, 
				uint16_t subDetectorId,
				uint16_t rodId, 
				uint32_t l1Id, 
				uint16_t bcId);
  
      /** Decode only header quickly */
      StatusCode decodeHeader(const ByteStream& vData);
  
      /** Get subDetectorId of member TgcRdo */
      uint16_t subDetectorId() const;
      /** Get rodId of member TgcRdo */
      uint16_t rodId() const;
      /** Get l1Id of member TgcRdo */
      uint32_t l1Id() const;
      /** Get bcId of member TgcRdo */
      uint16_t bcId() const;
      /** Get triggerType of member TgcRdo */
      uint16_t triggerType() const;
  
      /** Set ROD attributes (data header) to TgcRdo */
      StatusCode setRodAttributes (const uint16_t subDetectorId, 
				   const uint16_t rodId,
				   const uint32_t l1Id,
				   const uint16_t bcId,
				   const uint16_t triggerType);
  
    protected:
      enum {
	RawDataFragMask     =0xFF000000, // RawData Fragment HEADER  Mask
	FragmentIdMask      =0xFF000000, // Fragmen ID Mask
	FragmentCountMask   =0x00FFFFFF, // Fragmen ID Mask
	HeaderMask          =0xE0000000, // 
	HeaderEvent         =0x00000000, // Event Header
	HeaderError         =0x20000000, // Error Report
	HeaderSLB10         =0x40000000, // SLB Header 10
	HeaderSLB11         =0x60000000, // SLB Header 11
	HeaderSLBC          =0x80000000, // central bunch
	HeaderSLBP          =0xA0000000, // previous bunch
	HeaderSLBN          =0xC0000000,  // next bunch
	HeaderTrailer       =0x70000000,  // SLB Header 11
 
	ROD_START           = 0xEE1234EE,
	ROD_HEADER_SIZE     = 0x09,
	ROD_STATUS_SIZE     = 0x05
      };

      /** Set sbLoc */
      bool setSbLoc(uint16_t subDetectorId,
		    uint16_t rodId, 
		    TgcSlbData * slb, int rxId); 

    private:
      /** The number of RODs (1-24 for 12-fold, 0-15 for 8-fold) */
      enum NROD_SIDE {
	NROD = 24+1,
	ASIDE = 0x67, // 103
	CSIDE = 0x68  // 104
      };
      
      /** The number of failures on setRdo */
      unsigned int m_failedSetRdo;
      /** The number of failures on decodeRodToRdo */
      unsigned int m_failedDecodeRodToRdo[NROD+1];
      /** The number of strange header and SizeRawData */
      unsigned int m_failedHeaderSizeRawData[NROD+1];
      /** The number of failures on setSbLoc */
      unsigned int m_failedSetSbLoc[NROD+1];
      /** The number of failures on setType */
      unsigned int m_failedSetType[NROD+1];
      /** The number of failures on getSLBIDfromRxID */
      unsigned int m_failedGetSLBIDfromRxID[NROD+1];
      /** The number of failures on getReadoutIDfromSLBID */
      unsigned int m_failedGetReadoutIDfromSLBID[NROD+1];

      /** TGC RDO container */
      TgcRdo* m_tgcRdo;

      /** TGC Cabling Svc */
      const ITGCcablingSvc* m_cabling;

      /** TGC SLB data helper */
      TgcSlbDataHelper* m_tgcSlbDataHelper;

      TgcRODReadOut & operator=(const TgcRODReadOut &right);
      TgcRODReadOut(const TgcRODReadOut&);
    };   

} // end of namespace 

#endif // MUONTGC_CNVTOOLS_TGCRODREADOUT_H
