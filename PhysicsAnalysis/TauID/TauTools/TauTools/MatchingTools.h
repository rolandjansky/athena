/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MATCHTOOLS_H
#define MATCHTOOLS_H

//////////////////////////////////////////////////////////////////////////////////////
/// Name    : MatchingTools.h
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

#include "TauTools/RecoHandler.h"  
#include "TauTools/TruthHandler.h"

#include "JetEvent/JetCollection.h"
#include "ParticleTruth/TrackParticleTruth.h"
#include "ParticleTruth/TrackParticleTruthKey.h"
#include "ParticleTruth/TrackParticleTruthCollection.h"

class MatchingTools { 
  
public:
  
  MatchingTools();
  MatchingTools(float deltaRCut, float leadPiCut, float visETcut = 10., float visEtacut = 2.5);
  virtual ~MatchingTools();

  typedef std::map<const TruthParticle*, const Analysis::TauJet*> TauJetTruthMap;
  typedef std::map< const TruthParticle*, const Analysis::TauJet *>::iterator RecoMatchItr;

  ///////////Match reco tau with true tau
  /**Match reco tau with true tau. User is expected to provide hadronic truth tau container as input*/
  bool matchTauTruth(const Analysis::TauJet *tau, const TruthParticleContainer *hadTau);

  /**Match reco tau with visible true tau, requires recotrack==nprong. Provide the  entire truth container as input*/
  bool matchTruth(const Analysis::TauJet *tau, const TruthParticleContainer *mcpart);

  /**Match reco tau with visible true tau. Provide the  entire truth container as input*/
  bool matchTruthGeomOnly(const Analysis::TauJet *tau, const TruthParticleContainer *mcpart);
/**Match reco tau with visible true tau. Provide one truth particle as input**/
  bool matchTruthGeomOnly(const Analysis::TauJet *tau, 
				       const TruthParticle *mcpart);
  ///////////Match true tau with reco tau....useful for Efficiency methods
  /**Match true tau with reco tau.*/
  const Analysis::TauJet* matchTruthGeomOnly(const TruthParticle *mc, const Analysis::TauJetContainer *tau, bool &isMatched);

  ///////////Match true tau with reco tau....useful for charge mis-id
  /**Match true tau with reco tau.*/
  const TruthParticle* matchTruthGeomOnly(const Analysis::TauJet *tau, const TruthParticleContainer *mcpart, bool &isMatched);

/** Match reco tau to user-defined TruthParticleContainer.  
    Use TruthHandler::getHadronicTruth or TruthHandler::getAllFinalTaus to get a container of taus. **/
const TruthParticle* matchSelectedTruthGeomOnly(const Analysis::TauJet *tau, 
						const TruthParticleContainer *TrueTaus);
  /** Returns a map in which every object in  const TruthParticleContainer *TrueHadTaus is a key. Associated reconstructed matches
      are returned as a value for that key in the map.  Truth particles without a match will have a NULL associated value. User is
      expected to only provide truth particles of interest.  Generally this means using 
      TruthHandler::getHadronicTruth(const TruthParticleContainer *mcpartTES) to obtain the true hadronic taus from the event.  
      However, all types of truth particles are valid.  The user could provide, for example, a container of true electrons.  Providing
      the entire TruthParticleContainer for the event is not advised for memory reasons.

       MatchResolverFlag: Controls how the method MapTruthToReco handles cases in which multiple matches are possible. 
       Use the input parameter "int MatchResolverFlag" to MapTruthToReco in order to set the flag.  This variable is set internally. 
       Flags:

       MatchResolverFlag = 1 -- Resolved by taking closest dR match
       MatchResolverFlag = 2 -- Resolved by checking whether either match would fail loose energy requirements, 
                                then defaults to dR match if Energy does not resolve it.
       MatchResolverFlag = 3 -- Resolved by comparing the number of reconstructed tracks matched to true daughters.  
                                If same for both possible matches, choice defaults to closest dR match.

       MatchResolverFlag = 4 -- Resolved by Energy requirements, if not resolved then by track matching comparisons, 
                                and if not resolved then takes closest dR.

       MatchResolverFlag = 5 -- Resolved by  track matching comparisons, if not resolved then by Energy requirements, 
                                and if not resolved then takes closest dR.

    If tracking is used to resolve conflicts (Flags 3,4, and 5), then two additional containers must be provided:  
    const Rec::TrackParticleContainer* trackPartCont and const TrackParticleTruthCollection* trackTruthParticles.
  **/
  const TauJetTruthMap MapTruthToReco(const Analysis::TauJetContainer *RecoTaus, 
				      const TruthParticleContainer *TrueHadTaus, 
				      int MatchResolverFlag=1, 
				      const Rec::TrackParticleContainer* trackPartCont=0, 
				      const TrackParticleTruthCollection* trackTruthParticles=0);
  
