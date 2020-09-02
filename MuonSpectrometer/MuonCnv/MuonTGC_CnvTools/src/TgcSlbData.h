/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CNVTOOLS_TGCSLBDATA_H
#define MUONTGC_CNVTOOLS_TGCSLBDATA_H

#include <inttypes.h>

namespace Muon 
{

  /** @class TgcSlbData 
   *  This is the SLB data class which represents an SLB block. 
   *  @author Hisaya Kurashige 
   *  @author Susumu Oda
   *
   *  This class was developed by Tadashi Maeno. 
   */

  class TgcSlbData
    {
    public:
      enum 
	{
	  BC_UNDEFINED=0,
	  BC_PREVIOUS,
	  BC_CENTRAL,
	  BC_NEXT,
	  N_BC,
	  BITMAP_SIZE  = 200
	};

      enum  SlbType {
	SLB_HIT     =0,
	SLB_LPT_D_W,
	SLB_LPT_D_S,
	SLB_LPT_T_W,
	SLB_LPT_T_S,
	SLB_LPT_I_W,
	SLB_LPT_I_S,
	SLB_SL_F,
	SLB_SL_E,
	SLB_NONE
      };
  
      /** Costructor */
      TgcSlbData(uint16_t bcId, uint32_t l1Id, 
		 uint16_t sswId, uint16_t slbId);

      /** Destructor */
      virtual ~TgcSlbData(void) ;
  
      /** Check input IDs and held IDs are equal or not */ 
      bool equal(uint16_t bcId,
		 uint32_t l1Id,
		 uint16_t sswId,
		 uint16_t slbId) const;

      /** Get bitArray for a BC tag. */
      const bool * getBitArray(const uint16_t bcTag) const;

      /** Store bitmap. */
      void setBitmap (const uint16_t bcTag,
		      const uint32_t cellAddr, const uint32_t cellData);

      /** Set a bit. */
      void setBit (const uint16_t bcTag,
		   uint32_t iBit);
    
      /** Get SLB type. */
      int getType() const;

      /** Set SLB type. */
      void setType(int vType) ;

      /** Check if the SLB type is coincidence. */ 
      bool isCoincidence(void) const;

      /** Check if the SLB type is LowPt. */ 
      bool isLowPt(void) const;
  
      /** Check if the SLB type is HighPt. No SLB for HighPt and SLB for SL is used. */
      bool isHighPt(void) const;

      /** Check if the SLB type is SL. */ 
      bool isSL(void) const;

      /** Check if the SLB type is inner LowPt. */ 
      bool isInner(void) const;

      /** Set bcId */ 
      void setBcId(uint16_t bcId);
      /** Set l1Id */ 
      void setL1Id(uint32_t l1Id);
      /** Set sswId */ 
      void setSswId(uint16_t sswId);
      /** Set slbId */ 
      void setSlbId(uint16_t slbId);
      /** Set sbLoc */ 
      void setSbLoc(uint16_t sbLoc);
      /** Set Error */ 
      void setError(uint16_t error);

      /** Get bcId */ 
      uint16_t getBcId() const;
      /** Get l1Id */ 
      uint32_t getL1Id() const;
      /** Get sswId */ 
      uint16_t getSswId() const;
      /** Get slbId */ 
      uint16_t getSlbId() const;
      /** Get sbLoc */ 
      uint16_t getSbLoc() const;
      /** Get error */ 
      uint16_t getError() const;

    private:
      enum {
	CELL_SIZE     = 8,
	PADDING_WORD  = 0x1F
      };

      /** Costructor */
      TgcSlbData(void); 

      /** Get bitArray for a BC tag. */
      // Non-const version which is used in this class only
      bool * getBitArray(const uint16_t bcTag);

      uint16_t m_bcId;
      uint32_t m_l1Id;
      uint16_t m_sswId;
      uint16_t m_slbId;
      uint16_t m_sbLoc;
      uint16_t m_error;  

      /** Bit array at central bunch */
      bool m_bitArray_C[BITMAP_SIZE];  
      /** Bit array at previous bunch */
      bool m_bitArray_P[BITMAP_SIZE];  
      /** Bit array at next bunch */
      bool m_bitArray_N[BITMAP_SIZE];  
  
      /** SLB type */
      int m_type;

    };
} // end of namespace

#endif // MUONTGC_CNVTOOLS_TGCSLBDATA_H
