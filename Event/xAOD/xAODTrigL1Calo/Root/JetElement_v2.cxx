/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetElement_v2.cxx 652807 2015-03-09 21:52:07Z morrisj $

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
                                 const std::vector<uint16_t>& emEnergyVec,
                                 const std::vector<uint16_t>& hadEnergyVec,
                                 const std::vector<uint8_t>& emErrorVec,
                                 const std::vector<uint8_t>& hadErrorVec,
                                 const std::vector<uint8_t>& linkErrorVec,
                                 const uint8_t peak)
  {  
    setEta( eta );
    setPhi( phi );
    setKey( key );
    setEmEnergyVec( emEnergyVec );
    setHadEnergyVec( hadEnergyVec );
    setEmErrorVec( emErrorVec );
    setHadErrorVec( hadErrorVec );
    setLinkErrorVec( linkErrorVec );
    setPeak( peak );
  }
  
  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JetElement_v2 , float , eta , setEta )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JetElement_v2 , float , phi , setPhi )  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JetElement_v2 , unsigned int , key , setKey )  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JetElement_v2 , uint8_t , peak , setPeak )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( JetElement_v2 , std::vector<uint16_t> , emEnergyVec , setEmEnergyVec )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( JetElement_v2 , std::vector<uint16_t> , hadEnergyVec , setHadEnergyVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( JetElement_v2 , std::vector<uint8_t> , emErrorVec , setEmErrorVec )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( JetElement_v2 , std::vector<uint8_t> , hadErrorVec , setHadErrorVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( JetElement_v2 , std::vector<uint8_t> , linkErrorVec , setLinkErrorVec )    

  unsigned int JetElement_v2::emEnergy()  const
  {
    if( emEnergyVec()[ peak() ] < m_layerSaturationThreshold) {
      return emEnergyVec()[ peak() ];
    }
    return m_layerSaturationThreshold;    
  }
  
  unsigned int JetElement_v2::hadEnergy() const
  {
    if( hadEnergyVec()[ peak() ] < m_layerSaturationThreshold) {
      return hadEnergyVec()[ peak() ];
    }
    return m_layerSaturationThreshold;      
  }
  
  unsigned int JetElement_v2::energy()    const
  {
    
    if( ( emEnergy()  >= m_layerSaturationThreshold ) ||
        ( hadEnergy() >= m_layerSaturationThreshold ) ||
        ( ( emEnergy() + hadEnergy() ) >= m_saturationThreshold ) ){
          return m_saturationThreshold;
    }
    return emEnergy() + hadEnergy();
  }

  unsigned int JetElement_v2::emSliceEnergy(unsigned int slice) const
  {
    if( slice < emEnergyVec().size() ) {
      if( emEnergyVec()[ slice ] < m_layerSaturationThreshold ) {
        return emEnergyVec()[ slice ];
      }
      return m_layerSaturationThreshold;
    }
    return 0;    
  }
  
  unsigned int JetElement_v2::hadSliceEnergy(unsigned int slice) const
  {
    if( slice < hadEnergyVec().size() ) {
      if( hadEnergyVec()[ slice ] < m_layerSaturationThreshold ) {
        return hadEnergyVec()[ slice ];
      }
      return m_layerSaturationThreshold;
    }
    return 0;     
  }
  
  unsigned int JetElement_v2::sliceEnergy(unsigned int slice) const
  {
    if( ( emSliceEnergy(slice)  >= m_layerSaturationThreshold ) ||
        ( hadSliceEnergy(slice) >= m_layerSaturationThreshold ) ||
        ( ( emSliceEnergy(slice) + hadSliceEnergy(slice) ) >= m_saturationThreshold ) ){
          return m_saturationThreshold;
       }
    return emSliceEnergy(slice) + hadSliceEnergy(slice); 
  }
  

  bool JetElement_v2::isSaturated()    const
  {
    if( energy() == m_saturationThreshold ){
      return true;
    }
    return false;
  }
  
  bool JetElement_v2::isEmSaturated()  const
  {
    if( emEnergy() == m_layerSaturationThreshold ){
      return true;
    }
    return false;
  }
  
  bool JetElement_v2::isHadSaturated() const
  {
    if( hadEnergy() == m_layerSaturationThreshold ){
      return true;
    }
    return false;    
  }
  
  uint8_t JetElement_v2::emError()   const
  {
    return emErrorVec()[ peak() ];
  }
  
  uint8_t JetElement_v2::hadError()  const
  {
    return hadErrorVec()[ peak() ];
  }
  
  uint8_t JetElement_v2::linkError() const
  {
    return linkErrorVec()[ peak() ];
  }
  
} // namespace xAOD
