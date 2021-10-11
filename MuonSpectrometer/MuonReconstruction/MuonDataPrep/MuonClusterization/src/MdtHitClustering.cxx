/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonClusterization/MdtHitClustering.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"

#include "Identifier/Identifier.h"
#include "MuonIdHelpers/MdtIdHelper.h"

#include "MuonPrepRawData/MdtDriftCircleStatus.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace Muon {

  bool MdtHitClustering::init( const Identifier& chid ) {
    hitPattern.clear();
    clusters.clear();

    detEl1 = nullptr;
    detEl2 = nullptr;
    // number of layers and tubes
    nlay = 0;
    ntube1 = 0;
    ntube2 = 0;
    
    // number of multilayers in chamber
    nml = 0;

    int eta =  m_mdtIdHelper->stationEta(chid);
    int phi = m_mdtIdHelper->stationPhi(chid);
    int name = m_mdtIdHelper->stationName(chid);
    
    // get detEL for first ml (always there)
    detEl1 = m_detMgr->getMdtReadoutElement( m_mdtIdHelper->channelID( name,eta,phi,1,1,1 ) );
    if( !detEl1 ) return false;
    detEl2 = nullptr;
    // number of layers and tubes
    nlay = detEl1->getNLayers();
    ntube1 = detEl1->getNtubesperlayer();    
    ntube2 = 0;
    
    // number of multilayers in chamber
    nml = detEl1->nMDTinStation();
    
    // treament of chambers with two ml
    if( nml == 2 ){
      Identifier firstIdml1 = m_mdtIdHelper->channelID( name,eta,phi,2,1,1 );
      detEl2 = m_detMgr->getMdtReadoutElement( firstIdml1 ); 
      ntube2 = detEl2->getNtubesperlayer();
    }
    
    // here we are going to calculate the staggering lay out of the MDT chamber 
    // by comparing the difference of local positions of the first and second tube in the first layer
    // with the position of the first tube in the second layer
    Amg::Transform3D gToStation = detEl1->GlobalToAmdbLRSTransform();
    Identifier idml0lay0tube0  = m_mdtIdHelper->channelID( name,eta,phi,1,1,1 );
    Amg::Vector3D posMl0lay0tube0 = gToStation*(detEl1->surface( idml0lay0tube0 ).center());    
    Identifier idml0lay0tube1  = m_mdtIdHelper->channelID( name,eta,phi,1,1,2 );
    Amg::Vector3D posMl0lay0tube1 = gToStation*(detEl1->surface( idml0lay0tube1 ).center());    
    Identifier idml0lay1tube0  = m_mdtIdHelper->channelID( name,eta,phi,1,2,1 );
    Amg::Vector3D posMl0lay1tube0 = gToStation*(detEl1->surface( idml0lay1tube0 ).center());    
    bool posStagCor = true;
    if( (posMl0lay0tube1.y()-posMl0lay0tube0.y())*(posMl0lay1tube0.y()-posMl0lay0tube0.y()) < 0  ) posStagCor = false;
    
    /*
    1 0  -1   0->0    1->0,1   -> stag = -1
   1 0   -1   0->0,1  1->1,2   -> stag = 0

   1 0   -1   0->0,1  1->1,2   -> stag = 0
    1 0   1   0->0    1->0,1   -> stag = -1 

   0 1    1   0->0    1->0,1   -> stag = -1
    0 1   1   0->0,1  1->1,2   -> stag = 0

    0 1   1   0->0,1  1->1,2   -> stag = 0
   0 1   -1   0->0    1->0,1   -> stag = -1
    */


    hitPattern.resize(nml*nlay);
    staggeringCorrection.resize(nml*nlay,0);

    for( unsigned int i=0;i<staggeringCorrection.size();++i ) {
      unsigned int corlay = i;
      if( corlay >= nlay ) corlay-=nlay;
      if( posStagCor ){
	if( nlay == 4 && i >= nlay ){
	  if( corlay%2==1 ) staggeringCorrection[i] = 0;
	  else              staggeringCorrection[i] = -1;
	}else{
	  if( corlay%2==1 ) staggeringCorrection[i] = -1;
	  else              staggeringCorrection[i] = 0;
	}
      }else{
	if( nlay == 4 && i >= nlay ){
	  if( corlay%2==1 ) staggeringCorrection[i] = -1;
	  else              staggeringCorrection[i] = 0;
	}else{
	  if( corlay%2==1 ) staggeringCorrection[i] = 0;
	  else              staggeringCorrection[i] = -1;
	}
      }
    }
    unsigned int nl = 0;
    std::vector< std::vector<int> >::iterator tit = hitPattern.begin();
    std::vector< std::vector<int> >::iterator tit_end = hitPattern.end();
    for( ;tit!=tit_end;++tit,++nl) {
      if( nl < nlay ) tit->resize(ntube1,-1);
      else            tit->resize(ntube2,-1);
    }
    return true;
  }

  void MdtHitClustering::neighbouringTubes( const Id& id, std::vector<Id>& neighbours ) const {
    neighbours.clear();
    int ntubes = id.lay < (int)nlay ? ntube1 : ntube2;
    // first add neighbours in the same layer
    if( id.tube-1 >= 0 )     neighbours.emplace_back(id.lay,id.tube-1 );
    if( id.tube+1 < ntubes ) neighbours.emplace_back(id.lay,id.tube+1 );

    // now add layer below
    if( id.lay > 0 && id.lay != (int)nlay ){
      int prevLay = id.lay-1;
      int nb1 = id.tube + staggeringCorrection[prevLay];
      int nb2 = nb1 + 1;
      if( nb1 >= 0 )     neighbours.emplace_back(prevLay,nb1 );
      if( nb2 < ntubes ) neighbours.emplace_back(prevLay,nb2 );
    }
    unsigned int nextLay = id.lay+1;
    if( nextLay != nlay && nextLay < 2*nlay ){
      int nb1 = id.tube + staggeringCorrection[nextLay];
      int nb2 = nb1 + 1;
      if( nb1 >= 0 ) neighbours.emplace_back(nextLay,nb1 );
      if( nb2 < ntubes ) neighbours.emplace_back(nextLay,nb2 );
    }
  }

  void MdtHitClustering::layers( const MdtCluster& cl, int& all, int& inTime, int& inTimeAndBefore ) const {
    std::set<int> lays;
    std::set<int> laysInTime;
    std::set<int> laysInTimeAndBefore;
    for( MdtCluster::HitCit clit=cl.hitList.begin();clit!=cl.hitList.end();++clit ){
      int layer = m_mdtIdHelper->tubeLayer( (*clit)->identify());
      lays.insert(layer);
      if( (*clit)->adc() > 50 ){
	if( (*clit)->status() == Muon::MdtStatusDriftTime ) {
	  laysInTime.insert(layer);
	  laysInTimeAndBefore.insert(layer);
	}else if( (*clit)->status() == Muon::MdtStatusBeforeSpectrum ) {
	  laysInTimeAndBefore.insert(layer);
	}
      }
    }
    all = lays.size();
    inTime = laysInTime.size();
    inTimeAndBefore = laysInTimeAndBefore.size();
  }

  bool MdtHitClustering::cluster( const MdtPrepDataCollection& col ){
      
    if( col.empty() ) return true;
    MdtPrepDataCollection::const_iterator cit_begin = col.begin();
    MdtPrepDataCollection::const_iterator cit_end = col.end();
    const Identifier& chid = (*cit_begin)->identify();
    init(chid);
    
    if( debug ) std::cout << " handling chamber " << col.size() << std::endl;
    std::vector<Id> neighbours;
    std::vector<Id>::iterator nit,nit_end;
    MdtPrepDataCollection::const_iterator cit = cit_begin;   
    for( ; cit!=cit_end;++cit ) {
      const MdtPrepData* mdt = (*cit);
      
      const Identifier& ida = mdt->identify();
      int tube = m_mdtIdHelper->tube(ida)-1;
      int lay  = m_mdtIdHelper->tubeLayer(ida)-1;
      int ml   = m_mdtIdHelper->multilayer(ida)-1;
      if( ml == 1 )  lay += nlay;
	
      // first treat the case of a second hit in the same tube
      int tubeClusterNumber = hitPattern[lay][tube];
      if( tubeClusterNumber != -1 ){
	if( debug ) std::cout << " second hit " << lay << "  " << tube << " clusterid " << tubeClusterNumber << std::endl;
	MdtCluster& cluster = clusters[tubeClusterNumber];
	cluster.addSecond(mdt);
      }else{

	Id id(lay,tube);
	neighbouringTubes(id,neighbours);
	if( debug ) std::cout << " new tube " << id.lay << "  " << id.tube << " neighbours " << neighbours.size() << std::endl;
	nit = neighbours.begin();
	nit_end = neighbours.end();
	MdtCluster* currentCluster = nullptr;
	int currentClusterId = -1;
	for( ;nit!=nit_end;++nit ){
	  
	  int clusterNumber = hitPattern[nit->lay][nit->tube];
	  if( clusterNumber == -1 ) continue;
	  if( debug ) std::cout << "   new neighbour " << nit->lay << "  " << nit->tube << " clusterid " << clusterNumber;
	  
	  MdtCluster& cluster = clusters[clusterNumber];
	  
	  // if the hit is unassigned add it to the cluster
	  if( currentCluster == nullptr ){
	    cluster.add(mdt);
	    currentCluster = &cluster;
	    hitPattern[lay][tube] = clusterNumber;
	    currentClusterId = clusterNumber;
	    if( debug ) std::cout << " adding hit " << std::endl;
	  }else if( clusterNumber != currentClusterId ){
	    // the hit is already assigned to a cluster, merge the two clusters
	    // first update hitPattern
	    MdtCluster::HitIt h = cluster.hitList.begin();
	    MdtCluster::HitIt h_end = cluster.hitList.end();
	    for( ;h!=h_end;++h ) {
	      const Identifier& cid = (*h)->identify();
	      int ctube = m_mdtIdHelper->tube(cid)-1;
	      int clay  = m_mdtIdHelper->tubeLayer(cid)-1;
	      int cml   = m_mdtIdHelper->multilayer(cid)-1;
	      if( cml == 1 )  clay += nlay;
	      hitPattern[clay][ctube] = currentClusterId;
	    }
	    if( debug ) std::cout << " cluster overlap, merging clusters " << std::endl;
	    currentCluster->merge(cluster);
	  }else{
	    if( debug ) std::cout << " cluster overlap, same cluster " << std::endl;
	  }
	}
	// now check whether the hit was already assigned to a cluster, if not create a new cluster
	if( currentCluster == nullptr ){
	  if( debug ) std::cout << " no neighbouring hits, creating new cluster " << clusters.size() << std::endl;
	  MdtCluster cl;
	  cl.hitList.push_back(mdt);
	  hitPattern[lay][tube] = clusters.size();
	  clusters.push_back(cl);
	}
      }
    }
    
    if( debug ) dump();

    return true;
  }

  void MdtHitClustering::dump() const {
    std::cout << " dumping chamber hit map " << std::endl<< std::endl; 
    for( unsigned int nl=0;nl<hitPattern.size();++nl) {
      if( nl == nlay ) std::cout << std::endl << std::endl; 
	
      if( staggeringCorrection[nl] == -1 ) std::cout << " "; 
      for( unsigned int nt = 0;nt<hitPattern[nl].size();++nt ) std::cout << " " << std::setw(2) << hitPattern[nl][nt]+1;
      std::cout << std::endl << std::endl; 
    }
//     std::cout << " clusters " << clusters.size() << std::endl;
//     std::vector< MdtCluster >::const_iterator cit = clusters.begin();
//     std::vector< MdtCluster >::const_iterator cit_end = clusters.end();
//     for( ;cit!=cit_end;++cit ){
//       if( !cit->active() ) continue;
//       std::cout << " cluster " << cit->hitList.size() << std::endl;
//       for( MdtCluster::HitCit clit=cit->hitList.begin();clit!=cit->hitList.end();++clit ){
// 	std::cout << " ml " << m_mdtIdHelper->multilayer( (*clit)->identify())-1
// 		  << " lay " << m_mdtIdHelper->tubeLayer( (*clit)->identify())-1
// 		  << " tube " << m_mdtIdHelper->tube( (*clit)->identify())-1 << std::endl;
//       }
//     }
  }

}

