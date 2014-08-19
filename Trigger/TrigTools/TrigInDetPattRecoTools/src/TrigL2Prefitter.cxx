/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigSiSpacePointBase.h"
#include "TrigInDetPattRecoEvent/TrigSiSpacePointUtils.h"
#include "TrigInDetPattRecoEvent/TrigL2TrackCandidate.h"
#include "TrigInDetPattRecoTools/SimpleKalmanFilterClass.h"
#include "TrigInDetPattRecoTools/TrigL2Prefitter.h"

#include <iostream>
#include <cmath>
#include <cstring>
#include <list>
#include <algorithm>
#include <map>

void TrigL2Prefitter::run(TrigL2TrackCandidate* pTrack) {

  const double Bfield = 20.84;
  const double C=0.029997;
  const double maxChi2=500.0;
  const double maxChi2inGroup=6000.0;
  const double minPt=600.0;

  double chi2,Bz;
  int nAmbiLayers=0,nHits=0;

  Bz=(m_fieldOn) ? Bfield : 0.0;

  SimpleKalmanFilterClass KF(C*Bz);

  std::map<int,int> spUsageMap;

  std::vector<const TrigSiSpacePointBase*>::const_iterator spIt=pTrack->spacePoints().begin();

  for( ; spIt!=pTrack->spacePoints().end(); spIt++) {

    spUsageMap.insert(std::pair<int,int>((*spIt)->index(),1));

#ifdef IDSCAN_DEBUG
    std::cout << "IDSCAN_DEBUG: new group - layer = " << (*spIt)->layer()
	      << " phi= " << (*spIt)->phi()
	      << " rho= " << (*spIt)->r()
	      << " zed= " << (*spIt)->z()
	      << std::endl;
#endif
  }
	  
  int nAmbiSPs=0;

  //setting used=0 to spacepoints in the same layer

  spIt=pTrack->spacePoints().begin();

  for(; spIt!=pTrack->spacePoints().end(); spIt++) {

    // TO-DO implement isAmbi(sp1, sp2) method of the Prefitter class
    // if isAmbi ....

    int lay1=(*spIt)->layer();
    int type1=(lay1<OffsetEndcapPixels)?0:1;
    std::vector<const TrigSiSpacePointBase*>::const_iterator nextSpIt = spIt;
    nextSpIt++;
    for(; nextSpIt!=pTrack->spacePoints().end(); nextSpIt++) {

      int lay2=(*nextSpIt)->layer();
      int type2=(lay2<OffsetEndcapPixels)?0:1;
      if((lay2==lay1)&&(type2==type1)){
		    
	if(((type1==0)&&(fabs((*nextSpIt)->r()-(*spIt)->r())<5.0))
	   ||((type1==1)&&(fabs((*nextSpIt)->z()-(*spIt)->z())<2.0))) {
	  
	  if(spUsageMap[(*nextSpIt)->index()]!=0) {
	    nAmbiSPs++;
	    spUsageMap[(*nextSpIt)->index()]=0;
	  }
	  if(spUsageMap[(*spIt)->index()]!=0) {
	    nAmbiSPs++;
	    spUsageMap[(*spIt)->index()]=0;
	  }
	}
      }
    }
  }

  // now resolve ambiguities, if any, using backward Kalman filter
#ifdef IDSCAN_DEBUG
  std::cout<<"nAmbi = "<<nAmbiSPs<<std::endl;
#endif
  std::vector<const TrigSiSpacePointBase*> arrAmbiLayers[MAX_SILICON_LAYER_NUM];
  std::vector<const TrigSiSpacePointBase*> spVec;
  int arrAmbiIdx[MAX_SILICON_LAYER_NUM];

  const TrigSiSpacePointBase* pBestSP;
  spVec.clear();

  KF.newTrack(pTrack);
 
  memset(&arrAmbiIdx[0],0,sizeof(arrAmbiIdx));

  chi2=0.0;

  for(std::vector<const TrigSiSpacePointBase *>::const_iterator spIt=pTrack->spacePoints().begin();
      spIt!=pTrack->spacePoints().end();spIt++) {
    spVec.push_back((*spIt));
  }

  std::sort(spVec.begin(),spVec.end(),orderSpacePointsInLayer());

  for(std::vector<const TrigSiSpacePointBase *>::const_reverse_iterator spIt=spVec.rbegin();spIt!=spVec.rend();spIt++) {

      const TrigSiSpacePointBase* pSP=(*spIt);

      if(spUsageMap[pSP->index()]!=0) {// unmasked spacepoints

	double dchi2 = KF.getChi2(pSP);

	if(dchi2<maxChi2) {
	  chi2+=dchi2;
	  KF.update();
	  nHits++;
	}
      }
      else { // masked, i.e. ambi spacepoints
	int LayId=pSP->layer();
	arrAmbiIdx[LayId]++;
	arrAmbiLayers[LayId].push_back(pSP);
      }
  }
#ifdef IDSCAN_DEBUG        
  printf("Estimates before amb.resolving : d0=%f z0=%f phi0=%f pT=%f\n",
	 KF.m_Rk[0],KF.m_Rk[1],KF.m_Rk[2],1.0/KF.m_Rk[4]);
  printf("Chi2 = %f Total %d ambiguities\n",
	 chi2,nAmbiLayers);
#endif

  for(int i=0;i<MAX_SILICON_LAYER_NUM;i++) nAmbiLayers+=arrAmbiIdx[i];
  if(nAmbiLayers!=0) {

    for(int layId=MAX_SILICON_LAYER_NUM-1;layId>=0;layId--) {

      if(arrAmbiIdx[layId]==0) continue;

      pBestSP=NULL;
      double bestChi2=maxChi2;
#ifdef IDSCAN_DEBUG
      printf("Analyzing layer %d ...\n",layId);
#endif
      for(std::vector<const TrigSiSpacePointBase*>::const_iterator spIt=arrAmbiLayers[layId].begin();
	  spIt!=arrAmbiLayers[layId].end();++spIt) {

	double dchi2 = KF.getChi2((*spIt));

#ifdef IDSCAN_DEBUG
	printf("          dchi2=%f\n",dchi2);
#endif
	if(dchi2<bestChi2) {
	  bestChi2=dchi2;
	  pBestSP=(*spIt);
	}
      }
      if(pBestSP!=NULL) {
#ifdef IDSCAN_DEBUG
	printf("Best spacepoint with dchi2=%f\n",bestChi2);
#endif
	spUsageMap[pBestSP->index()] = 1;// using the best spacepoint
	chi2+=KF.getChi2(pBestSP);
	KF.update();
	nHits++;
      }
#ifdef IDSCAN_DEBUG
      else 
	printf("All SPs are rejected\n");
#endif
    }
  }

  double theta=atan(1.0/KF.m_Rk[3]);
  if(theta<0.0) theta+=M_PI;
  KF.m_Rk[2]+=KF.m_averagePhi0;
#ifdef IDSCAN_DEBUG        
  printf("Final estimates: d0=%f z0=%f phi0=%f theta=%f pT=%f\n",
	 KF.m_Rk[0],KF.m_Rk[1],KF.m_Rk[2],theta,1.0/KF.m_Rk[4]);
  printf("Chi2 = %f Total %d ambiguities\n",
	 chi2,nAmbiLayers);
#endif
  if((chi2>maxChi2inGroup)||(fabs(1.0/KF.m_Rk[4])<minPt)) {
    pTrack->deleted = true;
#ifdef IDSCAN_DEBUG        
    printf("Large Chi2 or very low pT - the track cand. is rejected\n");
#endif
    return;
  }

  pTrack->m_params.m_pT = 1/KF.m_Rk[4];
  pTrack->m_params.m_chi2 = chi2;
  pTrack->m_params.m_phi0 = KF.m_Rk[2];
  pTrack->m_params.m_z0 = KF.m_Rk[1];
  pTrack->m_params.m_eta = -log(sin(0.5*theta)/cos(0.5*theta));

  for(int i=0;i<MAX_SILICON_LAYER_NUM;i++) {
    if(arrAmbiIdx[i]!=0) {
      arrAmbiLayers[i].clear();
    }
  }
  //deleting spacepoints that have used==0

  if(nAmbiSPs!=0) {

    if(m_removeOutliers) {
      std::vector<const TrigSiSpacePointBase*>& spVec = 
	const_cast<std::vector<const TrigSiSpacePointBase*>&>(pTrack->spacePoints());
      std::vector<const TrigSiSpacePointBase*>::iterator spIt = spVec.begin();
      
      while(spIt!=spVec.end()) {
	
	if(spUsageMap[(*spIt)->index()]==0) {
	  
	  spIt=spVec.erase(spIt);
	 
	}
	else {
	  spIt++;
	}
      }
    }
    else pTrack->m_params.m_chi2 = 1e9;//to mark track with unresolved SPs
  }

#ifdef IDSCAN_DEBUG
  std::cout << "IDSCAN_DEBUG: after ambig. resol. - number of SPs = " << pTrack->spacePoints().size() << std::endl;
  spIt=pTrack->spacePoints().begin();
  for( ; spIt!=pTrack->spacePoints().end(); spIt++) {
    const TrigSiSpacePointBase* pSP=(*spIt);
    std::cout << "IDSCAN_DEBUG: after ambig. resol. - layer = " << pSP->layer()
	      << " phi= " << pSP->phi()
	      << " rho= " << pSP->r()
	      << " zed= " << pSP->z()
	      << std::endl;
  }
#endif

}

