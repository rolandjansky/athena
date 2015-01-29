// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JEMTobRoI_v2.h 639736 2015-01-15 15:27:50Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_JEMTOBROI_V2_H
#define XAODTRIGL1CALO_VERSIONS_JEMTOBROI_V2_H

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

namespace xAOD{

  /// Description of JEMTobRoI_v2
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 639736 $
  /// $Date: 2015-01-15 16:27:50 +0100 (Thu, 15 Jan 2015) $

  class JEMTobRoI_v2 : public SG::AuxElement {
    public:
      /// Default constructor
      JEMTobRoI_v2();

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
      /// Return energy large window size
      int energyLarge() const;
      /// Return energy small window size
      int energySmall() const;      
      
    private:

      /// RoI word ID
      static const int s_wordIdVal       = 0;
      //  Data locations
      static const int s_wordIdBit       = 29;
      static const int s_crateBit        = 28;
      static const int s_jemBit          = 24;
      static const int s_frameBit        = 21;
      static const int s_locationBit     = 19;
      static const int s_energySmallBit  = 10;
      static const int s_energyLargeBit  = 0;
      //  Data masks
      static const int s_wordIdMask      = 0x7;
      static const int s_crateMask       = 0x1;
      static const int s_jemMask         = 0xf;
      static const int s_frameMask       = 0x7;
      static const int s_locationMask    = 0x3;
      static const int s_energySmallMask = 0x1ff;
      static const int s_energyLargeMask = 0x3ff;      
   
  }; // class JEMTobRoI_v2
} // namespace xAOD
#endif

