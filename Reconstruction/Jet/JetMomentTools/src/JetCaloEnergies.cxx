/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMomentTools/JetCaloEnergies.h"

#include "xAODJet/JetAccessorMap.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "JetUtils/JetCaloQualityUtils.h"

#include <vector>
namespace {
  
}

JetCaloEnergies::JetCaloEnergies(const std::string & t) : JetModifierBase(t) {
  
}



int JetCaloEnergies::modifyJet( xAOD::Jet & jet)const  {

  static xAOD::JetAttributeAccessor::AccessorWrapper< std::vector<float> > & ePerSamplingAcc = *xAOD::JetAttributeAccessor::accessor< std::vector<float> >(xAOD::JetAttribute::EnergyPerSampling);


  size_t numConstit = jet.numConstituents();
  std::vector<float> & ePerSampling = ePerSamplingAcc( jet );
  ePerSampling.resize(CaloSampling::Unknown, 0.);
  
  for( float &e : ePerSampling) e=0.; // re-initialize
  
  if( numConstit == 0 ) return 1;
  
  // should find a more robust solution than using 1st constit type.
  if( jet.rawConstituent( 0 )->type() == xAOD::Type::CaloCluster ){
    // loop over raw constituents
    for(size_t i=0;i<numConstit;i++){
      for( size_t s= CaloSampling::PreSamplerB; s< CaloSampling::Unknown; s++){
        float samplE = dynamic_cast<const xAOD::CaloCluster*>(jet.rawConstituent( i ))->eSample( (xAOD::CaloCluster::CaloSample) s);
        //std::cout<< " ___ sampling "<< s << "  adding   "<< samplE << "   to "<< ePerSampling[s] << std::endl;
        ePerSampling[s] += samplE;
      }
    }

    // for(float & cs: ePerSampling){
    //   ATH_MSG_DEBUG( jet.index() << "  esampling = "<< cs ); }
    // ATH_MSG_DEBUG("_________________________");
    static xAOD::JetAttributeAccessor::AccessorWrapper<float>& emFracAcc = *xAOD::JetAttributeAccessor::accessor< float >(xAOD::JetAttribute::EMFrac);
    static xAOD::JetAttributeAccessor::AccessorWrapper<float>& hecFracAcc = *xAOD::JetAttributeAccessor::accessor< float >(xAOD::JetAttribute::HECFrac);
    emFracAcc(jet) = jet::JetCaloQualityUtils::emFraction( ePerSampling);
    hecFracAcc(jet) = jet::JetCaloQualityUtils::hecF( &jet );    
  }

  return 1;
}
