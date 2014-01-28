/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetElement_v1.cxx 579924 2014-01-24 10:26:51Z krasznaa $

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
    int modifiedEmEnergy = this->emEnergy() + emEnergyIn;
    if( modifiedEmEnergy > m_layerSaturationThreshold ){
      modifiedEmEnergy = m_layerSaturationThreshold;
    }
    
    // copy (I don't like this) the current Aux const data member vector to a non-const stack vector
    // modify the content of the stack vector and set it as the Aux data member
    std::vector<int> modifiedEmEnergyVec;
    const std::vector<int> currentEmEnergyVec = this->emEnergyVec();
    for(std::vector<int>::const_iterator i=currentEmEnergyVec.begin();i!=currentEmEnergyVec.end();++i){
      modifiedEmEnergyVec.push_back( (*i) );
    }
    modifiedEmEnergyVec[ this->peak() ] = modifiedEmEnergy;
    this->setEmEnergyVec( modifiedEmEnergyVec );
    
    
    /// Had Section
    
    /// Calclate the modified energy
    /// if it is above saturation threshold, set it to the saturation threshold
    int modifiedHadEnergy = this->hadEnergy() + hadEnergyIn;
    if( modifiedHadEnergy > m_layerSaturationThreshold ){
      modifiedEmEnergy = m_layerSaturationThreshold;
    }
    
    // copy (I don't like this) the current Aux const data member vector to a non-const stack vector
    // modify the content of the stack vector and set it as the Aux data member
    std::vector<int> modifiedHadEnergyVec;
    const std::vector<int> currentHadEnergyVec = this->hadEnergyVec();
    for(std::vector<int>::const_iterator i=currentHadEnergyVec.begin();i!=currentHadEnergyVec.end();++i){
      modifiedHadEnergyVec.push_back( (*i) );
    }
    modifiedHadEnergyVec[ this->peak() ] = modifiedHadEnergy;
    this->setHadEnergyVec( modifiedHadEnergyVec );    
    
    
  }
  

  void JetElement_v1::addSlice(int slice, int emEnergyIn, int hadEnergyIn,int emErrorIn, int hadErrorIn, int linkErrorIn)
  {
    /// This functions works just like addEnergy, it's just got more in it

    // EM Energy
    int modifiedEmEnergy = this->emSliceEnergy( slice ) + emEnergyIn;
    if( modifiedEmEnergy > m_layerSaturationThreshold ){
      modifiedEmEnergy = m_layerSaturationThreshold;
    }   
    std::vector<int> modifiedEmEnergyVec;
    const std::vector<int> currentEmEnergyVec = this->emEnergyVec();
    for(std::vector<int>::const_iterator i=currentEmEnergyVec.begin();i!=currentEmEnergyVec.end();++i){
      modifiedEmEnergyVec.push_back( (*i) );
    }
    modifiedEmEnergyVec[ slice ] = modifiedEmEnergy;
    this->setEmEnergyVec( modifiedEmEnergyVec ); 
    
    // Had Energy
    int modifiedHadEnergy = this->hadSliceEnergy( slice ) + hadEnergyIn;
    if( modifiedHadEnergy > m_layerSaturationThreshold ){
      modifiedEmEnergy = m_layerSaturationThreshold;
    }
    std::vector<int> modifiedHadEnergyVec;
    const std::vector<int> currentHadEnergyVec = this->hadEnergyVec();
    for(std::vector<int>::const_iterator i=currentHadEnergyVec.begin();i!=currentHadEnergyVec.end();++i){
      modifiedHadEnergyVec.push_back( (*i) );
    }
    modifiedHadEnergyVec[ slice ] = modifiedHadEnergy;
    this->setHadEnergyVec( modifiedHadEnergyVec );      
    
    // EM Error
    int modifiedEmError = this->emErrorVec()[ slice ] + emErrorIn;
    std::vector<int> modifiedEmErrorVec;
    const std::vector<int> currentEmErrorVec = this->emErrorVec();
    for(std::vector<int>::const_iterator i=currentEmErrorVec.begin();i!=currentEmErrorVec.end();++i){
      modifiedEmErrorVec.push_back( (*i) );
    }
    modifiedEmErrorVec[ slice ] = modifiedEmError;
    this->setEmErrorVec( modifiedEmErrorVec );  
    
    // Had Error
    int modifiedHadError = this->hadErrorVec()[ slice ] + hadErrorIn;
    std::vector<int> modifiedHadErrorVec;
    const std::vector<int> currentHadErrorVec = this->hadErrorVec();
    for(std::vector<int>::const_iterator i=currentHadErrorVec.begin();i!=currentHadErrorVec.end();++i){
      modifiedHadErrorVec.push_back( (*i) );
    }
    modifiedHadErrorVec[ slice ] = modifiedHadError;
    this->setHadErrorVec( modifiedHadErrorVec );   
    
    // Link Error
    int modifiedLinkError = this->linkErrorVec()[ slice ] + linkErrorIn;
    std::vector<int> modifiedLinkErrorVec;
    const std::vector<int> currentLinkErrorVec = this->linkErrorVec();
    for(std::vector<int>::const_iterator i=currentLinkErrorVec.begin();i!=currentLinkErrorVec.end();++i){
      modifiedLinkErrorVec.push_back( (*i) );
    }
    modifiedLinkErrorVec[ slice ] = modifiedLinkError;
    this->setLinkErrorVec( modifiedLinkErrorVec );      
    
    
  }

  int JetElement_v1::emEnergy()  const
  {
    if( this->emEnergyVec()[ this->peak() ] < m_layerSaturationThreshold) {
      return this->emEnergyVec()[ this->peak() ];
    }
    return m_layerSaturationThreshold;    
  }
  
  int JetElement_v1::hadEnergy() const
  {
    if( this->hadEnergyVec()[ this->peak() ] < m_layerSaturationThreshold) {
      return this->hadEnergyVec()[ this->peak() ];
    }
    return m_layerSaturationThreshold;      
  }
  
  int JetElement_v1::energy()    const
  {
    
    if( ( this->emEnergy()  >= m_layerSaturationThreshold ) ||
        ( this->hadEnergy() >= m_layerSaturationThreshold ) ||
        ( ( this->emEnergy() + this->hadEnergy() ) >= m_saturationThreshold ) ){
          return m_saturationThreshold;
    }
    return this->emEnergy() + this->hadEnergy();  
  }

  int JetElement_v1::emSliceEnergy(int slice) const
  {
    if( slice >= 0 && slice < (int)this->emEnergyVec().size() ) {
      if( this->emEnergyVec()[ slice ] < m_layerSaturationThreshold ) {
        return this->emEnergyVec()[ slice ];
      }
      return m_layerSaturationThreshold;
    }
    return 0;    
  }
  
  int JetElement_v1::hadSliceEnergy(int slice) const
  {
    if( slice >= 0 && slice < (int)this->hadEnergyVec().size() ) {
      if( this->hadEnergyVec()[ slice ] < m_layerSaturationThreshold ) {
        return this->hadEnergyVec()[ slice ];
      }
      return m_layerSaturationThreshold;
    }
    return 0;     
  }
  
  int JetElement_v1::sliceEnergy(int slice) const
  {
    if( ( this->emSliceEnergy(slice)  >= m_layerSaturationThreshold ) ||
        ( this->hadSliceEnergy(slice) >= m_layerSaturationThreshold ) ||
        ( ( this->emSliceEnergy(slice) + this->hadSliceEnergy(slice) ) >= m_saturationThreshold ) ){
          return m_saturationThreshold;
       }
    return this->emSliceEnergy(slice) + this->hadSliceEnergy(slice); 
  }
  

  bool JetElement_v1::isSaturated()    const
  {
    if( this->energy() == m_saturationThreshold ){
      return true;
    }
    return false;
  }
  
  bool JetElement_v1::isEmSaturated()  const
  {
    if( this->emEnergy() == m_layerSaturationThreshold ){
      return true;
    }
    return false;
  }
  
  bool JetElement_v1::isHadSaturated() const
  {
    if( this->hadEnergy() == m_layerSaturationThreshold ){
      return true;
    }
    return false;    
  }
  
  int JetElement_v1::emError()   const
  {
    return this->emErrorVec()[ this->peak() ];
  }
  
  int JetElement_v1::hadError()  const
  {
    return this->hadErrorVec()[ this->peak() ];
  }
  
  int JetElement_v1::linkError() const
  {
    return this->linkErrorVec()[ this->peak() ];
  }
  
} // namespace xAOD
