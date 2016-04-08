/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonClusterization/HitClustering.h"


namespace Muon {

  bool HitClusteringObj::cluster( const std::vector<const MuonCluster*>& col ){    
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
    ngasgaps = detEl->numberOfLayers(detEl->measuresPhi(id_first));

    if( debug ) std::cout << " performing clustering " << col.size() << " gasgaps " << ngasgaps << std::endl;

    clustersEta.clear();
    clustersPhi.clear();
    channelsEta.clear();
    channelsPhi.clear();
    
    //    std::cout << "channel check " << m_muonIdHelper->channelMax(id_first) << std::endl;
    channelsEta.resize(1340);//m_muonIdHelper->channelMax(id_first)+2); // Need better way to find max number of channels
    channelsPhi.resize(1340);//m_muonIdHelper->channelMax(id_first)+2);
    std::vector<Triplet>* channelsPtr = 0;
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
	if( debug ){
	  std::cout << " secondary hit " << channel << "  " << gasgap;
	  if( measuresPhi ) std::cout << " phi " << channelClusterNumber << std::endl;
	  else              std::cout << " eta " << channelClusterNumber << std::endl;
	}
	ClusterObj& cluster = clusters[channelClusterNumber];
	cluster.addSecond(prd,gasgap);
      }else{
	
	std::vector<Id> neighbours;
	if( channel != 0 )                         neighbours.push_back( Id(gasgap,channel-1) );
	if( channel < (int)channelsPtr->size()-1 ) neighbours.push_back( Id(gasgap,channel+1) );

	if( combinedGasGaps ){
	  if( gasgap != 1 ){
	    neighbours.push_back( Id(1,channel) );
	    if( channel != 0 )                         neighbours.push_back( Id(1,channel-1) );
	    if( channel < (int)channelsPtr->size()-1 ) neighbours.push_back( Id(1,channel+1) );
	  }
	  
	  if( gasgap != 2 ) {
	    neighbours.push_back( Id(2,channel) );
	    if( channel != 0 )                         neighbours.push_back( Id(2,channel-1) );
	    if( channel < (int)channelsPtr->size()-1 ) neighbours.push_back( Id(2,channel+1) );
	  }
	  
	  if( ngasgaps == 3 && gasgap != 3 ){
	    neighbours.push_back( Id(3,channel) );
	    if( channel != 0 )                         neighbours.push_back( Id(3,channel-1) );
	    if( channel < (int)channelsPtr->size()-1 ) neighbours.push_back( Id(3,channel+1) );
	  }
	}

	if( debug ) {
	  std::cout << " new hit " << channel << "  " << gasgap;
	  if( measuresPhi ) std::cout << " phi " << " neighbours " << neighbours.size() << std::endl;
	  else              std::cout << " eta " << " neighbours " << neighbours.size() << std::endl;
	}
	std::vector<Id>::iterator nit = neighbours.begin();
	std::vector<Id>::iterator nit_end = neighbours.end();
	ClusterObj* currentCluster = 0;
	int currentClusterId = -1;
	for( ;nit!=nit_end;++nit ){

	  Triplet& trip = (*channelsPtr)[nit->ch];
	  
	  int clusterNumber = nit->gp==1 ? trip.first : nit->gp==2 ? trip.second : trip.third;
	  if( clusterNumber == -1 ) continue;
	  if( debug ) std::cout << "    new neighbour " << nit->gp << "  " << nit->ch << " clusterid " << clusterNumber;
	  
	  ClusterObj& cluster = clusters[clusterNumber];
	  
	  // if the hit is unassigned add it to the cluster
	  if( currentCluster == 0 ){
	    cluster.add(prd,gasgap);
	    currentCluster = &cluster;
	    if( gasgap==1 )      triplet.first  = clusterNumber;
	    else if( gasgap==2 ) triplet.second = clusterNumber;
	    else                 triplet.third  = clusterNumber;
	    currentClusterId = clusterNumber;
	    if( debug ) std::cout << " adding hit " << std::endl;
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
	    if( debug ) std::cout << " cluster overlap, merging clusters " << std::endl;
	    currentCluster->merge(cluster);
	  }else{
	    if( debug ) std::cout << " cluster overlap, same cluster " << std::endl;
	  }
	}
	// now check whether the hit was already assigned to a cluster, if not create a new cluster
	if( currentCluster == 0 ){
	  if( debug ) std::cout << " no neighbouring hits, creating new cluster " << clusters.size() << std::endl;
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
    findBest();
    if( debug ) dump();
    return true;
  }


  void HitClusteringObj::dump() const {

 //    std::cout << " dumping eta chamber hit map: number of gasgaps " << ngasgaps << std::endl << std::endl; 
//     if( ngasgaps == 3 )
//       for( unsigned int nl=0;nl<channelsEta.size();++nl) 
// 	if( channelsEta[nl].third == -1 ) std::cout << " .";
// 	else std::cout << " " << std::setw(2) << channelsEta[nl].third; 

//     for( unsigned int nl=0;nl<channelsEta.size();++nl) 
//       if( channelsEta[nl].second == -1 ) std::cout << " .";
//       else std::cout << " " << std::setw(2) << channelsEta[nl].second; 

//     std::cout << std::endl << std::endl; 
//     for( unsigned int nl=0;nl<channelsEta.size();++nl) 
//       if( channelsEta[nl].first == -1 ) std::cout << " .";
//       else std::cout << " " << std::setw(2) << channelsEta[nl].first; 
//     std::cout << std::endl << std::endl; 
 
//     std::cout << " dumping phi chamber hit map " << std::endl << std::endl; 
//     for( unsigned int nl=0;nl<channelsPhi.size();++nl) 
//       if( channelsPhi[nl].second == -1 ) std::cout << " .";
//       else std::cout << " " << std::setw(2) << channelsPhi[nl].second; 
//     std::cout << std::endl << std::endl; 
//     for( unsigned int nl=0;nl<channelsPhi.size();++nl) 
//       if( channelsPhi[nl].first == -1 ) std::cout << " .";
//       else std::cout << " " << std::setw(2) << channelsPhi[nl].first; 
//     std::cout << std::endl << std::endl; 
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

  void HitClusteringObj::findBest() {
    std::vector<ClusterObj>::iterator it = clustersEta.begin();
    std::vector<ClusterObj>::iterator it_end = clustersEta.end();
    std::stable_sort(it,it_end,SortClusterObjs());
    it = clustersPhi.begin();
    it_end = clustersPhi.end();
    std::stable_sort(it,it_end,SortClusterObjs());
  }

  bool HitClusteringObj::buildClusters3D() {
    
    clusters3D.clear();

    if( clustersPhi.empty() || clustersEta.empty() ) return false;
    if( !bestEtaCluster() || !bestEtaCluster()->active() ) return false;
    
    const MuonCluster* etaHit = bestEtaCluster()->hitList.front(); 
    if( !etaHit ) return false;

    const MuonGM::TgcReadoutElement* detEl = dynamic_cast<const MuonGM::TgcReadoutElement*>(etaHit->detectorElement());
    if( !detEl ) return false;

    // now loop over eta and phi clusters and form space points
    if( debug ) std::cout << " eta clusters " << clustersEta.size() << " phi clusters " << clustersPhi.size() << std::endl;
    std::vector< ClusterObj >::iterator eit = clustersEta.begin();
    std::vector< ClusterObj >::iterator eit_end = clustersEta.end();
    for( ;eit!=eit_end;++eit ){
      
      if( !eit->active() ) continue;

      const MuonCluster* firstEta = eit->hitList.front();
      const MuonCluster* lastEta = eit->hitList.back();
      
      std::vector< ClusterObj >::iterator pit = clustersPhi.begin();
      std::vector< ClusterObj >::iterator pit_end = clustersPhi.end();
      for( ;pit!=pit_end;++pit ){

	if( !pit->active() ) continue;

	const MuonCluster* firstPhi = pit->hitList.front();
	const MuonCluster* lastPhi = pit->hitList.back();

	ClusterObj3D cl3D(*eit,*pit);
// 	cl3D.etaCluster = &*eit;
// 	cl3D.phiCluster = &*pit;
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
    
    if( debug && !clusters3D.empty() ){
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

    return true;
  }

 

}
