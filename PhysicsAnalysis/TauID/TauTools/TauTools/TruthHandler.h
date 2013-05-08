/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUTRUTH_H
#define TAUTRUTH_H

//////////////////////////////////////////////////////////////////////////////////////
/// Name    : TruthHandler.h
/// Package : offline/PhysicsAnalysis/TauID/TauTools
/// Author  : Jyothsna Komaragiri, Dugan O'Neil, Aldo Saavedra
/// Created : April 2008
/// Modified: Feb 2009 by Aldo Saavedra, Added Atautau and SingleTau filters
///
/// Last modified: Jennifer Godfrey
///
/// DESCRIPTION:
///
/// This class is a simple example for taus. It has methods to find the MC 
/// truth tau, determine if it is hadronic, match to reco candidates, etc.
/// This class was written to be run on events reconstructed in 14.x.x
///
//////////////////////////////////////////////////////////////////////////////////////
#include "McParticleEvent/TruthParticleContainer.h"

//for finding the truth charge
#include "PathResolver/PathResolver.h"
#include "HepPDT/TableBuilder.hh"
#include "HepPDT/ParticleData.hh"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <functional>
#include <algorithm>
#include <math.h>

#include "Gtypes.h"

class TruthHandler { 
  
public:
  
  TruthHandler();
  virtual ~TruthHandler();
  
  ////////////////////////////////////////////////
  // Methods related to truth particles
  //
  /** determines if a particle is only a G4 particle */
  bool   isGenerator( const TruthParticle* part );
  /** determines if a particle is not a reference particle */
  bool isGenerator( const  HepMC::GenParticle* part );
  /** determines if a particle is Tau or not */
  bool   isTauTruth( const TruthParticle* part );
  /**  determines if a particle is a true hadronic tau*/
  bool isHadTau(const HepMC::GenParticle* part, int Mother = -1 );
  /**  determines if a particle is a true hadronic tau*/
  bool isHadTau(const TruthParticle* part, int Mother = -1 );
  /** determine if a particle is charged pion.  */
  bool   isPiChTruth( const TruthParticle* part );
  /** determine if a particle is charged pion, exclude those which come from kaon decays.  */
  bool isGoodChrgPiDaughter(const HepMC::GenParticle* hadTau);
  /** determine if a particle is a final state stable particle  */
  bool IsGenStable( const HepMC::GenParticle* part );
  /** returns false if particle is only a reference particle in the generator.  Also checks whether particle is intermediate (Status==2) or final state and stable (Status==1)*/
  bool isGoodStatus(const HepMC::GenParticle* part, int Status);
  
  /** provides the number of charged daughters (pions) in tau decays, does not include pions that come from kaons. 
      Will include charged  kaons if UseKaon=true*/
  int nProngTruth(const HepMC::GenParticle* hadTau, bool UseKaon=true);
  /** provides the number of charged daughters  (pions) in tau decays, does not include pions that come from kaons.   
      Will include charged  kaons if UseKaon=true*/
  int nProngTruth( const TruthParticle* hadTau, bool UseKaon=true );
  
  /** returns the charge of a truth particle. Reads PDGTABLE.MEV to get the charge of truth particle. */
  float  getChargeTruth( const TruthParticle * part );
  /** method to count no. of neutral pions */
  int numNeutPion(const HepMC::GenParticle* hadTau );
  /** method to count no. of neutral pions */
  int numNeutPion( const TruthParticle* hadTau);
  
