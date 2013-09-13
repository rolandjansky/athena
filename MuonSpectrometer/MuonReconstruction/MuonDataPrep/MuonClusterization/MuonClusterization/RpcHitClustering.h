/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCHITCLUSTERING_H
#define RPCHITCLUSTERING_H

#include <vector>
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"

#include "Identifier/Identifier.h"
#include "MuonIdHelpers/RpcIdHelper.h"



namespace Muon {

  struct RpcClusterObj {
    
    typedef std::vector< const RpcPrepData* > HitList;
    typedef HitList::iterator                 HitIt;
    typedef HitList::const_iterator           HitCit;
    RpcClusterObj() : ngasgap1(0),ngasgap2(0) {}
    
    void add( const RpcPrepData* prd, int gasgap ) { 
      if( gasgap == 1 ) ++ngasgap1;
      else              ++ngasgap2;
      hitList.push_back(prd); 
    }

    bool addSecond( const RpcPrepData* prd, int gasgap ) { 

      HitIt it = hitList.begin(); 
      HitIt it_end = hitList.end(); 
      for( ;it!=it_end;++it ){ 
	if( (*it)->identify() != prd->identify() ) continue; 
	if( fabs( (*it)->time() - 12.5) > fabs( prd->time() - 12.5 ) ){
	  *it = prd;
	}
	return true;
      } 
      add(prd,gasgap); 
      return false;
    }

    void merge( RpcClusterObj& cluster ) {
      hitList.insert(hitList.end(),cluster.hitList.begin(),cluster.hitList.end());
      ngasgap1 += cluster.ngasgap1;
      ngasgap2 += cluster.ngasgap2;
      cluster.ngasgap1 = 0;
      cluster.ngasgap2 = 0;
      cluster.hitList.clear();
    }
      
    bool active() const { return !hitList.empty(); }
    
    int layers() const {
      if( hitList.empty() ) return 0;
      else if( ngasgap1 > 0 && ngasgap2 > 0 ) return 2;
      return 1;
    }

    int ngasgap1;
    int ngasgap2;
    HitList hitList;
  };

  struct SortRpcClusterObjs {
    bool operator()(const RpcClusterObj& cl1,const RpcClusterObj& cl2 ) const {
      if( cl1.layers() > cl2.layers() ) return true;
      else if( cl1.layers() < cl2.layers() ) return false;
      return cl1.hitList.size() < cl2.hitList.size();
    }
  };

  struct RpcHitClusteringObj {

    struct Id {
      Id( int g, int c ) : gp(g),ch(c) {}
      int gp;
      int ch;
    };

    struct Doublet {
      Doublet() : first(-1),second(-1) {}
      int first;
      int second;
      int sum() const { return first+second; }
    };
    typedef std::vector< Doublet > HitClustering;

    RpcHitClusteringObj( const RpcIdHelper& rpcIdHelper ) : 
      m_rpcIdHelper(&rpcIdHelper),debug(false),combinedGasGaps(true)
    {}
    
    bool cluster( const std::vector<const RpcPrepData*>& col );
    bool cluster( const std::vector<const RpcPrepData*>& col, const Identifier& subid );

    void dump() const;

    const RpcClusterObj* bestEtaCluster() const {
      if( clustersEta.empty() ) return 0;
      return &clustersEta.front();
    }

    const RpcClusterObj* bestPhiCluster() const {
      if( clustersPhi.empty() ) return 0;
      return &clustersPhi.front();
    }
 
    void findBest();

    const RpcIdHelper*      m_rpcIdHelper;
    std::vector<Doublet>    channelsEta;
    std::vector<Doublet>    channelsPhi;
    std::vector<RpcClusterObj> clustersEta;
    std::vector<RpcClusterObj> clustersPhi;
    std::vector<RpcClusterObj> clustersEtaTmp;
    std::vector<RpcClusterObj> clustersPhiTmp;
    bool debug;
    bool combinedGasGaps;
  };

}
#endif
