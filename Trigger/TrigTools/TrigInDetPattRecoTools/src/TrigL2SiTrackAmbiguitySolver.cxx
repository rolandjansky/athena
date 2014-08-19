/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigSiSpacePointBase.h"
#include "TrigInDetPattRecoEvent/TrigL2TrackCandidate.h"
#include "TrigInDetPattRecoTools/TrigL2SiTrackAmbiguitySolvingCuts.h"
#include "TrigInDetPattRecoTools/TrigL2SiTrackAmbiguitySolver.h"

#include<iostream>
#include<iomanip>
#include<map>
#include<algorithm>

//#define SITRACK_DEBUG 1

void TrigL2SiTrackAmbiguitySolver::execute(TrigL2TrackCandidate* m_full_trks, unsigned int m_full_trks_num) {

  // Fill ambiguity list

  std::multimap< float, std::pair<unsigned int, unsigned int> > trackPairs;

  // Loop on trks
#ifdef SITRACK_DEBUG
  std::cout<<"m_full_trks_num="<<m_full_trks_num<<std::endl;
  for(unsigned int trk1=0; trk1<m_full_trks_num; trk1++) {
    // Check if deleted
    if(m_full_trks[trk1].deleted) continue;
    
    for(unsigned int i=0;i<MAX_SILICON_LAYER_NUM;i++) {
      const TrigSiSpacePointBase* pSP=m_full_trks[trk1].sp(i);
      if(pSP!=NULL) {
	std::cout<<"["<<std::setw(4)<<pSP->index()<<"]";
      }
      else std::cout<<"[    ]";
    }
    std::cout<<std::endl;
    for(unsigned int i=0;i<MAX_SILICON_LAYER_NUM;i++) {
      const TrigSiSpacePointBase* pSP=m_full_trks[trk1].sp2(i);
      if(pSP!=NULL) {
	std::cout<<"["<<std::setw(4)<<pSP->index()<<"]";
      }
      else std::cout<<"[    ]";
    }
    std::cout<<std::endl;
  }
#endif

  for(unsigned int trk1=0; trk1<m_full_trks_num; trk1++) {
    // Check if deleted
    if(m_full_trks[trk1].deleted) continue;
    for(unsigned int trk2=trk1+1; trk2<m_full_trks_num; trk2++) {
      // Check if deleted
      if(m_full_trks[trk2].deleted) continue;
      float fraction = trackAmbiguityFraction(&(m_full_trks[trk1]), &(m_full_trks[trk2]));

      if((m_cuts.m_extensionSolveShared&&(fraction>0.3)) || (fraction>0.5)) {
#ifdef SITRACK_DEBUG
	std::cout<<"i="<<trk1<<" j="<<trk2<<" fraction = "<<fraction<<std::endl;
#endif
	trackPairs.insert(std::make_pair(fraction, std::make_pair(trk1,trk2)));
      }
    }    
  }

  //merge or remove
#ifdef SITRACK_DEBUG
  std::cout<<"Total "<<trackPairs.size()<<" ambiguities found"<<std::endl;
#endif
  std::for_each(trackPairs.rbegin(),trackPairs.rend(),TrigL2SiTrackAmbiguitySolver::TrackPairDispatcher(*this,m_full_trks));
  trackPairs.clear();

  int nGoodTracks=0;
  for(unsigned int trk1=0; trk1<m_full_trks_num; trk1++) {
    if(m_full_trks[trk1].deleted) continue;
    nGoodTracks++;
  }

#ifdef SITRACK_DEBUG
  std::cout<<"After merge/remove "<<nGoodTracks<<" tracks left"<<std::endl;

  for(unsigned int trk1=0; trk1<m_full_trks_num; trk1++) {
    // Check if deleted
    if(m_full_trks[trk1].deleted) continue;
    
    for(unsigned int i=0;i<MAX_SILICON_LAYER_NUM;i++) {
      const TrigSiSpacePointBase* pSP=m_full_trks[trk1].sp(i);
      if(pSP!=NULL) {
	std::cout<<"["<<std::setw(4)<<pSP->index()<<"]";
      }
      else std::cout<<"[    ]";
    }
    std::cout<<std::endl;
    for(unsigned int i=0;i<MAX_SILICON_LAYER_NUM;i++) {
      const TrigSiSpacePointBase* pSP=m_full_trks[trk1].sp2(i);
      if(pSP!=NULL) {
	std::cout<<"["<<std::setw(4)<<pSP->index()<<"]";
      }
      else std::cout<<"[    ]";
    }
    std::cout<<std::endl;
  }
#endif

  if(m_cuts.m_extensionSolveShared) {

    //filling shared sp map ...

    std::map<unsigned int,std::vector<unsigned int> > spUsageMap;

    for(unsigned int trk1=0; trk1<m_full_trks_num; trk1++) {
      if(m_full_trks[trk1].deleted) continue;
      for(unsigned int trk2=trk1+1; trk2<m_full_trks_num; trk2++) {
	if(m_full_trks[trk2].deleted) continue;
	trackAmbiguityShared(trk1, trk2, m_full_trks, spUsageMap);
      }
    }
    
    //analyzing ...

    for(unsigned int trk1=0; trk1<m_full_trks_num; trk1++) {
      if(m_full_trks[trk1].deleted) continue;
      if(spUsageMap.find(trk1)==spUsageMap.end()) continue;

      const std::vector<unsigned int>& vShared = (*spUsageMap.find(trk1)).second;
      TrigL2TrackCandidate& track = m_full_trks[trk1];

      int total=0, shared=0;
      for(unsigned int i=0; i<MAX_SILICON_LAYER_NUM; i++) {
	if(track.sp(i)!=0) {
	  total++;
	  shared += vShared[i];
	}
      }

      float fraction = ((1.0*shared)/total);

      if(fraction>0.2) {
	track.deleted=true;
      }
    }
  }
  nGoodTracks=0;
  for(unsigned int trk1=0; trk1<m_full_trks_num; trk1++) {
    if(m_full_trks[trk1].deleted) continue;
    nGoodTracks++;
  }
#ifdef SITRACK_DEBUG
  std::cout<<"After shared solving  "<<nGoodTracks<<" tracks left"<<std::endl;

  for(unsigned int trk1=0; trk1<m_full_trks_num; trk1++) {
    // Check if deleted
    if(m_full_trks[trk1].deleted) continue;
    
    for(unsigned int i=0;i<MAX_SILICON_LAYER_NUM;i++) {
      const TrigSiSpacePointBase* pSP=m_full_trks[trk1].sp(i);
      if(pSP!=NULL) {
	std::cout<<"["<<std::setw(4)<<pSP->index()<<"]";
      }
      else std::cout<<"[    ]";
    }
    std::cout<<std::endl;
    for(unsigned int i=0;i<MAX_SILICON_LAYER_NUM;i++) {
      const TrigSiSpacePointBase* pSP=m_full_trks[trk1].sp2(i);
      if(pSP!=NULL) {
	std::cout<<"["<<std::setw(4)<<pSP->index()<<"]";
      }
      else std::cout<<"[    ]";
    }
    std::cout<<std::endl;
  }
#endif



  //final step : Check if the track has too few space points or has been deleted
  for(unsigned int trk=0; trk<m_full_trks_num; trk++) {
    if(m_full_trks[trk].deleted) continue;

    TrigL2TrackCandidate& track = m_full_trks[trk];
    int sumSp=0, sumSpAll=0;

    for(unsigned int i=0; i<MAX_SILICON_LAYER_NUM; i++) {
      if(track.sp(i)!=0) {
	sumSp++;
	sumSpAll++;
      }
      if(track.sp2(i)!=0) {
	sumSpAll++;
      }
    }
    if((sumSp<4) || (sumSpAll<m_cuts.m_dynamic_extensionSpacePoints)) {
      track.deleted = true;
    }
  }
}


