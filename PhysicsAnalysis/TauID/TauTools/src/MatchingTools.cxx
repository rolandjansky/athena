/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////////////
/// Name    : MatchingTools.cxx
/// Package : offline/PhysicsAnalysis/TauID/TauTools
/// Author  : Jyothsna Komaragiri, Dugan O'Neil
/// Created : May 2008
///
/// Last modified: Jennifer Godfrey
///
/// DESCRIPTION:
///
/// This class is a simple example for matching reco taus with truth and jets.
/// This class was written to be run on events reconstructed in 14.x.x
///
//////////////////////////////////////////////////////////////////////////////////////

#include "TauTools/MatchingTools.h"

using CLHEP::GeV;

/////////////////////////
// Constructor
MatchingTools::MatchingTools() {

  m_deltaRMatchCut           = 0.2;
  m_LeadingChPiPt_Threshold  = 0.0;
  m_visETcut                 = 10.;
  m_visEtacut                = 2.5;

  /** Default settings for MapTruthToRec method **/
  m_dRThreshold              = 0.04;
  m_minTrackMatchProbability = 0.5;
  m_EnergyFracThreshold      = 0.5;
}

/////////////////////////
MatchingTools::MatchingTools(float deltaRCut, float leadPiCut, float visETcut, float visEtacut){

  setdeltaRMatchCut(deltaRCut);
  setleadChPiCut(leadPiCut);
  setvisETCut(visETcut);
  setvisEtaCut(visEtacut);

}
/////////////////////////
// Destructor
MatchingTools::~MatchingTools() {
}

/////////////////////////
/**Match reco tau with true tau. User is expected to provide hadronic truth tau container as input*/
bool MatchingTools::matchTauTruth(const Analysis::TauJet *tau, 
				  const TruthParticleContainer *hadTau) {
  
  bool isMatched = false;
  float minDR = m_deltaRMatchCut;         // Initialize the min Delta R to the Delta R cut

  TruthParticleContainer::const_iterator truthTauItr  = hadTau->begin();
  TruthParticleContainer::const_iterator truthTauItrE = hadTau->end();
  
  for (; truthTauItr != truthTauItrE; ++truthTauItr) {//truth loop
    // find any taus which match to good truth taus using deltaR cut
    float matchDR = tau->hlv().deltaR( (*truthTauItr)->hlv() );
    if(matchDR <= minDR) {//dRcut
      minDR = matchDR;
      isMatched = true;
    }//dRcut
  }//truth loop

  return isMatched;
}

/////////////////////////
/**Match reco tau with visible true tau, requires recotrack==nprong. Provide the  entire truth container as input*/
bool MatchingTools::matchTruth(const Analysis::TauJet *tau, 
			       const TruthParticleContainer *mcpart) {
  
  bool isMatched  = false;
  float minDR    = m_deltaRMatchCut;         // Initialize the min Delta R to the Delta R cut
  float visETcut = m_visETcut;
  float visEtacut = m_visEtacut;

  // Hadronic truth tau container
  const TruthParticleContainer* hadTau = 0;
  
  /** retrieve a container of hadronic taus and define iterators*/
  hadTau = truthHandler.getHadronicTruth(mcpart);
  
  TruthParticleContainer::const_iterator truthTauItr  = hadTau->begin();
  TruthParticleContainer::const_iterator truthTauItrE = hadTau->end();
  
  for (; truthTauItr != truthTauItrE; ++truthTauItr) {//truth loop

    CLHEP::HepLorentzVector visSum = truthHandler.getTauVisibleSumTruth(*truthTauItr);

    if ((visSum.et()/GeV < visETcut) || (fabs(visSum.eta()) > visEtacut)) continue;
    
    int nPiCh = truthHandler.nProngTruth((*truthTauItr));
    
    if ((this->recoHandler.TauType(tau) == TAU_1P3P) || (this->recoHandler.TauType(tau) == TAU_BOTH)) {      
      CLHEP::HepLorentzVector leadPiCh = truthHandler.getPiChTruth(*truthTauItr);
      if( leadPiCh.et()/GeV < m_LeadingChPiPt_Threshold ) continue; // cut on the pt of the leading track
    }
    
    if ( int(tau->numTrack()) != nPiCh) continue;

    // find any taus which match to good truth taus using deltaR cut
    // match reco tau with visible tau vector
    float matchDR = visSum.deltaR( tau->hlv() );
    
    if(matchDR <= minDR) {//dRcut
      minDR = matchDR;
      isMatched = true;
    }//dRcut
    
  }//truth loop

  // cleaning up
  if (hadTau != 0)
    delete hadTau;

  return isMatched;

}

 /**Match reco tau with visible true tau. Provide one truth particle as input**/
