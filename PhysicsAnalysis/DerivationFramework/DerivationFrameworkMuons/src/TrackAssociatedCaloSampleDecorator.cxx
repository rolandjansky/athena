/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


/*  Decorates the InDetTrackParticles Container with calorimeter  */
/*  sample information. 						  */

/*  Author: Hiroaki Hibi (hiroaki.hibi@cern.ch)   */	

#include "DerivationFrameworkMuons/TrackAssociatedCaloSampleDecorator.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "FourMomUtils/P4Helpers.h"

#include <vector>
#include <string>
#include <fstream>

namespace DerivationFramework {

  TrackAssociatedCaloSampleDecorator::TrackAssociatedCaloSampleDecorator( const std::string& t,
									  const std::string& n,
									  const IInterface* p) :
    AthAlgTool(t,n,p),
    m_containerName("")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("ContainerName", m_containerName);
    declareProperty("UpperPtLimit",m_upperPtLimit = 5 ,"upper limit to track pT in GeV");
    declareProperty("LowerPtLimit",m_lowerPtLimit = 1 ,"lower limit to track pT in GeV");
    declareProperty("dEtaCut",m_dEtaCut = 0.15);
    declareProperty("cosdPhiCut",m_cosdPhiCut = 0.6);
    declareProperty("sindPhiLongTailCut",m_sindPhiLongTailCut = 0.25);
    declareProperty("sindPhiShortTailCut",m_sindPhiShortTailCut = 0.15);
  }

  StatusCode TrackAssociatedCaloSampleDecorator::initialize() {

    if ( m_containerName.empty()) {
      ATH_MSG_FATAL("No TrackParticle collection provided for TrackAssociatedCaloSampleDecorator!");
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }


  StatusCode TrackAssociatedCaloSampleDecorator::addBranches() const  {


    static const SG::AuxElement::Decorator< std::vector<float>  > dec_caloclusEta( "trackCaloClusEta");
    static const SG::AuxElement::Decorator< std::vector<float>  > dec_caloclusPhi( "trackCaloClusPhi");
    static const SG::AuxElement::Decorator< std::vector< std::vector<float> > > dec_sampleE( "trackCaloSampleE");
    static const SG::AuxElement::Decorator< std::vector< std::vector<int> > > dec_sampleNumber( "trackCaloSampleNumber");


    //Tracks
    const xAOD::TrackParticleContainer* tracks = nullptr;
    ATH_CHECK(evtStore()->retrieve( tracks,m_containerName));

    // CaloClusters
    const xAOD::CaloClusterContainer* caloClusters = nullptr;
    ATH_CHECK(evtStore()->retrieve(caloClusters, "CaloCalTopoClusters" ));


     // calurate LowPtMuonDNNScore
    for(const xAOD::TrackParticle* trk : *tracks){
      std::vector<float> caloClusEta;
      std::vector<float> caloClusPhi;
      std::vector< std::vector<float> > vec_sampleE;
      std::vector< std::vector<int> > vec_sampleNumber;

      dec_caloclusEta(*trk) = caloClusEta;
      dec_caloclusPhi(*trk) = caloClusPhi;
      dec_sampleE(*trk)=vec_sampleE;
      dec_sampleNumber(*trk)=vec_sampleNumber;

      double trkPt=trk->pt()*0.001;// [GeV]
      if(trkPt>m_upperPtLimit || trkPt<m_lowerPtLimit){continue;}

      // calulate number of matched calo cluster to the track
      int N_caloClus=0;

      for( const auto& clus : *caloClusters ){
	std::vector<float> sampleE;
	std::vector<int> sampleNumber;

	double dEta=clus->eta()-trk->eta();
	double dPhi=P4Helpers::deltaPhi(clus->phi(),trk->phi());

	//extrapolate of Phi direction.
	//sin(dPhi) should be +/- 0.5/TrackPt. where dPhi is phi differance between calo cluster and track at IP; 
	double dSinPhi = trk->charge()==1 ? std::sin(dPhi)+0.5*(1/(trkPt)) : std::sin(dPhi)-0.5*(1/(trkPt));
	double cosdPhi = std::cos(dPhi);

	bool dPHImatching=trk->charge()==1 ? -m_sindPhiShortTailCut < dSinPhi && dSinPhi < m_sindPhiLongTailCut : -m_sindPhiLongTailCut < dSinPhi && dSinPhi < m_sindPhiShortTailCut;
	if(std::abs(dEta)>m_dEtaCut || !dPHImatching || cosdPhi<m_cosdPhiCut){continue;}

	N_caloClus++;
	caloClusEta.push_back(clus->eta());
	caloClusPhi.push_back(clus->phi());
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

    }

    return StatusCode::SUCCESS;
  } 

}
