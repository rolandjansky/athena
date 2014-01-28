// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMTower_v1.h 576279 2013-12-19 14:35:54Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_CPMTOWER_V1_H
#define XAODTRIGL1CALO_VERSIONS_CPMTOWER_V1_H

#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

namespace xAOD {

  /// Description of CPMTower_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 576279 $
  /// $Date: 2013-12-19 15:35:54 +0100 (Thu, 19 Dec 2013) $  
  
  class CPMTower_v1 : public SG::AuxElement{
    public:
      // Default constructor
      CPMTower_v1();
      
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

      int peak() const;
      void setPeak(int);     
        
              
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

// Set up a CLID for the class:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
  CLASS_DEF( xAOD::CPMTower_v1 , 14223686 , 1 )
#endif // not XAOD_STANDALONE
#endif // XAODTRIGL1CALO_VERSIONS_CPMTOWER_V1_H
