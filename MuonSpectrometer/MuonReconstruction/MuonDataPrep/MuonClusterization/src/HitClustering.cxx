/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonClusterization/HitClustering.h"


namespace Muon {

  bool HitClusteringObj::cluster( const std::vector<const MuonCluster*>& col,
                                  std::vector<ClusterObj>& clustersEta,
                                  std::vector<ClusterObj>& clustersPhi) const {
    if( col.empty() ) return false;

    std::vector<const MuonCluster*>::const_iterator cit_begin = col.begin();
    std::vector<const MuonCluster*>::const_iterator cit_end = col.end();
    if( cit_begin == cit_end ) return false;
    std::vector<const MuonCluster*>::const_iterator cit = cit_begin;
    const Muon::MuonCluster* prd_first = *cit;
    if( !prd_first ) return false;

    const MuonGM::MuonClusterReadoutElement*  detEl = prd_first->detectorElement();
    const Identifier& id_first = prd_first->identify();
    if( !detEl ) return false;
    int ngasgaps = detEl->numberOfLayers(detEl->measuresPhi(id_first));

    if( m_debug ) std::cout << " performing clustering " << col.size() << " gasgaps " << ngasgaps << std::endl;

    std::vector<Triplet>      channelsEta;
    std::vector<Triplet>      channelsPhi;

    clustersEta.clear();
    clustersPhi.clear();

    //    std::cout << "channel check " << m_muonIdHelper->channelMax(id_first) << std::endl;
    channelsEta.resize(1340);//m_muonIdHelper->channelMax(id_first)+2); // Need better way to find max number of channels
    channelsPhi.resize(1340);//m_muonIdHelper->channelMax(id_first)+2);
    std::vector<Triplet>* channelsPtr = nullptr;
    for( ; cit!=cit_end;++cit ) {
      const Muon::MuonCluster* prd = *cit;
      if( !prd ) continue;

      const Identifier& id = prd->identify();
      bool measuresPhi = m_muonIdHelper->measuresPhi(id); // like measuresPhi()
      std::vector<ClusterObj>& clusters = measuresPhi ? clustersPhi : clustersEta;
      int channel = m_muonIdHelper->channel(id); // between 1 and 135!
      int gasgap = m_muonIdHelper->gasGap(id);
	
      if(measuresPhi) channelsPtr = &channelsPhi;
      else            channelsPtr = &channelsEta;
      if( channel >= (int)channelsPtr->size() ){
	//ATH_MSG_WARNING("index channels out of range: " << channel << " max " << channelsPtr->size());
	continue;
      }
      Triplet& triplet = (*channelsPtr)[channel];

      // first treat the case of a second hit in the same tube
      int channelClusterNumber = gasgap==1 ? triplet.first : (gasgap==2 ? triplet.second : triplet.third );
      if( channelClusterNumber != -1 ){
	if( m_debug ){
	  std::cout << " secondary hit " << channel << "  " << gasgap;
	  if( measuresPhi ) std::cout << " phi " << channelClusterNumber << std::endl;
	  else              std::cout << " eta " << channelClusterNumber << std::endl;
	}
	ClusterObj& cluster = clusters[channelClusterNumber];
	cluster.addSecond(prd,gasgap);
      }else{
	
	std::vector<Id> neighbours;
	if( channel != 0 )                         neighbours.emplace_back(gasgap,channel-1 );
	if( channel < (int)channelsPtr->size()-1 ) neighbours.emplace_back(gasgap,channel+1 );

	if( m_combinedGasGaps ){
	  if( gasgap != 1 ){
	    neighbours.emplace_back(1,channel );
	    if( channel != 0 )                         neighbours.emplace_back(1,channel-1 );
	    if( channel < (int)channelsPtr->size()-1 ) neighbours.emplace_back(1,channel+1 );
	  }
	  
	  if( gasgap != 2 ) {
	    neighbours.emplace_back(2,channel );
	    if( channel != 0 )                         neighbours.emplace_back(2,channel-1 );
	    if( channel < (int)channelsPtr->size()-1 ) neighbours.emplace_back(2,channel+1 );
	  }
	  
	  if( ngasgaps == 3 && gasgap != 3 ){
	    neighbours.emplace_back(3,channel );
	    if( channel != 0 )                         neighbours.emplace_back(3,channel-1 );
	    if( channel < (int)channelsPtr->size()-1 ) neighbours.emplace_back(3,channel+1 );
	  }
	}

	if( m_debug ) {
	  std::cout << " new hit " << channel << "  " << gasgap;
	  if( measuresPhi ) std::cout << " phi " << " neighbours " << neighbours.size() << std::endl;
	  else              std::cout << " eta " << " neighbours " << neighbours.size() << std::endl;
	}
	std::vector<Id>::iterator nit = neighbours.begin();
	std::vector<Id>::iterator nit_end = neighbours.end();
	ClusterObj* currentCluster = nullptr;
	int currentClusterId = -1;
	for( ;nit!=nit_end;++nit ){

	  Triplet& trip = (*channelsPtr)[nit->ch];
	  
	  int clusterNumber = nit->gp==1 ? trip.first : nit->gp==2 ? trip.second : trip.third;
	  if( clusterNumber == -1 ) continue;
	  if( m_debug ) std::cout << "    new neighbour " << nit->gp << "  " << nit->ch << " clusterid " << clusterNumber;
	  
	  ClusterObj& cluster = clusters[clusterNumber];
	  
	  // if the hit is unassigned add it to the cluster
	  if( currentCluster == nullptr ){
	    cluster.add(prd,gasgap);
	    currentCluster = &cluster;
	    if( gasgap==1 )      triplet.first  = clusterNumber;
	    else if( gasgap==2 ) triplet.second = clusterNumber;
	    else                 triplet.third  = clusterNumber;
	    currentClusterId = clusterNumber;
	    if( m_debug ) std::cout << " adding hit " << std::endl;
	  }else if( clusterNumber != currentClusterId ){
	    // the hit is already assigned to a cluster, merge the two clusters
	    // first update hitPattern
	    std::vector<const MuonCluster*>::const_iterator h= cluster.hitList.begin();
	    std::vector<const MuonCluster*>::const_iterator  h_end = cluster.hitList.end();
	    for( ;h!=h_end;++h ) {
	      const Identifier& cid = (*h)->identify();
	      int ch = m_muonIdHelper->channel(cid);
	      int gp = m_muonIdHelper->gasGap(cid);
	      Triplet& trip = (*channelsPtr)[ch];
	      if( gp==1 )      trip.first  = currentClusterId;
	      else if( gp==2 ) trip.second = currentClusterId;
	      else             trip.third  = currentClusterId;
	    }
	    if( m_debug ) std::cout << " cluster overlap, merging clusters " << std::endl;
	    currentCluster->merge(cluster);
	  }else{
	    if( m_debug ) std::cout << " cluster overlap, same cluster " << std::endl;
	  }
	}
	// now check whether the hit was already assigned to a cluster, if not create a new cluster
	if( currentCluster == nullptr ){
	  if( m_debug ) std::cout << " no neighbouring hits, creating new cluster " << clusters.size() << std::endl;
	  ClusterObj cl;
	  cl.add(prd,gasgap);
	  Triplet& trip = (*channelsPtr)[channel];
	  if( gasgap==1 )      trip.first  = clusters.size();
	  else if( gasgap==2 ) trip.second = clusters.size();
	  else                 trip.third  = clusters.size();
	  clusters.push_back(cl);
	}
      }
    }
    findBest(clustersEta, clustersPhi);
    if( m_debug ) dump(clustersEta, clustersPhi);
    return true;
  }