bool MatchingTools::matchTruthGeomOnly(const Analysis::TauJet *tau, 
				       const TruthParticle *mcpart) {
  float minDR    = m_deltaRMatchCut;         // Initialize the min Delta R to the Delta R cut
  float visETcut = m_visETcut;
  float visEtacut = m_visEtacut;
  CLHEP::HepLorentzVector visSum = truthHandler.getTauVisibleSumTruth(mcpart);
  
  if ((visSum.et()/GeV < visETcut) || (fabs(visSum.eta()) > visEtacut)) return false;
  float matchDR = visSum.deltaR( tau->hlv() );
  if(matchDR > minDR) return false;

  return true;

}

/////////////////////////
/**Match reco tau with visible true tau. Provide the  entire truth container as input*/
bool MatchingTools::matchTruthGeomOnly(const Analysis::TauJet *tau, 
				       const TruthParticleContainer *mcpart) {
  
  bool isMatched  = false;
 
  // Hadronic truth tau container
  const TruthParticleContainer* hadTau = 0;

  /** retrieve a container of hadronic taus and define iterators*/
  hadTau = truthHandler.getHadronicTruth(mcpart);

  TruthParticleContainer::const_iterator truthTauItr  = hadTau->begin();
  TruthParticleContainer::const_iterator truthTauItrE = hadTau->end();
  
  for (; truthTauItr != truthTauItrE; ++truthTauItr) {//truth loop
    if(!matchTruthGeomOnly(tau, *truthTauItr)) continue;
      isMatched = true;

     }//truth loop

  // cleaning up
  if (hadTau != 0)
    delete hadTau;

  return isMatched;
}

////////////////////////////
/** Match reco tau to user-defined TruthParticleContainer.  
    Use TruthHandler::getHadronicTruth or TruthHandler::getAllFinalTaus to get a container of taus. **/
const TruthParticle* MatchingTools::matchSelectedTruthGeomOnly(const Analysis::TauJet *tau, 
							       const TruthParticleContainer *TrueTaus) {
  const TruthParticle* EmptyTruth=0;
  TruthParticleContainer::const_iterator truthTauItr  = TrueTaus->begin();
  TruthParticleContainer::const_iterator truthTauItrE = TrueTaus->end();
  for (; truthTauItr != truthTauItrE; ++truthTauItr) {//truth loop
    if(!matchTruthGeomOnly(tau, *truthTauItr)) continue;
    return *truthTauItr;
  }

  return EmptyTruth;
}
/////////////////////////
/**Match true tau with reco tau.*/
const Analysis::TauJet* MatchingTools::matchTruthGeomOnly(const TruthParticle *mc, 
							  const Analysis::TauJetContainer *tau, 
							  bool &isMatched) {
  
  isMatched = false;
  const Analysis::TauJet* chosen = NULL;   // To be returned
  
  float minDR = m_deltaRMatchCut;
  float visETcut = m_visETcut;
  float visEtacut = m_visEtacut;
  
  CLHEP::HepLorentzVector visSum = truthHandler.getTauVisibleSumTruth(mc);
  
  if ((visSum.et()/GeV > visETcut) && (fabs(visSum.eta()) < visEtacut)) {//apply visET and visETA cuts
    
    /** define iterators*/
    Analysis::TauJetContainer::const_iterator tauItr  = tau->begin();
    Analysis::TauJetContainer::const_iterator tauItrE = tau->end();
    
    for (; tauItr != tauItrE; ++tauItr) {//reco loop
      
      // find any truth taus which match to reco taus using deltaR cut
      float matchDR = visSum.deltaR( (*tauItr)->hlv() );
      if(matchDR <= minDR) {//dRcut
	minDR = matchDR;
	isMatched = true;
	chosen = *tauItr;
      }//dRcut
      
    }//reco loop
  }//apply visET and visETA cuts

  return chosen;
  
}

