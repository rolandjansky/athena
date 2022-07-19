/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


/*  Decorates the InDetTrackParticles Container with calorimeter  */
/*  sample information.   */

/*  Author: Kaito Sugizaki (kaito.sugizaki@cern.ch)   */

#include "DerivationFrameworkSUSY/TrackCaloClusterDecorator_LowPtE.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "FourMomUtils/P4Helpers.h"

#include <vector>
#include <string>
#include <fstream>

namespace DerivationFramework {

  TrackCaloClusterDecorator_LowPtE::TrackCaloClusterDecorator_LowPtE( const std::string& t,
									    const std::string& n,
									    const IInterface* p) :
    AthAlgTool(t,n,p),
    m_containerName(""),
    m_prefix("")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("ContainerName", m_containerName);
    declareProperty("UpperPtLimit",m_upperPtLimit = 5.0 ,"upper limit to track pT in GeV");
    declareProperty("LowerPtLimit",m_lowerPtLimit = 0.5 ,"lower limit to track pT in GeV");
    declareProperty("dEtaCut",m_dEtaCut = 0.1);
    declareProperty("cosdPhiCut",m_cosdPhiCut = 0.8);
    declareProperty("sindPhiCut",m_sindPhiCut = 0.2);
    declareProperty("Prefix", m_prefix);
  }

  StatusCode TrackCaloClusterDecorator_LowPtE::initialize() {

    if ( m_containerName.empty()) {
      ATH_MSG_FATAL("No TrackParticle collection provided for TrackCaloClusterDecorator_LowPtE!");
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }


  StatusCode TrackCaloClusterDecorator_LowPtE::addBranches() const  {


    static const SG::AuxElement::Decorator< std::vector<float>  > dec_caloclusEta(m_prefix+"trackCaloClusEta_ele");
    static const SG::AuxElement::Decorator< std::vector<float>  > dec_caloclusPhi(m_prefix+"trackCaloClusPhi_ele");
    static const SG::AuxElement::Decorator< std::vector<float>  > dec_caloclusE(m_prefix+"trackCaloClusE_ele");
    static const SG::AuxElement::Decorator< std::vector<float>  > dec_caloclusM(m_prefix+"trackCaloClusM_ele");
    static const SG::AuxElement::Decorator< std::vector< std::vector<float> > > dec_sampleE(m_prefix+"trackCaloSampleE_ele");
    static const SG::AuxElement::Decorator< std::vector< std::vector<int> > > dec_sampleNumber(m_prefix+"trackCaloSampleNumber_ele");


    //Tracks
    const xAOD::TrackParticleContainer* tracks = nullptr;
    ATH_CHECK(evtStore()->retrieve( tracks,m_containerName));

    // CaloClusters
    const xAOD::CaloClusterContainer* caloClusters = nullptr;
    ATH_CHECK(evtStore()->retrieve(caloClusters, "CaloCalTopoClusters" ));


    // Find calo clusters associated to each track
    for(const xAOD::TrackParticle* trk : *tracks){
      std::vector<float> caloClusEta;
      std::vector<float> caloClusPhi;
      std::vector<float> caloClusE;
      std::vector<float> caloClusM;
      std::vector< std::vector<float> > vec_sampleE;
      std::vector< std::vector<int> > vec_sampleNumber;

      dec_caloclusEta(*trk) = caloClusEta;
      dec_caloclusPhi(*trk) = caloClusPhi;
      dec_caloclusE(*trk) = caloClusE;
      dec_caloclusM(*trk) = caloClusM;
      dec_sampleE(*trk)=vec_sampleE;
      dec_sampleNumber(*trk)=vec_sampleNumber;

      double trkPt=trk->pt()*0.001; // [GeV]
      double trkCharge=trk->charge();
      if(trkPt>m_upperPtLimit || trkPt<m_lowerPtLimit){continue;}

      for( const auto& clus : *caloClusters ){
	std::vector<float> sampleE;
	std::vector<int> sampleNumber;

	double dEta=clus->eta()-trk->eta();
	double dPhi=P4Helpers::deltaPhi(clus->phi(),trk->phi());
	double abs_trkEta=std::abs(trk->eta());

	//extrapolate of phi direction.
	double cosdPhi = std::cos(dPhi);
	double sindPhi = std::sin(dPhi);
	double sindPhi_ext = abs_trkEta<1.5 ? sindPhi+0.45*trkCharge/trkPt : sindPhi+2/(exp(1.13*abs_trkEta)-exp(-1.13*abs_trkEta))*trkCharge/trkPt;

	bool dPhiMatched = (std::abs(sindPhi_ext)<0.2) && (cosdPhi>m_cosdPhiCut);
	if(std::abs(dEta)>m_dEtaCut || !dPhiMatched){continue;}

	// no calo cluster cuts in DAOD (20220708)

	caloClusEta.push_back(clus->eta());
	caloClusPhi.push_back(clus->phi());
	caloClusE.push_back(clus->e());
	caloClusM.push_back(clus->m());
	for (int iSample=CaloSampling::PreSamplerB;iSample < CaloSampling::Unknown; ++iSample){
	  CaloSampling::CaloSample sampling=static_cast<CaloSampling::CaloSample>(iSample);
	  if(clus->hasSampling (sampling)) {
	    sampleE.push_back(clus->eSample(sampling));
	    sampleNumber.push_back(iSample);
	  }
	}

	vec_sampleE.push_back(sampleE);
	vec_sampleNumber.push_back(sampleNumber);
 
      }

      dec_sampleE(*trk) = vec_sampleE;
      dec_sampleNumber(*trk) = vec_sampleNumber;
      dec_caloclusEta(*trk) = caloClusEta;
      dec_caloclusPhi(*trk) = caloClusPhi;
      dec_caloclusE(*trk) = caloClusE;
      dec_caloclusM(*trk) = caloClusM;

    }

    return StatusCode::SUCCESS;
  } 

}
