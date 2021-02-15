/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//********************************************************************//
// NAME:     TauSubstructureVariables.cxx                             //
// AUTHORS:  M. Trottier-McDonald                                     //
// CREATED:  January 11 2010                                          //
//********************************************************************//

#include <algorithm> 
#include <cmath>
#include <sstream>

#include "xAODJet/Jet.h"
#include "xAODTau/TauJet.h"

#include "tauRecTools/CaloClusterVariables.h"
#include "tauRecTools/TauSubstructureVariables.h"
#include "tauRecTools/HelperFunctions.h"

const float TauSubstructureVariables::DEFAULT = -1111.;

TauSubstructureVariables::TauSubstructureVariables( const std::string& name )
  : TauRecToolBase(name) {
  declareProperty("VertexCorrection", m_doVertexCorrection = true);
}



StatusCode TauSubstructureVariables::execute(xAOD::TauJet& tau) const {

  CaloClusterVariables CaloClusterVariablesTool;

  bool isFilled = CaloClusterVariablesTool.update(tau);

  if (!isFilled) {
    ATH_MSG_DEBUG("problem in calculating calo cluster variables -> will be set to -1111");

    tau.setDetail(xAOD::TauJetParameters::numTopoClusters, static_cast<int>(DEFAULT));
    tau.setDetail(xAOD::TauJetParameters::numEffTopoClusters, DEFAULT);
    tau.setDetail(xAOD::TauJetParameters::topoInvMass, DEFAULT);
    tau.setDetail(xAOD::TauJetParameters::effTopoInvMass, DEFAULT);
    tau.setDetail(xAOD::TauJetParameters::topoMeanDeltaR, DEFAULT);
    tau.setDetail(xAOD::TauJetParameters::effTopoMeanDeltaR, DEFAULT);
  } 
  else {
    tau.setDetail(xAOD::TauJetParameters::numTopoClusters, static_cast<int>(CaloClusterVariablesTool.numConstituents()));
    tau.setDetail(xAOD::TauJetParameters::numEffTopoClusters, static_cast<float>(CaloClusterVariablesTool.effectiveNumConstituents()));
    tau.setDetail(xAOD::TauJetParameters::topoInvMass, static_cast<float>(CaloClusterVariablesTool.totalMass()));
    tau.setDetail(xAOD::TauJetParameters::effTopoInvMass, static_cast<float>(CaloClusterVariablesTool.effectiveMass()));
    tau.setDetail(xAOD::TauJetParameters::topoMeanDeltaR, static_cast<float>(CaloClusterVariablesTool.averageRadius()));
    tau.setDetail(xAOD::TauJetParameters::effTopoMeanDeltaR, static_cast<float>(CaloClusterVariablesTool.averageEffectiveRadius()));
  }

  //*****************************************************
  // calculate some new cluster based ID variables
  //*****************************************************
  // New cluster-based variables
  float totalEnergy(0.);
  float calo_iso(0.);
  float dr(0.);

  TLorentzVector leadClusVec;
  TLorentzVector subLeadClusVec;
  TLorentzVector approxSubstructure4Vec;
  double clusELead = DEFAULT;
  double clusESubLead = DEFAULT;

  TLorentzVector tauAxis = tauRecTools::getTauAxis(tau, m_doVertexCorrection);

  // TODO: check which scale is needed here
  // p4 from cluster is at LC scale, p4 from vertexedCluster is at LC/EM scale for LC/EM seed jets
  std::vector<xAOD::CaloVertexedTopoCluster> vertexedClusterList = tau.vertexedClusters();
  for (const xAOD::CaloVertexedTopoCluster& vertexedCluster : vertexedClusterList){
    TLorentzVector clusterP4 = vertexedCluster.p4();

    totalEnergy += clusterP4.E();
		
    dr = tauAxis.DeltaR(clusterP4); 
    
    if (0.2 <= dr && dr < 0.4) {
      calo_iso += clusterP4.Et();
    }
    else if (dr < 0.2) {
      const xAOD::CaloCluster& cluster = vertexedCluster.clust();
      double clusEnergyBE = ( cluster.energyBE(0) + cluster.energyBE(1) + cluster.energyBE(2) );
		    
      if (clusEnergyBE > clusELead) {
	//change current leading cluster to subleading
	clusESubLead = clusELead;
	subLeadClusVec = leadClusVec;

	//set energy and 4-vector of leading cluster
	clusELead = clusEnergyBE;
	leadClusVec.SetPtEtaPhiM(clusELead/std::cosh(clusterP4.Eta()), clusterP4.Eta(), clusterP4.Phi(), 0.);
      }
      else if (clusEnergyBE > clusESubLead) {
	//set energy and 4-vector of subleading cluster only
	clusESubLead = clusEnergyBE;
	subLeadClusVec.SetPtEtaPhiM(clusESubLead/std::cosh(clusterP4.Eta()), clusterP4.Eta(), clusterP4.Phi(), 0.);
      }
    }	
  }

  if (clusELead > 0.) {
    approxSubstructure4Vec += leadClusVec;
  }
  if (clusESubLead > 0.) {
    approxSubstructure4Vec += subLeadClusVec;
  }

  // now sort cluster by energy
  auto compare = [](const xAOD::CaloVertexedTopoCluster& left, const xAOD::CaloVertexedTopoCluster& right) {
    return left.e() > right.e();
  };
  std::sort(vertexedClusterList.begin(), vertexedClusterList.end(), compare);

  // determine energy sum of leading 2 and leading 3 clusters
  float sum2LeadClusterE = 0.;
  if(vertexedClusterList.size()>0) {
    sum2LeadClusterE = vertexedClusterList.at(0).e();
    if(vertexedClusterList.size()>1) sum2LeadClusterE += vertexedClusterList.at(1).e();
  }
  float sum3LeadClusterE = sum2LeadClusterE;
  if(vertexedClusterList.size()>2) sum3LeadClusterE += vertexedClusterList.at(2).e();

  if (totalEnergy != 0.) {
    tau.setDetail(xAOD::TauJetParameters::lead2ClusterEOverAllClusterE, (sum2LeadClusterE / totalEnergy) );
    tau.setDetail(xAOD::TauJetParameters::lead3ClusterEOverAllClusterE, (sum3LeadClusterE / totalEnergy) );
  }

  tau.setDetail(xAOD::TauJetParameters::caloIso, calo_iso);

  // calculate calorimeter energies in different layers
  float PSSEnergy(0.);
  float EMEnergy(0.);
  float HADEnergy(0.);

  for (const xAOD::CaloVertexedTopoCluster& vertexedCluster : vertexedClusterList) {
    // It is at EM/LC scale for EM/LC seed jets
    float clEnergy = vertexedCluster.e();

    const xAOD::CaloCluster& cluster = vertexedCluster.clust();
    
    // Calculate the fractions of energy in different calorimeter layers
    float PreSampler = cluster.eSample(CaloSampling::PreSamplerB) + cluster.eSample(CaloSampling::PreSamplerE);
    float EMLayer1   = cluster.eSample(CaloSampling::EMB1) + cluster.eSample(CaloSampling::EME1);
    float EMLayer2   = cluster.eSample(CaloSampling::EMB2) + cluster.eSample(CaloSampling::EME2);

    float Energy = cluster.rawE();
    float PSSF = (Energy != 0.) ? (PreSampler + EMLayer1) / Energy : 0.;
    float EM2F = (Energy != 0.) ? EMLayer2 / Energy : 0.;
    float EMF = PSSF + EM2F;

    PSSEnergy += PSSF * clEnergy;
    EMEnergy  += EMF * clEnergy;
    HADEnergy += (Energy != 0.) ? (1 - EMF) * clEnergy : 0.;
  }

  // calculate trk momentum
  float trkSysMomentum(0.);
  for (size_t i=0; i < tau.nTracks(); ++i) {
    trkSysMomentum += tau.track(i)->pt() * std::cosh(tau.track(i)->eta());

    //adding the core tracks to the approximate substructure 4 vector
    approxSubstructure4Vec += tau.track(i)->p4();
  }

  // set new approximate energy flow variables for tau ID
  if(m_doVertexCorrection) {
    tau.setDetail(xAOD::TauJetParameters::ptRatioEflowApprox, static_cast<float>(approxSubstructure4Vec.Pt()/ tau.ptIntermediateAxis()) );
  }
  else {
    tau.setDetail(xAOD::TauJetParameters::ptRatioEflowApprox, static_cast<float>(approxSubstructure4Vec.Pt()/ tau.ptDetectorAxis()) );
  }
  tau.setDetail(xAOD::TauJetParameters::mEflowApprox, static_cast<float>(approxSubstructure4Vec.M()) );

  float fPSSFraction = (totalEnergy != 0.) ? PSSEnergy / totalEnergy : DEFAULT;
  float fChPIEMEOverCaloEME = (EMEnergy != 0.) ? (trkSysMomentum - HADEnergy) / EMEnergy : DEFAULT;
  float fEMPOverTrkSysP = DEFAULT;
  if (tau.nTracks() > 0) fEMPOverTrkSysP = (trkSysMomentum != 0.) ? EMEnergy / trkSysMomentum : DEFAULT;

  tau.setDetail(xAOD::TauJetParameters::PSSFraction, fPSSFraction);
  tau.setDetail(xAOD::TauJetParameters::ChPiEMEOverCaloEME, fChPIEMEOverCaloEME);
  tau.setDetail(xAOD::TauJetParameters::EMPOverTrkSysP, fEMPOverTrkSysP);

  // calculate dRMax
  size_t numTrack = tau.nTracks();
  if (numTrack > 0) {
    float dRmax = 0.;
    float dR = 0.;

    for (size_t i=0; i < numTrack; ++i) {
      dR = tau.track(i)->p4().DeltaR(tauAxis);
      if (dR > dRmax) dRmax = dR;
    }
    tau.setDetail(xAOD::TauJetParameters::dRmax, dRmax);
  }

  return StatusCode::SUCCESS;
}
