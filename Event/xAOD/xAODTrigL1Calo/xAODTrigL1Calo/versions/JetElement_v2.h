// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetElement_v2.h 652807 2015-03-09 21:52:07Z morrisj $
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
  /// $Revision: 652807 $
  /// $Date: 2015-03-09 22:52:07 +0100 (Mon, 09 Mar 2015) $  
  
  class JetElement_v2 : public SG::AuxElement{
    public:
      /// Default constructor
      JetElement_v2();
      /// Default desturctor
      virtual ~JetElement_v2(){}      
      
      /// initialize
      virtual void initialize(const float eta,const float phi,const unsigned int key);
      
      /// initialize
      virtual void initialize(const float eta,const float phi,const unsigned int key,
                              const std::vector<uint16_t>& emEnergyVec,
                              const std::vector<uint16_t>& hadEnergyVec,
                              const std::vector<uint8_t>& emErrorVec,
                              const std::vector<uint8_t>& hadErrorVec,
                              const std::vector<uint8_t>& linkErrorVec,
                              const uint8_t peak);
      
      /// get eta
      float eta() const;
      /// set eta
      void setEta(float); 
      
      /// get phi (note that for L1Calo phi runs from 0 to 2pi)
      float phi() const;
      /// set phi
      void setPhi(float);     
      
      /// get key
      unsigned int key() const;
      /// set key
      void setKey(unsigned int);
      
      /// get peak
      uint8_t peak() const;
      /// set key
      void setPeak(uint8_t);
      
      /// get emEnergyVec - emEnergy for all time slices
      const std::vector<uint16_t>& emEnergyVec() const;
      /// set emEnergyVec - emEnergy for all time slices
      void setEmEnergyVec(const std::vector<uint16_t>&);
      
      /// get hadEnergyVec - hadEnergy for all time slices
      const std::vector<uint16_t>& hadEnergyVec() const;
      /// set hadEnergyVec - hadEnergy for all time slices
      void setHadEnergyVec(const std::vector<uint16_t>&); 
      
      /// get emErrorVec - emError for all time slices
      const std::vector<uint8_t>& emErrorVec() const;
      /// set emErrorVec - emError for all time slices
      void setEmErrorVec(const std::vector<uint8_t>&);
      
      /// get hadErrorVec - hadError for all time slices
      const std::vector<uint8_t>& hadErrorVec() const;
      /// set hadErrorVec - hadError for all time slices
      void setHadErrorVec(const std::vector<uint8_t>&);      

      /// get linkErrorVec - linkError for all time slices
      const std::vector<uint8_t>& linkErrorVec() const;
      /// set linkErrorVec - linkError for all time slices
      void setLinkErrorVec(const std::vector<uint8_t>&); 
      

      /// get emEnery for emEnergyVec[peak]  - time slice that (hopefully) contains the collision  
      unsigned int emEnergy()  const;
      /// get hadEnery for hadEnergyVec[peak]  - time slice that (hopefully) contains the collision 
      unsigned int hadEnergy() const;
      /// get total energy. returns emEnergy() + hadEnergy()
      unsigned int energy()    const;

      /// get emEnery for emEnergyVec[slice] - time slice for arbitary slice
      unsigned int emSliceEnergy(unsigned int slice) const;
      /// get hadEnery for hadEnergyVec[slice] - time slice for arbitary slice
      unsigned int hadSliceEnergy(unsigned int slice) const;
      /// get total energy. returns emSliceEnergy(slice) + hadSliceEnergy(slice)
      unsigned int sliceEnergy(unsigned int slice) const;

      /// is JetElement saturated?
      bool isSaturated()    const;
      /// is EM component of JetElement saturated?
      bool isEmSaturated()  const;
      /// is Had component of JetElement saturated?
      bool isHadSaturated() const;
      
      /// get emError for emErrorVec[peak] - time slice that (hopefully) contains the collision
      uint8_t emError()   const;
      /// get hadError for hadErrorVec[peak] - time slice that (hopefully) contains the collision
      uint8_t hadError()  const;
      /// get linkError for linkErrorVec[peak] - time slice that (hopefully) contains the collision
      uint8_t linkError() const;
    
    private:
      static const int m_saturationThreshold = 1023;
      static const int m_layerSaturationThreshold = 511;     
  };
} // namespace xAOD

#endif // XAODTRIGL1CALO_VERSIONS_JETELEMENT_V2_H
