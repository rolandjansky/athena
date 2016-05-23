/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/StoreGateSvc.h"

#include "egammaTopoClusterMap.h"
#include "egammaRecEvent/egammaRecContainer.h"
#include "CaloUtils/CaloClusterStoreHelper.h"

bool CompareClusterET (const xAOD::CaloCluster* c1, const xAOD::CaloCluster* c2) 
{
  double Et1(c1->rawE()/cosh(c1->rawEta())), Et2(c2->rawE()/cosh(c2->rawEta()));
  return Et1 > Et2;
}

bool CompareClusterLinkET (ElementLink< xAOD::CaloClusterContainer > c1, ElementLink< xAOD::CaloClusterContainer > c2)
{
  double Et1((*c1)->rawE()/cosh((*c1)->rawEta())), Et2((*c2)->rawE()/cosh((*c2)->rawEta()));
  return Et1 > Et2;
}

//////////////////////////////////////////////////
// Athena interfaces.
//////////////////////////////////////////////////

//Constructor.
egammaTopoClusterMap::egammaTopoClusterMap(const std::string& type,
					   const std::string& name,
					   const IInterface* parent) :
  AthAlgTool(type, name, parent)
  
{

  // Declare interface.
  declareInterface<IegammaTopoClusterMap>(this);

  //Declare granularity of map.
  declareProperty("minEta",m_minEta = -6.,
		  "Minimum eta");

  declareProperty("minPhi",m_minPhi = -3.2,
		  "Minimum phi");

  declareProperty("maxEta",m_maxEta = 6.,
		  "Maximum eta");

  declareProperty("maxPhi",m_maxPhi = 3.2,
		  "Maximum phi");

  declareProperty("dEta",m_dEta = 0.3,
		  "Delta eta");

  declareProperty("dPhi",m_dPhi = 0.1,
		  "Delta phi");

  declareProperty("PUThresholdCut", m_puThresholdCut = 400.);
  declareProperty("TopoClusterMapName",          m_topoClusterMapName          = "EgammaTopoCluster2DMap");

}

//Destructor.
egammaTopoClusterMap::~egammaTopoClusterMap() {}

StatusCode egammaTopoClusterMap::initialize() {

  ATH_MSG_INFO("egammaTopoClusterMap: Initializing egammaTopoClusterMap");
  CHECK(ClearMap());
  return StatusCode::SUCCESS;
}