  /** method to count no. of neutral pions and get the pointer to pi+, rho+, a1+ */
  const TruthParticle* nPi0Truth( const TruthParticle* hadTau, int &nPi0 );
  /** determine whether tau decay includes given pdg_ID particle as an intermediate (Status==2) or final state (Status==1) daughter*/
  bool isTauTypeDecay(const HepMC::GenParticle* part, int PDGID, int Status );
  /** method to get a pointer to the intermediate daughter (either the pion, rho, or a1) */
  const TruthParticle* GetIntermDaughter(const TruthParticle* hadTau );
  /** determine whether tau decay includes given pdg_ID particle as an intermediate (Status==2) or final state (Status==1) daughter*/
  bool isTauTypeDecay(const TruthParticle* part, int PDGID, int Status  );
  /** determine whether tau decay is an electron decay*/
  bool isTauElecDecay(const HepMC::GenParticle* part );
  /** determine whether tau decay is an electron decay*/
  bool isTauElecDecay(const TruthParticle* part );
  /** determine whether tau decay is a muon decay*/
  bool isTauMuonDecay(const HepMC::GenParticle* part );
  /** determine whether tau decay is a muon decay*/
  bool isTauMuonDecay(const TruthParticle* part );
  /** determine whether tau decays directly to a rho (does not include a1->rho)*/
  bool isTauRhoDecay(const HepMC::GenParticle* part );
  /** determine whether tau decays directly to a rho does not include a1->rho)*/
  bool isTauRhoDecay(const TruthParticle* part );
  /** determine whether tau decays directly to an a1 particle */
  bool isTauA1Decay(const HepMC::GenParticle* part );
  /** determine whether tau decays directly to an a1 particle */
  bool isTauA1Decay(const TruthParticle* part );
  /** determines whether ``Daughter'' is a hadronic daughter of the decay of the ``TrueTau'', 
      (where status specifies intermediate daughter (Status==2) or final state daughter (Status==1))*/
  bool isHadTauDaughter(const HepMC::GenParticle* TrueTau,const HepMC::GenParticle* Daughter, int Status);
  /** determines whether ``Daughter'' is a hadronic daughter of the decay of the ``TrueTau'', 
      (where status specifies intermediate daughter (Status==2) or final state daughter (Status==1))*/
  bool isHadTauDaughter(const TruthParticle* TruthTau,const TruthParticle* TruthDaughter, int Status);
  /** determines whether ``Daughter'' is a hadronic daughter of the decay of a tau in the  ``TrueTauCollection'', 
      (where status specifies intermediate daughter (Status==2) or final state daughter (Status==1)).
      One should use getHadronicTruth(const TruthParticleContainer *mcpartTES) to obtain this ``TrueTauCollection''.*/
  bool isHadTauDaughter(const  HepMC::GenEvent* TrueTauCollection, const HepMC::GenParticle* Daughter, int Status);
  /** determines whether ``Daughter'' is a hadronic daughter of the decay of a tau in the  ``TrueTauCollection'', 
      (where status specifies intermediate daughter (Status==2) or final state daughter (Status==1))*/
  bool isHadTauDaughter(const TruthParticleContainer *TrueTauCollection,const TruthParticle* TruthDaughter, int Status);
  /** determines whether ``Daughter'' is a final stable daughter of the decay of the ``TrueTau'' */
  bool isHadTauFinalDaughter(const HepMC::GenParticle* TrueTau,const HepMC::GenParticle* Daughter);
  /** determines whether ``Daughter'' is a final stable daughter of the decay of the ``TrueTau'' */
  bool isHadTauFinalDaughter(const TruthParticle* TruthTau,const TruthParticle* TruthDaughter);
  /** determines whether ``Daughter'' is a final stable daughter of the decay of a tau in the ``TrueTauCollection''.
      One should use getHadronicTruth(const TruthParticleContainer *mcpartTES) to obtain this collection.*/
  bool isHadTauFinalDaughter(const  HepMC::GenEvent* TrueTauCollection, const HepMC::GenParticle* Daughter);
  /** determines whether ``Daughter'' is a final stable daughter of the decay of a tau in the ``TrueTauCollection''.
      One should use getHadronicTruth(const TruthParticleContainer *mcpartTES) to obtain this collection.*/
  bool isHadTauFinalDaughter(const TruthParticleContainer *TrueTauCollection,const TruthParticle* TruthDaughter);
  /** determines whether ``Daughter'' is an intermediate  daughter of the decay of the ``TrueTau'' */
  bool isHadTauIntermDaughter(const HepMC::GenParticle* TrueTau,const HepMC::GenParticle* Daughter);
  /** determines whether ``Daughter'' is an intermediate  daughter of the decay of the ``TrueTau'' */
  bool isHadTauIntermDaughter(const TruthParticle* TruthTau,const TruthParticle* TruthDaughter);
  /** determines whether ``Daughter'' is an intermediate  daughter of the decay of a tau in the ``TrueTauCollection''. 
One should use getHadronicTruth(const TruthParticleContainer *mcpartTES) to obtain this collection. */
  bool isHadTauIntermDaughter(const HepMC::GenEvent* TrueTauCollection,const HepMC::GenParticle* Daughter);
  /** determines whether ``Daughter'' is an intermediate  daughter of the decay of a tau in the ``TrueTauCollection''.
      One should use getHadronicTruth(const TruthParticleContainer *mcpartTES) to obtain this collection. */
  bool isHadTauIntermDaughter(const TruthParticleContainer *TrueTauCollection, const TruthParticle* TruthDaughter);
  /** returns a collection of the true intermediate daughters  (Status==2) or final daughters (Status==2) 
      of the true hadronic tau*/
TruthParticleContainer* getTrueHadDaughters(const TruthParticle *TrueTau, const TruthParticleContainer *mcpartTES, int Status);
  /** returns a collection of the true  final daughters of the true hadronic tau*/
 TruthParticleContainer* getFinalTrueHadDaughters(const TruthParticle *TrueTau, const TruthParticleContainer *mcpartTES);
  /** returns a collection of the true intermediate daughters of the true hadronic tau*/
  TruthParticleContainer* getIntermTrueHadDaughters(const TruthParticle *TrueTau, const TruthParticleContainer *mcpartTES);
  /** returns a collection of the true hadronics taus which decay to nProngs */
  TruthParticleContainer* getHadronicnProngTruth(const TruthParticleContainer *mcpartTES, int nProngs);
  /** returns a collection of the true hadronics taus which decay to 1 Prong */
  TruthParticleContainer* getHadronic1ProngTruth(const TruthParticleContainer *mcpartTES);
    /** returns a collection of the true hadronics taus which decay to 3 Prongs */
  TruthParticleContainer* getHadronic3ProngTruth(const TruthParticleContainer *mcpartTES);

