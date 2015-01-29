// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXJetTob_v2.h 642659 2015-01-29 12:41:06Z morrisj $
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
  /// $Revision: 642659 $
  /// $Date: 2015-01-29 13:41:06 +0100 (Thu, 29 Jan 2015) $

  class CMXJetTob_v2 : public SG::AuxElement {
    public:
      /// Default constructor
      CMXJetTob_v2();
      /// Alternative constructor
      CMXJetTob_v2(const int crate,const int jem,const int frame,const int loc);
      /// Alternative constructor
      CMXJetTob_v2(const int crate,const int jem,const int frame,const int loc,
                   const std::vector<int>& energyLargeVec,
                   const std::vector<int>& energySmallVec,
                   const std::vector<int>& errorVec,
                   const std::vector<unsigned int>& presenceMapVec,
                   const uint_least8_t peak);
      
      /// add data to existing object
      void addTob(const std::vector<int>& energyLargeVec,
                  const std::vector<int>& energySmallVec,
                  const std::vector<int>& errorVec,
                  const std::vector<unsigned int>& presenceMapVec);
      
      
      /// get crate
      int crate() const;
      /// set crate
      void setCrate(int);

      /// get jem
      int jem() const;
      /// set jem
      void setJem(int);

      /// get frame
      int frame() const;
      /// set frame
      void setFrame(int);

      /// get location
      int location() const;
      /// set location
      void setLocation(int);

      /// get peak
      uint_least8_t peak() const;
      /// set peak
      void setPeak(uint_least8_t);

      /// get energyLargeVec
      const std::vector<int>& energyLargeVec() const;
      /// set energyLargeVec
      void setEnergyLargeVec(const std::vector<int>&);

      /// get energySmallVec
      const std::vector<int>& energySmallVec() const;
      /// set energySmallVec
      void setEnergySmallVec(const std::vector<int>&);

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

      /// get energyLargeVec at peak bunch crossing
      int energyLarge() const;

      /// get energySmallVec at peak bunch crossing
      int energySmall() const;

      /// get errorVec at peak bunch crossing
      int error() const;

      /// get presenceMapVec at peak bunch crossing
      unsigned int presenceMap() const;

  }; // class CMXJetTob_v2
} // namespace xAOD
#endif

