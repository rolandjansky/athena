// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXCPHits_v1.h 687949 2015-08-06 15:48:49Z amazurov $
#ifndef XAODTRIGL1CALO_VERSIONS_CMXCPHITS_V1_H
#define XAODTRIGL1CALO_VERSIONS_CMXCPHITS_V1_H

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

// System include(s):
#include <stdint.h>

namespace xAOD{

  /// Description of CMXCPHits_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 687949 $
  /// $Date: 2015-08-06 17:48:49 +0200 (Thu, 06 Aug 2015) $

  class CMXCPHits_v1 : public SG::AuxElement {
    public:
      enum Sources { REMOTE_0, REMOTE_1, REMOTE_2, LOCAL, TOTAL,
                     TOPO_CHECKSUM, TOPO_OCCUPANCY_MAP, TOPO_OCCUPANCY_COUNTS,
                     MAXSOURCE };
    public:
      /// Default constructor
      CMXCPHits_v1();
      /// Default desturctor
      virtual ~CMXCPHits_v1(){}
      
      /// initialize
      virtual void initialize(const uint8_t crate,const uint8_t cmx,const uint8_t source);
      
      /// initialize
      virtual void initialize(const uint8_t crate,const uint8_t cmx,const uint8_t source,
                              const std::vector<uint32_t>& hitsVec0,
                              const std::vector<uint32_t>& hitsVec1,
                              const std::vector<uint32_t>& errorVec0,
                              const std::vector<uint32_t>& errorVec1,
                              const uint8_t peak);
      
      /// add hits to existing object
      void addHits(const std::vector<uint32_t>& hitsVec0,
                   const std::vector<uint32_t>& hitsVec1,
                   const std::vector<uint32_t>& errorVec0,
                   const std::vector<uint32_t>& errorVec1);
      
      /// get crate
      uint8_t crate() const;
      /// set crate
      void setCrate(uint8_t);

      /// get cmx
      uint8_t cmx() const;
      /// set cmx
      void setCmx(uint8_t);

      /// get source
      uint8_t sourceComponent() const;
      /// set source
      void setSourceComponent(uint8_t);

      /// get peak
      uint8_t peak() const;
      /// set peak
      void setPeak(uint8_t);

      /// get hitsVec0
      const std::vector<uint32_t>& hitsVec0() const;
      /// set hitsVec0
      void setHitsVec0(const std::vector<uint32_t>&);

      /// get hitsVec1
      const std::vector<uint32_t>& hitsVec1() const;
      /// set hitsVec1
      void setHitsVec1(const std::vector<uint32_t>&);

      /// get errorVec0
      const std::vector<uint32_t>& errorVec0() const;
      /// set errorVec0
      void setErrorVec0(const std::vector<uint32_t>&);

      /// get errorVec1
      const std::vector<uint32_t>& errorVec1() const;
      /// set errorVec1
      void setErrorVec1(const std::vector<uint32_t>&);

      /// Peak functions - return value of peak bunch crossing
      /// Assuming we have got the timing calibration correct,
      /// This is the bunch crossing containing the collision

      /// get hitsVec0 at peak bunch crossing
      uint32_t hits0() const;

      /// get hitsVec1 at peak bunch crossing
      uint32_t hits1() const;

      /// get errorVec0 at peak bunch crossing
      uint32_t error0() const;

      /// get errorVec1 at peak bunch crossing
      uint32_t error1() const;

  }; // class CMXCPHits_v1
} // namespace xAOD
#endif

