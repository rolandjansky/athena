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
#include "ElectronPhotonSelectorTools/IAsgElectronLikelihoodTool.h"
#include "LumiBlockComps/ILumiBlockMuTool.h"
#include "PATCore/TAccept.h"            // for TAccept
#include "PATCore/TResult.h"            // for TResult

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
  std::string                 m_athElectronLHIDSelectorToolName;
  std::string                 m_electronCollectionKey;
  bool                        m_useAthElectronLHIDSelector;
  unsigned int                m_IsEMrequiredBits;  //!< isem flag bits required per electron, matches pt cuts
  bool                        m_applyIsEM;  //!< true if isem flag required 
  ToolHandle<IAsgElectronIsEMSelector> m_egammaElectronCutIDTool;
  ToolHandle<IAsgElectronLikelihoodTool> m_athElectronLHIDSelectorTool;
  /** Luminosity Tool */
  ToolHandle<ILumiBlockMuTool>  m_lumiBlockMuTool; // used by LH selector
  
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
  std::vector<bool> isLHAcceptTrigs;
  
  float mindR2 = mindR*mindR;

  bool useLumiTool=false;
  double mu = 0.;
  double avg_mu = 0.;
  if(m_useAthElectronLHIDSelector && m_lumiBlockMuTool){
      useLumiTool=true;
      mu = m_lumiBlockMuTool->actualInteractionsPerCrossing(); // (retrieve mu for the current BCID)
      avg_mu = m_lumiBlockMuTool->averageInteractionsPerCrossing();
      ATH_MSG_DEBUG("REGTEST: Retrieved Mu Value : " << mu << ", Average Mu Value   : " << avg_mu);
  }

  

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
	    bool isLHAcceptTrig = false;
	    float lhval=0;
	    if( m_applyIsEM ) {
	      if(m_useAthElectronLHIDSelector){
		if( m_athElectronLHIDSelectorTool != 0){
		  if(useLumiTool){
                    const Root::TAccept& acc = m_athElectronLHIDSelectorTool->accept(*efmu,avg_mu);
                    lhval=m_athElectronLHIDSelectorTool->getTResult().getResult(0);
                    ATH_MSG_DEBUG("LHValue with mu " << lhval);
                    isLHAcceptTrig = (bool) (acc);
		  }else {
                    ATH_MSG_DEBUG("Lumi tool returns mu = 0, do not pass mu");
                    const Root::TAccept& acc = m_athElectronLHIDSelectorTool->accept(*efmu);
                    lhval=m_athElectronLHIDSelectorTool->getTResult().getResult(0);
                    ATH_MSG_DEBUG("LHValue without mu " << lhval);
                    isLHAcceptTrig = (bool) (acc);
		  }
		} // end of m_athElectronLHIDSelectorTool != 0
	      }else{
		if( m_egammaElectronCutIDTool != 0 ){
		  if( m_egammaElectronCutIDTool->execute( (*efmu) ).isFailure() )
		    ATH_MSG_DEBUG(" can not calculate isEM on electron " << (*efmu)->pt() );
		  isEMTrig = m_egammaElectronCutIDTool->IsemValue();
		  ATH_MSG_DEBUG(" set isEM Trig " << std::hex << isEMTrig << " on electron " << (*efmu)->pt() );
		}
	      } // end of  m_egammaElectronCutIDTool != 0
	    }else{
	      ATH_MSG_DEBUG(" No check of isEM is done " );
	    }
	  
	    float pt = fabs((*efmu)->pt()) ;
	    // check if value in GeV or MeV, if it was >350 GeV and multiplied by 1000, it does not matter
	    if( pt < 350. && pt>0.01 ) pt *= 1000.;
	    pts.push_back(pt);	    
	    outVec.push_back(efmu);
	    isEMs.push_back(isEMTrig);
	    isLHAcceptTrigs.push_back(isLHAcceptTrig);
	    ATH_MSG_DEBUG("Found electron, pt= " << pt << ", isEMTrig= 0x"<< std::hex << isEMTrig
			  << " (required isEM= 0x"<< std::hex << isEMBitCuts << std::dec<< ")"
			  << ", isLHAccept = " << isLHAcceptTrig );
	    unsigned int isEMbit=0;
	    
	    ATH_MSG_DEBUG("  passing isEMLoose =  " << (*efmu)->selectionisEM(isEMbit,"isEMLoose") << " 0x" << std::hex <<  isEMbit );
	  }
      }//}// end loop over electrons in one TE
    } // end getFeaturesLinks
   }} // end loop over all TEs

    //=== check if it is enough electrons
  if( (int)outVec.size() < nObjMin ) {
    ATH_MSG_DEBUG("Rejecting: " <<" #electrons= " <<  outVec.size() << " while need "<< nObjMin );
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
    if(m_useAthElectronLHIDSelector){
      failElectronIsEM = isLHAcceptTrigs[ipt];
    }else {
      if( (isEMs[ipt] & isEMBitCuts)!=0 ) 
	failElectronIsEM = true;
    }
  }
  if( failElectronPt ){
    ATH_MSG_DEBUG("Fail electron pt cut" );
    return 2;
  }
  if( failElectronIsEM ){
    ATH_MSG_DEBUG("Fail electron PID cut " );
    return 3;
  }
  // here would be good to limit number of objects to the minimum

  return 0;

}


#endif
