/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloClusterMatching/TopoClusterMap.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"

bool CompareClusterET (const xAOD::CaloCluster* c1, const xAOD::CaloCluster* c2) 
{

  double Et1(c1->e()/cosh(c1->eta())), Et2(c2->e()/cosh(c2->eta()));
  return Et1 > Et2;

}

//////////////////////////////////////////////////
//Athena interfaces.
//////////////////////////////////////////////////

//Constructor.
TopoClusterMap::TopoClusterMap(float minEta, float minPhi, float maxEta, float maxPhi,
			       float dEta, float dPhi)
{
  m_minEta = minEta;
  m_minPhi = minPhi;
  m_maxEta = maxEta;
  m_maxPhi = maxPhi;
  m_dEta = dEta;
  m_dPhi = dPhi;
}

//Destructor.
TopoClusterMap::~TopoClusterMap() {}

//////////////////////////////////////////////////
//Functional routines.
//////////////////////////////////////////////////

StatusCode TopoClusterMap::SetTopoClusters(const xAOD::CaloClusterContainer *inputTopoClusterContainer) 

{

  double eta, phi;
  int i(-1);

  for(const auto& cc : *inputTopoClusterContainer) 
  {
    i++;
    //Retrieve eta, phi from ith topocluster.
    eta = cc->eta();
    phi = cc->phi();

    //Put it in appropriate vector.
    m_map[GetEtaPhiKeys(eta,phi).first][GetEtaPhiKeys(eta,phi).second].push_back(cc);
  }

  SortGridVectors();

  return StatusCode::SUCCESS;
}

void TopoClusterMap::InsertTopoCluster(xAOD::CaloCluster* topo) {

  //Retrieve eta, phi from ith topocluster.
  double eta(topo->phi()), phi(topo->phi());

  //Put it in appropriate vector.
  m_map.at(GetEtaPhiKeys(eta,phi).first).at(GetEtaPhiKeys(eta,phi).second).push_back(topo);

  //Re-sort the vector according to Et.
  SortGridVector(GetEtaPhiKeys(eta,phi).first, GetEtaPhiKeys(eta,phi).second);
  
}

//Boolean comparison somewhere here that sorts vectors in grid by Et.
void TopoClusterMap::SortGridVectors() {
  
  for (int i = 0; i <= (GetEtaPhiKeys(m_maxEta, m_maxPhi).first); i++)
    for (int j = 0; j <= (GetEtaPhiKeys(m_maxEta, m_maxPhi).second); j++)
      SortGridVector(i,j);

}

void TopoClusterMap::SortGridVector(int eta_key, int phi_key) {

  if (!m_map[eta_key][phi_key].empty()) 
    sort( m_map[eta_key][phi_key].begin(), m_map[eta_key][phi_key].end(), CompareClusterET );
}

void TopoClusterMap::ClearMap() {

  for (int i = 0; i <= (GetEtaPhiKeys(m_maxEta, m_maxPhi).first); i++)
    for (int j = 0; j <= (GetEtaPhiKeys(m_maxEta, m_maxPhi).second); j++)
      m_map[i][j].clear();
}

// void TopoClusterMap::DumpMapContents() {

//   for (int i = 0; i <= (GetEtaPhiKeys(m_maxEta, m_maxPhi).first); i++)
//     for (int j = 0; j <= (GetEtaPhiKeys(m_maxEta, m_maxPhi).second); j++) {
//       ATH_MSG_DEBUG("Size of topocluster vector at (" << i << "," << j << "): " << m_map[i][j].size());
//       if (m_map[i][j].size()) {
// 	ATH_MSG_DEBUG("Contents of vector:");
// 	for (unsigned int k = 0; k < m_map[i][j].size(); k++) {
// 	  //ATH_MSG_DEBUG("E: %f, eta: %f, phi: %f, Pt: %f \n",(m_map[i][j])[k]->e(), (m_map[i][j])[k]->eta(), (m_map[i][j])[k]->phi(), (m_map[i][j])[k]->pt());
// 	  ATH_MSG_DEBUG("E: " << (m_map[i][j])[k]->e()
// 			<< ", eta: "     << (m_map[i][j])[k]->eta()
// 			<< ", phi: "     << (m_map[i][j])[k]->phi()
// 			<< ", Pt:  "     << (m_map[i][j])[k]->pt());
// 	}
//       }
//     }
	

// }

//Routine to retrieve vector of TopoClusters for a given cluster Pt.
std::vector<const xAOD::CaloCluster*> TopoClusterMap::RetrieveTopoClusters(double eta, 
									   double phi,
									   double Pt) const
{

  if ((Pt * 1e-3) < 15) {
    return RetrieveTopoClusters(eta, phi, 0.2, 0.2);
  } 
  else if ((Pt * 1e-3) < 50) {
    return RetrieveTopoClusters(eta, phi, 0.2, 0.4);
  }
  else {
    return RetrieveTopoClusters(eta, phi, 0.2, 0.6);
  }

}


//Routine to retrieve vector of TopoClusters for a given (eta, phi) region.
std::vector<const xAOD::CaloCluster*> TopoClusterMap::RetrieveTopoClusters(double eta, double phi,
									   double dEta,
									   double dPhi) const
{
  
  std::vector<const xAOD::CaloCluster*> clusters;

  //Need to be able to search within a broad window, then merge all vectors
  //within that window together.
  if (dEta > 0. && dPhi > 0.) {
    
    std::pair<double,double> lower_keys = GetEtaPhiKeys(eta-(dEta/2.), phi-(dPhi/2.));
    std::pair<double,double> upper_keys = GetEtaPhiKeys(eta+(dEta/2.), phi+(dPhi/2.));

    for (int ieta = lower_keys.first; ieta <= upper_keys.first; ieta++)
      for (int iphi = lower_keys.second; iphi <= upper_keys.second; iphi++)
	clusters.insert(clusters.end(), m_map.at(ieta).at(iphi).begin(), m_map.at(ieta).at(iphi).end());


    //Re-sort vector according to Et and return it.
    sort( clusters.begin(), clusters.end(), CompareClusterET );

  } else {
    clusters = m_map.at(GetEtaPhiKeys(eta,phi).first).at(GetEtaPhiKeys(eta,phi).second);
  }

  return clusters;

}

double TopoClusterMap::GetLArThirdLayerRatio (const xAOD::CaloCluster *clus)
{

  double totalEnergy(0.), thirdLayerEnergy(0.);

  if (clus->inBarrel()) {
    totalEnergy += clus->eSample(CaloSampling::PreSamplerB);
    totalEnergy += clus->eSample(CaloSampling::EMB1);
    totalEnergy += clus->eSample(CaloSampling::EMB2);
    totalEnergy += clus->eSample(CaloSampling::EMB3);

    thirdLayerEnergy += clus->eSample(CaloSampling::EMB3);
  }

  if (clus->inEndcap()) {
    totalEnergy += clus->eSample(CaloSampling::PreSamplerE);
    totalEnergy += clus->eSample(CaloSampling::EME1);
    totalEnergy += clus->eSample(CaloSampling::EME2);
    totalEnergy += clus->eSample(CaloSampling::EME3);

    thirdLayerEnergy += clus->eSample(CaloSampling::EME3);

  }

  return (thirdLayerEnergy / totalEnergy);
  

}
