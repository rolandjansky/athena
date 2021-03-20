/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetElement_v1.cxx 638578 2015-01-09 17:00:15Z morrisj $

// System include(s):
#include <cmath>

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/JetElement_v1.h"

namespace xAOD{ 
  
  JetElement_v1::JetElement_v1() :
    SG::AuxElement()
  {        
  }
  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JetElement_v1 , float , phi , setPhi )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JetElement_v1 , float , eta , setEta )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JetElement_v1 , unsigned int , key , setKey )  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( JetElement_v1 , int , peak , setPeak )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( JetElement_v1 , std::vector<int> , emEnergyVec , setEmEnergyVec )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( JetElement_v1 , std::vector<int> , hadEnergyVec , setHadEnergyVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( JetElement_v1 , std::vector<int> , emErrorVec , setEmErrorVec )  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( JetElement_v1 , std::vector<int> , hadErrorVec , setHadErrorVec )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( JetElement_v1 , std::vector<int> , linkErrorVec , setLinkErrorVec )    
 
  void JetElement_v1::addEnergy(int emEnergyIn, int hadEnergyIn)
  {
    /// EM Section
    
    /// Calclate the modified energy
    /// if it is above saturation threshold, set it to the saturation threshold
    int modifiedEmEnergy = emEnergy() + emEnergyIn;
    if( modifiedEmEnergy > m_layerSaturationThreshold ){
      modifiedEmEnergy = m_layerSaturationThreshold;
    }
    
    // copy (I don't like this) the current Aux const data member vector to a non-const stack vector
    // modify the content of the stack vector and set it as the Aux data member
    std::vector<int> modifiedEmEnergyVec;
    const std::vector<int> currentEmEnergyVec = emEnergyVec();
    for(std::vector<int>::const_iterator i=currentEmEnergyVec.begin();i!=currentEmEnergyVec.end();++i){
      modifiedEmEnergyVec.push_back( (*i) );
    }
    modifiedEmEnergyVec[ peak() ] = modifiedEmEnergy;
    setEmEnergyVec( modifiedEmEnergyVec );
    
    
    /// Had Section
    
    /// Calclate the modified energy
    /// if it is above saturation threshold, set it to the saturation threshold
    int modifiedHadEnergy = hadEnergy() + hadEnergyIn;
    if( modifiedHadEnergy > m_layerSaturationThreshold ){
      modifiedEmEnergy = m_layerSaturationThreshold;
    }
    
    // copy (I don't like this) the current Aux const data member vector to a non-const stack vector
    // modify the content of the stack vector and set it as the Aux data member
    std::vector<int> modifiedHadEnergyVec;
    const std::vector<int> currentHadEnergyVec = hadEnergyVec();
    for(std::vector<int>::const_iterator i=currentHadEnergyVec.begin();i!=currentHadEnergyVec.end();++i){
      modifiedHadEnergyVec.push_back( (*i) );
    }
    modifiedHadEnergyVec[ peak() ] = modifiedHadEnergy;
    setHadEnergyVec( modifiedHadEnergyVec );    
    
    
  }
  

  void JetElement_v1::addSlice(int slice, int emEnergyIn, int hadEnergyIn,int emErrorIn, int hadErrorIn, int linkErrorIn)
  {
    /// This functions works just like addEnergy, it's just got more in it

    // EM Energy
    int modifiedEmEnergy = emSliceEnergy( slice ) + emEnergyIn;
    if( modifiedEmEnergy > m_layerSaturationThreshold ){
      modifiedEmEnergy = m_layerSaturationThreshold;
    }   
    std::vector<int> modifiedEmEnergyVec;
    const std::vector<int> currentEmEnergyVec = emEnergyVec();
    for(std::vector<int>::const_iterator i=currentEmEnergyVec.begin();i!=currentEmEnergyVec.end();++i){
      modifiedEmEnergyVec.push_back( (*i) );
    }
    modifiedEmEnergyVec[ slice ] = modifiedEmEnergy;
    setEmEnergyVec( modifiedEmEnergyVec ); 
    
    // Had Energy
    int modifiedHadEnergy = hadSliceEnergy( slice ) + hadEnergyIn;
    if( modifiedHadEnergy > m_layerSaturationThreshold ){
      modifiedEmEnergy = m_layerSaturationThreshold;
    }
    std::vector<int> modifiedHadEnergyVec;
    const std::vector<int> currentHadEnergyVec = hadEnergyVec();
    for(std::vector<int>::const_iterator i=currentHadEnergyVec.begin();i!=currentHadEnergyVec.end();++i){
      modifiedHadEnergyVec.push_back( (*i) );
    }
    modifiedHadEnergyVec[ slice ] = modifiedHadEnergy;
    setHadEnergyVec( modifiedHadEnergyVec );      
    
    // EM Error
    int modifiedEmError = emErrorVec()[ slice ] + emErrorIn;
    std::vector<int> modifiedEmErrorVec;
    const std::vector<int> currentEmErrorVec = emErrorVec();
    for(std::vector<int>::const_iterator i=currentEmErrorVec.begin();i!=currentEmErrorVec.end();++i){
      modifiedEmErrorVec.push_back( (*i) );
    }
    modifiedEmErrorVec[ slice ] = modifiedEmError;
    setEmErrorVec( modifiedEmErrorVec );  
    
    // Had Error
    int modifiedHadError = hadErrorVec()[ slice ] + hadErrorIn;
    std::vector<int> modifiedHadErrorVec;
    const std::vector<int> currentHadErrorVec = hadErrorVec();
    for(std::vector<int>::const_iterator i=currentHadErrorVec.begin();i!=currentHadErrorVec.end();++i){
      modifiedHadErrorVec.push_back( (*i) );
    }
    modifiedHadErrorVec[ slice ] = modifiedHadError;
    setHadErrorVec( modifiedHadErrorVec );   
    
    // Link Error
    int modifiedLinkError = linkErrorVec()[ slice ] + linkErrorIn;
    std::vector<int> modifiedLinkErrorVec;
    const std::vector<int> currentLinkErrorVec = linkErrorVec();
    for(std::vector<int>::const_iterator i=currentLinkErrorVec.begin();i!=currentLinkErrorVec.end();++i){
      modifiedLinkErrorVec.push_back( (*i) );
    }
    modifiedLinkErrorVec[ slice ] = modifiedLinkError;
    setLinkErrorVec( modifiedLinkErrorVec );      
    
    
  }

  int JetElement_v1::emEnergy()  const
  {
    if( emEnergyVec()[ peak() ] < m_layerSaturationThreshold) {
      return emEnergyVec()[ peak() ];
    }
    return m_layerSaturationThreshold;    
  }
  
  int JetElement_v1::hadEnergy() const
  {
    if( hadEnergyVec()[ peak() ] < m_layerSaturationThreshold) {
      return hadEnergyVec()[ peak() ];
    }
    return m_layerSaturationThreshold;      
  }
  
  int JetElement_v1::energy()    const
  {
    
    if( ( emEnergy()  >= m_layerSaturationThreshold ) ||
        ( hadEnergy() >= m_layerSaturationThreshold ) ||
        ( ( emEnergy() + hadEnergy() ) >= m_saturationThreshold ) ){
          return m_saturationThreshold;
    }
    return emEnergy() + hadEnergy();  
  }

  int JetElement_v1::emSliceEnergy(int slice) const
  {
    if( slice >= 0 && slice < (int)emEnergyVec().size() ) {
      if( emEnergyVec()[ slice ] < m_layerSaturationThreshold ) {
        return emEnergyVec()[ slice ];
      }
      return m_layerSaturationThreshold;
    }
    return 0;    
  }
  
  int JetElement_v1::hadSliceEnergy(int slice) const
  {
    if( slice >= 0 && slice < (int)hadEnergyVec().size() ) {
      if( hadEnergyVec()[ slice ] < m_layerSaturationThreshold ) {
        return hadEnergyVec()[ slice ];
      }
      return m_layerSaturationThreshold;
    }
    return 0;     
  }
  
  int JetElement_v1::sliceEnergy(int slice) const
  {
    if( ( emSliceEnergy(slice)  >= m_layerSaturationThreshold ) ||
        ( hadSliceEnergy(slice) >= m_layerSaturationThreshold ) ||
        ( ( emSliceEnergy(slice) + hadSliceEnergy(slice) ) >= m_saturationThreshold ) ){
          return m_saturationThreshold;
       }
    return emSliceEnergy(slice) + hadSliceEnergy(slice); 
  }
  

  bool JetElement_v1::isSaturated()    const
  {
    return energy() == m_saturationThreshold;
  }
  
  bool JetElement_v1::isEmSaturated()  const
  {
    return emEnergy() == m_layerSaturationThreshold;
  }
  
  bool JetElement_v1::isHadSaturated() const
  {
    return hadEnergy() == m_layerSaturationThreshold;    
  }
  
  int JetElement_v1::emError()   const
  {
    return emErrorVec()[ peak() ];
  }
  
  int JetElement_v1::hadError()  const
  {
    return hadErrorVec()[ peak() ];
  }
  
  int JetElement_v1::linkError() const
  {
    return linkErrorVec()[ peak() ];
  }
  
} // namespace xAOD
