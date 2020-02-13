/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigT1TGC/TGCTrackSelectorOut.h"

namespace LVL1TGCTrigger {

  TGCTrackSelectorOut::TGCTrackSelectorOut()
  {

 
  
    for( int i=0; i<NCandidateInTrackSelector; i+=1){
      m_ptLevel[i]=0;
      m_r[i]=0;
      m_phi[i]=0;
      m_dr[i]=99;
      m_dphi[i]=99;
      m_iVeto[i]=false;
      m_charge[i]=0;
      m_coincidenceTypeFlag[i]=-1;
      m_goodMFFlag[i]=false;
      m_innerCoincidenceFlag[i]=false;
    }
    m_nCandidate=0;
  }



  void TGCTrackSelectorOut::reset()
  {
    for( int i=0; i<NCandidateInTrackSelector; i+=1){
      m_ptLevel[i]=0;
      m_r[i]=0;
      m_phi[i]=0;
      m_dr[i]=99;
      m_dphi[i]=99;
      m_iVeto[i]=false;
      m_charge[i]=0;
      m_coincidenceTypeFlag[i]=-1;
      m_goodMFFlag[i]=false;
      m_innerCoincidenceFlag[i]=false;
    }
    m_nCandidate=0;  
  }



  TGCTrackSelectorOut::~TGCTrackSelectorOut()
  {
  }

  
  void TGCTrackSelectorOut::setPtLevel(int order, int ptLevelIn)
  {
  
    if(order<NCandidateInTrackSelector){
      m_ptLevel[order]=ptLevelIn;
      m_nCandidate++;
    }
  
  }



} //end of namespace bracket