 /** Loops through the truth taus in the event to determine if they are hadronic decays or not. 
      It returns a container of hadronic taus. Option to enforce the pdgID of the parent particle to be of type Mother. */
  TruthParticleContainer* getHadronicTruth(const TruthParticleContainer *mcpartTES, int Mother = -1 );
  /**Handle tau truth information. Return a container of all truth taus 
     Option to enforce the pdgID of the parent particle to be of type Mother.**/

  TruthParticleContainer* getAllFinalTaus(const TruthParticleContainer *mcpartTES, int Mother=-1);

  
  /** Returns the truth particle container of had-had, had-el, had-mu etc depending in the int decayMode. */
  TruthParticleContainer* getZTauDecayMode(const TruthParticleContainer *mcpartTES, int decayMode);
  
  /** Given an hadronic truth tau, calculate the visible Sum */
  CLHEP::HepLorentzVector getTauVisibleSumTruth( const TruthParticle* hadTau );
  
  /** This method returns the HepLorentzVector of leading charged pion */
  CLHEP::HepLorentzVector getPiChTruth( const TruthParticle* hadTau );
  
  /** ATauTau filter: this method determines whether the generated taus passed the Atau tau filter as implemented for dataset 5188 */  
  bool AtautauFilter( const TruthParticleContainer *mcpartTES); 
  
