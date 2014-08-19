/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigSiSpacePointBase.h"
#include "TrigInDetPattRecoEvent/TrigL2TrackCandidate.h"
#include "TrigInDetPattRecoTools/TrigL2SiTrackTripletMergingCuts.h"
#include "TrigInDetPattRecoTools/TrigL2SiTrackTripletMerger.h"

#include<map>

//#define SITRACK_DEBUG 1

bool TrigL2SiTrackTripletMerger::operator()(TrigL2TrackCandidate& tree) {

#ifdef SITRACK_DEBUG
  std::cout<<"new tree, m_full_trks_num="<<m_full_trks_num<<" max = "<<m_cuts.m_recoMaxTracks<<std::endl;
  std::cout<<"deleted ? "<<std::boolalpha<<tree.deleted<<std::endl;
#endif

  if((int)(m_full_trks_num)>=m_cuts.m_recoMaxTracks) 
    return true;

  if(tree.deleted) 
    return false;

#ifdef SITRACK_DEBUG
  for(unsigned int iTrip=0;iTrip<tree.triplet_num;++iTrip) {
    std::cout<<"["<<tree.triplet_sp3(iTrip)->index()<<"]"<<" params: "<<tree.tripletParameters(iTrip).m_d0<<
      " "<<tree.tripletParameters(iTrip).m_z0<<" "<<tree.tripletParameters(iTrip).m_phi0<<" "<<
      tree.tripletParameters(iTrip).m_eta<<" "<<tree.tripletParameters(iTrip).m_pT<<" "<<
      tree.tripletParameters(iTrip).m_chi2*1000.0<<std::endl;
  }
#endif  

  TripletTable table(tree);

  std::for_each(tree.spacePoints().begin()+2,tree.spacePoints().end(),
		std::bind1st(std::mem_fun(&TripletTable::addSpacePoint), &table));

#ifdef SITRACK_DEBUG
  std::cout<<"N SP = "<<tree.spacePoints().size()<<" Triplet table size = "<<table.m_size<<std::endl;
  std::cout<<table;
#endif

  std::multimap< float, std::pair<unsigned int,unsigned int> > tripletDistanceMap;
  
  for(unsigned int t1 = 0; t1<tree.triplet_num; t1++) {
    for(unsigned int t2 = t1+1; t2<tree.triplet_num; t2++) {

      float distance = tripletDistance(tree,t1,t2);

      if(distance<m_cuts.m_extensionSolveTripletsDRCut) {
#ifdef SITRACK_DEBUG
      std::cout<<"i="<<t1<<" j="<<t2<<" dist="<<distance<<" cut"<<m_cuts.m_extensionSolveTripletsDRCut<<std::endl;
#endif 
      tripletDistanceMap.insert(std::make_pair(distance, std::make_pair(t1,t2)));
      }
    }
  }
#ifdef SITRACK_DEBUG
  std::cout<<"Triplet distance map size = "<<tripletDistanceMap.size()<<std::endl;
#endif

  if(tripletDistanceMap.size()==0) {
    tree.deleted=true;
    return false;
  }

  std::for_each(tripletDistanceMap.begin(),tripletDistanceMap.end(),
		std::bind1st(std::mem_fun(&TripletTable::mergeRows),&table));

#ifdef SITRACK_DEBUG
  std::cout<<"After row merging..."<<std::endl;
  std::cout<<table;
#endif
  std::vector<const TripletTableRow*> rV;
  table.getMergedTriplets(rV);
  const TripletTableRow* pBest = *(std::min_element(rV.begin(),rV.end(),TripletTableRow::compareTriplets()));

  // Delete pattern if too few space points in best ambiguity

  if(pBest->m_nLay < m_cuts.m_dynamic_extensionCutSpacePoints) {
    tree.deleted = true;
    return false;
  }

  TrigL2TrackCandidate* pTrack = m_full_trks+m_full_trks_num;
  pTrack->reset();
  pTrack->addSpacePoint(tree.seed_sp1());
  pTrack->addSpacePoint(tree.seed_sp2());

  for(unsigned int i = 0; i<MAX_SILICON_LAYER_NUM; i++) {

    char sp1_idx = pBest->m_triplets[i][0];
    char sp2_idx = pBest->m_triplets[i][1];

    if(sp1_idx!=0) {
      sp1_idx-=2;
      pTrack->addSpacePoint(tree.triplet_sp3(sp1_idx));
    }
    if(sp2_idx!=0) {
      sp2_idx-=2;
      pTrack->addSpacePoint(tree.triplet_sp3(sp2_idx));
    }
  }

  pTrack->m_params = tree.tripletParameters(pBest->m_tripletIdx);
  pTrack->m_params.m_chi2 = pBest->m_chi2;
  m_full_trks_num++;
#ifdef SITRACK_DEBUG
  std::cout<<"New track created..."<<std::endl;
#endif
  return false;
}

