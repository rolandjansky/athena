/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCHITCLUSTERING_H
#define TGCHITCLUSTERING_H

#include <vector>
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/MuonCluster.h"
#include "MuonReadoutGeometry/MuonClusterReadoutElement.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include "Identifier/Identifier.h"
#include "MuonIdHelpers/MuonIdHelper.h"

namespace Muon {

  struct ClusterObj {    
    ClusterObj() : ngasgap1(0),ngasgap2(0),ngasgap3(0) {}    

    void add( const MuonCluster* prd, int gasgap ) { 
      if( gasgap == 1 )      ++ngasgap1;
      else if( gasgap == 2 ) ++ngasgap2;
      else                   ++ngasgap3;
      hitList.push_back( prd ); 
    }


    bool addSecond( const MuonCluster* prd, int gasgap ) { 
      std::vector< const MuonCluster* >::iterator it = hitList.begin(); 
      std::vector< const MuonCluster* >::iterator it_end = hitList.end(); 
    
      const RpcPrepData* rp_prd =dynamic_cast<const RpcPrepData*>(prd);
      if(!rp_prd) return true;//If cast fails then the iterator is not an RpcPrepDataObject return as if it was a tgcPerpDataObject

      for( ;it!=it_end;++it ){ 
	const RpcPrepData* rp_it =dynamic_cast<const RpcPrepData*>(*it); 
	if(!rp_it) return true;//If the iterator is an RpcPrepDataObject continue
	if( rp_it->identify() != prd->identify() ) continue;
	if( fabs( rp_it->time() - 12.5) > fabs( rp_prd->time() - 12.5 ) ){
	  return true;
	}     
      }
      add(prd,gasgap); 
      return false;
    }


    void merge( ClusterObj& cluster ) {
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
    std::vector<const MuonCluster*> hitList;
  };

  struct ClusterObj3D {
    ClusterObj3D( const ClusterObj& etaC, const ClusterObj& phiC )  : etaCluster(etaC), phiCluster(phiC) {}

    ClusterObj etaCluster;
    ClusterObj phiCluster;

    Amg::Vector3D p11;
    Amg::Vector3D p12;
    Amg::Vector3D p21;
    Amg::Vector3D p22;
  };

  struct SortClusterObjs {
    bool operator()(const ClusterObj& cl1,const ClusterObj& cl2 ) const {
      if( cl1.layers() > cl2.layers() ) return true;
      else if( cl1.layers() < cl2.layers() ) return false;
      return cl1.hitList.size() < cl2.hitList.size();
    }
  };

  struct SortHitList {
    bool operator()(const MuonCluster* h1,const MuonCluster* h2 ) const {
      return h1->localPosition().x() < h2->localPosition().x();
    }
  };

  struct HitClusteringObj {

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

    struct Data
    {
      std::vector<ClusterObj>   clustersEta;
      std::vector<ClusterObj>   clustersPhi;
    };



    HitClusteringObj( const MuonIdHelper& muonIdHelper,
                      bool combinedGasGaps,
                      bool debug = false) : 
      m_muonIdHelper(&muonIdHelper),
      m_debug(debug),m_combinedGasGaps(combinedGasGaps)
    {}

    
    bool cluster( const std::vector<const MuonCluster*>& col,
                  std::vector<ClusterObj>& clustersEta,
                  std::vector<ClusterObj>& clustersPhi) const;

    bool cluster( const std::vector<const TgcPrepData*>& col,
                  std::vector<ClusterObj>& clustersEta,
                  std::vector<ClusterObj>& clustersPhi) const
    {
      std::vector<const MuonCluster*> prds;      
      for(unsigned int i=0; i < col.size(); i++){
	prds.push_back(col.at(i));
      }
      return cluster( prds, clustersEta, clustersPhi );
    };


    std::vector<ClusterObj3D>
    buildClusters3D(const std::vector<ClusterObj>& clustersEta,
                    const std::vector<ClusterObj>& clustersPhi) const;

    void dump(const std::vector<ClusterObj>& clustersEta,
              const std::vector<ClusterObj>& clustersPhi) const;

    const ClusterObj* bestCluster(const std::vector<ClusterObj>& clusters) const {
      if( clusters.empty() ) return nullptr;
      return &clusters.front();
    }

    void findBest(std::vector<ClusterObj>& clustersEta,
                  std::vector<ClusterObj>& clustersPhi) const;

  private:
    const MuonIdHelper*       m_muonIdHelper;
    bool m_debug;
    bool m_combinedGasGaps;
  };

}
#endif
