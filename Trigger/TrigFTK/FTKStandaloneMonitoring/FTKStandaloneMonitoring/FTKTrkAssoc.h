/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef __FTKTRKASSOC_H__
#define __FTKTRKASSOC_H__
#include "TrigFTK_RawData/FTK_RawTrack.h"
#include "TrigInDetAnalysisUtils/BestMatcher.h"
#include "TMath.h"
typedef BestMatcher<const FTK_RawTrack> TrkMatch;
class FTKTrkAssoc : public TrkMatch { 
    public:
      FTKTrkAssoc(const std::string& name, double d) : 
        TrkMatch(name, d)
    { } 
      virtual ~FTKTrkAssoc() { } 
      double distance( const FTK_RawTrack* t0,  const FTK_RawTrack* t1 ) const {
        double deta = TMath::ASinH(t0->getCotTh())-TMath::ASinH(t1->getCotTh());
        double dphi = t0->getPhi()-t1->getPhi(); 
        if ( dphi> M_PI ) dphi-=2*M_PI; 
        if ( dphi<-M_PI ) dphi+=2*M_PI;
        return std::sqrt( deta*deta + dphi*dphi );
      }
  };
#endif