  void HitClusteringObj::dump(const std::vector<ClusterObj>& clustersEta,
                              const std::vector<ClusterObj>& clustersPhi) const {

    int clid = -1;
    std::vector<ClusterObj>::const_iterator cit = clustersEta.begin();
    std::vector<ClusterObj>::const_iterator cit_end = clustersEta.end();
    for( ;cit!=cit_end;++cit ){
      ++clid;
      if( !cit->active() ) continue;
      std::cout << " new cluster " << clid << "  size " << cit->hitList.size() << std::endl;
      std::vector< const MuonCluster* >::const_iterator hit = cit->hitList.begin();
      std::vector< const MuonCluster* >::const_iterator hit_end = cit->hitList.end();
      for( ;hit!=hit_end;++hit ){	
	const Muon::MuonCluster& prd = **hit;
	const Identifier& id = prd.identify();
	std::cout << "   hit " << m_muonIdHelper->gasGap(id) << " " << m_muonIdHelper->channel(id)-1;
	bool measuresPhi = m_muonIdHelper->measuresPhi(id); 
	if(measuresPhi) std::cout << " phi" << std::endl;
	else            std::cout << " eta" << std::endl;
      }
    }
    cit = clustersPhi.begin();
    cit_end = clustersPhi.end();
    for( ;cit!=cit_end;++cit ){
      ++clid;
      if( !cit->active() ) continue;
      std::cout << " new cluster " << clid << "  size " << cit->hitList.size() << std::endl;
      std::vector< const MuonCluster* >::const_iterator hit = cit->hitList.begin();
      std::vector< const MuonCluster* >::const_iterator hit_end = cit->hitList.end();
      for( ;hit!=hit_end;++hit ){	
	const Muon::MuonCluster& prd = **hit;
	const Identifier& id = prd.identify();
	std::cout << "   hit " << m_muonIdHelper->gasGap(id) << " " << m_muonIdHelper->channel(id)-1;
	bool measuresPhi = m_muonIdHelper->measuresPhi(id); 
	if(measuresPhi) std::cout << " phi" << std::endl;
	else            std::cout << " eta" << std::endl;
      }
    }
  }

