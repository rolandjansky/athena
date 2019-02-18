// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JEMTobRoI_v1.h 685662 2015-07-27 10:30:15Z amazurov $
#ifndef XAODTRIGL1CALO_VERSIONS_JEMTOBROI_V1_H
#define XAODTRIGL1CALO_VERSIONS_JEMTOBROI_V1_H

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"
#include <ostream>

namespace xAOD{

  /// Description of JEMTobRoI_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 685662 $
  /// $Date: 2015-07-27 12:30:15 +0200 (Mon, 27 Jul 2015) $

  class JEMTobRoI_v1 : public SG::AuxElement {
    public:
      /// Default constructor
      JEMTobRoI_v1();
      /// Default desturctor
      virtual ~JEMTobRoI_v1(){}        

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

      virtual void initialize(const int crate, const int jem, const int frame,
        const int location, const int energyLarge, const int energySmall );     
      
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
   
  }; // class JEMTobRoI_v1


  std::ostream &operator<<(std::ostream &os, const xAOD::JEMTobRoI_v1 &el);


} // namespace xAOD
#endif

