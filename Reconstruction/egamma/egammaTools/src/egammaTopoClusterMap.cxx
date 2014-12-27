/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaTopoClusterMap.h"
//#include "egammaUtils/EMType.h"
#include "egammaUtils/EMConversionUtils.h"

bool CompareClusterET (const xAOD::CaloCluster* c1, const xAOD::CaloCluster* c2) 
{

  double Et1(c1->e()/cosh(c1->eta())), Et2(c2->e()/cosh(c2->eta()));
  return Et1 > Et2;

}

//================================================
//Athena interfaces.
//================================================

//Constructor.
egammaTopoClusterMap::egammaTopoClusterMap(const std::string& type,
					   const std::string& name,
					   const IInterface* parent) :
  AthAlgTool(type, name, parent)
  
{

  // Declare interface.
  declareInterface<IegammaTopoClusterMap>(this);

  //Declare granularity of map.
  declareProperty("minEta",_minEta = -6.,
		  "Minimum eta");

  declareProperty("minPhi",_minPhi = -3.2,
		  "Minimum phi");

  declareProperty("maxEta",_maxEta = 6.,
		  "Maximum eta");

  declareProperty("maxPhi",_maxPhi = 3.2,
		  "Maximum phi");

  declareProperty("dEta",_dEta = 0.3,
		  "Delta eta");

  declareProperty("dPhi",_dPhi = 0.1,
		  "Delta phi");


}

//Destructor.
egammaTopoClusterMap::~egammaTopoClusterMap() {}

StatusCode egammaTopoClusterMap::initialize() {

  ATH_MSG_INFO("egammaTopoClusterMap: Initializing egammaTopoClusterMap");
  ClearMap();
  return StatusCode::SUCCESS;

}

StatusCode egammaTopoClusterMap::finalize() {

  ClearMap();
  return StatusCode::SUCCESS;
  
}

//================================================
//Functional routines.
//================================================

StatusCode egammaTopoClusterMap::execute(const xAOD::CaloClusterContainer *inputTopoClusterContainer) 

{

  ATH_MSG_INFO("egammaTopoClusterMap::Setting map with granularity dEta x dPhi = " << _dEta << " x " << _dPhi);

  ClearMap();

  SetTopoClusters(inputTopoClusterContainer);
  ATH_MSG_INFO("Topoclusters set ");

  //Now sort the vectors according to Et.
  SortGridVectors();
  ATH_MSG_INFO("Vectors sorted according to Et ");

  return StatusCode::SUCCESS;

}

void egammaTopoClusterMap::SetTopoClusters(const xAOD::CaloClusterContainer *inputTopoClusterContainer) 

{

  typedef xAOD::CaloClusterContainer::const_iterator clus_iterator;

  double eta, phi;

  for(clus_iterator cciter = inputTopoClusterContainer->begin();
                    cciter != inputTopoClusterContainer->end(); 
                    ++cciter) 
  {

    if (GetLArThirdLayerRatio((*cciter)) > 0.1) continue;

    //Retrieve eta, phi from ith topocluster.
    eta = (*cciter)->eta();
    phi = (*cciter)->phi();

    //Put it in appropriate vector.
    _map[GetEtaPhiKeys(eta,phi).first][GetEtaPhiKeys(eta,phi).second].push_back((*cciter));
    

  }

}

void egammaTopoClusterMap::InsertTopoCluster(xAOD::CaloCluster* topo) {

  //Retrieve eta, phi from ith topocluster.
  double eta(topo->phi()), phi(topo->phi());

  //Put it in appropriate vector.
  _map[GetEtaPhiKeys(eta,phi).first][GetEtaPhiKeys(eta,phi).second].push_back(topo);

  //Re-sort the vector according to Et.
  SortGridVector(GetEtaPhiKeys(eta,phi).first, GetEtaPhiKeys(eta,phi).second);
  
}

