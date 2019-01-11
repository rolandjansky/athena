/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBphysHypo/TrigBphysMuonCounter.h
 **
 **   this algo just counts number of (EF) muons in the whole event and rejects if insufficient
 **
 **   Author:Olya Igonkina (Nikhef)
 **
 **   Created:   28/03/2017
 **   Modified:     
 **
 **************************************************************************/ 

#ifndef TRIG_TrigBphysMuonCounter_H
#define TRIG_TrigBphysMuonCounter_H

// standard stuff
#include <string>
#include <map>
#include <cmath> 
#include <algorithm>

#include "TrigInterfaces/AllTEAlgo.h"

class TrigTimer;

class TrigBphysMuonCounter: public HLT::AllTEAlgo  {
  
  public:
    TrigBphysMuonCounter(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigBphysMuonCounter();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();     

    HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTEs, unsigned int output);
    
  private:

 

  template<class Tin, class Tout> bool passNObjects(int nObjMin,
						    const std::vector<float>& ptObjMin,
						    std::vector<std::vector<HLT::TriggerElement*> >& inputTE ,
						    Tout & outVec,
						    //std::string printTag,
						    const std::string&  collectionKey,
						    float mindR) ;

    
  TrigTimer* m_BmmHypTot;

  int m_nEfMuon ; // muons have to be present in TE, but are not required to match to tracks
  std::vector<float> m_ptMuonMin;
  float m_mindR;
  // OI: should we check for CB flag?
  std::string m_muonCollectionKey;
  
  //Counters
  uint32_t m_countTotalEvents;
  uint32_t m_countPassedEvents;

    
  //Monitored variables 
  std::vector<int>   m_mon_Acceptance;
  float              m_mon_nEFMuons;
};


template<class Tin, class Tout> bool TrigBphysMuonCounter::passNObjects(int nObjMin,
					      const std::vector<float>& ptObjMin,
					      std::vector<std::vector<HLT::TriggerElement*> >& inputTE ,
					      Tout & outVec,
					      //std::string printTag,
					      const std::string&  collectionKey,
					      float mindR) 
{
  if( nObjMin <= 0 ) return true;
  ElementLinkVector<Tin> inVecColl;
  outVec.clear();
  std::vector<float> pts;
  
  float mindR2 = mindR*mindR;

  unsigned int nTEs = inputTE.size();
  for ( unsigned int i=0; i < nTEs; ++i) {
   unsigned int mTEs = inputTE[i].size();
   for ( unsigned int j=0; j < mTEs; ++j) {
    if(getFeaturesLinks<Tin,Tin>(inputTE[i][j], inVecColl, collectionKey)==HLT::OK ) {
      for( const auto& efmu : inVecColl){	
	  // check for overlap
	  bool found = false;
	  for(const auto& part : outVec ){
	    double deta = (*part)->eta() - (*efmu)->eta();
	    double dphi = (*part)->phi() - (*efmu)->phi();
	    double deltaR2 = deta*deta +dphi*dphi;
	    if( deltaR2 <= mindR2) found = true;
	  }
	  if( !found ){ 
	    float pt = fabs((*efmu)->pt()) ;
	    // check if value in GeV or MeV, if it was >350 GeV and multiplied by 1000, it does not matter
	    if( pt < 350. && pt>0.01 ) pt *= 1000.;
	    pts.push_back(pt);	    
	    outVec.push_back(efmu); 
	    if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Found muon, pt= " << pt << endmsg;
	  }
      }//}// end loop over muons in one TE
    } // end getFeaturesLinks
   }} // end loop over all TEs

    //=== check if it is enough muons
  if( (int)outVec.size() < nObjMin ) {
    if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Rejecting: "
    					 <<" #muons= " <<  outVec.size() 
    					 << " while need "<< nObjMin << endmsg;
    return false;
  }
  //== check that muons have correct pts
  std::sort(pts.begin(), pts.end(), std::greater<float>());

  unsigned int Ncheck = std::min( nObjMin, int(ptObjMin.size()) );
  bool failMuonPt = false;
  for ( unsigned int ipt=0; ipt < Ncheck; ++ipt) {
    if(  pts[ipt] < ptObjMin[ipt] ) 
      failMuonPt = true;	  
  }
  if( failMuonPt ){
    if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Fail muon pt cut" << endmsg;
    return false;
  }
  // here would be good to limit number of objects to the minimum

  return true;

}


#endif
