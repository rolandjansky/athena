// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMTower_v2.h 652807 2015-03-09 21:52:07Z morrisj $
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
  /// $Revision: 652807 $
  /// $Date: 2015-03-09 22:52:07 +0100 (Mon, 09 Mar 2015) $  
  
  class CPMTower_v2 : public SG::AuxElement{
    public:
      /// Default constructor
      CPMTower_v2();
      /// Default desturctor
      virtual ~CPMTower_v2(){}        
      
      /// initialize
      virtual void initialize(const float eta,const float phi);
      
      /// initialize
      virtual void initialize(const float eta,const float phi,
                              const std::vector<uint8_t>& emEnergyVec,
                              const std::vector<uint8_t>& hadEnergyVec,
                              const std::vector<uint8_t>& emErrorVec,
                              const std::vector<uint8_t>& hadErrorVec,
                              const uint8_t peak);
      
      /// get emEnergyVec - emEnergy for all time slices
      const std::vector<uint8_t>& emEnergyVec() const;
      /// set emEnergyVec - emEnergy for all time slices
      void setEmEnergyVec(const std::vector<uint8_t>&);
      
      /// get hadEnergyVec - hadEnergy for all time slices
      const std::vector<uint8_t>& hadEnergyVec() const;
      /// set hadEnergyVec - hadEnergy for all time slices
      void setHadEnergyVec(const std::vector<uint8_t>&);  
      
      /// get emErrorVec - emError for all time slices
      const std::vector<uint8_t>& emErrorVec() const;
      /// set emErrorVec - emError for all time slices
      void setEmErrorVec(const std::vector<uint8_t>&);
      
      /// get hadErrorVec - hadError for all time slices
      const std::vector<uint8_t>& hadErrorVec() const;
      /// set hadErrorVec - hadError for all time slices
      void setHadErrorVec(const std::vector<uint8_t>&);      
          
      /// get eta
      float eta() const;
      /// set eta
      void setEta(float);
      
      /// get phi (note that for L1Calo phi runs from 0 to 2pi)
      float phi() const;
      /// set phi
      void setPhi(float);

      uint8_t peak() const;
      void setPeak(uint8_t);     
        
              
      /// get emEnergy for emEnergyVec[peak]  - time slice that (hopefully) contains the collision
      uint8_t emEnergy() const;
      /// get hadEnergy for hadEnergyVec[peak]  - time slice that (hopefully) contains the collision      
      uint8_t hadEnergy() const;

      /// get emEnergy for emEnergyVec[slice] - time slice for arbitary slice
      uint8_t emSliceEnergy(unsigned int slice) const;
      /// get hadEnergy for hadEnergyVec[slice] - time slice for arbitary slice
      uint8_t hadSliceEnergy(unsigned int slice) const;

      // get emError for emErrorVec[peak]  - time slice that (hopefully) contains the collision
      uint8_t emError() const;
      // get hadError for hadErrorVec[peak]  - time slice that (hopefully) contains the collision
      uint8_t hadError() const;
      
      /// get emError for emErrorVec[slice] - time slice for arbitary slice
      uint8_t emSliceError(unsigned int slice) const;
      /// get hadError for hadErrorVec[slice] - time slice for arbitary slice
      uint8_t hadSliceError(unsigned int slice) const;

  };
} // namespace xAOD

#endif // XAODTRIGL1CALO_VERSIONS_CPMTOWER_V2_H