/////////////////////////
/**Match true tau with reco tau.*/
const TruthParticle* MatchingTools::matchTruthGeomOnly(const Analysis::TauJet *tau, 
						       const TruthParticleContainer *mcpart, 
						       bool &isMatched) {

  isMatched = false;

  float minDR    = m_deltaRMatchCut;         // Initialize the min Delta R to the Delta R cut
  float visETcut = m_visETcut;
  float visEtacut = m_visEtacut;

  const TruthParticle* chosen = NULL;   // To be returned
  
  // Hadronic truth tau container
  const TruthParticleContainer* hadTau = 0;

  /** retrieve a container of hadronic taus and define iterators*/
  hadTau = truthHandler.getHadronicTruth(mcpart);
  
  TruthParticleContainer::const_iterator truthTauItr  = hadTau->begin();
  TruthParticleContainer::const_iterator truthTauItrE = hadTau->end();
  
  for (; truthTauItr != truthTauItrE; ++truthTauItr) {//truth loop
    
    CLHEP::HepLorentzVector visSum = truthHandler.getTauVisibleSumTruth(*truthTauItr);
    
    if ((visSum.et()/GeV < visETcut) || (fabs(visSum.eta()) > visEtacut)) continue;    
    
    // find any taus which match to good truth taus using deltaR cut
    //match reco tau with visible tau vector
    float matchDR = visSum.deltaR( tau->hlv() );
    
    if(matchDR <= minDR) {//dRcut
      minDR = matchDR;
      isMatched = true;
      chosen = *truthTauItr;
    }//dRcut
    
  }//truth loop
  
  // cleaning up
  if (hadTau != 0)
    delete hadTau;
  
  return chosen;
  
}



/////////////////////////
/////////////////////////
/** Match truth/reco jet with reco tau candidates, these reco taus are unmatched to true hadronic taus */
bool MatchingTools::matchJet(const Jet *pjet, 
			     const Analysis::TauJetContainer *tau, 
			     const TruthParticleContainer *mcpart) {

  bool isMatched = false;
  float minDR = m_deltaRMatchCut;         // Initialize the min Delta R to the Delta R cut
  
  /** define iterators*/
  Analysis::TauJetContainer::const_iterator tauItr  = tau->begin();
  Analysis::TauJetContainer::const_iterator tauItrE = tau->end();
  
  for (; tauItr != tauItrE; ++tauItr) {//reco loop
    
    if ( matchTruth(*tauItr, mcpart) ) continue;
    
    float matchDR = pjet->hlv().deltaR( (*tauItr)->hlv() );
    
    if(matchDR <= minDR) {//dRcut
      minDR = matchDR;      
      isMatched = true;
    }//dRcut
  }//reco loop
  
  return isMatched;
  
}

/////////////////////////
/** Match reco tau candidates with truth/reco jets, these reco taus are unmatched to true hadronic taus */
bool MatchingTools::matchJet(const Analysis::TauJet *tau, 
			     const JetCollection *jet, 
			     const TruthParticleContainer *mcpart) {

  bool isMatched = false;
  float minDR = m_deltaRMatchCut;

  //exclude the reco taus matched to visible true taus
  if ( !matchTruth(tau, mcpart) ) {
    
    /** define iterators*/
    JetCollection::const_iterator jetItr  = jet->begin();
    JetCollection::const_iterator jetItrE = jet->end();
    
    for (; jetItr != jetItrE; ++jetItr) {//jet loop
      
      float matchDR = tau->hlv().deltaR( (*jetItr)->hlv() );
      
      if(matchDR <= minDR) {//dRcut
	minDR = matchDR;      
	isMatched = true;
      }//dRcut
    }//jet loop
  }
  return isMatched;
  
}

