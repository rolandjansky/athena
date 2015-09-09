/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMomentTools/JetCaloEnergies.h"

#include "xAODJet/JetAccessorMap.h"

#include "JetUtils/JetCaloQualityUtils.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODPFlow/PFO.h"

#include <vector>

//**********************************************************************

JetCaloEnergies::JetCaloEnergies(const std::string& name)
: JetModifierBase(name) { }

//**********************************************************************

int JetCaloEnergies::modifyJet(xAOD::Jet& jet) const {
  ATH_MSG_VERBOSE("Begin modifying jet.");
  static xAOD::JetAttributeAccessor::AccessorWrapper< std::vector<float> >& ePerSamplingAcc =
    *xAOD::JetAttributeAccessor::accessor< std::vector<float> >(xAOD::JetAttribute::EnergyPerSampling);
  size_t numConstit = jet.numConstituents();
  std::vector<float> & ePerSampling = ePerSamplingAcc(jet);
  ePerSampling.resize(CaloSampling::Unknown, 0.);
  for ( float& e : ePerSampling ) e = 0.0; // re-initialize

  if ( numConstit == 0 ) {
    ATH_MSG_VERBOSE("Jet has no constituents.");
    return 1;
  }

  // should find a more robust solution than using 1st constit type.
  xAOD::Type::ObjectType ctype = jet.rawConstituent( 0 )->type();
  if ( ctype  == xAOD::Type::CaloCluster ) {
    ATH_MSG_VERBOSE("  Constituents are calo clusters.");
    fillEperSamplingCluster(jet, ePerSampling);
    
  } else if (ctype  == xAOD::Type::ParticleFlow) {
    ATH_MSG_VERBOSE("  Constituents are pflow objects.");
    fillEperSamplingPFO(jet, ePerSampling);

  }else {
    ATH_MSG_VERBOSE("Constituents are not calo clusters nor pflow objects.");
  }

  return 1;
}


void JetCaloEnergies::fillEperSamplingCluster(xAOD::Jet& jet, std::vector<float> & ePerSampling ) const {
  // loop over raw constituents
  size_t numConstit = jet.numConstituents();    
  for ( size_t i=0; i<numConstit; i++ ) {
    const xAOD::CaloCluster* constit = dynamic_cast<const xAOD::CaloCluster*>(jet.rawConstituent(i));      
    for ( size_t s= CaloSampling::PreSamplerB; s< CaloSampling::Unknown; s++ ) {
        ePerSampling[s] += constit->eSample( (xAOD::CaloCluster::CaloSample) s );
      }
    }
  static xAOD::JetAttributeAccessor::AccessorWrapper<float>& emFracAcc =
    *xAOD::JetAttributeAccessor::accessor< float >(xAOD::JetAttribute::EMFrac);
  static xAOD::JetAttributeAccessor::AccessorWrapper<float>& hecFracAcc =
    *xAOD::JetAttributeAccessor::accessor< float >(xAOD::JetAttribute::HECFrac);      
  
  emFracAcc(jet) = jet::JetCaloQualityUtils::emFraction( ePerSampling);
  hecFracAcc(jet) = jet::JetCaloQualityUtils::hecF( &jet );       
}

void JetCaloEnergies::fillEperSamplingPFO(xAOD::Jet & jet, std::vector<float> & ePerSampling ) const {
  float emTot=0;
  float hecTot=0;
  float eTot =0;
  size_t numConstit = jet.numConstituents();
  for ( size_t i=0; i<numConstit; i++ ) {
    const xAOD::PFO* constit = dynamic_cast<const xAOD::PFO*>(jet.rawConstituent(i));      
    
    float att = 0.;
    if( constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_HEC0, att) )
      ePerSampling[CaloSampling::HEC0] += att;
    // approximation : if there is energy in HEC0, assume nothing in EMB3 and all in EME3
    CaloSampling::CaloSample ems =  att > 0 ?   CaloSampling::EME3  : CaloSampling::EMB3 ;
    // similarly either everything in TileExt0 or in TileBar0
    CaloSampling::CaloSample tiles =  att > 0 ?   CaloSampling::TileExt0  : CaloSampling::TileBar0 ;

    if( constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_EM3, att) )
      ePerSampling[ems]  += att;
    if( constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_Tile0, att) )
      ePerSampling[tiles]+=  att;
    
    if(constit->ptEM() != 0. ){
      eTot += att*constit->eEM();
      if( constit->attribute<float>(xAOD::PFODetails::eflowRec_EM_FRAC_ENHANCED, att) )
        emTot += att*constit->eEM();
      if( constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_HEC, att) )
        hecTot += att*constit->eEM();
    }
  }
  static xAOD::JetAttributeAccessor::AccessorWrapper<float>& emFracAcc =
    *xAOD::JetAttributeAccessor::accessor< float >(xAOD::JetAttribute::EMFrac);
  static xAOD::JetAttributeAccessor::AccessorWrapper<float>& hecFracAcc =
    *xAOD::JetAttributeAccessor::accessor< float >(xAOD::JetAttribute::HECFrac);      
  
  if(eTot != 0.0){
    emFracAcc(jet) = emTot/eTot;
    hecFracAcc(jet) = hecTot/eTot;
  } else {
    emFracAcc(jet)  = 0.;
    hecFracAcc(jet) = 0.;
  }
}


//**********************************************************************