  /** Set ATauTau filter cut values on pT, eta of e/mu/had tau decay and dphi between tau daughters in lep-lep, lep-had, had-had tau decay modes*/
  void SetAtautauFilter(float Etacut, 
			float llPtcute, float llPtcutmu, 
			float lhPtcute, float lhPtcutmu, float lhPtcuth, 
			float hhPtcut, 
			float maxdphi);
  
  /** Tau filter: this method determines whether the generated tau passed the tau filter as implemented for dataset 5107 */  
  bool TauFilter( const TruthParticleContainer *mcpartTES); 
  
  /** This method adds the momentum of the daughter neutrinos of the tau, it is called recursively */
  CLHEP::HepLorentzVector sumDaughterNeutrinos( const TruthParticle *part );
  /** Set Tau filter cut values on no. of taus and pT, eta of e/mu/had tau decay*/
  void SetTauFilter(int ntaumin, 
		    float Etacute, float Ptcute, 
		    float Etacutmu, float Ptcutmu, 
		    float Etacuthad, float Ptcuthad);
  
  /** Print the stats for the filter used, 1 for Atautau filter and 2 for Tau filter */
  void PrintFilterStats(int which_filter);
  
  /** read the PDT table */
  void readPDT();
  
  /** set the debug level */
  void SetDebugLevel(bool debug) {m_debug = debug;};
  
  /** use Kaons(pdgID=321) in counting prongs */
  void SetProngCount(bool Kaon) {m_Kaon = Kaon;};
  
  /** Print the summary in a logfile and save histograms in a root file */
  void finalize();
  
private:
  
  /** Particle Property service */
  HepPDT::ParticleDataTable *m_particleDataTable;
  
  /** counters to tell us how many taus of each type we find */
  unsigned int m_nochild_counter;
  unsigned int m_totaltruthtau_counter;
  unsigned int m_totalHadCand_counter;
  
  /** debug output*/
  bool m_debug;
  
  /** use Kaons */
  bool m_Kaon;
  
  /** counters for the providing Atautau filter statistics */
  int m_eventsll_2tauf;       // events with two taus decaying leptonically
  int m_eventsllacc_2tauf;    // events with two taus decaying leptonically accepted
  int m_eventslh_2tauf;       // events with one tau decaying leptonically and the other hadronically
  int m_eventslhacc_2tauf;    // events with one tau decaying leptonically and the other hadronically accepted
  int m_eventshh_2tauf;       // events with two taus decaying hadronically
  int m_eventshhacc_2tauf;    // events with two taus decaying hadronically accepted
  int m_eventsaccepted_2tauf; // total number of events that passed 
  int m_eventsrefused_2tauf;  // total number of events that got rejected
  int m_eventsnotau_2tauf;    // total number of events with no taus
  
  /** Cut values for Atautau filter  */
  float m_llPtmine_2tauf;
  float m_llPtminmu_2tauf;
  float m_lhPtmine_2tauf;
  float m_lhPtminmu_2tauf;
  float m_lhPtminh_2tauf;
  float m_hhPtmin_2tauf;
  float m_EtaRange_2tauf;
  float m_maxdphi_2tauf;
  
  /** counters for the providing Tau filter statistics */
  int m_eventse_tauf;
  int m_eventseacc_tauf;
  int m_eventsmu_tauf;
  int m_eventsmuacc_tauf;
  int m_eventshad_tauf;
  int m_eventshadacc_tauf;
  int m_eventsaccepted_tauf;
  int m_eventsrefused_tauf;
  
  /** Cut values for Tau filter  */
  float m_pTmine_tauf;
  float m_etaMaxe_tauf;
  float m_pTminmu_tauf;
  float m_etaMaxmu_tauf;
  float m_pTminhad_tauf;
  float m_etaMaxhad_tauf;
  int   m_NTau_tauf;         // the minimum number of taus required to that pass the cuts for a given event
  
};

#endif // TAUTRUTH_H
