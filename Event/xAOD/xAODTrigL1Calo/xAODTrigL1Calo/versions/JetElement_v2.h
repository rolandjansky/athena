// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetElement_v2.h 642659 2015-01-29 12:41:06Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_JETELEMENT_V2_H
#define XAODTRIGL1CALO_VERSIONS_JETELEMENT_V2_H

#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

// System include(s):
#include <stdint.h>

namespace xAOD {

  /// Description of JetElement_v2
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 642659 $
  /// $Date: 2015-01-29 13:41:06 +0100 (Thu, 29 Jan 2015) $  
  
  class JetElement_v2 : public SG::AuxElement{
    public:
      // Default constructor
      JetElement_v2();
      
      /// get phi (note that for L1Calo phi runs from 0 to 2pi)
      float phi() const;
      /// set phi
      void setPhi(float);
      
      /// get eta
      float eta() const;
      /// set eta
      void setEta(float); 
      
      /// get key
      unsigned int key() const;
      /// set key
      void setKey(unsigned int);
      
      /// get peak
      uint_least8_t peak() const;
      /// set key
      void setPeak(uint_least8_t);
      
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

      /// get linkErrorVec - linkError for all time slices
      const std::vector<int>& linkErrorVec() const;
      /// set linkErrorVec - linkError for all time slices
      void setLinkErrorVec(const std::vector<int>&); 
      
      /// Add ET to triggered time slice 
      void addEnergy(int emEnergy, int hadEnergy);
      /// Add ET values to specified slice 
      void addSlice(int slice, int emEnergy, int hadEnergy,int emError, int hadError, int linkError);

      /// get emEnery for emEnergyVec[peak]  - time slice that (hopefully) contains the collision  
      int emEnergy()  const;
      /// get hadEnery for hadEnergyVec[peak]  - time slice that (hopefully) contains the collision 
      int hadEnergy() const;
      /// get total energy. returns emEnergy() + hadEnergy()
      int energy()    const;

      /// get emEnery for emEnergyVec[slice] - time slice for arbitary slice
      int emSliceEnergy(int slice) const;
      /// get hadEnery for hadEnergyVec[slice] - time slice for arbitary slice
      int hadSliceEnergy(int slice) const;
      /// get total energy. returns emSliceEnergy(slice) + hadSliceEnergy(slice)
      int sliceEnergy(int slice) const;

      /// is JetElement saturated?
      bool isSaturated()    const;
      /// is EM component of JetElement saturated?
      bool isEmSaturated()  const;
      /// is Had component of JetElement saturated?
      bool isHadSaturated() const;
      
      /// get emError for emErrorVec[peak] - time slice that (hopefully) contains the collision
      int emError()   const;
      /// get hadError for hadErrorVec[peak] - time slice that (hopefully) contains the collision
      int hadError()  const;
      /// get linkError for linkErrorVec[peak] - time slice that (hopefully) contains the collision
      int linkError() const;
    
    private:
      static const int m_saturationThreshold = 1023;
      static const int m_layerSaturationThreshold = 511;     
  };
} // namespace xAOD

#endif // XAODTRIGL1CALO_VERSIONS_JETELEMENT_V2_H
