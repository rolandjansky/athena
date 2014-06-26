/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 /**
 * file: TauPi0Clusters.cxx
 *
 * Author: Michel Trottier-McDonald (mtm@cern.ch)
 */

#include "TauDiscriminant/TauPi0Clusters.h"
#include <utility>

#include "xAODTau/TauJet.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "AnalysisUtils/AnalysisMisc.h"
#include "xAODTracking/VertexContainer.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloUtils/CaloVertexedCluster.h"

#include <TLorentzVector.h>

#include "math.h"

//---------------------------------------------------------
// Constructor
//---------------------------------------------------------
TauPi0Clusters::TauPi0Clusters(const xAOD::TauJet& tauJet) : m_cl1_Pt(0.),
    m_cl1_Eta(0.),
    m_cl1_Phi(0.),
    m_cl2_Pt(0.),
    m_cl2_Eta(0.),
    m_cl2_Phi(0.),
    m_tau_vis_Pt(0.),
    m_tau_vis_Eta(0.),
    m_tau_vis_Phi(0.),
    m_tau_vis_M(0.)
{
    runPi0Finder(tauJet);
}




//---------------------------------------------------------
// run Pi0Finder
//---------------------------------------------------------
void TauPi0Clusters::runPi0Finder(const xAOD::TauJet& tauJet)
{
    //Get tracks
    int nTracks = tauJet.nTracks();
    std::vector<TLorentzVector> tracks;

    for(int i = 0; i < nTracks; ++i)
    {
	float track_Pt  = tauJet.track(i)->pt();
	float track_Eta = tauJet.track(i)->eta();
	float track_Phi = tauJet.track(i)->phi();

	if(track_Pt > 0.0 and track_Eta < 5.0)
	{
	    TLorentzVector newTrack = TLorentzVector();
	    newTrack.SetPtEtaPhiM(track_Pt, track_Eta, track_Phi, 0.0);
	    tracks.push_back(newTrack);
	}
    }


    //Get clusters

    std::vector<const xAOD::CaloCluster*> Clusters;

    const xAOD::Jet* pJetSeed = (*tauJet.jetLink());
    xAOD::JetConstituentVector jcv = pJetSeed->getConstituents();
    
    xAOD::JetConstituentVector::const_iterator firstcluster = jcv.begin();
    xAOD::JetConstituentVector::const_iterator lastcluster = jcv.end();

    for ( ; firstcluster != lastcluster; firstcluster++ ) {
    	const xAOD::CaloCluster *p_cluster = dynamic_cast<const xAOD::CaloCluster*> ((*firstcluster)->rawConstituent()); //to get to cluster-specific variables
    	if (!p_cluster) continue;
    	Clusters.push_back(p_cluster);
    }


    
    // now insert clusters into event ordered by energy
    // this makes it much faster to recalculate cluster-based
    // variables in macros later
    
    int nClusters = int(Clusters.size());

    std::vector<TLorentzVector> clusters;
    std::vector<float> PSSFs;
    std::vector<float> EM2Fs;
    std::vector<float> EM3Fs;

    if(nClusters > 0)
    {

	AnalysisUtils::Sort::e (&Clusters);
	
	for(int i = 0; i < nClusters; ++i)
	{
	    const xAOD::CaloCluster *p_cluster = Clusters[i];
	    // Simplified Sampling information
	    float PreSampler = p_cluster->eSample(CaloSampling::PreSamplerB) + p_cluster->eSample(CaloSampling::PreSamplerE);
	    float EMLayer1   = p_cluster->eSample(CaloSampling::EMB1)        + p_cluster->eSample(CaloSampling::EME1);
	    float EMLayer2   = p_cluster->eSample(CaloSampling::EMB2)        + p_cluster->eSample(CaloSampling::EME2);
	    float EMLayer3   = p_cluster->eSample(CaloSampling::EMB3)        + p_cluster->eSample(CaloSampling::EME3);
	    
	    float Energy   = p_cluster->rawE();
	    
	    float PSSF = (PreSampler + EMLayer1)/Energy;
	    float EM2F = EMLayer2/Energy;
	    float EM3F = EMLayer3/Energy;
	    
	    if(PSSF < 0.) PSSF = 0.;
	    if(PSSF > 1.) PSSF = 1.;
	    
	    if(EM2F < 0.) EM2F = 0.;
	    if(EM2F > 1.) EM2F = 1.;
	    
	    if(EM3F < 0.) EM3F = 0.;
	    if(EM3F > 1.) EM3F = 1.;

	    xAOD::CaloVertexedCluster* clusterCorr;
	    if (tauJet.vertexLink()) {
	    	// Corrected cluster direction information (leave as jet constituents on purpose!)
	    	clusterCorr = new xAOD::CaloVertexedCluster(*p_cluster, (*tauJet.vertexLink())->position());
	    }
	    else {
	    	//no correction
	    	clusterCorr = new xAOD::CaloVertexedCluster(*p_cluster);
	    }

	    float cluster_Eta = clusterCorr->eta();
	    float cluster_Pt  = clusterCorr->e()/cosh(cluster_Eta);
	    float cluster_Phi = clusterCorr->phi();

	    if(cluster_Pt > 0.0 && cluster_Eta < 5.0)
	    {
		TLorentzVector newCluster = TLorentzVector();
		newCluster.SetPtEtaPhiM(cluster_Pt, cluster_Eta, cluster_Phi, 0.0);
		clusters.push_back(newCluster);
		PSSFs.push_back(PSSF);
		EM2Fs.push_back(EM2F);
		EM3Fs.push_back(EM3F);
	    }
	    
	    // clean up corrected cluster
	    if (clusterCorr) delete clusterCorr;

	}
    }

    if(nTracks > 0 && nClusters > 0)
    {

	Pi0Finder pi0F = Pi0Finder(tracks, clusters, PSSFs, EM2Fs, EM3Fs);
	TLorentzVector cl1 = pi0F.pi0TLV1();
	TLorentzVector cl2 = pi0F.pi0TLV2();
	TLorentzVector tau = pi0F.visTauTLV();

	m_cl1_Pt  = cl1.Pt();
	if(m_cl1_Pt > 0.0)
	    m_cl1_Eta = cl1.Eta();
	m_cl1_Phi = cl1.Phi();

	m_cl2_Pt  = cl2.Pt();
	if(m_cl2_Pt > 0.0)
	    m_cl2_Eta = cl2.Eta();
	m_cl2_Phi = cl2.Phi();

	m_tau_vis_Pt  = tau.Pt();
	if(m_tau_vis_Pt > 0.0)
	    m_tau_vis_Eta = tau.Eta();
	m_tau_vis_Phi = tau.Phi();
	m_tau_vis_M   = tau.M();
    }
}
