// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXEtSums_v2.h 642659 2015-01-29 12:41:06Z morrisj $
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
  /// $Revision: 642659 $
  /// $Date: 2015-01-29 13:41:06 +0100 (Thu, 29 Jan 2015) $

  class CMXEtSums_v2 : public SG::AuxElement {
    public:
      /// Default constructor
      CMXEtSums_v2();
      /// Alternative constructor
      CMXEtSums_v2(const int crate,const int source);
      /// Alternative constructor
      CMXEtSums_v2(const int crate,const int source,
                   const std::vector<unsigned int>& etVec,
                   const std::vector<unsigned int>& exVec,
                   const std::vector<unsigned int>& eyVec,
                   const std::vector<int>& etErrorVec,
                   const std::vector<int>& exErrorVec,
                   const std::vector<int>& eyErrorVec,
                   const uint_least8_t peak);
      
      /// add data to existing object
      /// add Et
      void addEt(const std::vector<unsigned int>& etVec,const std::vector<int>& etErrorVec);
      /// add Ex
      void addEx(const std::vector<unsigned int>& exVec,const std::vector<int>& exErrorVec);      
      /// add Ey
      void addEy(const std::vector<unsigned int>& eyVec,const std::vector<int>& eyErrorVec);      
      
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

      /// get etVec
      const std::vector<unsigned int>& etVec() const;
      /// set etVec
      void setEtVec(const std::vector<unsigned int>&);

      /// get exVec
      const std::vector<unsigned int>& exVec() const;
      /// set exVec
      void setExVec(const std::vector<unsigned int>&);

      /// get eyVec
      const std::vector<unsigned int>& eyVec() const;
      /// set eyVec
      void setEyVec(const std::vector<unsigned int>&);

      /// get etErrorVec
      const std::vector<int>& etErrorVec() const;
      /// set etErrorVec
      void setEtErrorVec(const std::vector<int>&);

      /// get exErrorVec
      const std::vector<int>& exErrorVec() const;
      /// set exErrorVec
      void setExErrorVec(const std::vector<int>&);

      /// get eyErrorVec
      const std::vector<int>& eyErrorVec() const;
      /// set eyErrorVec
      void setEyErrorVec(const std::vector<int>&);

      /// Peak functions - return value of peak bunch crossing
      /// Assuming we have got the timing calibration correct,
      /// This is the bunch crossing containing the collision

      /// get etVec at peak bunch crossing
      unsigned int et() const;

      /// get exVec at peak bunch crossing
      unsigned int ex() const;

      /// get eyVec at peak bunch crossing
      unsigned int ey() const;

      /// get etErrorVec at peak bunch crossing
      int etError() const;

      /// get exErrorVec at peak bunch crossing
      int exError() const;

      /// get eyErrorVec at peak bunch crossing
      int eyError() const;

  }; // class CMXEtSums_v2
} // namespace xAOD
#endif

