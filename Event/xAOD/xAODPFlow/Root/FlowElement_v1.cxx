/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"
#include "AthLinks/ElementLink.h"


// Local include(s):
#include "xAODPFlow/versions/FlowElement_v1.h"

#include "Math/Vector4D.h"

using ROOT::Math::PtEtaPhiMVector ;

namespace xAOD {


  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST(FlowElement_v1, float, double, pt)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST(FlowElement_v1, float, double, eta)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST(FlowElement_v1, float, double, phi)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST(FlowElement_v1, float, double, m)

  double FlowElement_v1::e() const {
    return PtEtaPhiMVector(pt(), eta(), phi(), m() ).E();
  }

  double FlowElement_v1::rapidity() const {
    return PtEtaPhiMVector(pt(), eta(), phi(), m() ).Rapidity();
  }

  FlowElement_v1::FourMom_t FlowElement_v1::p4() const {
    FourMom_t p4;
    p4.SetPtEtaPhiE( pt(), eta(), phi(), e() );       
    return p4;
  }

  void FlowElement_v1::setP4(float pt, float eta, float phi, float m){
    static const Accessor< float > acc1( "pt" );     
    acc1( *this ) = pt;     
    static const  Accessor< float > acc2( "eta" );     
    acc2( *this ) = eta;     
    static const  Accessor< float > acc3( "phi" );     
    acc3( *this ) = phi;     
    static const  Accessor< float > acc4( "m" );     
    acc4( *this ) = m;     

  }


  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(FlowElement_v1, float, charge, setCharge) 
  bool  FlowElement_v1::isCharged() const { return !bool( signalType()& Neutral );}
  

  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(FlowElement_v1, FlowElement_v1::signal_t, signalType, setSignalType) 
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(FlowElement_v1, FlowElement_v1::vertex_t, vertexType, setVertexType) 

  bool  FlowElement_v1::isMatchedToPV(MatchedPVType vxtype) const {
    return (vertexType()==vxtype);
  }
      

  Type::ObjectType FlowElement_v1::type() const {
    return Type::FlowElement;
  }


  

  AUXSTORE_OBJECT_SETTER_AND_GETTER( FlowElement_v1, 
				     std::vector< ElementLink< xAOD::IParticleContainer > >, 
				     chargedObjectLinks, 
				     setChargedObjectLinks)


  AUXSTORE_OBJECT_GETTER( FlowElement_v1, std::vector<float>, chargedObjectWeights)
  
  void FlowElement_v1::setChargedObjectLinks(const std::vector<ElementLink<IParticleContainer>> & elV, const std::vector<float> & wV){
    if(elV.size() != wV.size() ){
      throw std::runtime_error("FlowElement::setChargedObjectLinks : Can not set vectors of links and weights with different sizes");
    }
    static const SG::AuxElement::Accessor< std::vector<ElementLink< xAOD::IParticleContainer >> >  accL( "chargedObjectLinks" );
    static const SG::AuxElement::Accessor< std::vector<float> >  accW( "chargedObjectWeights" );
    accL(*this) = elV;
    accW(*this) = wV;    
  }
  
  
  std::vector<const xAOD::IParticle*> FlowElement_v1::chargedObjects() const {
    const auto & elV = chargedObjectLinks();
    std::vector<const xAOD::IParticle*> result;
    result.reserve( elV.size() );
    for(const auto& el: elV ){
      result.push_back( el.isValid() ? *el : nullptr ) ; 
    }
    return result;
  }

  std::vector<std::pair<const xAOD::IParticle*,float> > FlowElement_v1::chargedObjectsAndWeights() const {

    const auto & elV = chargedObjectLinks();
    const std::vector<float> & wV = chargedObjectWeights();
    std::vector< std::pair<const xAOD::IParticle*,float> > result;
    result.reserve( elV.size() );
    if(wV.empty()) {
      for(const auto& el: elV ){
	result.push_back( { el.isValid() ? *el : nullptr , 1.} ) ; 
      }
    } else {
      for(size_t i=0;i<elV.size();i++){
	const xAOD::IParticle* p = elV[i].isValid() ? *(elV[i]) : nullptr;
	result.push_back( {p , wV[i]} ); 
      }
    }      
    return result;
  }

