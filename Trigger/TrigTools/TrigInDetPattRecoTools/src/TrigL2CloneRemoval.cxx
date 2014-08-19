/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigSiSpacePointBase.h"
#include "TrigInDetPattRecoEvent/TrigL2TrackCandidate.h"
#include "TrigInDetPattRecoTools/TrigL2CloneRemoval.h"

#include <iostream>
#include <cmath>
#include <cstring>
#include <list>
#include <algorithm>
#include <map>

void TrigL2CloneRemoval::removeClones(std::vector<TrigL2TrackCandidate*>& trCandidates) {

  int nTrackCounter=0;
  int nSign,nBinNum;
  char nContribHisto[1000];
  std::map<int,int> spUsageMap;
  spUsageMap.clear();
  std::vector<int> nSharedHits,nSharedTotal;
  for (unsigned int i=0;i<trCandidates.size();i++) {
    nSharedHits.push_back(0); nSharedTotal.push_back(0); }

  std::sort(trCandidates.begin(),trCandidates.end(),CompareFoMs());
  for(std::vector<TrigL2TrackCandidate*>::iterator pTCCIt=trCandidates.begin();pTCCIt!=trCandidates.end();++pTCCIt) {
    
    nTrackCounter++;
    if ( (*pTCCIt)->deleted ) continue;

    //printf("Sorted track params: pT=%f nHits=%d\n",(*pTCCIt)->m_params.m_pT,(int)((*pTCCIt)->spacePoints().size()));

    nSign=((*pTCCIt)->m_params.m_pT<0)?-1:1;
    int nHitShift=nTrackCounter*nSign;
    memset(nContribHisto,0,sizeof(nContribHisto));

    for(std::vector<const TrigSiSpacePointBase*>::const_iterator spIt=(*pTCCIt)->spacePoints().begin();
	spIt!=(*pTCCIt)->spacePoints().end();spIt++) {
      
      const TrigSiSpacePointBase* pSP=(*spIt);
      int spIndex = pSP->index();

      if(spUsageMap.find(spIndex)==spUsageMap.end()) {// not in the map yet
	spUsageMap.insert(std::pair<int,int>(spIndex,nHitShift));
      }
      else {// in the map i.e. used by some other track
	int usedBy = spUsageMap[spIndex];
	if( (((usedBy*nSign)>0) && m_usePtSign) || (!m_usePtSign)) {
	  nBinNum=abs(usedBy);
	  if(nBinNum<1000) {
	    nContribHisto[nBinNum]++;
	    if(nContribHisto[nBinNum]>nSharedHits[nTrackCounter-1])
	      (nSharedHits[nTrackCounter-1])++;
	  }
	}
	(nSharedTotal[nTrackCounter-1])++;
      }
    }
  }
  
  nTrackCounter=0;
  for(std::vector<TrigL2TrackCandidate*>::iterator pTCCIt=trCandidates.begin();pTCCIt!=trCandidates.end();++pTCCIt) {
    nTrackCounter++;
    if ( (*pTCCIt)->deleted ) continue;
    unsigned int nHits = (*pTCCIt)->spacePoints().size();
    if( nSharedHits[nTrackCounter-1] >= m_cloneRemovalCut || nSharedTotal[nTrackCounter-1] > 0.45*nHits )
      (*pTCCIt)->deleted = true;
    else (*pTCCIt)->deleted = false;//confirmed track cand.
  }

}