float TrigL2SiTrackTripletMerger::tripletDistance(TrigL2TrackCandidate& t, unsigned int i, unsigned int j) {

  // Scaling factors
  float scale;
  float dphi, deta;
 
  // Phi distance
  dphi = t.tripletParameters(i).m_phi0 - t.tripletParameters(j).m_phi0;
  if(dphi>M_PI)  
    dphi = dphi - 2*M_PI;
  if(dphi<-M_PI) 
    dphi = dphi + 2*M_PI;

#ifdef SITRACK_DEBUG
  std::cout<<"dphi="<<dphi<<" ";
#endif

  scale = m_cuts.m_extensionSolveTripletsPhiScale;
  dphi /= scale;

  // Eta distance
  deta = t.tripletParameters(i).m_eta - t.tripletParameters(j).m_eta;

#ifdef SITRACK_DEBUG
  std::cout<<t.triplet_num<<" i="<<i<<" eta="<<t.tripletParameters(i).m_eta<<" j="<<j<<" eta="<<t.tripletParameters(j).m_eta<<"deta="<<deta<<std::endl;
#endif
  scale = m_cuts.m_extensionSolveTripletsEtaScale;
  deta /= scale;

  // Return squared distance
  float dr = dphi*dphi + deta*deta;
  return dr;

}


bool operator == (const TripletTableRow& t1, const TripletTableRow& t2) {

  if (t1.m_nLay!=t2.m_nLay) return false;
  if (t1.m_nSP !=t2.m_nSP) return false;

  return true;
}

bool operator < (const TripletTableRow& t1, const TripletTableRow& t2) {

  bool isLayBetter        = (t1.m_nLay >  t2.m_nLay);
  bool isLayEqual         = (t1.m_nLay == t2.m_nLay);
  bool isSpBetter         = (t1.m_nSP  >  t2.m_nSP);
  // Check if first track is better
  return (isLayBetter || (isLayEqual&&isSpBetter));
}

bool TripletTable::compareRows(unsigned int i, unsigned int j) {

  if(m_rows[i]==m_rows[j]) {
    return (m_rows[i].m_chi2<m_rows[j].m_chi2);
  } else {
    return (m_rows[i]<m_rows[j]);
  }
}

void TripletTable::mergeRows(std::pair<float, std::pair<unsigned int, unsigned int> > tp) {

  unsigned int it = tp.second.first;
  unsigned int jt = tp.second.second;

#ifdef SITRACK_DEBUG
  std::cout<<"distance = "<<tp.first<<"i="<<it<<" with j="<<jt<<std::endl;
#endif

  // Skip deleted
  
  if(m_deleted[it] || m_deleted[jt]) return;

  // Check order and set flags

  if(compareRows(jt,it)) {
    unsigned int tmp=it;
    it=jt;jt=tmp;
  }

#ifdef SITRACK_DEBUG
  std::cout<<"Merging i="<<it<<" with j="<<jt<<" table size="<<m_size<<std::endl;
#endif

  TripletTableRow& t1 = m_rows[it];
  TripletTableRow& t2 = m_rows[jt];

  // Insert second track space points in first track
  for(unsigned int i=0; i<MAX_SILICON_LAYER_NUM; i++) {

    // Add first SP in module overlap

#ifdef SITRACK_DEBUG
    std::cout<<"checking layer "<<i<<std::endl;
#endif

    if(t1.m_triplets[i][0]==0 && t2.m_triplets[i][0]!=0) {

#ifdef SITRACK_DEBUG
      std::cout<<"sp_idx= "<<t2.m_triplets[i][0]<<" to pos0"<<std::endl;
#endif     

      t1.m_triplets[i](0,t2.m_triplets[i][0]);
      t1.m_nLay++;t1.m_nSP++;
    }

    // Add second SP in module overlap 

    if(t1.m_triplets[i][0]!=0 && t1.m_triplets[i][1]==0 && t2.m_triplets[i][0]!=0) {
      // Barrel
      bool added=false;
#ifdef SITRACK_DEBUG
      std::cout<<"retrieving sps: "<<t2.m_triplets[i][0]-2<<" and "<<t1.m_triplets[i][0]-2<<std::endl;
#endif 
      const TrigSiSpacePointBase* pSP2 = m_tree.triplet_sp3(t2.m_triplets[i][0]-2);
      const TrigSiSpacePointBase* pSP1 = m_tree.triplet_sp3(t1.m_triplets[i][0]-2);
      if(i<OffsetEndcapPixels ) {

	float dr = pSP2->r()-pSP1->r();
	if(dr>0.5)  {
	  t1.m_triplets[i](1,t2.m_triplets[i][0]); 
	  added=true;
	}
	if(dr<-0.5) {
	  t1.m_triplets[i](1,t1.m_triplets[i][0]);
	  t1.m_triplets[i](0,t2.m_triplets[i][0]);
	  added=true;
	}
      }
      // Endcap
      else {
	float dz = pSP2->z()-pSP1->z();
	if(pSP2->z()<0) 
	  dz=-dz;
	if(dz>0.5){
	  t1.m_triplets[i](1,t2.m_triplets[i][0]);
	  added=true;
	}
	if(dz<-0.5) {
	  t1.m_triplets[i](1,t1.m_triplets[i][0]);
	  t1.m_triplets[i](0,t2.m_triplets[i][0]);
	  added=true;
	}
      }
      if(added) {
	t1.m_nSP++;//and layer count stays the same
      }
    }
  }
  t1.m_chi2+=t2.m_chi2;
  t2.m_chi2=1e8;
  m_deleted[jt]=true;
}
