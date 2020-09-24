/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//********************************************************************//
// NAME:     TauSubstructureVariables.cxx                             //
// PACKAGE:  offline/Reconstruction/tauRec                            //
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

#define GeV 1000
const float TauSubstructureVariables::DEFAULT = -1111.;



TauSubstructureVariables::TauSubstructureVariables( const std::string& name )
  : TauRecToolBase(name) {
  declareProperty("IncShowerSubtr", m_incShowerSubtr = true);
}



TauSubstructureVariables::~TauSubstructureVariables() {
}



StatusCode TauSubstructureVariables::initialize() {
  ATH_CHECK(m_tauVertexCorrection.retrieve()); 
  return StatusCode::SUCCESS;
}



StatusCode TauSubstructureVariables::execute(xAOD::TauJet& pTau) const {

  CaloClusterVariables CaloClusterVariablesTool;
  CaloClusterVariablesTool.setIncSub(m_incShowerSubtr);

  bool isFilled = CaloClusterVariablesTool.update(pTau, m_tauVertexCorrection);

  if (!isFilled) {
    ATH_MSG_DEBUG("problem in calculating calo cluster variables -> will be set to -1111");

    pTau.setDetail(xAOD::TauJetParameters::numTopoClusters, static_cast<int>(DEFAULT));
    pTau.setDetail(xAOD::TauJetParameters::numEffTopoClusters, DEFAULT);
    pTau.setDetail(xAOD::TauJetParameters::topoInvMass, DEFAULT);
    pTau.setDetail(xAOD::TauJetParameters::effTopoInvMass, DEFAULT);
    pTau.setDetail(xAOD::TauJetParameters::topoMeanDeltaR, DEFAULT);
    pTau.setDetail(xAOD::TauJetParameters::effTopoMeanDeltaR, DEFAULT);
  } 
  else {
    pTau.setDetail(xAOD::TauJetParameters::numTopoClusters, static_cast<int>(CaloClusterVariablesTool.numConstituents()));
    pTau.setDetail(xAOD::TauJetParameters::numEffTopoClusters, static_cast<float>(CaloClusterVariablesTool.effectiveNumConstituents()));
    pTau.setDetail(xAOD::TauJetParameters::topoInvMass, static_cast<float>(CaloClusterVariablesTool.totalMass()));
    pTau.setDetail(xAOD::TauJetParameters::effTopoInvMass, static_cast<float>(CaloClusterVariablesTool.effectiveMass()));
    pTau.setDetail(xAOD::TauJetParameters::topoMeanDeltaR, static_cast<float>(CaloClusterVariablesTool.averageRadius()));
    pTau.setDetail(xAOD::TauJetParameters::effTopoMeanDeltaR, static_cast<float>(CaloClusterVariablesTool.averageEffectiveRadius()));
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

  if (! pTau.jetLink().isValid()) {
    ATH_MSG_ERROR("Tau jet link is invalid.");
    return StatusCode::FAILURE;
  }
  const xAOD::Jet *jetSeed = pTau.jet();
  
  const xAOD::Vertex* jetVertex = m_tauVertexCorrection->getJetVertex(*jetSeed);
  
  const xAOD::Vertex* tauVertex = nullptr;
  if (pTau.vertexLink().isValid()) tauVertex = pTau.vertex();

  TLorentzVector tauAxis = m_tauVertexCorrection->getTauAxis(pTau);

  std::vector<const xAOD::CaloCluster*> vClusters;
  ATH_CHECK(tauRecTools::GetJetClusterList(jetSeed, vClusters, m_incShowerSubtr));
  
  for (auto cluster : vClusters){
    totalEnergy += cluster->e();
		
    TLorentzVector clusterP4 = m_tauVertexCorrection->getVertexCorrectedP4(*cluster, tauVertex, jetVertex);
    dr = tauAxis.DeltaR(clusterP4);    
    
    if (0.2 <= dr && dr < 0.4) {
	  calo_iso += clusterP4.Et();
    }
    else if (dr < 0.2) {
	  double clusEnergyBE = ( cluster->energyBE(0) + cluster->energyBE(1) + cluster->energyBE(2) );
		    
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
  std::sort(vClusters.begin(), vClusters.end(), DefCaloClusterCompare());

  // determine energy sum of leading 2 and leading 3 clusters
  float sum2LeadClusterE = 0.;
  if(vClusters.size()>0) {
    sum2LeadClusterE = vClusters.at(0)->e();
    if(vClusters.size()>1) sum2LeadClusterE += vClusters.at(1)->e();
  }
  float sum3LeadClusterE = sum2LeadClusterE;
  if(vClusters.size()>2) sum3LeadClusterE += vClusters.at(2)->e();

  if (totalEnergy != 0.) {
    pTau.setDetail(xAOD::TauJetParameters::lead2ClusterEOverAllClusterE, (sum2LeadClusterE / totalEnergy) );
    pTau.setDetail(xAOD::TauJetParameters::lead3ClusterEOverAllClusterE, (sum3LeadClusterE / totalEnergy) );
  }

  pTau.setDetail(xAOD::TauJetParameters::caloIso, calo_iso);

  // calculate calorimeter energies in different layers
  float PSSEnergy(0.);
  float EMEnergy(0.);
  float HADEnergy(0.);

  for (auto cl : vClusters) {
    float clEnergy = cl->e();

    //Calculate the fractions of energy in different calorimeter layers
    float PreSampler = cl->eSample(CaloSampling::PreSamplerB) + cl->eSample(CaloSampling::PreSamplerE);
    float EMLayer1   = cl->eSample(CaloSampling::EMB1) + cl->eSample(CaloSampling::EME1);
    float EMLayer2   = cl->eSample(CaloSampling::EMB2) + cl->eSample(CaloSampling::EME2);

    float Energy = cl->rawE();
    float PSSF = Energy != 0 ? (PreSampler + EMLayer1) / Energy : 0;
    float EM2F = Energy != 0 ? EMLayer2 / Energy : 0;
    float EMF = PSSF + EM2F;

    PSSEnergy += PSSF * clEnergy;
    EMEnergy  += EMF * clEnergy;
    HADEnergy += (Energy != 0) ? (1 - EMF) * clEnergy : 0;
  }

  // calculate trk momentum
  float trkSysMomentum(0.);
  for (size_t i=0; i < pTau.nTracks(); ++i) {
    trkSysMomentum += pTau.track(i)->pt() * std::cosh(pTau.track(i)->eta());

    //adding the core tracks to the approximate substructure 4 vector
    approxSubstructure4Vec += pTau.track(i)->p4();
  }

  // set new approximate energy flow variables for tau ID
  pTau.setDetail(xAOD::TauJetParameters::ptRatioEflowApprox, static_cast<float>(approxSubstructure4Vec.Pt()/ pTau.ptDetectorAxis()) );
  pTau.setDetail(xAOD::TauJetParameters::mEflowApprox, static_cast<float>(approxSubstructure4Vec.M()) );

  float fPSSFraction = (totalEnergy != 0.) ? PSSEnergy / totalEnergy : DEFAULT;
  float fChPIEMEOverCaloEME = (EMEnergy != 0.) ? (trkSysMomentum - HADEnergy) / EMEnergy : DEFAULT;
  float fEMPOverTrkSysP = DEFAULT;
  if (pTau.nTracks() > 0) fEMPOverTrkSysP = (trkSysMomentum != 0.) ? EMEnergy / trkSysMomentum : DEFAULT;

  pTau.setDetail(xAOD::TauJetParameters::PSSFraction, fPSSFraction);
  pTau.setDetail(xAOD::TauJetParameters::ChPiEMEOverCaloEME, fChPIEMEOverCaloEME);
  pTau.setDetail(xAOD::TauJetParameters::EMPOverTrkSysP, fEMPOverTrkSysP);

  // calculate dRMax
  size_t numTrack = pTau.nTracks();
  if (numTrack > 0) {
    float dRmax = 0.;
    float dR = 0.;

    for (size_t i=0; i < numTrack; ++i) {
      dR = pTau.track(i)->p4().DeltaR(tauAxis);
      if (dR > dRmax) dRmax = dR;
    }
    pTau.setDetail(xAOD::TauJetParameters::dRmax, dRmax);
  }

  return StatusCode::SUCCESS;
}
