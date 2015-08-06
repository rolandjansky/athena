// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXCPTob_v1.h 687949 2015-08-06 15:48:49Z amazurov $
#ifndef XAODTRIGL1CALO_VERSIONS_CMXCPTOB_V1_H
#define XAODTRIGL1CALO_VERSIONS_CMXCPTOB_V1_H

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

// System include(s):
#include <stdint.h>

namespace xAOD{

  /// Description of CMXCPTob_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 687949 $
  /// $Date: 2015-08-06 17:48:49 +0200 (Thu, 06 Aug 2015) $

  class CMXCPTob_v1 : public SG::AuxElement {
    public:
      /// Default constructor
      CMXCPTob_v1();
      /// Default desturctor
      virtual ~CMXCPTob_v1(){}      
      
      /// initialize     
      virtual void initialize(const uint8_t crate,const uint8_t cmx,
                              const uint8_t cpm,const uint8_t chip,const uint8_t loc);
      
      /// initialize
      virtual void initialize(const uint8_t crate,const uint8_t cmx,
                              const uint8_t cpm,const uint8_t chip,const uint8_t loc,
                              const std::vector<uint8_t>& energyVec,
                              const std::vector<uint8_t>& isolationVec,
                              const std::vector<uint32_t>& errorVec,
                              const std::vector<uint16_t>& presenceMapVec,
                              const uint8_t peak);
      
      /// add data to existing object
      void addTob(const std::vector<uint8_t>& energyVec,
                  const std::vector<uint8_t>& isolationVec,
                  const std::vector<uint32_t>& errorVec,
                  const std::vector<uint16_t>& presenceMapVec);

      /// get crate
      uint8_t crate() const;
      /// set crate
      void setCrate(uint8_t);

      /// get cmx
      uint8_t cmx() const;
      /// set cmx
      void setCmx(uint8_t);

      /// get cpm
      uint8_t cpm() const;
      /// set cpm
      void setCpm(uint8_t);

      /// get chip
      uint8_t chip() const;
      /// set chip
      void setChip(uint8_t);

      /// get location
      uint8_t location() const;
      /// set location
      void setLocation(uint8_t);

      /// get peak
      uint8_t peak() const;
      /// set peak
      void setPeak(uint8_t);

      /// get energyVec
      const std::vector<uint8_t>& energyVec() const;
      /// set energyVec
      void setEnergyVec(const std::vector<uint8_t>&);

      /// get isolationVec
      const std::vector<uint8_t>& isolationVec() const;
      /// set isolationVec
      void setIsolationVec(const std::vector<uint8_t>&);

      /// get errorVec
      const std::vector<uint32_t>& errorVec() const;
      /// set errorVec
      void setErrorVec(const std::vector<uint32_t>&);

      /// get presenceMapVec
      const std::vector<uint16_t>& presenceMapVec() const;
      /// set presenceMapVec
      void setPresenceMapVec(const std::vector<uint16_t>&);

      /// Peak functions - return value of peak bunch crossing
      /// Assuming we have got the timing calibration correct,
      /// This is the bunch crossing containing the collision

      /// get energyVec at peak bunch crossing
      uint8_t energy() const;

      /// get isolationVec at peak bunch crossing
      uint8_t isolation() const;

      /// get errorVec at peak bunch crossing
      uint32_t error() const;

      /// get presenceMapVec at peak bunch crossing
      uint16_t presenceMap() const;

  }; // class CMXCPTob_v1
} // namespace xAOD
#endif