StatusCode egammaTopoClusterMap::finalize() {
  CHECK(ClearMap());  
  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////
//Functional routines.
//////////////////////////////////////////////////

StatusCode egammaTopoClusterMap::execute(const xAOD::CaloClusterContainer *inputTopoClusterContainer) {

  ATH_MSG_DEBUG("egammaTopoClusterMap::Setting map with granularity dEta x dPhi = " << m_dEta << " x " << m_dPhi);
  CHECK (ClearMap());

  CHECK(SetTopoClusters(inputTopoClusterContainer));
  ATH_MSG_DEBUG("Topoclusters set ");

  //Now sort the vectors according to Et.
  SortGridVectors();
  ATH_MSG_DEBUG("Vectors sorted according to Et ");

  //Record the map to storegate for later retrieval.
  TopoCluster2DMap *new_map = new TopoCluster2DMap(m_map); 
  if (evtStore()->record(new_map, m_topoClusterMapName).isFailure()) {
    ATH_MSG_ERROR("Could not record topocluster map!");
    return StatusCode::FAILURE;
  }  

  return StatusCode::SUCCESS;

}

StatusCode egammaTopoClusterMap::SetTopoClusters(const xAOD::CaloClusterContainer *inputTopoClusterContainer) {

  typedef xAOD::CaloClusterContainer::const_iterator clus_iterator;

  double eta, phi;
  int i(0);
 
  for(clus_iterator cciter = inputTopoClusterContainer->begin();
      cciter != inputTopoClusterContainer->end(); 
      ++cciter,++i) {
    
    //Some basic cuts on energy.
    if ((*cciter)->rawE() < m_puThresholdCut) 
      continue;
    const ElementLink< xAOD::CaloClusterContainer > clusterLink( *inputTopoClusterContainer, i );
    const std::vector< ElementLink<xAOD::CaloClusterContainer> > elClusters {clusterLink};    
    //Retrieve eta, phi from ith topocluster.
    eta = (*cciter)->rawEta();
    phi = (*cciter)->rawPhi();
    //Put it in appropriate vector.
    m_map[GetEtaPhiKeys(eta,phi).first][GetEtaPhiKeys(eta,phi).second].push_back(clusterLink);
  }
  return StatusCode::SUCCESS;
}

//Boolean comparison somewhere here that sorts vectors in grid by Et.
void egammaTopoClusterMap::SortGridVectors() {
  for (int i = 0; i <= (GetEtaPhiKeys(m_maxEta, m_maxPhi).first); i++){
    for (int j = 0; j <= (GetEtaPhiKeys(m_maxEta, m_maxPhi).second); j++){
      SortGridVector(i,j);
    }
  }
}

void egammaTopoClusterMap::SortGridVector(int eta_key, int phi_key) {
  if (m_map[eta_key][phi_key].size()>0){ 
    sort( m_map[eta_key][phi_key].begin(), m_map[eta_key][phi_key].end(), CompareClusterLinkET );
  }
}

StatusCode egammaTopoClusterMap::ClearMap() {

  TopoCluster2DMap::iterator mapIter = m_map.begin();
  TopoCluster2DMap::iterator mapEnd  = m_map.end();
  
  for (; mapIter != mapEnd; mapIter++) {

    TopoClusterMap::iterator subMapIter = (*mapIter).second.begin();
    TopoClusterMap::iterator subMapEnd  = (*mapIter).second.end();

    for (; subMapIter != subMapEnd; subMapIter++){
      (*subMapIter).second.clear();
    }
  }
  return StatusCode::SUCCESS;
}

//Routine to retrieve vector of TopoClusters for a given cluster Pt.
std::vector<const xAOD::CaloCluster*> egammaTopoClusterMap::RetrieveTopoClusters(const double eta, 
										 const double phi,
										 const double Pt) const{
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
std::vector<const xAOD::CaloCluster*> egammaTopoClusterMap::RetrieveTopoClusters(const double eta, const double phi,
										 const double dEta,
										 const double dPhi) const {  
  std::vector<const xAOD::CaloCluster*> clusters;
  //Need to be able to search within a broad window, then merge all vectors
  //within that window together.
  if (dEta > 0. && dPhi > 0.) {    
    std::pair<int,int> lower_keys = GetEtaPhiKeys(eta-(dEta/2.), phi-(dPhi/2.));
    std::pair<int,int> upper_keys = GetEtaPhiKeys(eta+(dEta/2.), phi+(dPhi/2.));
    for (int ieta = lower_keys.first; ieta <= upper_keys.first; ieta++){
      for (int iphi = lower_keys.second; iphi <= upper_keys.second; iphi++){
	//clusters.insert(clusters.end(), m_map.at(ieta).at(iphi).begin(), m_map.at(ieta).at(iphi).end());

	std::vector< ElementLink< xAOD::CaloClusterContainer > >                 mapVec   = m_map.at(ieta).at(iphi);
	std::vector< ElementLink< xAOD::CaloClusterContainer > >::const_iterator linkIter = mapVec.begin();
	std::vector< ElementLink< xAOD::CaloClusterContainer > >::const_iterator linkEnd  = mapVec.end();
	
	for (; linkIter != linkEnd; linkIter++){
	  clusters.insert(clusters.end(), *(*linkIter));
	}	
      }
    }
    //Re-sort vector according to Et and return it.
    sort( clusters.begin(), clusters.end(), CompareClusterET );
  } else {
    // FIX ME: Pick out the specific vector at (eta, phi) and iterate over that instead. Functionally the same 
    // as below, but more obvious to reader.
    int eta_key(GetEtaPhiKeys(eta,phi).first), phi_key(GetEtaPhiKeys(eta,phi).second);
    std::vector< ElementLink< xAOD::CaloClusterContainer > >                 mapVec   = m_map.at(eta_key).at(phi_key);
    std::vector< ElementLink< xAOD::CaloClusterContainer > >::const_iterator linkIter = mapVec.begin();
    std::vector< ElementLink< xAOD::CaloClusterContainer > >::const_iterator linkEnd  = mapVec.end();

    for (; linkIter != linkEnd; linkIter++){
      clusters.insert(clusters.end(), *(*linkIter));
    }
    
  }
  return clusters;
}

//Routine to retrieve vector of TopoClusters for a given (eta, phi) region.
std::vector<const xAOD::CaloCluster*> egammaTopoClusterMap::RetrieveTopoClusters(const int eta_key, const int phi_key) const {
  std::vector<const xAOD::CaloCluster*> clusters;
  std::vector< ElementLink< xAOD::CaloClusterContainer > >::const_iterator linkIter = m_map.at(eta_key).at(phi_key).begin();
  std::vector< ElementLink< xAOD::CaloClusterContainer > >::const_iterator linkEnd  = m_map.at(eta_key).at(phi_key).end();
  for (; linkIter != linkEnd; linkIter++){
    clusters.insert(clusters.end(), *(*linkIter));
  }
  return clusters;
}
