/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigSiSpacePointBase.h"
#include "TrigInDetPattRecoEvent/TrigL2TrackCandidate.h"
#include "TrigInDetPattRecoEvent/TrigL2SiLayerRoad.h"
#include "TrigInDetPattRecoEvent/TrigL2SpacePointStorage.h"
#include "TrigInDetPattRecoEvent/TrigL2TimeoutException.h"
#include "TrigInDetPattRecoTools/TrigL2SiTrackSeedingCuts.h"
#include "AthenaKernel/Timeout.h"
#include "TrigInDetPattRecoTools/TrigL2SiTrackSeedMaker.h"


#include<algorithm>

//#define SITRACK_DEBUG 1


void TrigL2SiTrackSeedMaker::operator()(const TrigL2SiLayerRoad& r) throw(TrigL2TimeoutException) {

  unsigned int limit = m_cuts.m_seedMakingLimit;

  if(m_trks_num>=limit) {
#ifdef SITRACK_DEBUG
    std::cout<<"Stop road processing -> too many seeds already"<<std::endl;
#endif
    return;
  }

  if(m_spLay[r.lay1].size()==0 || m_spLay[r.lay2].size()==0) return;

  float cutd02, ptMin, cuteta, mindzdr, maxdzdr;

  cutd02 = m_cuts.m_recoD0Max+m_cuts.m_recoD0MaxSeedDiff;
  cutd02 = cutd02*cutd02;
  ptMin = m_cuts.m_recoPtMin+m_cuts.m_recoPtMinSeedDiff;
 
  if(ptMin<1.0) 
    ptMin=1.0;
  cuteta = 0.0325 + 490.0/ptMin; 
  if(cuteta<m_cuts.m_etaHalfWidth) 
    cuteta=m_cuts.m_etaHalfWidth; 
  if(cuteta>10)
    cuteta=10;
  mindzdr = exp(-(m_cuts.m_roiEta-cuteta));
  maxdzdr = exp(-(m_cuts.m_roiEta+cuteta));
  mindzdr = 0.5*(1/mindzdr - mindzdr);
  maxdzdr = 0.5*(1/maxdzdr - maxdzdr);
  
  TrigL2TrackCandidate* pSeed=&m_trks[m_trks_num];
  unsigned int nNewSeeds=0;
  unsigned int nTotal=m_trks_num;

  const std::vector<const TrigSiSpacePointBase*>& layer1 = m_spLay[r.lay1];
  const std::vector<const TrigSiSpacePointBase*>& layer2 = m_spLay[r.lay2];

  for(std::vector<const TrigSiSpacePointBase*>::const_iterator it = layer1.begin(); it!=layer1.end(); ++it) {

    // Check if event timeout was reached
    if (Athena::Timeout::instance().reached()) {
      throw TrigL2TimeoutException(1);// 1 - SeedMaker
    }
    
    const TrigSiSpacePointBase* s1 = (*it);

    for(std::vector<const TrigSiSpacePointBase*>::const_iterator spIt = layer2.begin(); spIt!=layer2.end(); ++spIt) {
	  
      const TrigSiSpacePointBase* s2 = (*spIt);
      
      float dr = s2->r()-s1->r();
      if(dr<=0.0) continue;

      float dz=s2->z()-s1->z();
      if((dz<mindzdr*dr) || (dz>maxdzdr*dr)) continue;

      float dphi = s2->phi()-s1->phi();
      if(dphi>M_PI) 
	dphi=dphi-2*M_PI;  
      if(dphi<-M_PI) 
	dphi=dphi+2*M_PI;
      
      if(fabs(dphi)>m_cuts.m_dynamicalSeedingPreCutRPhi*dr) continue;

      float z0 = s1->z() - dz*s1->r()/dr;
  
      if(fabs(z0)>m_cuts.m_vertexingCutRZ) continue;

      float dx=s1->x()-s2->x();
      float dy=s2->y()-s1->y();
      float d02 = dx*s1->y()+dy*s1->x();
      d02=d02*d02;
	  
      if(d02 > cutd02*(dy*dy+dx*dx)) continue;
    
      pSeed->deleted   = false;
      pSeed->m_params.m_z0 = z0;	  
      pSeed->seed_sp1() = s1;
      pSeed->seed_sp2() = s2;
	
      // Increase counters
      nNewSeeds++;
      nTotal++;
      pSeed = pSeed+1;;

      if(nTotal>=limit) break;
      
    }
    if(nTotal>=limit) {
#ifdef SITRACK_DEBUG
      std::cout<<"Nseeds="<<nTotal<<" cut="<<m_cuts.m_seedMakingLimit<<std::endl;
      std::cout<<"Seed limit reached"<<std::endl;
#endif
      break;
    }
  }
  m_trks_num+=nNewSeeds;
}