  /** Returns the dR separation between reco and true tau. Uses visible hlv of the true particle. **/
  double VisdRSeparation(const Analysis::TauJet *RecoTau, const TruthParticle* TrueTau); 

 ///////////Match reco tau with truth/reco jet
  /** Match truth/reco jet with reco tau candidates, these reco taus are unmatched to true hadronic taus */
  bool matchJet(const Jet *pjet, const Analysis::TauJetContainer *tau, const TruthParticleContainer *mcpart);

  /** Match reco tau candidates with truth/reco jets, these reco taus are unmatched to true hadronic taus */   
  bool matchJet(const Analysis::TauJet *Rtau, const JetCollection *Rjet, const TruthParticleContainer *mcpart);

  /** Match reco tau candidates with truth/reco jets */
  bool matchJet(const Analysis::TauJet *Rtau, const JetCollection *Rjet);

  /** Setters and Getters for deltaR match */
  float getdeltaRMatchCut() { return m_deltaRMatchCut; }
  void  setdeltaRMatchCut( float p1 ) { m_deltaRMatchCut = p1; }

  /** Setters and Getters for pT cut on lead chargedPi */
  float getleadChPiCut() { return m_LeadingChPiPt_Threshold; }
  void  setleadChPiCut( float p2 ) { m_LeadingChPiPt_Threshold = p2; }

  /** Setters and Getters for visET cut */
  float getvisETCut() { return m_visETcut; }
  void  setvisETCut( float p1 ) { m_visETcut = p1; }

  /** Setters and Getters for visEta cut */
  float getvisEtaCut() { return m_visEtacut; }
  void  setvisEtaCut( float p1 ) { m_visEtacut = p1; }

  /** Setters and Getters for m_dRThreshold -- the difference in spatial separation between truth and reco for two possible reco tau matches before the resolver is called to choose the best match.  Used to determine whether to resolve matching conflict between reco taus in MapTruthToRec.  Relevant only if MatchResolverFlag is not 1 (default). **/
  float getdRThreshold(){return m_dRThreshold;}
  void  setdRThreshold( float p1 ){ m_dRThreshold = p1;}

  /** Setters and Getters for minTrackMatchProbability -- the probability required for a reconstructed track to be matched to a truth track. For match conflict resolving in MapTruthToRec with MatchResolverFlag=3, 4, or 5 only. **/
  float getminTrackMatchProbability(){return m_minTrackMatchProbability;}
  void setminTrackMatchProbability( float p1 ){m_minTrackMatchProbability = p1;}

  /** Setters and Getters for EnergyFracThreshold -- the fraction of energy that a reco tau must have reconstructed to be considered a match to a true tau.  For match conflict resolving in MapTruthToRec with MatchResolverFlag=2, 4, or 5 only. **/
  float getEnergyFracThreshold(){return m_EnergyFracThreshold;}
  void setEnergyFracThreshold( float p1 ){m_EnergyFracThreshold = p1;}

private:

  /** Instantiate TruthHandler object */
  TruthHandler truthHandler;

  /** Instantiate RecoHandler object */
  RecoHandler  recoHandler;

  float m_deltaRMatchCut;
  float m_LeadingChPiPt_Threshold;
  float m_visETcut;
  float m_visEtacut;

