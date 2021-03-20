/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetElement_v2.cxx 687949 2015-08-06 15:48:49Z amazurov $

// System include(s):
#include <cmath>

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/JetElement_v2.h"

namespace xAOD{ 
  
  JetElement_v2::JetElement_v2() :
    SG::AuxElement()
  {        
  }
  
  /// initialize
  void JetElement_v2::initialize(const float eta,const float phi,const unsigned int key)
  {     
    setEta( eta );
    setPhi( phi );
    setKey( key );
  }
  
  /// initialize
  void JetElement_v2::initialize(const float eta,const float phi,const unsigned int key,
                                 const std::vector<uint16_t>& emJetElementETVec,
                                 const std::vector<uint16_t>& hadJetElementETVec,
                                 const std::vector<uint32_t>& emJetElementErrorVec,
                                 const std::vector<uint32_t>& hadJetElementErrorVec,
                                 const std::vector<uint32_t>& linkErrorVec,
                                 const uint8_t peak)
  {  
    setEta( eta );
    setPhi( phi );
    setKey( key );
    setEmJetElementETVec( emJetElementETVec );
    setHadJetElementETVec( hadJetElementETVec );
    setEmJetElementErrorVec( emJetElementErrorVec );
    setHadJetElementErrorVec( hadJetElementErrorVec );
    setLinkErrorVec( linkErrorVec );
    setPeak( peak );
  }
  
  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JetElement_v2 , float , eta , setEta )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JetElement_v2 , float , phi , setPhi )  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JetElement_v2 , unsigned int , key , setKey )  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JetElement_v2 , uint8_t , peak , setPeak )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( JetElement_v2 , std::vector<uint16_t> , emJetElementETVec , setEmJetElementETVec )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( JetElement_v2 , std::vector<uint16_t> , hadJetElementETVec , setHadJetElementETVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( JetElement_v2 , std::vector<uint32_t> , emJetElementErrorVec , setEmJetElementErrorVec )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( JetElement_v2 , std::vector<uint32_t> , hadJetElementErrorVec , setHadJetElementErrorVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( JetElement_v2 , std::vector<uint32_t> , linkErrorVec , setLinkErrorVec )    

  unsigned int JetElement_v2::emJetElementET()  const
  {
    if( emJetElementETVec()[ peak() ] < m_layerSaturationThreshold) {
      return emJetElementETVec()[ peak() ];
    }
    return m_layerSaturationThreshold;    
  }
  
  unsigned int JetElement_v2::hadJetElementET() const
  {
    if( hadJetElementETVec()[ peak() ] < m_layerSaturationThreshold) {
      return hadJetElementETVec()[ peak() ];
    }
    return m_layerSaturationThreshold;      
  }
  
  unsigned int JetElement_v2::et()    const
  {
    
    if( ( emJetElementET()  >= m_layerSaturationThreshold ) ||
        ( hadJetElementET() >= m_layerSaturationThreshold ) ||
        ( ( emJetElementET() + hadJetElementET() ) >= m_saturationThreshold ) ){
          return m_saturationThreshold;
    }
    return emJetElementET() + hadJetElementET();
  }

  unsigned int JetElement_v2::emJetElementETSlice(unsigned int slice) const
  {
    if( slice < emJetElementETVec().size() ) {
      if( emJetElementETVec()[ slice ] < m_layerSaturationThreshold ) {
        return emJetElementETVec()[ slice ];
      }
      return m_layerSaturationThreshold;
    }
    return 0;    
  }
  
  unsigned int JetElement_v2::hadJetElementETSlice(unsigned int slice) const
  {
    if( slice < hadJetElementETVec().size() ) {
      if( hadJetElementETVec()[ slice ] < m_layerSaturationThreshold ) {
        return hadJetElementETVec()[ slice ];
      }
      return m_layerSaturationThreshold;
    }
    return 0;     
  }
  
  unsigned int JetElement_v2::sliceET(unsigned int slice) const
  {
    if( ( emJetElementETSlice(slice)  >= m_layerSaturationThreshold ) ||
        ( hadJetElementETSlice(slice) >= m_layerSaturationThreshold ) ||
        ( ( emJetElementETSlice(slice) + hadJetElementETSlice(slice) ) >= m_saturationThreshold ) ){
          return m_saturationThreshold;
       }
    return emJetElementETSlice(slice) + hadJetElementETSlice(slice); 
  }
  

  bool JetElement_v2::isSaturated()    const
  {
    return et() == m_saturationThreshold;
  }
  
  bool JetElement_v2::isEmSaturated()  const
  {
    return emJetElementET() == m_layerSaturationThreshold;
  }
  
  bool JetElement_v2::isHadSaturated() const
  {
    return hadJetElementET() == m_layerSaturationThreshold;    
  }
  
  uint32_t JetElement_v2::emJetElementError()   const
  {
    return emJetElementErrorVec()[ peak() ];
  }
  
  uint32_t JetElement_v2::hadJetElementError()  const
  {
    return hadJetElementErrorVec()[ peak() ];
  }
  
  uint32_t JetElement_v2::linkError() const
  {
    return linkErrorVec()[ peak() ];
  }
  
} // namespace xAOD