/////////////////////////
/** Match reco tau candidates with truth/reco jets */
bool MatchingTools::matchJet(const Analysis::TauJet *tau, 
			     const JetCollection *jet) {
  
  bool isMatched = false;
  float minDR = m_deltaRMatchCut;
  
  /** define iterators*/
  JetCollection::const_iterator jetItr  = jet->begin();
  JetCollection::const_iterator jetItrE = jet->end();
  
  for (; jetItr != jetItrE; ++jetItr) {//jet loop
    
    float matchDR = tau->hlv().deltaR( (*jetItr)->hlv() );
    
    if(matchDR <= minDR) {//dRcut
      minDR = matchDR;      
      isMatched = true;
      }//dRcut
  }//jet loop
  
  return isMatched;
  
}

/////////////////////////

const MatchingTools::TauJetTruthMap MatchingTools::MapTruthToReco(const Analysis::TauJetContainer *tauJetCont, 
								  const TruthParticleContainer *truthTauCont, 
								  int MatchResolverFlag,  
								  const Rec::TrackParticleContainer* trackPartCont, 
								  const TrackParticleTruthCollection* trackTruthParticles){
  
  m_trackPartContainer= trackPartCont;
  m_trackTruthParticles=trackTruthParticles;
  m_resolveFlag = MatchResolverFlag;
  float visETcut = m_visETcut;
  float visEtacut = m_visEtacut;
  
  // create output map:
  TauJetTruthMap candTruthMap;// = new TauJetTruthMap();
  
  TruthParticleContainer::const_iterator mcpartItr  = truthTauCont->begin();
  TruthParticleContainer::const_iterator mcpartItrE = truthTauCont->end();
  
  //loop over MC particles
  for (; mcpartItr != mcpartItrE; ++mcpartItr) {
    CLHEP::HepLorentzVector visSum = truthHandler.getTauVisibleSumTruth(*mcpartItr);
    if ((visSum.et()/GeV < visETcut) || (fabs(visSum.eta()) > visEtacut))   continue;
    
    const Analysis::TauJet* matchedCand = NULL;
    const TruthParticle* TrueTau = *mcpartItr;
    matchedCand = GetClosestMatch((TrueTau), (tauJetCont), candTruthMap);
    candTruthMap.insert(std::make_pair(TrueTau, matchedCand));
  } // end for (candItr)
  
  
  return candTruthMap;
}

/////////////////////////
const TruthParticle* MatchingTools::isUniqueMatch(const Analysis::TauJet *MatchedRecoTau, 
						  bool &isUnique, 
						  MatchingTools::TauJetTruthMap  MatchMap){
  MatchingTools::RecoMatchItr Itr;
  TruthParticle* emptyPart=NULL;
  
  for(Itr=MatchMap.begin(); Itr!=MatchMap.end(); Itr++){
    if(Itr->second == MatchedRecoTau){ 
      isUnique=false;
      return Itr->first;
    }
  }
  isUnique=true;
  return emptyPart;
  
}