  /** minTrackMatchProbability -- the probability required for a reconstructed track to be matched to a truth track. 
      For match conflict resolving in MapTruthToRec with MatchResolverFlag=3, 4, or 5 only. **/
  float   m_minTrackMatchProbability; //
  /**  m_resolveFlag -- Controls how the method MapTruthToReco handles cases in which multiple matches are possible. 
       Use the input parameter "int MatchResolverFlag" to MapTruthToReco in order to set the flag.  This variable is set internally. 
       Flags:
       
       MatchResolverFlag = 1 -- Resolved by taking closest dR match
       MatchResolverFlag = 2 -- Resolved by checking whether either match would fail loose energy requirements, 
                                then defaults to dR match if Energy does not resolve it.
       MatchResolverFlag = 3 -- Resolved by comparing the number of reconstructed tracks matched to true daughters.  
                                If same for both possible matches, choice defaults to closest dR match.       
       MatchResolverFlag = 4 -- Resolved by Energy requirements, if not resolved then by track matching comparisons, 
                                and if not resolved then takes closest dR.
       MatchResolverFlag = 5 -- Resolved by  track matching comparisons, if not resolved then by Energy requirements, 
                                and if not resolved then takes closest dR.
  **/
  int     m_resolveFlag; 
  /**m_dRThreshold: the difference in spatial separation between truth and reco for two possible reco tau matches 
     before the resolver is called to choose the best match.  Used to determine whether to resolve 
     matching conflict between reco taus in MapTruthToRec.  Relevant only if MatchResolverFlag is not 1 (default). **/
  float   m_dRThreshold; 
  /** EnergyFracThreshold -- the fraction of energy that a reco tau must have reconstructed to be considered a match to a true tau.  For match conflict resolving in MapTruthToRec with MatchResolverFlag=2, 4, or 5 only. **/
  float   m_EnergyFracThreshold;//
  
  const Rec::TrackParticleContainer* m_trackPartContainer;
  const TrackParticleTruthCollection* m_trackTruthParticles;
  
  /** Internal method for public MapTruthToReco method. Checks whether reco tau is already a key in the map  **/
  const TruthParticle* isUniqueMatch(const Analysis::TauJet *MatchedRecoTau, bool &isUnique, MatchingTools::TauJetTruthMap  MatchMap);
  /** Internal method for public MapTruthToReco method. Returns best reco tau match to TruthParticle, eliminating double counting. **/
  const Analysis::TauJet* GetClosestMatch(const TruthParticle* TrueTau, const Analysis::TauJetContainer* RecoContainer, MatchingTools::TauJetTruthMap&  MatchMap);
  /** Internal method for public MapTruthToReco method. Compares dR separation for possible matches. **/
  bool isSmallerdR(const Analysis::TauJet *MatchedRecoTau, const TruthParticle* CurrentTruth, const TruthParticle* MappedTruth);/** Internal method for public MapTruthToReco method. Returns TruthParticle with closest dR separation for possible matches. **/
  const TruthParticle* BestTruthMatch(const Analysis::TauJet *MatchedRecoTau, const TruthParticle* CurrentTruth, const  TruthParticle* MappedTruth);  /** Internal method for public MapTruthToReco method. Returns best reco match when 2 are possible matches, uses flags to choose method.**/
  const Analysis::TauJet * BestRecoTau(const Analysis::TauJet *RecoTau1, const Analysis::TauJet *RecoTau2, const  TruthParticle* TrueTau);
  /** Internal method for public MapTruthToReco method. Gets truth particle associated with reco track.  Taken from 
      /PhysicsAnalysis/TauID/TauTrackTools/src/TrackBasedTruthMatchUtility.cxx  **/
  const HepMC::GenParticle* getTruth(const Rec::TrackParticle* trackParticle,  const TrackParticleTruthCollection* mcpartColl, const Rec::TrackParticleContainer* trackColl) const;
  /** Internal method for public MapTruthToReco method. Counts number of reco tracks matched to daughters of TruthParticle **/
  int NumRecTrueTrackMatch(const Analysis::TauJet *RecoTau, const TruthParticle* TrueTau);  /** Internal method for public MapTruthToReco method. Compares possible matches using loose energy requirements. **/
  const Analysis::TauJet* BestbyEnergy(const Analysis::TauJet *RecoTau1, const Analysis::TauJet *RecoTau2, const  TruthParticle* TrueTau);  /** Internal method for public MapTruthToReco method. Compares possible matches using loose energy requirements.**/
   const TruthParticle*  BestbyEnergy(const Analysis::TauJet *MatchedRecoTau, const TruthParticle* CurrentTruth, const  TruthParticle* MappedTruth); 
  /** Internal method for public MapTruthToReco method. Returns fraction of reconstructed energy. **/
  double EnergyFraction(const Analysis::TauJet *RecoTau, const TruthParticle* TruthTau);
};

#endif //MATCHTOOLS_H
