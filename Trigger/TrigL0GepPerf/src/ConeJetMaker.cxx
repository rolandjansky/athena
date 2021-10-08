/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#include "TrigL0GepPerf/CustomJet.h"
#include "TrigL0GepPerf/ConeJetMaker.h"


double deltaR (double eta_1, double eta_2, double phi_1, double phi_2);

std::vector<Gep::CustomJet> Gep::ConeJetMaker::makeJet( const std::vector<Gep::CustomTopoCluster> &clusters)
{
  std::vector<Gep::CustomJet> jets;

  for (auto seed: *m_seeds) {
    
    float seedEt = seed->et8x8();
    // skip seeds with Et below threshold
    if(seedEt < m_seedEtThreshold) continue; 
    float seedEta = seed->eta();
    float seedPhi = seed->phi();

    Gep::CustomJet jet;

    jet.radius = m_jetR;
    jet.seedEta = seedEta;
    jet.seedPhi = seedPhi;
    jet.seedEt = seedEt;

    TLorentzVector jetVec;
    float px{0}, py{0}, pz{0};
    int clusterIndex {0};
    
    //build jet with clusters within dR from seed
    for (const auto &cl: clusters) {
      float dR_seed_cl = deltaR(seedEta, cl.vec.Eta(), seedPhi, cl.vec.Phi());

      if (dR_seed_cl < m_jetR) {
        jetVec += cl.vec;        
        px += cl.vec.Px();
	py += cl.vec.Py();
	pz += cl.vec.Pz();

        jet.constituentsIndices.push_back(clusterIndex);
      }
      clusterIndex++;
    }


    // skip cone jets with 0 constituents
    if (jet.constituentsIndices.size()==0) continue;
    

    // recombination scheme
    if (m_recombScheme == "EScheme") {
      // default option: add four-vectors of constituents
      jet.vec = jetVec;
    }
    else if (m_recombScheme == "SeedScheme") {
      // massless jet, correct pt, re-using seed (eta,phi)
      float m = 0;
      float pt = std::sqrt(px*px + py*py);
      jet.vec.SetPtEtaPhiM(pt, seedEta, seedPhi, m);
    }
    else {
      std::cerr << "Error: " << m_recombScheme << " is not a valid recombination scheme.\n";
      std::exit(1);
    }

    jets.push_back(jet);
  }

  return jets;
}


double deltaR (double eta1, double eta2, double phi1, double phi2) {
  double deltaPhi = TVector2::Phi_mpi_pi(phi1 - phi2);
  double deltaEta = eta1 - eta2;
  return std::sqrt( deltaEta*deltaEta + deltaPhi*deltaPhi );
}