//Boolean comparison somewhere here that sorts vectors in grid by Et.
void egammaTopoClusterMap::SortGridVectors() {
  
  for (int i = 0; i <= (GetEtaPhiKeys(_maxEta, _maxPhi).first); i++)
    for (int j = 0; j <= (GetEtaPhiKeys(_maxEta, _maxPhi).second); j++)
      SortGridVector(i,j);

}

void egammaTopoClusterMap::SortGridVector(int eta_key, int phi_key) {

  if (_map[eta_key][phi_key].size()>0) 
    sort( _map[eta_key][phi_key].begin(), _map[eta_key][phi_key].end(), CompareClusterET );
}

void egammaTopoClusterMap::ClearMap() {

  for (int i = 0; i <= (GetEtaPhiKeys(_maxEta, _maxPhi).first); i++)
    for (int j = 0; j <= (GetEtaPhiKeys(_maxEta, _maxPhi).second); j++)
      _map[i][j].clear();
}

void egammaTopoClusterMap::DumpMapContents() {

  for (int i = 0; i <= (GetEtaPhiKeys(_maxEta, _maxPhi).first); i++)
    for (int j = 0; j <= (GetEtaPhiKeys(_maxEta, _maxPhi).second); j++) {
      ATH_MSG_INFO("Size of topocluster vector at (" << i << "," << j << "): " << _map[i][j].size());
      if (_map[i][j].size()) {
	ATH_MSG_INFO("Contents of vector:");
	for (unsigned int k = 0; k < _map[i][j].size(); k++) {
	  //ATH_MSG_INFO("E: %f, eta: %f, phi: %f, Pt: %f \n",(_map[i][j])[k]->e(), (_map[i][j])[k]->eta(), (_map[i][j])[k]->phi(), (_map[i][j])[k]->pt());
	  ATH_MSG_INFO("E: " << (_map[i][j])[k]->e()
		    << ", eta: "     << (_map[i][j])[k]->eta()
		    << ", phi: "     << (_map[i][j])[k]->phi()
		    << ", Pt:  "     << (_map[i][j])[k]->pt());
	}
      }
    }
	

}

//Routine to retrieve vector of TopoClusters for a given cluster Pt.
std::vector<const xAOD::CaloCluster*> egammaTopoClusterMap::RetrieveTopoClusters(double eta, 
									   double phi,
									   double Pt)
{

  if ((Pt / 1000.) < 15) {
    return RetrieveTopoClusters(eta, phi, 0.2, 0.2);
  } 
  else if ((Pt/1000.) < 50) {
    return RetrieveTopoClusters(eta, phi, 0.2, 0.4);
  }
  else {
    return RetrieveTopoClusters(eta, phi, 0.2, 0.6);
  }

}


//Routine to retrieve vector of TopoClusters for a given (eta, phi) region.
std::vector<const xAOD::CaloCluster*> egammaTopoClusterMap::RetrieveTopoClusters(double eta, double phi,
									   double dEta,
									   double dPhi)
{
  
  std::vector<const xAOD::CaloCluster*> clusters;

  //Need to be able to search within a broad window, then merge all vectors
  //within that window together.
  if (dEta > 0. && dPhi > 0.) {
    
    std::pair<double,double> lower_keys = GetEtaPhiKeys(eta-(dEta/2.), phi-(dPhi/2.));
    std::pair<double,double> upper_keys = GetEtaPhiKeys(eta+(dEta/2.), phi+(dPhi/2.));

    for (int ieta = lower_keys.first; ieta <= upper_keys.first; ieta++)
      for (int iphi = lower_keys.second; iphi <= upper_keys.second; iphi++)
	clusters.insert(clusters.end(), _map[ieta][iphi].begin(), _map[ieta][iphi].end());


    //Re-sort vector according to Et and return it.
    sort( clusters.begin(), clusters.end(), CompareClusterET );

  } else {
    clusters = _map[GetEtaPhiKeys(eta,phi).first][GetEtaPhiKeys(eta,phi).second];
  }

  return clusters;

}

double egammaTopoClusterMap::GetLArThirdLayerRatio (const xAOD::CaloCluster *clus)
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