  void HitClusteringObj::findBest(std::vector<ClusterObj>& clustersEta,
                                  std::vector<ClusterObj>& clustersPhi) const {
    std::vector<ClusterObj>::iterator it = clustersEta.begin();
    std::vector<ClusterObj>::iterator it_end = clustersEta.end();
    std::stable_sort(it,it_end,SortClusterObjs());
    it = clustersPhi.begin();
    it_end = clustersPhi.end();
    std::stable_sort(it,it_end,SortClusterObjs());
  }

  std::vector<ClusterObj3D>
  HitClusteringObj::buildClusters3D(const std::vector<ClusterObj>& clustersEta,
                                    const std::vector<ClusterObj>& clustersPhi) const {
    
    std::vector<ClusterObj3D> clusters3D;

    if( clustersPhi.empty() || clustersEta.empty() ) return clusters3D;
    if( !bestCluster(clustersEta) || !bestCluster(clustersEta)->active() ) return clusters3D;
    
    const MuonCluster* etaHit = bestCluster(clustersEta)->hitList.front(); 
    if( !etaHit ) return clusters3D;

    const MuonGM::TgcReadoutElement* detEl = dynamic_cast<const MuonGM::TgcReadoutElement*>(etaHit->detectorElement());
    if( !detEl ) return clusters3D;

    // now loop over eta and phi clusters and form space points
    if( m_debug ) std::cout << " eta clusters " << clustersEta.size() << " phi clusters " << clustersPhi.size() << std::endl;
    for (const ClusterObj& cl_eta : clustersEta) {
      if( !cl_eta.active() ) continue;

      const MuonCluster* firstEta = cl_eta.hitList.front();
      const MuonCluster* lastEta = cl_eta.hitList.back();

      for (const ClusterObj& cl_phi : clustersPhi) {
	if( !cl_phi.active() ) continue;

	const MuonCluster* firstPhi = cl_phi.hitList.front();
	const MuonCluster* lastPhi = cl_phi.hitList.back();

	ClusterObj3D cl3D(cl_eta,cl_phi);
	detEl->spacePointPosition( firstPhi->identify(),firstEta->identify(),cl3D.p11 );
	if( lastPhi != firstPhi ) detEl->spacePointPosition( lastPhi->identify(),firstEta->identify(),cl3D.p12 );
	else                      cl3D.p12 = cl3D.p11;
	if( lastEta != firstEta ) {
	  detEl->spacePointPosition( firstPhi->identify(),lastEta->identify(),cl3D.p21 );
	  if( lastPhi != firstPhi ) detEl->spacePointPosition( lastPhi->identify(),lastEta->identify(),cl3D.p22 );
	  else                      cl3D.p22 = cl3D.p21;
	}else{
	  cl3D.p22 = cl3D.p12;
	  cl3D.p21 = cl3D.p11;
	}
	clusters3D.push_back(cl3D);
      }
    }
    
    if( m_debug && !clusters3D.empty() ){
      std::cout << " 3D clusters " << clusters3D.size() << std::endl;
      std::vector<ClusterObj3D>::iterator it = clusters3D.begin();
      std::vector<ClusterObj3D>::iterator it_end = clusters3D.end();
      for( ;it!=it_end;++it ){
// 	std::cout << " eta cluster " << it->etaCluster->hitList.size() << " phi cluster " << it->phiCluster->hitList.size()
// 		  << " position " << it->p11 << std::endl;
	std::cout << " eta cluster " << it->etaCluster.hitList.size() << " phi cluster " << it->phiCluster.hitList.size()
		  << " position " << it->p11 << std::endl;
      }
    }

    return clusters3D;
  }

 

}
