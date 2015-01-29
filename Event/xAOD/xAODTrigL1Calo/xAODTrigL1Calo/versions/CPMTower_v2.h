// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMTower_v2.h 642659 2015-01-29 12:41:06Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_CPMTOWER_V2_H
#define XAODTRIGL1CALO_VERSIONS_CPMTOWER_V2_H

#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

// System include(s):
#include <stdint.h>

namespace xAOD {

  /// Description of CPMTower_v2
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 642659 $
  /// $Date: 2015-01-29 13:41:06 +0100 (Thu, 29 Jan 2015) $  
  
  class CPMTower_v2 : public SG::AuxElement{
    public:
      // Default constructor
      CPMTower_v2();
      
      /// get emEnergyVec - emEnergy for all time slices
      const std::vector<int>& emEnergyVec() const;
      /// set emEnergyVec - emEnergy for all time slices
      void setEmEnergyVec(const std::vector<int>&);
      
      /// get hadEnergyVec - hadEnergy for all time slices
      const std::vector<int>& hadEnergyVec() const;
      /// set hadEnergyVec - hadEnergy for all time slices
      void setHadEnergyVec(const std::vector<int>&);  
      
      /// get emErrorVec - emError for all time slices
      const std::vector<int>& emErrorVec() const;
      /// set emErrorVec - emError for all time slices
      void setEmErrorVec(const std::vector<int>&);
      
      /// get hadErrorVec - hadError for all time slices
      const std::vector<int>& hadErrorVec() const;
      /// set hadErrorVec - hadError for all time slices
      void setHadErrorVec(const std::vector<int>&);      
          
      /// get eta
      float eta() const;
      /// set eta
      void setEta(float);
      
      /// get phi (note that for L1Calo phi runs from 0 to 2pi)
      float phi() const;
      /// set phi
      void setPhi(float);

      uint_least8_t peak() const;
      void setPeak(uint_least8_t);     
        
              
      /// get emEnergy for emEnergyVec[peak]  - time slice that (hopefully) contains the collision
      int emEnergy() const;
      /// get hadEnergy for hadEnergyVec[peak]  - time slice that (hopefully) contains the collision      
      int hadEnergy() const;

      /// get emEnergy for emEnergyVec[slice] - time slice for arbitary slice
      int emSliceEnergy(int slice) const;
      /// get hadEnergy for hadEnergyVec[slice] - time slice for arbitary slice
      int hadSliceEnergy(int slice) const;

      // get emError for emErrorVec[peak]  - time slice that (hopefully) contains the collision
      int emError() const;
      // get hadError for hadErrorVec[peak]  - time slice that (hopefully) contains the collision
      int hadError() const;
      
      /// get emError for emErrorVec[slice] - time slice for arbitary slice
      int emSliceError(int slice) const;
      /// get hadError for hadErrorVec[slice] - time slice for arbitary slice
      int hadSliceError(int slice) const;

  };
} // namespace xAOD

#endif // XAODTRIGL1CALO_VERSIONS_CPMTOWER_V2_H