void TrigL2SiTrackAmbiguitySolver::TrackPairDispatcher::operator()(std::pair<const float, std::pair<unsigned int, unsigned int> >& pair) {

  TrigL2TrackCandidate* p1 = &m_full_trks[pair.second.first];
  TrigL2TrackCandidate* p2 = &m_full_trks[pair.second.second];
  
  if(p1->deleted || p2->deleted) return; 
#ifdef SITRACK_DEBUG 
  std::cout<<"RESOLVING: "<<pair.second.first<<" and "<<pair.second.second<<std::endl;
#endif

#ifdef SITRACK_DEBUG 
  std::cout<<"T1: "<<std::endl;
  for(unsigned int i=0; i<MAX_SILICON_LAYER_NUM; i++) {
    std::cout<<"[";
    if(p1->sp(i)!=0) 
      std::cout<<std::setw(4)<<p1->sp(i)->index();
    else std::cout<<"    ";
    std::cout<<"]";
  }
  std::cout<<std::endl;
  for(unsigned int i=0; i<MAX_SILICON_LAYER_NUM; i++) {
    std::cout<<"[";
    if(p1->sp2(i)!=0) 
      std::cout<<std::setw(4)<<p1->sp2(i)->index();
    else std::cout<<"    ";
    std::cout<<"]";
  }
  std::cout<<std::endl;
  std::cout<<"T2: "<<std::endl;
  for(unsigned int i=0; i<MAX_SILICON_LAYER_NUM; i++) {
    std::cout<<"[";
    if(p2->sp(i)!=0) 
      std::cout<<std::setw(4)<<p2->sp(i)->index();
    else std::cout<<"    ";
    std::cout<<"]";
  }
  std::cout<<std::endl;
  for(unsigned int i=0; i<MAX_SILICON_LAYER_NUM; i++) {
    std::cout<<"[";
    if(p2->sp2(i)!=0) 
      std::cout<<std::setw(4)<<p2->sp2(i)->index();
    else std::cout<<"    ";
    std::cout<<"]";
  }
  std::cout<<std::endl;
  std::cout<<pair.first<<" ";
#endif
   
  float fraction = m_parent.trackAmbiguityFraction(p1,p2);
  bool doMerge = (fraction>=0.75);

  if(doMerge) {
    float distance = m_parent.trackAmbiguityDistance(p1,p2);
    doMerge = (distance<m_parent.m_cuts.m_extensionSolveTracksDRCut);
    if(doMerge) {
#ifdef SITRACK_DEBUG 
      std::cout<<"MERGE fraction="<<fraction<<std::endl;
#endif
    }
    else {
#ifdef SITRACK_DEBUG 
      std::cout<<"REMOVE fraction="<<fraction<<std::endl;
#endif
    }
  }
  else {
#ifdef SITRACK_DEBUG 
    std::cout<<"REMOVE fraction="<<fraction<<std::endl;
#endif
  }
  if(doMerge)
    m_parent.trackMergeAmbiguity(p1,p2);
  else
    m_parent.trackRemoveAmbiguity(p1,p2);
}


