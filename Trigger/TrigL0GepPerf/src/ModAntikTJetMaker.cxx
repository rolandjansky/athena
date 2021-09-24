/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#include "TrigL0GepPerf/ModAntikTJetMaker.h"

std::vector<Gep::CustomJet> Gep::ModAntikTJetMaker::makeJet(const std::vector<Gep::CustomTopoCluster> &clusters) {

  std::vector<Gep::CustomTopoCluster> constituents = clusters;

  std::vector<Gep::CustomJet> v_jets;

  const unsigned int n_constituents = constituents.size();
  std::vector<int> n_constitutents_in_jet(n_constituents, 1);

  int niter = 0;

  //Iterate over clusters
  while(constituents.size() > 0 && niter < m_nIter){

    const unsigned int n_clusters = constituents.size();

    //create factorized separation list
    std::vector< std::vector< float > > DeltaR2(n_clusters, std::vector< float >(n_clusters, 0.0));
    std::vector< float > InvPt2 (n_clusters, 0.0);
    for (unsigned int i = 0; i < n_clusters; i++){
      InvPt2[i] = 1/constituents.at(i).vec.Perp2();
      DeltaR2[i][i] = 1.0;
      for(unsigned int j = i+1; j < n_clusters; j++){
        float deltaEta = constituents.at(i).vec.Eta() - constituents.at(j).vec.Eta();
        float deltaPhi = constituents.at(i).vec.DeltaPhi(constituents.at(j).vec);
        DeltaR2[i][j] = (deltaEta*deltaEta + deltaPhi*deltaPhi)/(m_jetR*m_jetR);
	DeltaR2[j][i] = (deltaEta*deltaEta + deltaPhi*deltaPhi)/(m_jetR*m_jetR);
      }
    }
    

    // indices of minimum DeltaR in i-th row (aka j in d_ij)
    std::vector< int > minDeltaR2_indices(n_clusters, 0.0);
    for (unsigned int i = 0; i < n_clusters; i++ ) {
      minDeltaR2_indices[i] = std::distance( DeltaR2[i].begin(), std::min_element(std::begin(DeltaR2[i]), std::end(DeltaR2[i])) );
    }

    //d_ij vector of with minDeltaR2_indices_ij
    std::vector< float > InvPt2DeltaR2(n_clusters, 0.0);
    for (unsigned int i = 0; i < n_clusters; i++ ) InvPt2DeltaR2[i] = InvPt2[i]*DeltaR2[i][minDeltaR2_indices[i]];

    // index of minimum d_ij (aka i in d_ij)
    int minInvPt2DeltaR2_index = std::distance(InvPt2DeltaR2.begin() , std::min_element(std::begin(InvPt2DeltaR2), std::end(InvPt2DeltaR2)) );

    // if d_ij < diB, merge j into i, otherwise i is jet
    if (InvPt2DeltaR2[minInvPt2DeltaR2_index] < InvPt2[minInvPt2DeltaR2_index]){
      //This is the WTA pt merging scheme
      // only momentum components are modified
      TVector3 momentum_vector(constituents.at(minInvPt2DeltaR2_index).vec.Px() + constituents.at(minDeltaR2_indices[minInvPt2DeltaR2_index]).vec.Px(),
			       constituents.at(minInvPt2DeltaR2_index).vec.Py() + constituents.at(minDeltaR2_indices[minInvPt2DeltaR2_index]).vec.Py(),
			       constituents.at(minInvPt2DeltaR2_index).vec.Pz() + constituents.at(minDeltaR2_indices[minInvPt2DeltaR2_index]).vec.Pz());

      //px, py and pz are recalculated from pt, eta and phi, leaving eta and phi unchanged 
      constituents.at(minInvPt2DeltaR2_index).vec.SetPtEtaPhiE(momentum_vector.Pt(), 
							 constituents.at(minInvPt2DeltaR2_index).vec.Eta(), 
							 constituents.at(minInvPt2DeltaR2_index).vec.Phi(), 
							 momentum_vector.Mag() );  

      // add one to minInvPt2DeltaR2_index to keep track on number of 
      // constituents already recombined into a single object
      n_constitutents_in_jet.at(minInvPt2DeltaR2_index)++;

      constituents.erase(constituents.begin() + minDeltaR2_indices[minInvPt2DeltaR2_index]);
      n_constitutents_in_jet.erase(n_constitutents_in_jet.begin() + minDeltaR2_indices[minInvPt2DeltaR2_index]);

    } else {

      //create custom jet based on constituents.at(minInvPt2DeltaR2_index)
      Gep::CustomJet jet;
      jet.vec.SetXYZT(constituents.at(minInvPt2DeltaR2_index).vec.Px(), constituents.at(minInvPt2DeltaR2_index).vec.Py(),
		    constituents.at(minInvPt2DeltaR2_index).vec.Pz(), constituents.at(minInvPt2DeltaR2_index).vec.E());
      jet.nConstituents = n_constitutents_in_jet.at(minInvPt2DeltaR2_index);

      v_jets.push_back(jet);

      constituents.erase(constituents.begin() + minInvPt2DeltaR2_index);
      n_constitutents_in_jet.erase(n_constitutents_in_jet.begin() + minDeltaR2_indices[minInvPt2DeltaR2_index]);

    }

    niter++;
  }

  return   v_jets;
}

