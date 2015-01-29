// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXJetHits_v2.h 642659 2015-01-29 12:41:06Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_CMXJETHITS_V2_H
#define XAODTRIGL1CALO_VERSIONS_CMXJETHITS_V2_H

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

// System include(s):
#include <stdint.h>

namespace xAOD{

  /// Description of CMXJetHits_v2
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 642659 $
  /// $Date: 2015-01-29 13:41:06 +0100 (Thu, 29 Jan 2015) $

  class CMXJetHits_v2 : public SG::AuxElement {
    public:
      /// Default constructor
      CMXJetHits_v2();
      /// Alternative constructor
      CMXJetHits_v2(const int crate,const int source);
      /// Alternative constructor
      CMXJetHits_v2(const int crate,const int source,
                    const std::vector<unsigned int>& hitsVec0,
                    const std::vector<unsigned int>& hitsVec1,
                    const std::vector<int>& errorVec0,
                    const std::vector<int>& errorVec1,
                    const uint_least8_t peak);
      
      /// add data to existing object
      void addHits(const std::vector<unsigned int>& hitsVec0,
                   const std::vector<unsigned int>& hitsVec1,
                   const std::vector<int>& errorVec0,
                   const std::vector<int>& errorVec1);
      
      /// get crate
      int crate() const;
      /// set crate
      void setCrate(int);

      /// get source
      int source() const;
      /// set source
      void setSource(int);

      /// get peak
      uint_least8_t peak() const;
      /// set peak
      void setPeak(uint_least8_t);

      /// get hitsVec0
      const std::vector<unsigned int>& hitsVec0() const;
      /// set hitsVec0
      void setHitsVec0(const std::vector<unsigned int>&);

      /// get hitsVec1
      const std::vector<unsigned int>& hitsVec1() const;
      /// set hitsVec1
      void setHitsVec1(const std::vector<unsigned int>&);

      /// get errorVec0
      const std::vector<int>& errorVec0() const;
      /// set errorVec0
      void setErrorVec0(const std::vector<int>&);

      /// get errorVec1
      const std::vector<int>& errorVec1() const;
      /// set errorVec1
      void setErrorVec1(const std::vector<int>&);

      /// Peak functions - return value of peak bunch crossing
      /// Assuming we have got the timing calibration correct,
      /// This is the bunch crossing containing the collision

      /// get hitsVec0 at peak bunch crossing
      unsigned int hits0() const;

      /// get hitsVec1 at peak bunch crossing
      unsigned int hits1() const;

      /// get errorVec0 at peak bunch crossing
      int error0() const;

      /// get errorVec1 at peak bunch crossing
      int error1() const;

  }; // class CMXJetHits_v2
} // namespace xAOD
#endif

