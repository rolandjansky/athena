// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JEMRoI_v1.h 631127 2014-11-26 09:54:48Z gwatts $
#ifndef XAODTRIGL1CALO_VERSIONS_JEMROI_V1_H
#define XAODTRIGL1CALO_VERSIONS_JEMROI_V1_H

extern "C" {
#   include <stdint.h>
}

#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

namespace xAOD {

  /// Description of JEMRoI_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 631127 $
  /// $Date: 2014-11-26 10:54:48 +0100 (Wed, 26 Nov 2014) $  
  
  class JEMRoI_v1 : public SG::AuxElement{
    public:
      // Default constructor
      JEMRoI_v1();
          
      /// get roiWord
      uint32_t roiWord() const;
      /// set roiWord
      void setRoiWord(uint32_t);
      
      /// Return crate number (0-1)
      int crate()    const;
      /// Return JEM number (0-15)
      int jem()      const;
      /// Return RoI frame number (0-7)
      int frame()    const;
      /// Return location (RoI local coords) (0-3)
      int location() const;
      /// Return forward jet flag (0/1)
      int forward()  const;
      /// Return Jet hit map (8 bits Main or 4 bits Forward)
      int hits()     const;
      /// Return error flags (bit 0 Saturation, bit 1 Parity)
      int error()    const;
      /// Return parity error flag (0/1)
      int parity()     const;
      /// Return saturation flag (0/1)
      int saturation() const;            
    
    private:
      /// RoI word ID
      static const int s_wordIdVal       = 0x4;
      //  Data locations
      static const int s_wordIdBit       = 29;
      static const int s_crateBit        = 28;
      static const int s_jemBit          = 24;
      static const int s_frameBit        = 21;
      static const int s_locationBit     = 19;
      static const int s_forwardBit      = 18;
      static const int s_parityBit       = 17;
      static const int s_saturationBit   = 16;
      static const int s_forwardHitsBit  = 8;
      static const int s_mainHitsBit     = 0;
      //  Data masks
      static const int s_wordIdMask      = 0x7;
      static const int s_crateMask       = 0x1;
      static const int s_jemMask         = 0xf;
      static const int s_frameMask       = 0x7;
      static const int s_locationMask    = 0x3;
      static const int s_forwardMask     = 0x1;
      static const int s_parityMask      = 0x1;
      static const int s_saturationMask  = 0x1;
      static const int s_forwardHitsMask = 0xf;
      static const int s_mainHitsMask    = 0xff;     
                  
  };
} // namespace xAOD
#endif // XAODTRIGL1CALO_VERSIONS_JEMROI_V1_H
