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
    if (constit){
      eTot += constit->eEM();
      if (0 == constit->charge()){

        float E_PreSamplerB = 0.0;
        if (constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_PreSamplerB, E_PreSamplerB)) ePerSampling[CaloSampling::PreSamplerB] += E_PreSamplerB;
          
        float E_EMB1 = 0.0;
        if (constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_EMB1, E_EMB1)) ePerSampling[CaloSampling::EMB1] += E_EMB1;
          
        float E_EMB2 = 0.0;
        if (constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_EMB2, E_EMB2)) ePerSampling[CaloSampling::EMB2] += E_EMB2;
	  
        float E_EMB3 = 0.0;
        if (constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_EMB3, E_EMB3)) ePerSampling[CaloSampling::EMB3] += E_EMB3;
          
        float E_PreSamplerE = 0.0;
        if (constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_PreSamplerE, E_PreSamplerE)) ePerSampling[CaloSampling::PreSamplerE] += E_PreSamplerE;
          
        float E_EME1 = 0.0;
        if (constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_EME1, E_EME1)) ePerSampling[CaloSampling::EME1] += E_EME1;
          
        float E_EME2 = 0.0;
        if (constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_EME2, E_EME2)) ePerSampling[CaloSampling::EME2] += E_EME2;
          
        float E_EME3 = 0.0;
        if (constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_EME3, E_EME3)) ePerSampling[CaloSampling::EME3] += E_EME3;
          
        float E_HEC0= 0.0;
        if (constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_HEC0, E_HEC0)) ePerSampling[CaloSampling::HEC0] += E_HEC0;
          
        float E_HEC1= 0.0;
        if (constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_HEC1, E_HEC1)) ePerSampling[CaloSampling::HEC1] += E_HEC1;
          
        float E_HEC2= 0.0;
        if (constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_HEC2, E_HEC2)) ePerSampling[CaloSampling::HEC2] += E_HEC2;
          
        float E_HEC3= 0.0;
        if (constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_HEC3, E_HEC3)) ePerSampling[CaloSampling::HEC3] += E_HEC3;

	float E_TileBar0= 0.0;
        if (constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_TileBar0, E_TileBar0)) ePerSampling[CaloSampling::TileBar0] += E_TileBar0;

        float E_TileBar1= 0.0;
        if (constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_TileBar1, E_TileBar1)) ePerSampling[CaloSampling::TileBar1] += E_TileBar1;

        float E_TileBar2= 0.0;
        if (constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_TileBar2, E_TileBar2)) ePerSampling[CaloSampling::TileBar2] += E_TileBar2;

        float E_TileGap1= 0.0;
        if (constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_TileGap1, E_TileGap1)) ePerSampling[CaloSampling::TileGap1] += E_TileGap1;

        float E_TileGap2= 0.0;
        if (constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_TileGap2, E_TileGap2)) ePerSampling[CaloSampling::TileGap2] += E_TileGap2;
        
        float E_TileGap3= 0.0;
        if (constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_TileGap3, E_TileGap3)) ePerSampling[CaloSampling::TileGap3] += E_TileGap3;

        float E_TileExt0= 0.0;
        if (constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_TileExt0, E_TileExt0)) ePerSampling[CaloSampling::TileExt0] += E_TileExt0;

        float E_TileExt1= 0.0;
        if (constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_TileExt1, E_TileExt1)) ePerSampling[CaloSampling::TileExt1] += E_TileExt1;

        float E_TileExt2= 0.0;
        if (constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_TileExt2, E_TileExt2)) ePerSampling[CaloSampling::TileExt2] += E_TileExt2;

        float E_FCAL0= 0.0;
        if (constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_FCAL0, E_FCAL0)) ePerSampling[CaloSampling::FCAL0] += E_FCAL0;

        float E_FCAL1= 0.0;
        if (constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_FCAL1, E_FCAL1)) ePerSampling[CaloSampling::FCAL1] += E_FCAL1;

        float E_FCAL2= 0.0;
        if (constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_FCAL2, E_FCAL2)) ePerSampling[CaloSampling::FCAL2] += E_FCAL2;

        float E_MINIFCAL0= 0.0;
        if (constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_MINIFCAL0, E_MINIFCAL0)) ePerSampling[CaloSampling::MINIFCAL0] += E_MINIFCAL0;

        float E_MINIFCAL1= 0.0;
        if (constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_MINIFCAL1, E_MINIFCAL1)) ePerSampling[CaloSampling::MINIFCAL1] += E_MINIFCAL1;

        float E_MINIFCAL2= 0.0;
        if (constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_MINIFCAL2, E_MINIFCAL2)) ePerSampling[CaloSampling::MINIFCAL2] += E_MINIFCAL2;

        float E_MINIFCAL3= 0.0;
        if (constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_MINIFCAL3, E_MINIFCAL3)) ePerSampling[CaloSampling::MINIFCAL3] += E_MINIFCAL3;

	//We calculate emTot and hecTot from these stored values, because above layer energies are NOT stored in the xAOD/ESD.
	float E_EM = 0.0;
	if (constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_EM,E_EM)) emTot += E_EM;
	
	float E_HEC = 0.0;
	if (constit->attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_HEC,E_HEC)) hecTot += E_HEC;
	
	 }//only consider neutral PFO
    }//if dynamic cast worked
  }

  static xAOD::JetAttributeAccessor::AccessorWrapper<float>& emFracAcc = *xAOD::JetAttributeAccessor::accessor< float >(xAOD::JetAttribute::EMFrac);
  if(eTot != 0.0){
    emFracAcc(jet) = emTot/eTot; //ratio of EM layer calorimeter energy of neutrals to sum of all constituents at EM scale (note charged PFO have an EM scale at track scale, and charged weights are ignored)
  }
  else {
    emFracAcc(jet)  = 0.;
  }

  static xAOD::JetAttributeAccessor::AccessorWrapper<float>& hecFracAcc = *xAOD::JetAttributeAccessor::accessor< float >(xAOD::JetAttribute::HECFrac);     
  if (eTot != 0.0){
    hecFracAcc(jet) = hecTot/eTot;
  }
  else{
    hecFracAcc(jet) = 0.;
  }
  
}

//**********************************************************************