/////////////////////////
const Analysis::TauJet* MatchingTools::GetClosestMatch(const TruthParticle* TrueTau, 
                               const Analysis::TauJetContainer* RecoContainer, 
                               MatchingTools::TauJetTruthMap&  MatchMap){
  
  float minDRcut    = m_deltaRMatchCut;         // Initialize the min Delta R to the Delta R cut  
  float  minDr = 999;
  float DrDiffThreshold = m_dRThreshold;
  std::vector<const Analysis::TauJet*> MatchedRecoTauVector;
  MatchedRecoTauVector.clear();
  const Analysis::TauJet* MatchedRecoTau = NULL;
  const TruthParticle* BestTrueMatch = NULL;
  Analysis::TauJetContainer::const_iterator tauItr  = RecoContainer->begin();
  Analysis::TauJetContainer::const_iterator tauItrE = RecoContainer->end();
  bool isUnique = true;          
  const TruthParticle* Duplicate=NULL;
  for (; tauItr != tauItrE; ++tauItr) {//reco loop
    double currentDr = VisdRSeparation(*tauItr,TrueTau);
    
    if( (currentDr > minDRcut) ) continue;
    if( currentDr < minDRcut && ( currentDr - minDr) > DrDiffThreshold )  continue;
    
    Duplicate = isUniqueMatch(*tauItr, isUnique, MatchMap);   
    if(!isUnique){
      const TruthParticle* BestTrueMatch= BestTruthMatch(*tauItr, TrueTau, Duplicate);
      if(  BestTrueMatch!= TrueTau){
	isUnique=true; 
	continue;
      }
    }
    
    MatchedRecoTauVector.push_back(*tauItr);
    if(MatchedRecoTau != NULL) MatchedRecoTau =  BestRecoTau( *tauItr, MatchedRecoTau, TrueTau);
    else  MatchedRecoTau = *tauItr;
    
    minDr = VisdRSeparation(MatchedRecoTau, TrueTau);
  }
  
  if(MatchedRecoTau==NULL) return MatchedRecoTau;
  
  if(!isUnique && BestTrueMatch == TrueTau){
    MatchMap.insert(std::make_pair(TrueTau, MatchedRecoTau));    
    MatchMap.erase(Duplicate);
    const Analysis::TauJet *NewMatched = GetClosestMatch(Duplicate, RecoContainer, MatchMap);
    MatchMap.insert(std::make_pair(Duplicate, NewMatched));    
    
  }  
  
  return  MatchedRecoTau;
}


/////////////////////////
double MatchingTools::VisdRSeparation(const Analysis::TauJet *RecoTau, 
				      const TruthParticle* TrueTau){
  if(RecoTau==NULL || TrueTau==NULL){
    std::cout<<"ERROR: tau is a null pointer"<<std::endl;
    return 0;
  }
  CLHEP::HepLorentzVector visSum = truthHandler.getTauVisibleSumTruth(TrueTau);
  return visSum.deltaR( RecoTau->hlv() );
}


/////////////////////////
bool MatchingTools::isSmallerdR(const Analysis::TauJet *MatchedRecoTau, const TruthParticle* CurrentTruth,const  TruthParticle* MappedTruth){
  if(MatchedRecoTau==NULL){
    std::cout<<"ERROR: Reco tau is NULL"<<std::endl;
    return false;
  }  
  
  if(VisdRSeparation(MatchedRecoTau, CurrentTruth) < VisdRSeparation(MatchedRecoTau, MappedTruth)) return true;
  return false;
}

