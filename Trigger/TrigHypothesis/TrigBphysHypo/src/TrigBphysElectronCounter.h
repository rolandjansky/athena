/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBphysHypo/TrigBphysElectronCounter.h
 **
 **   this algo just counts number of (EF) electrons in the whole event and rejects if insufficient
 **
 **   Author:Olya Igonkina (Nikhef)
 **
 **   Created:   28/03/2017
 **   Modified:     
 **
 **************************************************************************/ 

#ifndef TRIG_TrigBphysElectronCounter_H
#define TRIG_TrigBphysElectronCounter_H

// standard stuff
#include <string>
#include <map>
#include <cmath> 
#include <algorithm>

#include "TrigInterfaces/AllTEAlgo.h"
#include "ElectronPhotonSelectorTools/IAsgElectronIsEMSelector.h"


class TrigTimer;

class TrigBphysElectronCounter: public HLT::AllTEAlgo  {
  
  public:
    TrigBphysElectronCounter(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigBphysElectronCounter();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();     

    HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTEs, unsigned int output);
    
  private:

 

  template<class Tin, class Tout> int passNObjects(int nObjMin,
						    const std::vector<float>& ptObjMin,
						    unsigned int isEMBitCuts,
						    std::vector<std::vector<HLT::TriggerElement*> >& inputTE ,
						    Tout & outVec,
						    //std::string printTag,
						    const std::string&  collectionKey,
						    float mindR) ;

    
  TrigTimer* m_BmmHypTot;

  int m_nEfElectron ; // electrons have to be present in TE, but are not required to match to tracks
  std::vector<float> m_ptElectronMin;
  float m_mindR;
  // OI: should we check for CB flag?
  std::string                 m_egammaElectronCutIDToolName;
  std::string                 m_electronCollectionKey;
  unsigned int                m_IsEMrequiredBits;  //!< isem flag bits required per electron, matches pt cuts
  bool                        m_applyIsEM;  //!< true if isem flag required 
  ToolHandle<IAsgElectronIsEMSelector> m_egammaElectronCutIDTool;

  //Counters
  uint32_t m_countTotalEvents;
  uint32_t m_countPassedPt;
  uint32_t m_countPassedIsEM;
  uint32_t m_countPassedEvents;

    
  //Monitored variables 
  std::vector<int>   m_mon_Acceptance;
  float              m_mon_nEFElectrons;
  std::vector<unsigned int> m_IsEMRequiredBits;
  std::vector<unsigned int> m_IsEMRequiredBitsAfterCut;
};


template<class Tin, class Tout> int TrigBphysElectronCounter::passNObjects(int nObjMin,
					      const std::vector<float>& ptObjMin,
	 				      unsigned int isEMBitCuts,
					      std::vector<std::vector<HLT::TriggerElement*> >& inputTE ,
					      Tout & outVec,
					      //std::string printTag,
					      const std::string&  collectionKey,
					      float mindR) 
{
  if( nObjMin <= 0 ) return 1;
  ElementLinkVector<Tin> inVecColl;
  outVec.clear();
  std::vector<float> pts;
  std::vector<unsigned int> isEMs;
  
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
	    // calculate isEM
	    unsigned int isEMTrig = 0;
	    if( m_applyIsEM && m_egammaElectronCutIDTool != 0 ){
	      if( m_egammaElectronCutIDTool->execute( (*efmu) ).isFailure() )
		if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << " can not calculate isEM on electron " << (*efmu)->pt() << endmsg;
	      isEMTrig = m_egammaElectronCutIDTool->IsemValue();
	      if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << " set isEM Trig " << std::hex << isEMTrig << " on electron " << (*efmu)->pt() << endmsg;

	    }else{
	      if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << " No check of isEM is done " << endmsg;
	    }
	    
	    float pt = fabs((*efmu)->pt()) ;
	    // check if value in GeV or MeV, if it was >350 GeV and multiplied by 1000, it does not matter
	    if( pt < 350. && pt>0.01 ) pt *= 1000.;
	    pts.push_back(pt);	    
	    outVec.push_back(efmu);
	    isEMs.push_back(isEMTrig);	    
	    if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Found electron, pt= " << pt << ", isEMTrig= 0x"<< std::hex << isEMTrig << " (required isEM= 0x"<< std::hex << isEMBitCuts << std::dec<< ")"<< endmsg;
	    unsigned int isEMbit=0;
	    
	    if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "  passing isEMLoose =  " << (*efmu)->selectionisEM(isEMbit,"isEMLoose") << " 0x" << std::hex <<  isEMbit << endmsg;
	  }
      }//}// end loop over electrons in one TE
    } // end getFeaturesLinks
   }} // end loop over all TEs

    //=== check if it is enough electrons
  if( (int)outVec.size() < nObjMin ) {
    if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Rejecting: "
    					 <<" #electrons= " <<  outVec.size() 
    					 << " while need "<< nObjMin << endmsg;
    return 1;
  }
  //std::sort(isEMs.begin(), isEMs.end(), [&pts](size_t i, size_t j) {return pts[i] > pts[j];});
  std::sort(pts.begin(), pts.end(), std::greater<float>());

  //== check that electrons have correct pts and isEM
  unsigned int Ncheck = std::min( nObjMin, int(ptObjMin.size()) );
  bool failElectronPt = false;
  bool failElectronIsEM = false;
  for ( unsigned int ipt=0; ipt < Ncheck; ++ipt) {
    if(  pts[ipt] < ptObjMin[ipt] ) 
      failElectronPt = true;
    // here we need a logic that allows different isEM cuts for different electrons. Disable it for now
    //if( (isEM[ipt] & m_IsEMrequiredBits[ipt])!=0 ) 
    if( (isEMs[ipt] & isEMBitCuts)!=0 ) 
      failElectronIsEM = true;    
  }
  if( failElectronPt ){
    if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Fail electron pt cut" << endmsg;
    return 2;
  }
  if( failElectronIsEM ){
    if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Fail electron PID cut " << endmsg;
    return 3;
  }
  // here would be good to limit number of objects to the minimum

  return 0;

}


#endif
