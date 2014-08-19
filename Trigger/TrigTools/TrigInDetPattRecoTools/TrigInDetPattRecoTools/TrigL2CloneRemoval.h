/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_CLONE_REMOVAL_H__
#define __TRIG_L2_CLONE_REMOVAL_H__

class TrigL2TrackCandidate;

#include <cmath>
#include <vector>


class TrigL2CloneRemoval {
  
 public:
  
  TrigL2CloneRemoval(bool usePtSign,int cloneRemovalCut) : 
    m_usePtSign(usePtSign), m_cloneRemovalCut(cloneRemovalCut) 
    {};
    
    virtual ~TrigL2CloneRemoval(){};
    
    void removeClones(std::vector<TrigL2TrackCandidate*>& trCandidates);

    struct CompareFoMs {
      bool operator()(const TrigL2TrackCandidate* pLhs, const TrigL2TrackCandidate* pRhs) {
	// figure of merit can change here
	double fomL = (double)(pLhs->spacePoints().size());
	double fomR = (double)(pRhs->spacePoints().size());
	if ( fomL == fomR ) return ( fabs(pLhs->m_params.m_pT) > fabs(pRhs->m_params.m_pT) );
	return (fomL > fomR);
      }
    };

 private:
    
    bool m_usePtSign;
    int m_cloneRemovalCut;
  
};

#endif
