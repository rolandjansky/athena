// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetElement_v2.h 687949 2015-08-06 15:48:49Z amazurov $
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
  /// $Revision: 687949 $
  /// $Date: 2015-08-06 17:48:49 +0200 (Thu, 06 Aug 2015) $  
  
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
                              const std::vector<uint16_t>& emJetElementETVec,
                              const std::vector<uint16_t>& hadJetElementETVec,
                              const std::vector<uint32_t>& emJetElementErrorVec,
                              const std::vector<uint32_t>& hadJetElementErrorVec,
                              const std::vector<uint32_t>& linkErrorVec,
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
      
      /// get emJetElementETVec - emJetElementET for all time slices
      const std::vector<uint16_t>& emJetElementETVec() const;
      /// set emJetElementETVec - emJetElementET for all time slices
      void setEmJetElementETVec(const std::vector<uint16_t>&);
      
      /// get hadJetElementETVec - hadJetElementET for all time slices
      const std::vector<uint16_t>& hadJetElementETVec() const;
      /// set hadJetElementETVec - hadJetElementET for all time slices
      void setHadJetElementETVec(const std::vector<uint16_t>&); 
      
      /// get emJetElementErrorVec - emJetElementError for all time slices
      const std::vector<uint32_t>& emJetElementErrorVec() const;
      /// set emJetElementErrorVec - emJetElementError for all time slices
      void setEmJetElementErrorVec(const std::vector<uint32_t>&);
      
      /// get hadJetElementErrorVec - hadJetElementError for all time slices
      const std::vector<uint32_t>& hadJetElementErrorVec() const;
      /// set hadJetElementErrorVec - hadJetElementError for all time slices
      void setHadJetElementErrorVec(const std::vector<uint32_t>&);      

      /// get linkErrorVec - linkError for all time slices
      const std::vector<uint32_t>& linkErrorVec() const;
      /// set linkErrorVec - linkError for all time slices
      void setLinkErrorVec(const std::vector<uint32_t>&); 
      

      /// get emJetElementET for emJetElementETVec[peak]  - time slice that (hopefully) contains the collision  
      unsigned int emJetElementET()  const;
      /// get hadJetElementET for hadJetElementETVec[peak]  - time slice that (hopefully) contains the collision 
      unsigned int hadJetElementET() const;
      /// get total et. returns emJetElementET() + hadJetElementET()
      unsigned int et()    const;

      /// get emJetElementET for emJetElementETVec[slice] - time slice for arbitary slice
      unsigned int emJetElementETSlice(unsigned int slice) const;
      /// get hadJetElementET for hadJetElementETVec[slice] - time slice for arbitary slice
      unsigned int hadJetElementETSlice(unsigned int slice) const;
      /// get total et. returns emJetElementETSlice(slice) + hadJetElementETSlice(slice)
      unsigned int sliceET(unsigned int slice) const;

      /// is JetElement saturated?
      bool isSaturated()    const;
      /// is EM component of JetElement saturated?
      bool isEmSaturated()  const;
      /// is Had component of JetElement saturated?
      bool isHadSaturated() const;
      
      /// get emJetElementError for emJetElementErrorVec[peak] - time slice that (hopefully) contains the collision
      uint32_t emJetElementError()   const;
      /// get hadJetElementError for hadJetElementErrorVec[peak] - time slice that (hopefully) contains the collision
      uint32_t hadJetElementError()  const;
      /// get linkError for linkErrorVec[peak] - time slice that (hopefully) contains the collision
      uint32_t linkError() const;
    
    private:
      static const int m_saturationThreshold = 1023;
      static const int m_layerSaturationThreshold = 511;     
  };
} // namespace xAOD

#endif // XAODTRIGL1CALO_VERSIONS_JETELEMENT_V2_H
