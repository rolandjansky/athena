// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXEtSums_v2.h 646335 2015-02-12 01:16:10Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_CMXETSUMS_V2_H
#define XAODTRIGL1CALO_VERSIONS_CMXETSUMS_V2_H

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

// System include(s):
#include <stdint.h>

namespace xAOD{

  /// Description of CMXEtSums_v2
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 646335 $
  /// $Date: 2015-02-12 02:16:10 +0100 (Thu, 12 Feb 2015) $

  class CMXEtSums_v2 : public SG::AuxElement {
    public:
      /// Default constructor
      CMXEtSums_v2();
      /// Default desturctor
      virtual ~CMXEtSums_v2(){}       
      
      /// initialize
      virtual void initialize(const uint_least8_t crate,const uint_least8_t source);
      
      /// initialize
      virtual void initialize(const uint_least8_t crate,const uint_least8_t source,
                              const std::vector<uint_least16_t>& etVec,
                              const std::vector<uint_least16_t>& exVec,
                              const std::vector<uint_least16_t>& eyVec,
                              const std::vector<uint_least8_t>& etErrorVec,
                              const std::vector<uint_least8_t>& exErrorVec,
                              const std::vector<uint_least8_t>& eyErrorVec,
                              const uint_least8_t peak);
      
      /// add data to existing object
      /// add Et
      void addEt(const std::vector<uint_least16_t>& etVec,const std::vector<uint_least8_t>& etErrorVec);
      /// add Ex
      void addEx(const std::vector<uint_least16_t>& exVec,const std::vector<uint_least8_t>& exErrorVec);      
      /// add Ey
      void addEy(const std::vector<uint_least16_t>& eyVec,const std::vector<uint_least8_t>& eyErrorVec);      
      
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

      /// get etVec
      const std::vector<uint_least16_t>& etVec() const;
      /// set etVec
      void setEtVec(const std::vector<uint_least16_t>&);

      /// get exVec
      const std::vector<uint_least16_t>& exVec() const;
      /// set exVec
      void setExVec(const std::vector<uint_least16_t>&);

      /// get eyVec
      const std::vector<uint_least16_t>& eyVec() const;
      /// set eyVec
      void setEyVec(const std::vector<uint_least16_t>&);

      /// get etErrorVec
      const std::vector<uint_least8_t>& etErrorVec() const;
      /// set etErrorVec
      void setEtErrorVec(const std::vector<uint_least8_t>&);

      /// get exErrorVec
      const std::vector<uint_least8_t>& exErrorVec() const;
      /// set exErrorVec
      void setExErrorVec(const std::vector<uint_least8_t>&);

      /// get eyErrorVec
      const std::vector<uint_least8_t>& eyErrorVec() const;
      /// set eyErrorVec
      void setEyErrorVec(const std::vector<uint_least8_t>&);

      /// Peak functions - return value of peak bunch crossing
      /// Assuming we have got the timing calibration correct,
      /// This is the bunch crossing containing the collision

      /// get etVec at peak bunch crossing
      uint_least16_t et() const;

      /// get exVec at peak bunch crossing
      uint_least16_t ex() const;

      /// get eyVec at peak bunch crossing
      uint_least16_t ey() const;

      /// get etErrorVec at peak bunch crossing
      uint_least8_t etError() const;

      /// get exErrorVec at peak bunch crossing
      uint_least8_t exError() const;

      /// get eyErrorVec at peak bunch crossing
      uint_least8_t eyError() const;

  }; // class CMXEtSums_v2
} // namespace xAOD
#endif

