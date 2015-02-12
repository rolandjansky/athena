// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXJetTob_v2.h 646335 2015-02-12 01:16:10Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_CMXJETTOB_V2_H
#define XAODTRIGL1CALO_VERSIONS_CMXJETTOB_V2_H

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

// System include(s):
#include <stdint.h>

namespace xAOD{

  /// Description of CMXJetTob_v2
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 646335 $
  /// $Date: 2015-02-12 02:16:10 +0100 (Thu, 12 Feb 2015) $

  class CMXJetTob_v2 : public SG::AuxElement {
    public:
      /// Default constructor
      CMXJetTob_v2();
      /// Default desturctor
      virtual ~CMXJetTob_v2(){}       
      
      /// initialize
      virtual void initialize(const uint_least8_t crate,const uint_least8_t jem,const uint_least8_t frame,const uint_least8_t loc);
      
      /// initialize
      virtual void initialize(const uint_least8_t crate,const uint_least8_t jem,const uint_least8_t frame,const uint_least8_t loc,
                              const std::vector<uint_least16_t>& energyLargeVec,
                              const std::vector<uint_least16_t>& energySmallVec,
                              const std::vector<uint_least8_t>& errorVec,
                              const std::vector<uint_least16_t>& presenceMapVec,
                              const uint_least8_t peak);
      
      /// add data to existing object
      void addTob(const std::vector<uint_least16_t>& energyLargeVec,
                  const std::vector<uint_least16_t>& energySmallVec,
                  const std::vector<uint_least8_t>& errorVec,
                  const std::vector<uint_least16_t>& presenceMapVec);
      
      
      /// get crate
      uint_least8_t crate() const;
      /// set crate
      void setCrate(uint_least8_t);

      /// get jem
      uint_least8_t jem() const;
      /// set jem
      void setJem(uint_least8_t);

      /// get frame
      uint_least8_t frame() const;
      /// set frame
      void setFrame(uint_least8_t);

      /// get location
      uint_least8_t location() const;
      /// set location
      void setLocation(uint_least8_t);

      /// get peak
      uint_least8_t peak() const;
      /// set peak
      void setPeak(uint_least8_t);

      /// get energyLargeVec
      const std::vector<uint_least16_t>& energyLargeVec() const;
      /// set energyLargeVec
      void setEnergyLargeVec(const std::vector<uint_least16_t>&);

      /// get energySmallVec
      const std::vector<uint_least16_t>& energySmallVec() const;
      /// set energySmallVec
      void setEnergySmallVec(const std::vector<uint_least16_t>&);

      /// get errorVec
      const std::vector<uint_least8_t>& errorVec() const;
      /// set errorVec
      void setErrorVec(const std::vector<uint_least8_t>&);

      /// get presenceMapVec
      const std::vector<uint_least16_t>& presenceMapVec() const;
      /// set presenceMapVec
      void setPresenceMapVec(const std::vector<uint_least16_t>&);

      /// Peak functions - return value of peak bunch crossing
      /// Assuming we have got the timing calibration correct,
      /// This is the bunch crossing containing the collision

      /// get energyLargeVec at peak bunch crossing
      uint_least16_t energyLarge() const;

      /// get energySmallVec at peak bunch crossing
      uint_least16_t energySmall() const;

      /// get errorVec at peak bunch crossing
      uint_least8_t error() const;

      /// get presenceMapVec at peak bunch crossing
      uint_least16_t presenceMap() const;

  }; // class CMXJetTob_v2
} // namespace xAOD
#endif