  std::size_t FlowElement_v1::nChargedObjects() const {
    return chargedObjectLinks().size();
  }
  
  const xAOD::IParticle* FlowElement_v1::chargedObject( std::size_t i ) const{
    const auto & elV = chargedObjectLinks();
    // should we check if i >= size() and throw ourselves ? or trust the user ?
    return elV[i].isValid()? *(elV[i]) : nullptr ;
  }

  std::pair<const xAOD::IParticle*, float > FlowElement_v1::chargedObjectAndWeight( std::size_t i ) const {
    const auto & elV = chargedObjectLinks();
    const std::vector<float> & wV = chargedObjectWeights();

    if( elV[i].isValid() )  return { *(elV[i]), wV[i] } ;
    else return {nullptr, wV[i] } ;    
  }
  


  

  AUXSTORE_OBJECT_SETTER_AND_GETTER( FlowElement_v1, 
				     std::vector< ElementLink< xAOD::IParticleContainer > >, 
				     otherObjectLinks, 
				     setOtherObjectLinks)
  AUXSTORE_OBJECT_GETTER( FlowElement_v1, std::vector<float>, otherObjectWeights)
    
  void FlowElement_v1::setOtherObjectLinks(const std::vector<ElementLink<IParticleContainer>> & elV, const std::vector<float> & wV){
    if(elV.size() != wV.size() ){
      throw std::runtime_error("FlowElement::setOtherObjectLinks : Can not set vectors of links and weights with different sizes");
      return;
    }
    static const SG::AuxElement::Accessor< std::vector<ElementLink< xAOD::IParticleContainer > > >  accL( "otherObjectLinks" );
    static const  SG::AuxElement::Accessor< std::vector<float>  >  accW( "otherObjectWeights" );
    accL(*this) = elV;
    accW(*this) = wV;    
  }
  
  
  std::vector<const xAOD::IParticle*> FlowElement_v1::otherObjects() const {
    const auto & elV = otherObjectLinks();
    std::vector<const xAOD::IParticle*> result;
    result.reserve( elV.size() );
    for(const auto& el: elV ){
      result.push_back( el.isValid() ? *el : nullptr ) ; 
    }
    return result;
  }

  std::vector<std::pair<const xAOD::IParticle*,float> > FlowElement_v1::otherObjectsAndWeights() const {

    const auto & elV = otherObjectLinks();
    const std::vector<float> & wV = otherObjectWeights();
    std::vector< std::pair<const xAOD::IParticle*,float> > result;
    result.reserve( elV.size() );
    if(wV.empty()) {
      for(const auto& el: elV ){
	result.push_back( { el.isValid() ? *el : nullptr , 1.} ) ; 
      }
    } else {
      for(size_t i=0;i<elV.size();i++){
	const xAOD::IParticle* p = elV[i].isValid() ? *(elV[i]) : nullptr;
	result.push_back( {p , wV[i]} ); 
      }
    }      
    return result;
  }
  
  std::size_t FlowElement_v1::nOtherObjects() const {
    return otherObjectLinks().size();
  }

  const xAOD::IParticle* FlowElement_v1::otherObject( std::size_t i ) const{
    const auto & elV = otherObjectLinks();
    // should we check if i >= size() and throw ourselves ? or trust the user ?
    return elV[i].isValid()? *(elV[i]) : nullptr ;
  }
  
  std::pair< const xAOD::IParticle*, float > FlowElement_v1::otherObjectAndWeight( std::size_t i ) const {
    const auto & elV = otherObjectLinks();
    const std::vector<float> & wV = otherObjectWeights();

    if( elV[i].isValid() )  return { *(elV[i]), wV[i] };
    else return {nullptr, wV[i] } ;    
  }

  
}