void TrigL2SiTrackAmbiguitySolver::trackAmbiguityShared(unsigned int idx1, unsigned int idx2, 
							TrigL2TrackCandidate* tracks, 
							std::map<unsigned int,std::vector<unsigned int> >& spShared) {

  // Count identical spacepoints
#ifdef SITRACK_DEBUG 
  std::cout<<"SHARED: "<<idx1<<" and "<<idx2<<std::endl;
#endif
  if(compareTracks(&tracks[idx2],&tracks[idx1])) {//swapping 
    unsigned int tmp;
    tmp=idx1;idx1=idx2;idx2=tmp;
#ifdef SITRACK_DEBUG 
    std::cout<<"SWAP"<<std::endl;
#endif
  }
  TrigL2TrackCandidate* p1 = &tracks[idx1];
  TrigL2TrackCandidate* p2 = &tracks[idx2];

  // Count identical spacepoints

  if(spShared.find(idx2)==spShared.end()) {
    std::vector<unsigned int> content(MAX_SILICON_LAYER_NUM,0);
    spShared.insert(std::make_pair(idx2,content));
  }

  std::vector<unsigned int>& shared = (*spShared.find(idx2)).second;

  for(int i=0; i<MAX_SILICON_LAYER_NUM; i++) {
    // bool Found=false;
    if(p1->sp(i)!=0  && p1->sp(i)==p2->sp(i))  {
      //  Found=true;
      shared[i]=1;
    }
    if(p1->sp(i)!=0  && p1->sp(i)==p2->sp2(i)) {
      //  Found=true;
      shared[i]=1;
    }
    if(p1->sp2(i)!=0 && p1->sp2(i)==p2->sp(i)) {
      // Found=true;
      shared[i]=1;
    }
    if(p1->sp2(i)!=0 && p1->sp2(i)==p2->sp2(i)){
      // Found=true;
      shared[i]=1;
    }
#ifdef SITRACK_DEBUG 
    if(Found) std::cout<<i<<" ";
#endif
  }
#ifdef SITRACK_DEBUG 
  std::cout<<std::endl;
#endif
}




