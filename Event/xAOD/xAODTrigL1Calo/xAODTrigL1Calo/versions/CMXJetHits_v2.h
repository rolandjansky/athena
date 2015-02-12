// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXJetHits_v2.h 646335 2015-02-12 01:16:10Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_CMXJETHITS_V2_H
#define XAODTRIGL1CALO_VERSIONS_CMXJETHITS_V2_H

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

// STL include(s):
#include <vector>
// System include(s):
#include <stdint.h>

namespace xAOD{

  /// Description of CMXJetHits_v2
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 646335 $
  /// $Date: 2015-02-12 02:16:10 +0100 (Thu, 12 Feb 2015) $

  class CMXJetHits_v2 : public SG::AuxElement {
    public:
      /// Default constructor
      CMXJetHits_v2();
      /// Default desturctor
      virtual ~CMXJetHits_v2(){}        
      
      /// initialize
      virtual void initialize(const uint_least8_t crate,const uint_least8_t source);
      
      /// initialize
      virtual void initialize(const uint_least8_t crate,const uint_least8_t source,
                              const std::vector<uint_least8_t>& hitsVec0,
                              const std::vector<uint_least8_t>& hitsVec1,
                              const std::vector<uint_least8_t>& errorVec0,
                              const std::vector<uint_least8_t>& errorVec1,
                              const uint_least8_t peak);
      
      /// add data to existing object
      void addHits(const std::vector<uint_least8_t>& hitsVec0,
                   const std::vector<uint_least8_t>& hitsVec1,
                   const std::vector<uint_least8_t>& errorVec0,
                   const std::vector<uint_least8_t>& errorVec1);
      
      /// get crate
      uint_least8_t crate() const;
      /// set crate
      void setCrate(uint_least8_t);

      /// get source
      uint_least8_t source() const;
      /// set source
      void setSource(uint_least8_t);

      /// get peak
      uint_least8_t peak() const;
      /// set peak
      void setPeak(uint_least8_t);

      /// get hitsVec0
      const std::vector<uint_least8_t>& hitsVec0() const;
      /// set hitsVec0
      void setHitsVec0(const std::vector<uint_least8_t>&);

      /// get hitsVec1
      const std::vector<uint_least8_t>& hitsVec1() const;
      /// set hitsVec1
      void setHitsVec1(const std::vector<uint_least8_t>&);

      /// get errorVec0
      const std::vector<uint_least8_t>& errorVec0() const;
      /// set errorVec0
      void setErrorVec0(const std::vector<uint_least8_t>&);

      /// get errorVec1
      const std::vector<uint_least8_t>& errorVec1() const;
      /// set errorVec1
      void setErrorVec1(const std::vector<uint_least8_t>&);

      /// Peak functions - return value of peak bunch crossing
      /// Assuming we have got the timing calibration correct,
      /// This is the bunch crossing containing the collision

      /// get hitsVec0 at peak bunch crossing
      uint_least8_t hits0() const;

      /// get hitsVec1 at peak bunch crossing
      uint_least8_t hits1() const;

      /// get errorVec0 at peak bunch crossing
      uint_least8_t error0() const;

      /// get errorVec1 at peak bunch crossing
      uint_least8_t error1() const;

  }; // class CMXJetHits_v2
} // namespace xAOD
#endif