/////////////////////////
const TruthParticle* MatchingTools::BestTruthMatch(const Analysis::TauJet *MatchedRecoTau, 
						   const TruthParticle* CurrentTruth, 
						   const  TruthParticle* MappedTruth){
  
  int ResolveFlag = m_resolveFlag;
  const TruthParticle* BestMatch = NULL;
  if( ResolveFlag == 1){
    if(isSmallerdR( MatchedRecoTau, CurrentTruth, MappedTruth)) return CurrentTruth;
    else return MappedTruth;
  }

  else if( ResolveFlag == 2 ){ //Energy only
    BestMatch = BestbyEnergy(MatchedRecoTau, CurrentTruth, MappedTruth);
    if(BestMatch != NULL) return BestMatch;
    if(isSmallerdR( MatchedRecoTau, CurrentTruth, MappedTruth)) return CurrentTruth;
    else return MappedTruth;
  }

  else if( ResolveFlag == 3 ){ // Track only
    if(m_trackTruthParticles==0){
      std::cout <<"ERROR: Rec::TrackParticleContainer not valid. Must set to do track matching!"<<endreq;
      return 0;    
    }
    if( m_trackPartContainer==0){
      std::cout <<"TrackParticleTruthCollection not valid. Must set to do track matching!"<<endreq;
      return 0;    
    }
    int CurrentTruthMatches = NumRecTrueTrackMatch(MatchedRecoTau, CurrentTruth);
    int MappedTruthMatches = NumRecTrueTrackMatch(MatchedRecoTau, MappedTruth);
    
    if(CurrentTruthMatches > MappedTruthMatches) return CurrentTruth;
    else if( MappedTruthMatches > CurrentTruthMatches ) return MappedTruth;
    else if(isSmallerdR( MatchedRecoTau, CurrentTruth, MappedTruth)) return CurrentTruth;
    else return MappedTruth;
  }

  else if( ResolveFlag == 4 ){ //Energy then Track
    
    BestMatch = BestbyEnergy(MatchedRecoTau, CurrentTruth, MappedTruth);
    if(BestMatch != NULL) return BestMatch;
    else{
      if(m_trackTruthParticles==0){
	std::cout <<"ERROR: Rec::TrackParticleContainer not valid. Must set to do track matching!"<<endreq;
	return 0;    
      }
      if( m_trackPartContainer==0){
	std::cout <<"ERROR: TrackParticleTruthCollection not valid. Must set to do track matching!"<<endreq;
	return 0;    
      }
      int CurrentTruthMatches = NumRecTrueTrackMatch(MatchedRecoTau, CurrentTruth);
      int MappedTruthMatches = NumRecTrueTrackMatch(MatchedRecoTau, MappedTruth);
      
      if(CurrentTruthMatches > MappedTruthMatches) return CurrentTruth;
      else if( MappedTruthMatches > CurrentTruthMatches ) return MappedTruth;
    }
    if(isSmallerdR( MatchedRecoTau, CurrentTruth, MappedTruth)) return CurrentTruth;
    else return MappedTruth;
  }

  else if( ResolveFlag == 5 ){ // Track then Energy
    if(m_trackTruthParticles==0){
       std::cout <<"ERROR: Rec::TrackParticleContainer not valid. Must set to do track matching!"<<endreq;
       return 0;    
    }
    if( m_trackPartContainer==0){
      std::cout <<"ERROR: TrackParticleTruthCollection not valid. Must set to do track matching!"<<endreq;
      return 0;    
    }
    int CurrentTruthMatches = NumRecTrueTrackMatch(MatchedRecoTau, CurrentTruth);
    int MappedTruthMatches = NumRecTrueTrackMatch(MatchedRecoTau, MappedTruth);
    
    if(CurrentTruthMatches > MappedTruthMatches) return CurrentTruth;
    else if( MappedTruthMatches > CurrentTruthMatches ) return MappedTruth;
    else{
      BestMatch = BestbyEnergy(MatchedRecoTau, CurrentTruth, MappedTruth);
      if(BestMatch != NULL) return BestMatch;
    }
    if(isSmallerdR( MatchedRecoTau, CurrentTruth, MappedTruth)) return CurrentTruth;
    else return MappedTruth;
  }
  else { // Weird Flag
    std::cout <<"ERROR: Matching Flag: "<<ResolveFlag<<" is unknown!"<<endreq;
    return 0;    
  }
  
  
}

/////////////////////////
double MatchingTools::EnergyFraction(const Analysis::TauJet *RecoTau, 
				     const TruthParticle* TruthTau){

  CLHEP::HepLorentzVector visSum = truthHandler.getTauVisibleSumTruth(TruthTau);
  
  double EnergyFrac = RecoTau->et()/visSum.et();
  
  if(EnergyFrac > 1.0) EnergyFrac = 1.0/EnergyFrac;
  return  EnergyFrac;
}

/////////////////////////
const TruthParticle*  MatchingTools::BestbyEnergy(const Analysis::TauJet *MatchedRecoTau, 
						  const TruthParticle* CurrentTruth, 
						  const  TruthParticle* MappedTruth){
  double  EnergyThreshold =  m_EnergyFracThreshold;
  const TruthParticle* BestMatch = NULL;
  
  double CurrentTruthFrac = EnergyFraction(MatchedRecoTau, CurrentTruth);
  double MappedTruthFrac = EnergyFraction(MatchedRecoTau, MappedTruth);
  
  if(CurrentTruthFrac >= EnergyThreshold && MappedTruthFrac < EnergyThreshold ) BestMatch = CurrentTruth;
  else if(MappedTruthFrac >= EnergyThreshold && CurrentTruthFrac < EnergyThreshold ) BestMatch = MappedTruth;
  return BestMatch;
}