void TrigL2SiTrackAmbiguitySolver::trackMergeAmbiguity(TrigL2TrackCandidate* p1, TrigL2TrackCandidate* p2) {
 
  // Skip deleted

  if(p1->deleted || p2->deleted) return;

  if(compareTracks(p2,p1)) {//swapping 
    TrigL2TrackCandidate* tmp;
    tmp=p1;p1=p2;p2=tmp;
#ifdef SITRACK_DEBUG
    std::cout<<"SWAP"<<std::endl;
#endif
  }

  p1->merge(p2);
  p2->deleted=true;
}


bool TrigL2SiTrackAmbiguitySolver::compareTracks(const TrigL2TrackCandidate* p1, const TrigL2TrackCandidate* p2) {

  // Count space points
  int sp1=0, sp2=0;
  int lay1=0, lay2=0;

  for(unsigned int i=0; i<MAX_SILICON_LAYER_NUM; i++) {
    if(p1->sp(i)!=0) {
      sp1++; lay1++;
    }
    if(p2->sp(i)!=0) {
      sp2++; lay2++;
    }
    if(p1->sp2(i)!=0) 
      sp1++;
    if(p2->sp2(i)!=0) 
      sp2++;
  }

  if(p1->deleted) lay1=0;
  if(p2->deleted) lay2=0;

  // Evaluate bool relations
  bool isLayBetter  = (lay1             >  lay2);
  bool isLayEqual   = (lay1             == lay2);
  bool isSpBetter   = (sp1              >  sp2);
  bool isSpEqual    = (sp1              == sp2);
  bool isChi2Better = (p1->m_params.m_chi2 <  p2->m_params.m_chi2);

  // Check if first track is better
  return (isLayBetter || (isLayEqual&&isSpBetter)|| (isLayEqual&&isSpEqual&&isChi2Better));
}



void TrigL2SiTrackAmbiguitySolver::trackRemoveAmbiguity(TrigL2TrackCandidate* p1, TrigL2TrackCandidate* p2) {

  // Skip deleted
  if(p1->deleted || p2->deleted) return;

  // Check order and set flags
  if(compareTracks(p1,p2)) {
    p2->deleted=true;
  }
  else {
    p1->deleted=true;
#ifdef SITRACK_DEBUG
    std::cout<<"SWAP"<<std::endl;
#endif
  }
}


float TrigL2SiTrackAmbiguitySolver::trackAmbiguityDistance(const TrigL2TrackCandidate* p1, const TrigL2TrackCandidate* p2) {

  // Scaling factors
  double scale;
  double dphi, deta;

  // Phi distance
  dphi = p1->m_params.m_phi0 - p2->m_params.m_phi0;
  if(dphi>M_PI)  
    dphi = dphi - 2*M_PI;
  if(dphi<-M_PI) 
    dphi = dphi + 2*M_PI;
  scale = m_cuts.m_extensionSolveTracksPhiScale;
  dphi /= scale;

  // Eta distance
  deta = p1->m_params.m_eta - p2->m_params.m_eta;
  scale = m_cuts.m_extensionSolveTracksEtaScale;
  deta /= scale;

  // Return squared distance
  double dr = dphi*dphi + deta*deta;
  return dr;
}

float TrigL2SiTrackAmbiguitySolver::trackAmbiguityFraction(const TrigL2TrackCandidate* p1, const TrigL2TrackCandidate* p2) {

  // Count identical spacepoints

  int total=0, common=0;

  bool first=true, commonFirst=false;

  for(int i=0; i<MAX_SILICON_LAYER_NUM; i++) {
    if(p1->sp(i)!=0 && p2->sp(i)!=0) total++;
    if(p1->sp(i)!=0 && p1->sp(i)==p2->sp(i)) {
      if(first) {
	commonFirst=true; first=false;
      }
      common++;
    }
    if(p1->sp2(i)!=0 && p1->sp2(i)==p2->sp2(i)) {
      common++;
    }
    if((p1->sp(i)!=0 || p2->sp(i)!=0) && (p1->sp(i)!=p2->sp(i))) 
      first=false;
  }
  
  // Evaluate fraction
  float ambiguity=0.0;
  if(total!=0) {
    ambiguity=((float)common)/total;
    if(m_cuts.m_extensionSolveV0Tracks && commonFirst) 
      ambiguity += 1.0;
  }
  
  return ambiguity;
}

