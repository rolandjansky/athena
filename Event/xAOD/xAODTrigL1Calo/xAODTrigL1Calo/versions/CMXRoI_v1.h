// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXRoI_v1.h 681007 2015-07-07 13:34:12Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_CMXROI_V1_H
#define XAODTRIGL1CALO_VERSIONS_CMXROI_V1_H

extern "C" {
#   include <stdint.h>
}

#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

namespace xAOD {
  
  /// Description of CMXRoI_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 681007 $
  /// $Date: 2015-07-07 15:34:12 +0200 (Tue, 07 Jul 2015) $  
  
  class CMXRoI_v1 : public SG::AuxElement{
    public:
      
      enum SumType { NORMAL = 0, MASKED = 1 };
      
      // Default constructor
      CMXRoI_v1();
      /// Default desturctor
      virtual ~CMXRoI_v1(){}
      
      /// initialize
      virtual void initialize(uint32_t roiWord0, uint32_t roiWord1, uint32_t roiWord2,
                              uint32_t roiWord3, uint32_t roiWord4, uint32_t roiWord5);

      /// initialize
      virtual void initialize(unsigned int ex, unsigned int ey, unsigned int et,
                              int exError, int eyError, int etError,
                              unsigned int sumEtHits, unsigned int missingEtHits,
                              unsigned int missingEtSigHits,
                              unsigned int exM, unsigned int eyM, unsigned int etM,
                              int exErrorM, int eyErrorM, int etErrorM,
                              unsigned int sumEtHitsM, unsigned int missingEtHitsM); 
      
      /// get cmxRoIWords
      const std::vector<uint32_t>& cmxRoIWords() const;
      /// set cmxRoIWords
      void setCmxRoIWords(const std::vector<uint32_t>&);

      /// Return Ex
      unsigned int ex(SumType type = NORMAL)            const;
      /// Return Ey
      unsigned int ey(SumType type = NORMAL)            const;
      /// Return Et
      unsigned int et(SumType type = NORMAL)            const;
      /// Return Ex error flags (bit 0 Overflow)
      int exError(SumType type = NORMAL)                const;
      /// Return Ey error flags (bit 0 Overflow)
      int eyError(SumType type = NORMAL)                const;
      /// Return Et error flags (bit 0 Overflow)
      int etError(SumType type = NORMAL)                const;
      /// Return Sum-ET hits
      unsigned int sumEtHits(SumType type = NORMAL)     const;
      /// Return Missing-ET hits
      unsigned int missingEtHits(SumType type = NORMAL) const;
      /// Return Missing-ET-Sig hits
      unsigned int missingEtSigHits()                   const;

      /// Return packed Energy RoI word (0-5)
      uint32_t roiWord(int word) const;
      /// Return Ex RoI word
      unsigned int exWord(SumType type = NORMAL)        const;
      /// Return Ey RoI word
      unsigned int eyWord(SumType type = NORMAL)        const;
      /// Return ET RoI word
      unsigned int etWord(SumType type = NORMAL)        const;     
      
    
    private:   
      //  RoI word IDs
      static const int s_wordIdVal0           = 0x4;
      static const int s_wordIdVal1           = 0x6;
      static const int s_wordIdVal2           = 0x5;
      //  Data locations
      static const int s_wordIdBit            = 28;
      static const int s_sumEtHitsBit         = 16;
      static const int s_missingEtHitsBit     = 16;
      static const int s_missingEtSigHitsBit  = 16;
      static const int s_energyBit            = 0;
      static const int s_overflowBit          = 15;
      static const int s_sumTypeBit           = 26;
      //  Data masks
      static const int s_wordIdMask           = 0xf;
      static const int s_sumEtHitsMask        = 0xff;
      static const int s_missingEtHitsMask    = 0xff;
      static const int s_missingEtSigHitsMask = 0xff;
      static const int s_energyMask           = 0x7fff;
      static const int s_overflowMask         = 0x1;
      static const int s_sumTypeMask          = 0x1;
      //  RoI word array offsets
      static const int s_exOffset    = 0;
      static const int s_eyOffset    = 2;
      static const int s_etOffset    = 4;
      static const int s_maxRoiWords = 6;  
        
  };
} // namespace xAOD

#endif // XAODTRIGL1CALO_VERSIONS_CMMROI_V1_H
