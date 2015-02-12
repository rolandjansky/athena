// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXCPTob_v2.h 646335 2015-02-12 01:16:10Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_CMXCPTOB_V2_H
#define XAODTRIGL1CALO_VERSIONS_CMXCPTOB_V2_H

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

// System include(s):
#include <stdint.h>

namespace xAOD{

  /// Description of CMXCPTob_v2
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 646335 $
  /// $Date: 2015-02-12 02:16:10 +0100 (Thu, 12 Feb 2015) $

  class CMXCPTob_v2 : public SG::AuxElement {
    public:
      /// Default constructor
      CMXCPTob_v2();
      /// Default desturctor
      virtual ~CMXCPTob_v2(){}      
      
      /// initialize     
      virtual void initialize(const uint_least8_t crate,const uint_least8_t cmx,
                              const uint_least8_t cpm,const uint_least8_t chip,const uint_least8_t loc);
      
      /// initialize
      virtual void initialize(const uint_least8_t crate,const uint_least8_t cmx,
                              const uint_least8_t cpm,const uint_least8_t chip,const uint_least8_t loc,
                              const std::vector<uint_least8_t>& energyVec,
                              const std::vector<uint_least8_t>& isolationVec,
                              const std::vector<uint_least8_t>& errorVec,
                              const std::vector<uint_least16_t>& presenceMapVec,
                              const uint_least8_t peak);
      
      /// add data to existing object
      void addTob(const std::vector<uint_least8_t>& energyVec,
                  const std::vector<uint_least8_t>& isolationVec,
                  const std::vector<uint_least8_t>& errorVec,
                  const std::vector<uint_least16_t>& presenceMapVec);

      /// get crate
      uint_least8_t crate() const;
      /// set crate
      void setCrate(uint_least8_t);

      /// get cmx
      uint_least8_t cmx() const;
      /// set cmx
      void setCmx(uint_least8_t);

      /// get cpm
      uint_least8_t cpm() const;
      /// set cpm
      void setCpm(uint_least8_t);

      /// get chip
      uint_least8_t chip() const;
      /// set chip
      void setChip(uint_least8_t);

      /// get location
      uint_least8_t location() const;
      /// set location
      void setLocation(uint_least8_t);

      /// get peak
      uint_least8_t peak() const;
      /// set peak
      void setPeak(uint_least8_t);

      /// get energyVec
      const std::vector<uint_least8_t>& energyVec() const;
      /// set energyVec
      void setEnergyVec(const std::vector<uint_least8_t>&);

      /// get isolationVec
      const std::vector<uint_least8_t>& isolationVec() const;
      /// set isolationVec
      void setIsolationVec(const std::vector<uint_least8_t>&);

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

      /// get energyVec at peak bunch crossing
      uint_least8_t energy() const;

      /// get isolationVec at peak bunch crossing
      uint_least8_t isolation() const;

      /// get errorVec at peak bunch crossing
      uint_least8_t error() const;

      /// get presenceMapVec at peak bunch crossing
      uint_least16_t presenceMap() const;

  }; // class CMXCPTob_v2
} // namespace xAOD
#endif