void TrigL2SiTrackAmbiguitySolver::findClusters(TrigL2TrackCandidate* m_full_trks, unsigned int m_full_trks_num) {

  std::vector<std::pair<unsigned int, unsigned int> > trackPairs;
  std::vector<unsigned int> trackNumbers;

  trackNumbers.resize(m_full_trks_num);

  int nTracksTotal=0;

  // Loop on trks
  for(unsigned int trk1=0; trk1<m_full_trks_num; trk1++) {
    // Check if deleted
    if(m_full_trks[trk1].deleted) continue;

    trackNumbers[trk1]=trk1;//initialization
    nTracksTotal++;
    for(unsigned int trk2=trk1+1; trk2<m_full_trks_num; trk2++) {
      // Check if deleted
      if(m_full_trks[trk2].deleted) continue;
      float fraction = trackAmbiguityFraction(&(m_full_trks[trk1]), &(m_full_trks[trk2]));
      if((m_cuts.m_extensionSolveShared&&(fraction>0.3)) || (fraction>0.5)) {
	trackPairs.push_back(std::make_pair(trk1,trk2));
      }
    }    
  }
   
  int nFlips=0;
  int nIterations=0;
  
  do {
    nFlips=0;
    for(std::vector<std::pair<unsigned int, unsigned int> >::iterator it=trackPairs.begin(); 
	it!=trackPairs.end();++it) {
      unsigned int idx1,idx2;
      idx1=(*it).first;idx2=(*it).second;
      if(trackNumbers[idx1]==trackNumbers[idx2]) {// already in the same cluster
	continue;
      }
      else {
	unsigned int clusterIdx = (trackNumbers[idx1]<trackNumbers[idx2]) ? trackNumbers[idx1] : trackNumbers[idx2];
	trackNumbers[idx1] = clusterIdx;
	trackNumbers[idx2] = clusterIdx;
	nFlips++;
      }
    }
    nIterations++;
  } while ((nFlips!=0)&&(nIterations<100));

#ifdef SITRACK_DEBUG 
  std::cout<<"AMB DBG: Track numbers assigned, nTracks="<<nTracksTotal<<" nIterations="<<nIterations<<std::endl;
#endif

  //collecting track clusters

  std::map<unsigned int, std::vector<unsigned int> > trackClusters;

  for(unsigned int trk1=0; trk1<m_full_trks_num; trk1++) {

    if(m_full_trks[trk1].deleted) continue;

    unsigned int clusterIdx = trackNumbers[trk1];

    if(trk1==clusterIdx) continue;//will be added later if there are any other tracks in the same cluster

    std::map<unsigned int, std::vector<unsigned int> >::iterator clIt = trackClusters.find(clusterIdx); 
    if(trackClusters.end()==clIt) {
      std::vector<unsigned int> cluster;
      cluster.push_back(trk1);
      trackClusters.insert(std::pair<unsigned int, std::vector<unsigned int> >(clusterIdx,cluster));
    }
    else {
      (*clIt).second.push_back(trk1);
    }
  }

  //now adding track that actually starts the cluster, i.e. track number = cluster index

  for(std::map<unsigned int, std::vector<unsigned int> >::iterator clIt = trackClusters.begin();clIt!=trackClusters.end();++clIt) {
    (*clIt).second.push_back((*clIt).first);
  }

#ifdef SITRACK_DEBUG 
  std::cout<<"AMB DBG: "<<nTracksTotal<<" tracks arranged into "<<trackClusters.size()<<" clusters"<<std::endl;
  unsigned int nBiggest=1;
  for(std::map<unsigned int, std::vector<unsigned int> >::iterator clIt = trackClusters.begin();clIt!=trackClusters.end();++clIt) {
    if(nBiggest<(*clIt).second.size()) {
      nBiggest=(*clIt).second.size();
    }
    std::cout<<"CL"<<(*clIt).first<<" size ="<<(*clIt).second.size()<<std::endl;
    for(std::vector<unsigned int>::iterator trIt = (*clIt).second.begin(); trIt!=(*clIt).second.end();++trIt) {
      std::cout<<"["<<(*trIt)<<"]";
    }
    std::cout<<std::endl;
  }
  std::cout<<"AMB DBG: The biggest cluster contains "<<(100.0*nBiggest)/nTracksTotal<<" percents of all tracks"<<std::endl;
#endif

}
