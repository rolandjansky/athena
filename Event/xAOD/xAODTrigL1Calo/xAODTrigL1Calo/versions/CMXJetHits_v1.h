// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXJetHits_v1.h 663426 2015-04-28 11:45:23Z amazurov $
#ifndef XAODTRIGL1CALO_VERSIONS_CMXJETHITS_V1_H
#define XAODTRIGL1CALO_VERSIONS_CMXJETHITS_V1_H

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

// STL include(s):
#include <vector>
// System include(s):
#include <stdint.h>

namespace xAOD{

  /// Description of CMXJetHits_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 663426 $
  /// $Date: 2015-04-28 13:45:23 +0200 (Tue, 28 Apr 2015) $

  class CMXJetHits_v1 : public SG::AuxElement {
    public:
      enum Sources {
        REMOTE_MAIN = 0,
        LOCAL_MAIN = 1,
        TOTAL_MAIN = 2,
        REMOTE_FORWARD = 4,
        LOCAL_FORWARD = 5,
        TOTAL_FORWARD = 6,
        TOPO_CHECKSUM = 8,
        TOPO_OCCUPANCY_MAP = 9,
        TOPO_OCCUPANCY_COUNTS = 10,
        MAX_SOURCE = 15
      };
    public:
      /// Default constructor
      CMXJetHits_v1();
      /// Default desturctor
      virtual ~CMXJetHits_v1(){}        
      
      /// initialize
      virtual void initialize(const uint8_t crate,const uint8_t source);
      
      /// initialize
      virtual void initialize(const uint8_t crate,const uint8_t source,
                              const std::vector<uint8_t>& hitsVec0,
                              const std::vector<uint8_t>& hitsVec1,
                              const std::vector<uint8_t>& errorVec0,
                              const std::vector<uint8_t>& errorVec1,
                              const uint8_t peak);
      
      /// add data to existing object
      void addHits(const std::vector<uint8_t>& hitsVec0,
                   const std::vector<uint8_t>& hitsVec1,
                   const std::vector<uint8_t>& errorVec0,
                   const std::vector<uint8_t>& errorVec1);
      
      /// get crate
      uint8_t crate() const;
      /// set crate
      void setCrate(uint8_t);

      /// get source
      uint8_t sourceComponent() const;
      /// set source
      void setSourceComponent(uint8_t);

      /// get peak
      uint8_t peak() const;
      /// set peak
      void setPeak(uint8_t);

      /// get hitsVec0
      const std::vector<uint8_t>& hitsVec0() const;
      /// set hitsVec0
      void setHitsVec0(const std::vector<uint8_t>&);

      /// get hitsVec1
      const std::vector<uint8_t>& hitsVec1() const;
      /// set hitsVec1
      void setHitsVec1(const std::vector<uint8_t>&);

      /// get errorVec0
      const std::vector<uint8_t>& errorVec0() const;
      /// set errorVec0
      void setErrorVec0(const std::vector<uint8_t>&);

      /// get errorVec1
      const std::vector<uint8_t>& errorVec1() const;
      /// set errorVec1
      void setErrorVec1(const std::vector<uint8_t>&);

      /// Peak functions - return value of peak bunch crossing
      /// Assuming we have got the timing calibration correct,
      /// This is the bunch crossing containing the collision

      /// get hitsVec0 at peak bunch crossing
      uint8_t hits0() const;

      /// get hitsVec1 at peak bunch crossing
      uint8_t hits1() const;

      /// get errorVec0 at peak bunch crossing
      uint8_t error0() const;

      /// get errorVec1 at peak bunch crossing
      uint8_t error1() const;

  }; // class CMXJetHits_v1
} // namespace xAOD
#endif

