/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBphysHypo/TrigMultiTrkFex.h
 **
 **
 **   Author:Olya Igonkina (Nikhef)
 **
 **   Created:   15/07/2016
 **   Modified:     
 **
 **************************************************************************/ 

#ifndef TRIG_TrigMultiTrk_H
#define TRIG_TrigMultiTrk_H

// standard stuff
#include <string>
#include <map>
#include <cmath> 
#include <algorithm>

#include "TrigInterfaces/AllTEAlgo.h"
#include "xAODTrigBphys/TrigBphysContainer.h"

class TrigBphysHelperUtilsTool;
class TrigTimer;

class TrigMultiTrkFex: public HLT::AllTEAlgo  {
  
  public:
    TrigMultiTrkFex(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigMultiTrkFex();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();     

    HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTEs, unsigned int output);
    
  private:

  // OI I would have loved to make it template, but templates outsmarted me..
  static bool sortTracks(const ElementLink<xAOD::TrackParticleContainer> &l1, const ElementLink<xAOD::TrackParticleContainer> &l2)
  {
    const xAOD::TrackParticle* xTrack1 = *l1;
    const xAOD::TrackParticle* xTrack2 = *l2;
	
    return xTrack1->pt()>xTrack2->pt();
  }


  template<class Tin, class Tout> bool passNObjects(int nObjMin,
						    const std::vector<float>& ptObjMin,
						    std::vector<std::vector<HLT::TriggerElement*> >& inputTE ,
						    Tout & outVec,
						    //std::string printTag,
						    const std::string&  collectionKey,
						    float mindR) ;

  bool passNTracks(int nObjMin,
		   const std::vector<float>& ptObjMin,
		   std::vector<std::vector<HLT::TriggerElement*> >& inputTE ,
		   std::vector<ElementLink<xAOD::TrackParticleContainer> > & outVec,
		   const std::string&  collectionKey,
		   float mindR) ;


  //void processTriMuon(HLT::TEConstVec& inputTE, xAOD::TrigBphysContainer & outputContainer);
  // void buildDiMu (const std::vector<const xAOD::Muon*> &muons, xAOD::TrigBphysContainer & outputContainer);
  //void buildTriMu(const std::vector<const xAOD::Muon*> &muons, xAOD::TrigBphysContainer & outputContainer);
    
    ToolHandle <TrigBphysHelperUtilsTool> m_bphysHelperTool;
    
    
    TrigTimer* m_BmmHypTot;


  std::string m_trackCollectionKey;
  std::string m_bphysCollectionKey;
  int m_nTrk ;
  int m_nTrkQ; // if negative - no cut
  int m_nTrkVertexChi2; // if negative - no cut
  float              m_trkMass; // track mass hypothesis for all tracks selected
  std::vector<float> m_ptTrkMin;
  std::vector<float> m_nTrkMassMin; // this has to be in pair
  std::vector<float> m_nTrkMassMax;
  std::vector<float> m_diTrkMassMin; // this has to be in pair
  std::vector<float> m_diTrkMassMax;
  int m_diTrkQ; // if negative - no cut
  int m_nEfMuon ; // muons have to be present in TE, but are not required to match to tracks
  int m_nL2CombMuon ;
  int m_nL2SAMuon ;
  std::vector<float> m_ptMuonMin;
  float m_mindR;

  // to set Accept-All mode: should be done with force-accept when possible
  bool m_acceptAll;

    
  //Counters
  uint32_t m_countTotalEvents;
  uint32_t m_countPassedEvents;
  uint32_t m_countPassedCombination;

    
  //Monitored variables 
  std::vector<int>   m_mon_Errors;
  std::vector<int>   m_mon_Acceptance;
  float m_mon_NTrk;
  float m_mon_highptNTrk;
  float m_mon_accepted_highptNTrk;
  std::vector<float> m_mon_NTrkMass;
  std::vector<float> m_mon_NTrkFitMass;
  std::vector<float> m_mon_NTrkChi2;
  float m_mon_NPair;
  float m_mon_acceptedNPair;
  std::vector<float> m_mon_pairMass;
    

};


template<class Tin, class Tout> bool TrigMultiTrkFex::passNObjects(int nObjMin,
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
