// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXCPTob_v2.h 642659 2015-01-29 12:41:06Z morrisj $
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
  /// $Revision: 642659 $
  /// $Date: 2015-01-29 13:41:06 +0100 (Thu, 29 Jan 2015) $

  class CMXCPTob_v2 : public SG::AuxElement {
    public:
      /// Default constructor
      CMXCPTob_v2();
      /// Alternative constructor
      CMXCPTob_v2(const int crate,const int cmx,const int cpm,const int chip,const int loc);
      /// Alternative constructor
      CMXCPTob_v2(const int crate,const int cmx,const int cpm,const int chip,const int loc,
                  const std::vector<int>& energyVec,
                  const std::vector<int>& isolationVec,
                  const std::vector<int>& errorVec,
                  const std::vector<unsigned int>& presenceMapVec,
                  const uint_least8_t peak);
      
      /// add data to existing object
      void addTob(const std::vector<int>& energyVec,
                  const std::vector<int>& isolationVec,
                  const std::vector<int>& errorVec,
                  const std::vector<unsigned int>& presenceMapVec);

      /// get crate
      int crate() const;
      /// set crate
      void setCrate(int);

      /// get cmx
      int cmx() const;
      /// set cmx
      void setCmx(int);

      /// get cpm
      int cpm() const;
      /// set cpm
      void setCpm(int);

      /// get chip
      int chip() const;
      /// set chip
      void setChip(int);

      /// get location
      int location() const;
      /// set location
      void setLocation(int);

      /// get peak
      uint_least8_t peak() const;
      /// set peak
      void setPeak(uint_least8_t);

      /// get energyVec
      const std::vector<int>& energyVec() const;
      /// set energyVec
      void setEnergyVec(const std::vector<int>&);

      /// get isolationVec
      const std::vector<int>& isolationVec() const;
      /// set isolationVec
      void setIsolationVec(const std::vector<int>&);

      /// get errorVec
      const std::vector<int>& errorVec() const;
      /// set errorVec
      void setErrorVec(const std::vector<int>&);

      /// get presenceMapVec
      const std::vector<unsigned int>& presenceMapVec() const;
      /// set presenceMapVec
      void setPresenceMapVec(const std::vector<unsigned int>&);

      /// Peak functions - return value of peak bunch crossing
      /// Assuming we have got the timing calibration correct,
      /// This is the bunch crossing containing the collision

      /// get energyVec at peak bunch crossing
      int energy() const;

      /// get isolationVec at peak bunch crossing
      int isolation() const;

      /// get errorVec at peak bunch crossing
      int error() const;

      /// get presenceMapVec at peak bunch crossing
      unsigned int presenceMap() const;

  }; // class CMXCPTob_v2
} // namespace xAOD
#endif

