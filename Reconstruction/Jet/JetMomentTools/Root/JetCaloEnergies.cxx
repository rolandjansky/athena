/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMomentTools/JetCaloEnergies.h"

#include "xAODJet/JetAccessorMap.h"

#include "JetUtils/JetCaloQualityUtils.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODPFlow/PFO.h"

#include <vector>

//**********************************************************************

JetCaloEnergies::JetCaloEnergies(const std::string& name)
: AsgTool(name) { }

//**********************************************************************

StatusCode JetCaloEnergies::initialize() {
  ATH_MSG_INFO("Initializing JetCaloEnergies " << name());
  
  if(m_jetContainerName.empty()){
    ATH_MSG_ERROR("JetCaloEnergies needs to have its input jet container configured!");
    return StatusCode::FAILURE;
  }
  
  m_ePerSamplingKey = m_jetContainerName + "." + m_ePerSamplingKey.key();
  m_emFracKey = m_jetContainerName + "." + m_emFracKey.key();
  m_hecFracKey = m_jetContainerName + "." + m_hecFracKey.key();
  m_psFracKey = m_jetContainerName + "." + m_psFracKey.key();

  ATH_CHECK(m_ePerSamplingKey.initialize());
  ATH_CHECK(m_emFracKey.initialize());
  ATH_CHECK(m_hecFracKey.initialize());
  ATH_CHECK(m_psFracKey.initialize());

  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode JetCaloEnergies::decorate(const xAOD::JetContainer& jets) const {
  ATH_MSG_VERBOSE("Begin decorating jets.");
  for(const xAOD::Jet* jet : jets) {
    SG::WriteDecorHandle<xAOD::JetContainer, std::vector<float> > ePerSamplingHandle(m_ePerSamplingKey);
    size_t numConstit = jet->numConstituents();
    ePerSamplingHandle(*jet) = std::vector<float>(CaloSampling::Unknown, 0.);
    std::vector<float>& ePerSampling = ePerSamplingHandle(*jet);
    for ( float& e : ePerSampling ) e = 0.0; // re-initialize

    if ( numConstit == 0 ) {
      ATH_MSG_VERBOSE("Jet has no constituents.");
      continue;
    }
    
    // should find a more robust solution than using 1st constit type.
    xAOD::Type::ObjectType ctype = jet->rawConstituent( 0 )->type();
    if ( ctype  == xAOD::Type::CaloCluster ) {
      ATH_MSG_VERBOSE("  Constituents are calo clusters.");
      fillEperSamplingCluster(*jet, ePerSampling);
    
    } else if (ctype  == xAOD::Type::ParticleFlow) {
      ATH_MSG_VERBOSE("  Constituents are pflow objects.");
      fillEperSamplingPFO(*jet, ePerSampling);

    }else {
      ATH_MSG_VERBOSE("Constituents are not calo clusters nor pflow objects.");
    }

  }
  return StatusCode::SUCCESS;
}

void JetCaloEnergies::fillEperSamplingCluster(const xAOD::Jet& jet, std::vector<float> & ePerSampling ) const {
  // loop over raw constituents
  size_t numConstit = jet.numConstituents();    
  for ( size_t i=0; i<numConstit; i++ ) {
    if(jet.rawConstituent(i)->type()!=xAOD::Type::CaloCluster) {
      ATH_MSG_WARNING("Tried to call fillEperSamplingCluster with a jet constituent that is not a cluster!");
      continue;
    }
    const xAOD::CaloCluster* constit = static_cast<const xAOD::CaloCluster*>(jet.rawConstituent(i));      
    for ( size_t s= CaloSampling::PreSamplerB; s< CaloSampling::Unknown; s++ ) {
      ePerSampling[s] += constit->eSample( (xAOD::CaloCluster::CaloSample) s );
    }
  }
  SG::WriteDecorHandle<xAOD::JetContainer, float> emFracHandle(m_emFracKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> hecFracHandle(m_hecFracKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> psFracHandle(m_psFracKey);
  
  emFracHandle(jet) = jet::JetCaloQualityUtils::emFraction( ePerSampling );
  hecFracHandle(jet) = jet::JetCaloQualityUtils::hecF( &jet );
  psFracHandle(jet) = jet::JetCaloQualityUtils::presamplerFraction( &jet );
}

#define FillESamplingPFO( LAYERNAME )					\
  float E_##LAYERNAME = 0.0;						\
  if (constit->attribute(xAOD::PFODetails::eflowRec_LAYERENERGY_##LAYERNAME, E_##LAYERNAME)) { \
    ePerSampling[CaloSampling::LAYERNAME] += E_##LAYERNAME;		\
  }
  
void JetCaloEnergies::fillEperSamplingPFO(const xAOD::Jet & jet, std::vector<float> & ePerSampling ) const {

  float emTot=0;
  float hecTot=0;
  float eTot =0;
  size_t numConstit = jet.numConstituents();

  for ( size_t i=0; i<numConstit; i++ ) {
    if (jet.rawConstituent(i)->type()==xAOD::Type::ParticleFlow){
      const xAOD::PFO* constit = static_cast<const xAOD::PFO*>(jet.rawConstituent(i));
      if ( fabs(constit->charge())>FLT_MIN ){
	eTot += constit->track(0)->e();
      } else {
	eTot += constit->e();
	FillESamplingPFO(PreSamplerB);
	FillESamplingPFO(EMB1);
	FillESamplingPFO(EMB2);
	FillESamplingPFO(EMB3);

	FillESamplingPFO(PreSamplerE);
	FillESamplingPFO(EME1);
	FillESamplingPFO(EME2);
	FillESamplingPFO(EME3);

	FillESamplingPFO(HEC0);
	FillESamplingPFO(HEC1);
	FillESamplingPFO(HEC2);
	FillESamplingPFO(HEC3);

	FillESamplingPFO(TileBar0);
	FillESamplingPFO(TileBar1);
	FillESamplingPFO(TileBar2);

	FillESamplingPFO(TileGap1);
	FillESamplingPFO(TileGap2);
	FillESamplingPFO(TileGap3);

	FillESamplingPFO(TileExt0);
	FillESamplingPFO(TileExt1);
	FillESamplingPFO(TileExt2);

	FillESamplingPFO(FCAL0);
	FillESamplingPFO(FCAL1);
	FillESamplingPFO(FCAL2);

	FillESamplingPFO(MINIFCAL0);
	FillESamplingPFO(MINIFCAL1);
	FillESamplingPFO(MINIFCAL2);
	FillESamplingPFO(MINIFCAL3);	

	emTot += ( E_PreSamplerB+E_EMB1+E_EMB2+E_EMB3+
		  E_PreSamplerE+E_EME1+E_EME2+E_EME3+
		  E_FCAL0 );
	hecTot += ( E_HEC0+E_HEC1+E_HEC2+E_HEC3 );
	
      }//only consider neutral PFO
    } else {
      ATH_MSG_WARNING("Tried to call fillEperSamplingPFlow with a jet constituent that is not a PFO!");
    }
  }

  SG::WriteDecorHandle<xAOD::JetContainer, float> emFracHandle(m_emFracKey);
  if(eTot != 0.0){
    emFracHandle(jet) = emTot/eTot; 
    /*
     * Ratio of EM layer calorimeter energy of neutrals to sum of all constituents 
     * at EM scale (note charged PFO have an EM scale at track scale, and charged weights are ignored)
     * */
  }
  else {
    emFracHandle(jet)  = 0.;
  }

  SG::WriteDecorHandle<xAOD::JetContainer, float> hecFracHandle(m_hecFracKey);
  if (eTot != 0.0){
    hecFracHandle(jet) = hecTot/eTot;
  }
  else{
    hecFracHandle(jet) = 0.;
  }
  
}

//**********************************************************************
