/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonClusterization/RpcHitClustering.h"



namespace Muon {

  bool RpcHitClusteringObj::cluster( const std::vector<const RpcPrepData*>& col ){

//    std::cout << " RPC collection size " << col.size() << std::endl;
    if( col.empty() ) return  false;
    std::vector<const RpcPrepData*>::const_iterator cit_begin = col.begin();
    std::vector<const RpcPrepData*>::const_iterator cit_end = col.end();
    if( cit_begin == cit_end ) return false;
    std::vector<const RpcPrepData*>::const_iterator cit = cit_begin;
    const Muon::RpcPrepData* prd_first = *cit;
    if( !prd_first ) return false;
    clustersEta.clear();
    clustersPhi.clear();

    std::set<Identifier> subModuleIds;
    for( ; cit!=cit_end;++cit ) {
      const Muon::RpcPrepData* prd = *cit;
      const Identifier& id = prd->identify();
      Identifier elId = m_rpcIdHelper->elementID(id);	
      int doubZ = m_rpcIdHelper->doubletZ(id);
      int doubPhi = m_rpcIdHelper->doubletPhi(id);
      Identifier detElId = m_rpcIdHelper->channelID(elId,doubZ,doubPhi,1,0,1);
      subModuleIds.insert(detElId);
    }
    if( debug ) {
      std::cout << " RPC performing clustering " << col.size() << " sub modules " << subModuleIds.size() << std::endl;
      std::set<Identifier>::iterator it = subModuleIds.begin();
      std::set<Identifier>::iterator it_end = subModuleIds.end();
      for( ;it!=it_end;++it ){
	std::cout << " sub module " << m_rpcIdHelper->print_to_string(*it) << std::endl;
      }
    }
    std::set<Identifier>::iterator it = subModuleIds.begin();
    std::set<Identifier>::iterator it_end = subModuleIds.end();
    for( ;it!=it_end;++it ){
      if( !cluster(col,*it) ) return false;
    }

    findBest();
    if( debug ) dump();
    return true;
  }