/////////////////////////
const Analysis::TauJet* MatchingTools::BestbyEnergy(const Analysis::TauJet *RecoTau1, 
					      const Analysis::TauJet *RecoTau2, 
					      const  TruthParticle* TrueTau){
  double  EnergyThreshold =  m_EnergyFracThreshold;
  const Analysis::TauJet*  BestMatch = NULL;
  
  double Frac1 = EnergyFraction(RecoTau1, TrueTau);
  double Frac2 = EnergyFraction(RecoTau2,TrueTau);
  
  if(Frac1 >= EnergyThreshold && Frac2 < EnergyThreshold ) BestMatch = RecoTau1;
  else if(Frac2 >= EnergyThreshold && Frac1 < EnergyThreshold ) BestMatch = RecoTau2;
  
  return BestMatch;
}


/////////////////////////
// adapted from PhysicsAnalysis/AnalysisCommon/AnalysisExamples/src/JetTagAna.cxx
const HepMC::GenParticle* MatchingTools::getTruth(const Rec::TrackParticle* trackParticle,
						  const TrackParticleTruthCollection* mcpartColl,
						  const Rec::TrackParticleContainer* trackColl) const
{
  //this method returns pointer to truth particle linked to given track
  
  const HepMC::GenParticle* genPart =0;
	
  ElementLink<Rec::TrackParticleContainer> trackPrtlink;
  trackPrtlink.setElement(const_cast<Rec::TrackParticle*>(trackParticle));
  trackPrtlink.setStorableObject(*trackColl);
  
  std::map<Rec::TrackParticleTruthKey,TrackParticleTruth>::const_iterator tempTrackPrtTruthItr = mcpartColl->find(trackPrtlink);
  
  if (tempTrackPrtTruthItr != mcpartColl->end()) {
    const HepMcParticleLink& tempHepMcLink = (*tempTrackPrtTruthItr).second.particleLink();
    if (tempHepMcLink.isValid() && (*tempTrackPrtTruthItr).second.probability() > m_minTrackMatchProbability) {
      long theBarcode = tempHepMcLink.barcode();
      if (theBarcode!=0) genPart = tempHepMcLink.cptr();
    }
  }
  return genPart;
}

int MatchingTools::NumRecTrueTrackMatch(const Analysis::TauJet *RecoTau, 
					const TruthParticle* TrueTau){
  int NmatchTracks = 0;
  const HepMC::GenParticle* TrueTauGen = TrueTau->genParticle();
  
  for (unsigned int trackIndex = 0; trackIndex < RecoTau->numTrack(); trackIndex++){
    
    const Rec::TrackParticle* track = RecoTau->track(trackIndex);
    if (!track)  continue;
    const HepMC::GenParticle* TrueGenDaughter = getTruth(track, m_trackTruthParticles, m_trackPartContainer);
    
    if(TrueGenDaughter==0) continue;
    
    if(truthHandler.isHadTauFinalDaughter(TrueTauGen, TrueGenDaughter)
       || truthHandler.isHadTauIntermDaughter(TrueTauGen, TrueGenDaughter)
       || TrueGenDaughter->barcode() == TrueTauGen->barcode() ){
      
      NmatchTracks++;  
    }    
  }
  
  
  return NmatchTracks;
}

