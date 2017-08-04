/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCHITCLUSTERING_H
#define TGCHITCLUSTERING_H

#include <vector>
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/TgcPrepData.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include "Identifier/Identifier.h"
#include "MuonIdHelpers/TgcIdHelper.h"

namespace Muon {

  class TgcClusterObj {
  public:
    
    typedef std::vector< const TgcPrepData* > HitList;
    typedef HitList::iterator                 HitIt;
    typedef HitList::const_iterator           HitCit;
    TgcClusterObj() : ngasgap1(0),ngasgap2(0),ngasgap3(0) {}
    
    void add( const TgcPrepData* prd, int gasgap ) { 
      if( gasgap == 1 )      ++ngasgap1;
      else if( gasgap == 2 ) ++ngasgap2;
      else                   ++ngasgap3;
      hitList.push_back( prd ); 
    }

    void addSecond( const TgcPrepData* /*prd*/, int /*gasgap*/ ) { 
      return;
      /* 	HitIt it = hitList.begin(); */
      /* 	HitIt it_end = hitList.end(); */
      /* 	for( ;it!=it_end;++it ){ */
      /* 	  if( (*it)->identify() != prd->identify() ) continue; */
      /* 	} */
      /* 	std::cout << "could not find duplicate prd " << std::endl; */
      /* 	add(prd); */
    }

    void merge( TgcClusterObj& cluster ) {
      hitList.insert(hitList.end(),cluster.hitList.begin(),cluster.hitList.end());
      ngasgap1 += cluster.ngasgap1;
      ngasgap2 += cluster.ngasgap2;
      ngasgap3 += cluster.ngasgap3;
      cluster.ngasgap1 = 0;
      cluster.ngasgap2 = 0;
      cluster.ngasgap3 = 0;
      cluster.hitList.clear();
    }
      
    bool active() const { return !hitList.empty(); }
    
    int layers() const {
      if( hitList.empty() ) return 0;
      int nl = 0;
      if( ngasgap1 > 0 ) ++nl;
      if( ngasgap2 > 0 ) ++nl;
      if( ngasgap3 > 0 ) ++nl;
      return nl;
    }

    int ngasgap1;
    int ngasgap2;
    int ngasgap3;
    HitList hitList;
  };

  class TgcClusterObj3D {
  public:
    TgcClusterObj3D( const TgcClusterObj& etaC, const TgcClusterObj& phiC )  : etaCluster(etaC), phiCluster(phiC) {}

    TgcClusterObj etaCluster;
    TgcClusterObj phiCluster;

    Amg::Vector3D p11;
    Amg::Vector3D p12;
    Amg::Vector3D p21;
    Amg::Vector3D p22;
  };

  struct SortTgcClusterObjs {
    bool operator()(const TgcClusterObj& cl1,const TgcClusterObj& cl2 ) const {
      if( cl1.layers() > cl2.layers() ) return true;
      else if( cl1.layers() < cl2.layers() ) return false;
      return cl1.hitList.size() < cl2.hitList.size();
    }
  };

  struct SortTgcHitList {
    bool operator()(const TgcPrepData* h1,const TgcPrepData* h2 ) const {
      return h1->localPosition().x() < h2->localPosition().x();
    }
  };

  struct TgcHitClusteringObj {

    struct Id {
      Id( int g, int c ) : gp(g),ch(c) {}
      int gp;
      int ch;
    };

    struct Triplet {
      Triplet() : first(-1),second(-1),third(-1) {}
      int first;
      int second;
      int third;
      int sum() const { return first+second+third; }
    };
    typedef std::vector< Triplet > HitClustering;

    TgcHitClusteringObj( const TgcIdHelper& tgcIdHelper ) : 
      m_tgcIdHelper(&tgcIdHelper),bunchIdBestEta(-99),bunchIdBestPhi(-99),
      ngasgaps(-99),debug(false),combinedGasGaps(true)
    {}

    
    bool cluster( const std::vector<const TgcPrepData*>& col );

    bool buildClusters3D();

    void dump() const;

    const TgcClusterObj* bestEtaCluster() const {
      if( clustersEta.empty() ) return 0;
      return &clustersEta.front();
    }

    const TgcClusterObj* bestPhiCluster() const {
      if( clustersPhi.empty() ) return 0;
      return &clustersPhi.front();
    }
 
    void findBest();

    const TgcIdHelper*        m_tgcIdHelper;
    std::vector<Triplet>      channelsEta;
    std::vector<Triplet>      channelsPhi;
    std::vector<TgcClusterObj>   clustersEta;
    std::vector<TgcClusterObj>   clustersPhi;
    std::vector<TgcClusterObj3D> clusters3D;
    int bunchIdBestEta;
    int bunchIdBestPhi;
    int ngasgaps;
    bool debug;
    bool combinedGasGaps;
  };

}
#endif