  bool RpcHitClusteringObj::cluster( const std::vector<const RpcPrepData*>& col, const Identifier& subid ){

    /**
       Core clustering routine that runs on a single RPC module (two gas-gaps)
       Strategy: 
         - create a hash vector with the channel number as index and store a Doublet object in it. 
           The Doublet contains two integers, one for each gasgap. The integers are used to store the cluster number the hit in that channel is 
           associated with.
         - loop over the input hits, filtering out all those in different modules
           - calculate the channel number and look-up the cluster number, if any, for the channel nummber
             - if there is already a cluster associated with the channel (meaning this is a duplicate hit), add the hit as secondary hit
           - calculate all potential neightbours of the channel number
           - loop over the neighbour channels and check if any of them is already assigned to a cluster
             - if one of the neighbours already is assigned to a cluster, add the hit to that cluster
             - if another neighbouring channel is assigned to a different cluster, the hit connects the two clusters -> merge the two clusters
         - add the clusters to the final cluster list
     */

    // check if there was an input
    if( col.empty() ) return  false;
    std::vector<const RpcPrepData*>::const_iterator cit_begin = col.begin();
    std::vector<const RpcPrepData*>::const_iterator cit_end = col.end();
    if( cit_begin == cit_end ) return false;
    if( debug ) std::cout << " RPC performing clustering: " << col.size() << "  " << m_rpcIdHelper->print_to_string(subid) << std::endl;
    
    // find the first PRD matching the subid
    int doubZ = m_rpcIdHelper->doubletZ(subid);
    int doubPhi = m_rpcIdHelper->doubletPhi(subid);
    std::vector<const RpcPrepData*>::const_iterator cit = cit_begin;
    const Muon::RpcPrepData* prd_first = 0;
    for( ; cit!=cit_end;++cit ) {
      const Muon::RpcPrepData* prd = *cit;
      const Identifier& id = prd->identify();
      if( doubZ != m_rpcIdHelper->doubletZ(id) ) continue;
      if( doubPhi != m_rpcIdHelper->doubletPhi(id) ) continue;
      prd_first = prd;
      break;
    }
      
    // exit if none found
    if( !prd_first ) return false;

    // setup arrays
    clustersEtaTmp.clear();
    clustersPhiTmp.clear();
    channelsEta.clear();
    channelsPhi.clear();
    channelsEta.resize(prd_first->detectorElement()->NetaStrips());
    channelsPhi.resize(prd_first->detectorElement()->NphiStrips());

    // loop over hits 
    std::vector<Doublet>* channelsPtr = 0; 
    cit = cit_begin;
    for( ; cit!=cit_end;++cit ) {
      
      // drop hits in other subid's
      const Muon::RpcPrepData* prd = *cit;
      const Identifier& id = prd->identify();
      if( doubZ != m_rpcIdHelper->doubletZ(id) ) continue;
      if( doubPhi != m_rpcIdHelper->doubletPhi(id) ) continue;
      if( debug ) std::cout << "  adding prd " << m_rpcIdHelper->print_to_string(id) << std::endl;
      
      // decode identifier
      bool measuresPhi = m_rpcIdHelper->measuresPhi(id); 
      int gasgap = m_rpcIdHelper->gasGap(id);
      int channel = m_rpcIdHelper->strip(id)-1;
      if(measuresPhi) channelsPtr = &channelsPhi;
      else            channelsPtr = &channelsEta;
      if( channel >= (int)channelsPtr->size() ){
	std::cout << "index channels out of range: " << channel << " max " << channelsPtr->size() << std::endl;
	continue;
      }

      // 
      std::vector<RpcClusterObj>& clusters = measuresPhi ? clustersPhiTmp : clustersEtaTmp;

      Doublet& doublet = (*channelsPtr)[channel];

      // first treat the case of a second hit in the same tube
      int channelClusterNumber = gasgap==1 ? doublet.first : doublet.second;
      if( channelClusterNumber != -1 ){
	if( debug ){
	  std::cout << " secondary hit " << channel << "  " << gasgap;
	  if( measuresPhi ) std::cout << " phi " << channelClusterNumber << std::endl;
	  else              std::cout << " eta " << channelClusterNumber << std::endl;
	}
	RpcClusterObj& cluster = clusters[channelClusterNumber];
	if( !cluster.addSecond(prd,gasgap) ){
// 	  std::cout << " secondary hit " << channel << "  " << gasgap;
// 	  if( measuresPhi ) std::cout << " phi " << channelClusterNumber << std::endl;
// 	  else              std::cout << " eta " << channelClusterNumber << std::endl;

// 	  std::cout << " new id " << m_rpcIdHelper->print_to_string(prd->identify()) << std::endl;
// 	  RpcClusterObj::HitIt it = cluster.hitList.begin(); 
// 	  RpcClusterObj::HitIt it_end = cluster.hitList.end(); 
// 	  for( ;it!=it_end;++it ){ 
// 	    std::cout << " added id " << m_rpcIdHelper->print_to_string((*it)->identify()) << std::endl;
// 	  }
// 	  dump();
//      std::cout << "could not find duplicate rpc prd " << std::endl; 
	}
      }else{
	
	std::vector<Id> neighbours;
	if( combinedGasGaps ){
	  if( gasgap == 1 )                     {
	    neighbours.push_back( Id(2,channel) );
	    if( channel != 0 )                         neighbours.push_back( Id(2,channel-1) );
	    if( channel < (int)channelsPtr->size()-1 ) neighbours.push_back( Id(2,channel+1) );
	  }else if( gasgap == 2 ){
	    neighbours.push_back( Id(1,channel) );
	    if( channel != 0 )                         neighbours.push_back( Id(1,channel-1) );
	    if( channel < (int)channelsPtr->size()-1 ) neighbours.push_back( Id(1,channel+1) );
	  }
	}
	if( channel != 0 )                         neighbours.push_back( Id(gasgap,channel-1) );
	if( channel < (int)channelsPtr->size()-1 ) neighbours.push_back( Id(gasgap,channel+1) );

	if( debug ) {
	  std::cout << "   adding new channel " << channel << "  " << gasgap;
	  if( measuresPhi ) std::cout << " phi " << " neighbours " << neighbours.size() << std::endl;
	  else              std::cout << " eta " << " neighbours " << neighbours.size() << std::endl;
	}
	std::vector<Id>::iterator nit = neighbours.begin();
	std::vector<Id>::iterator nit_end = neighbours.end();
	RpcClusterObj* currentCluster = 0;
	int currentClusterId = -1;
	for( ;nit!=nit_end;++nit ){

	  Doublet& doub = (*channelsPtr)[nit->ch];
	  
	  int clusterNumber =  nit->gp==1 ? doub.first : doub.second;
	  if( clusterNumber == -1 ) continue;
	  if( debug ) std::cout << "     new neighbour " << nit->gp << "  " << nit->ch << " clusterid " << clusterNumber;
	  
	  RpcClusterObj& cluster = clusters[clusterNumber];
	  
	  // if the hit is unassigned add it to the cluster
	  if( currentCluster == 0 ){
	    cluster.add(prd,gasgap);
	    currentCluster = &cluster;
	    if( gasgap==1 ) doublet.first  = clusterNumber;
	    else            doublet.second = clusterNumber;
	    currentClusterId = clusterNumber;
	    if( debug ) std::cout << "     adding hit to neighbour cluster with ID " << clusterNumber << std::endl;
	  }else if( clusterNumber != currentClusterId ){
	    // the hit is already assigned to a cluster, merge the two clusters
	    // first update hitPattern
	    RpcClusterObj::HitIt h = cluster.hitList.begin();
	    RpcClusterObj::HitIt h_end = cluster.hitList.end();
	    for( ;h!=h_end;++h ) {
	      const Identifier& cid = (*h)->identify();
	      int ch = m_rpcIdHelper->strip(cid)-1;
	      int gp = m_rpcIdHelper->gasGap(cid);
	      Doublet& doub = (*channelsPtr)[ch];
	      if( gp==1 )  doub.first  = currentClusterId;
	      else         doub.second = currentClusterId;
	    }
	    if( debug ) std::cout << "     found cluster overlap, merging clusters " << std::endl;
	    currentCluster->merge(cluster);
	  }else{
	    if( debug ) std::cout << "     cluster overlap, same cluster " << std::endl;
	  }
	}
	// now check whether the hit was already assigned to a cluster, if not create a new cluster
	if( currentCluster == 0 ){
	  if( debug ) std::cout << "       no neighbouring hits, creating new cluster " << clusters.size() << std::endl;
	  RpcClusterObj cl;
	  cl.add(prd,gasgap);
	  Doublet& doub = (*channelsPtr)[channel];
	  if( gasgap==1 )  doub.first  = clusters.size();
	  else             doub.second = clusters.size();
	  clusters.push_back(cl);
	}
      }
    }
    
    if( debug ) {
      std::cout << " finished clustering: eta clusters  " <<  clustersEtaTmp.size() << " phi clusters " << clustersPhiTmp.size() << std::endl;
      for( auto& cl : clustersEtaTmp ){
        std::cout << "   cluster " << cl.ngasgap1 << " " << cl.ngasgap2 << " hits " << cl.hitList.size() << std::endl;
        for( auto hit : cl.hitList ){
          std::cout << "       " << m_rpcIdHelper->print_to_string(hit->identify()) << std::endl;
        }
      }
    }
    // add all active clusters
    for( auto& cl : clustersEtaTmp ){
      if( cl.active() ) clustersEta.push_back(cl);
    }
    for( auto& cl : clustersPhiTmp ){
      if( cl.active() ) clustersPhi.push_back(cl);
    }

    return true;
  }