//////////////////////////////////////////
const Analysis::TauJet * MatchingTools::BestRecoTau(const Analysis::TauJet *RecoTau1, 
					      const Analysis::TauJet *RecoTau2, 
					      const  TruthParticle* TrueTau){
  
  int ResolveFlag = m_resolveFlag;
  
  const Analysis::TauJet*  BestMatch = NULL;
  if( ResolveFlag == 1){
    if(VisdRSeparation(RecoTau1, TrueTau) < VisdRSeparation(RecoTau2, TrueTau)) return RecoTau1;
    else return RecoTau2;
  }
  else if( ResolveFlag == 2 ){ //Energy only
    BestMatch = BestbyEnergy(RecoTau1, RecoTau2, TrueTau);
    if(BestMatch != NULL) return BestMatch;
    else if(VisdRSeparation(RecoTau1, TrueTau) < VisdRSeparation(RecoTau2, TrueTau)) return RecoTau1;
    else return RecoTau2;
  }
  else if( ResolveFlag == 3 ){ // Track only
    if(m_trackTruthParticles==0){
      std::cout <<"ERROR: Rec::TrackParticleContainer not valid. Must set to do track matching!"<<endreq;
      return 0;    
    }
    if( m_trackPartContainer==0){
      std::cout <<"ERROR: TrackParticleTruthCollection not valid. Must set to do track matching!"<<endreq;
      return 0;    
    }
    int TrackMatches1 = NumRecTrueTrackMatch(RecoTau1, TrueTau);
    int TrackMatches2 = NumRecTrueTrackMatch(RecoTau2, TrueTau);
    std::cout<<"TrackMatches1 = "<<TrackMatches1<<" TrackMatches2 = "<<TrackMatches2<<std::endl;
    
    if(TrackMatches1 > TrackMatches2) return RecoTau1;
    else if(TrackMatches2 > TrackMatches1 ) return RecoTau2;
    else if(VisdRSeparation(RecoTau1, TrueTau) < VisdRSeparation(RecoTau2, TrueTau)) return RecoTau1;
    else return RecoTau2;
  }
  else if( ResolveFlag == 4 ){ //Energy then Track
    BestMatch = BestbyEnergy(RecoTau1, RecoTau2, TrueTau);
    if(BestMatch != NULL) return BestMatch;
    else{
      if(m_trackTruthParticles==0){
	std::cout <<"ERROR: Rec::TrackParticleContainer not valid. Must set to do track matching!"<<endreq;
	return 0;    
      }
      if( m_trackPartContainer==0){
	std::cout <<"ERROR: TrackParticleTruthCollection not valid. Must set to do track matching!"<<endreq;
	return 0;    
      }
      int TrackMatches1 = NumRecTrueTrackMatch(RecoTau1, TrueTau);
      int TrackMatches2 = NumRecTrueTrackMatch(RecoTau2, TrueTau);
      
      if(TrackMatches1 > TrackMatches2) return RecoTau1;
      else if(TrackMatches2 > TrackMatches1 ) return RecoTau2;
    }
    
    if(VisdRSeparation(RecoTau1, TrueTau) < VisdRSeparation(RecoTau2, TrueTau)) return RecoTau1;
    else return RecoTau2;
    
  }
  else if( ResolveFlag == 5 ){ // Track then Energy
    if(m_trackTruthParticles==0){
      std::cout <<"ERROR: Rec::TrackParticleContainer not valid. Must set to do track matching!"<<endreq;
      return 0;    
    }
    if( m_trackPartContainer==0){
      std::cout <<"ERROR: TrackParticleTruthCollection not valid. Must set to do track matching!"<<endreq;
      return 0;    
    }
    int TrackMatches1 = NumRecTrueTrackMatch(RecoTau1, TrueTau);
    int TrackMatches2 = NumRecTrueTrackMatch(RecoTau2, TrueTau);
    
    if(TrackMatches1 > TrackMatches2) return RecoTau1;
    else if(TrackMatches2 > TrackMatches1 ) return RecoTau2;
    BestMatch = BestbyEnergy(RecoTau1, RecoTau2, TrueTau);
    if(BestMatch != NULL) return BestMatch;
    
    else if(VisdRSeparation(RecoTau1, TrueTau) < VisdRSeparation(RecoTau2, TrueTau)) return RecoTau1;
    else return RecoTau2;
  }
  else { // Weird Flag
    std::cout <<"ERROR: Matching Flag: "<<ResolveFlag<<" is unknown!"<<endreq;
    return 0;    
  }
  
  
}
/////////////////////////