  void RpcHitClusteringObj::dump() const {
//     std::cout << " dumping eta chamber hit map " << std::endl<< std::endl; 
//     for( unsigned int nl=0;nl<channelsEta.size();++nl) 
//       if( channelsEta[nl].second == -1 ) std::cout << "  .";
//       else std::cout << " " << std::setw(2) << channelsEta[nl].second; 

//     std::cout << std::endl << std::endl; 
//     for( unsigned int nl=0;nl<channelsEta.size();++nl) 
//       if( channelsEta[nl].first == -1 ) std::cout << "  .";
//       else std::cout << " " << std::setw(2) << channelsEta[nl].first; 
//     std::cout << std::endl << std::endl; 

//     std::cout << " dumping phi chamber hit map " << std::endl<< std::endl; 
//     for( unsigned int nl=0;nl<channelsPhi.size();++nl) 
//       if( channelsPhi[nl].second == -1 ) std::cout << "  .";
//       else std::cout << " " << std::setw(2) << channelsPhi[nl].second; 
//     std::cout << std::endl << std::endl; 
//     for( unsigned int nl=0;nl<channelsPhi.size();++nl) 
//       if( channelsPhi[nl].first == -1 ) std::cout << "  .";
//       else std::cout << " " << std::setw(2) << channelsPhi[nl].first; 
//     std::cout << std::endl << std::endl; 
    int clid = -1;
    std::vector<RpcClusterObj>::const_iterator cit = clustersEta.begin();
    std::vector<RpcClusterObj>::const_iterator cit_end = clustersEta.end();
    for( ;cit!=cit_end;++cit ){
      ++clid;
      if( !cit->active() ) continue;
      std::cout << " new cluster " << clid << "  size " << cit->hitList.size() << std::endl;
      RpcClusterObj::HitCit hit = cit->hitList.begin();
      RpcClusterObj::HitCit hit_end = cit->hitList.end();
      for( ;hit!=hit_end;++hit ){	
	const Muon::RpcPrepData& prd = **hit;
	const Identifier& id = prd.identify();
	std::cout << "   hit " << m_rpcIdHelper->gasGap(id) << " " << m_rpcIdHelper->strip(id)-1;
	bool measuresPhi = m_rpcIdHelper->measuresPhi(id); 
	if(measuresPhi) std::cout << " phi" << std::endl;
	else            std::cout << " eta" << std::endl;
      }
    }
  }

  void RpcHitClusteringObj::findBest() {
    std::vector<RpcClusterObj>::iterator it = clustersEta.begin();
    std::vector<RpcClusterObj>::iterator it_end = clustersEta.end();
    std::stable_sort(it,it_end,SortRpcClusterObjs());
    it = clustersPhi.begin();
    it_end = clustersPhi.end();
    std::stable_sort(it,it_end,SortRpcClusterObjs());
  }
}
