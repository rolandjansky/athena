#ifndef TAUTRUTH_H
#define TAUTRUTH_H

//////////////////////////////////////////////////////////////////////////////////////
/// Name    : TruthHandler.h
/// Package : offline/PhysicsAnalysis/TauID/TauTools
/// Author  : Jyothsna Komaragiri, Dugan O'Neil, Aldo Saavedra
/// Created : April 2008
/// Modified: Feb 2009 by Aldo Saavedra, Added Atautau and SingleTau filters
///
/// modified: Jennifer Godfrey
///
/// modified by: Felix Friedrich, 2014
///
/// modified by: Guillermo Hamity, 2014
///
/// DESCRIPTION:
///
/// This class contains methods to access truth tau information. It has methods to find the MC
/// truth tau, determine if it is hadronic, match to reco candidates, etc.
///
//////////////////////////////////////////////////////////////////////////////////////
#include "McParticleEvent/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertex.h"

#include "HepPDT/ParticleData.hh"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include <vector>
#include "TLorentzVector.h"
#include <functional>
#include <algorithm>
#include <math.h>
#include "Gtypes.h"

// FF,2014:
// this class is just a collection of free functions. Having them collected into a class is for backwards compatibility.
// in principle all of the methods can be moved to a namespace

class TruthHandler {
public:
	// let the compiler auto generate the constructor and destructor
	//TruthHandler();
	//~TruthHandler();

		/** determines if a particle is only a G4 particle */
		bool isGenerator(int barcode, int status);
		bool isGenerator( const TruthParticle* part )       { return isGenerator(part->barcode(), part->status()); }
		bool isGenerator( const HepMC::GenParticle* part )  { return isGenerator(part->barcode(), part->status()); }
		bool isGenerator( const xAOD::TruthParticle* part)  { return isGenerator(part->barcode(), part->status()); }

		/** determine if a particle is a final state stable particle  */
		bool isGenStable( const HepMC::GenParticle* part );
		bool isGenStable( const xAOD::TruthParticle* part );

		/** returns false if particle is only a reference particle in the generator.  Also checks whether particle is intermediate (Status==2) or final state and stable (Status==1)*/
		bool isGoodStatus(int Status);
		bool isGoodStatus(const HepMC::GenParticle* part, int StatusToCheck);
		bool isGoodStatus(const xAOD::TruthParticle* part, int StatusToCheck);

		/** return true if xAODTruth particle as a child with pdgID==pdgIDtoCheck. Only direct children are checked. No iteration on children-of-children. */
		bool hasChild(const xAOD::TruthParticle* part, int pdgIDtoCheck);
    
		/** determines if a particle is Tau or not */
		bool isTauTruth( const TruthParticle* part );
		bool isTauTruth( const xAOD::TruthParticle* part );

		/**  determines if a particle is a true hadronic tau*/
		bool isHadTau(const HepMC::GenParticle* part, int Mother = -1 );
		bool isHadTau(const xAOD::TruthParticle* part, int Mother = -1 );
		bool isHadTau(const TruthParticle* part, int Mother = -1 ) { return isHadTau( (const HepMC::GenParticle*) part->genParticle(), Mother); }

		/** determine if a particle is charged pion, exclude those which come from kaon decays.  */
		bool isGoodChrgPiDaughter(const HepMC::GenParticle* hadTau);
		bool isGoodChrgPiDaughter(const xAOD::TruthParticle* hadTau);

		/** determine whether tau decay includes given pdg_ID particle as an intermediate (Status==2) or final state (Status==1) daughter*/
		bool isTauTypeDecay(const HepMC::GenParticle* part, int PDGID, int Status );
		bool isTauTypeDecay(const TruthParticle* part, int PDGID, int Status  ) { return isTauTypeDecay( (const HepMC::GenParticle*) part->genParticle(), PDGID, Status); }

		/** determine whether tau decay is an electron decay*/
		bool isTauElecDecay(const HepMC::GenParticle* part ) { return isTauTypeDecay(part, 11, 1); }
		bool isTauElecDecay(const TruthParticle* part )      { return isTauTypeDecay(part, 11, 1); }

		/** determine whether tau decay is a muon decay*/
		bool isTauMuonDecay(const HepMC::GenParticle* part ) { return isTauTypeDecay(part, 13, 1); }
		bool isTauMuonDecay(const TruthParticle* part )      { return isTauTypeDecay(part, 13, 1); }

		/** determine whether tau decays directly to a rho (does not include a1->rho)*/
		bool isTauRhoDecay(const HepMC::GenParticle* part ) { return isTauTypeDecay(part, PDG::rho_plus, 2); }
		bool isTauRhoDecay(const TruthParticle* part )      { return isTauTypeDecay(part, PDG::rho_plus, 2); }

		/** determine whether tau decays directly to an a1 particle */
		bool isTauA1Decay(const HepMC::GenParticle* part ) {  return isTauTypeDecay(part, PDG::a_1_plus,2); }
		bool isTauA1Decay(const TruthParticle* part )      {  return isTauTypeDecay(part, PDG::a_1_plus,2); }

		/** determine if a particle is charged pion.  */
		bool   isPiChTruth( const TruthParticle* part ) { return ( abs((int)part->pdgId()) == PDG::pi_plus); }

		/** Loops through the truth taus in the event to determine if they are hadronic decays or not.
		It returns a container of hadronic taus. Option to enforce the pdgID of the parent particle to be of type Mother. */
		TruthParticleContainer* getHadronicTruth(const TruthParticleContainer *mcpartTES, int Mother = -1 );
		xAOD::TruthParticleContainer* getHadronicTruth(const xAOD::TruthParticleContainer *mcpartTES, int Mother = -1 );

		/**Handle tau truth information. Return a container of all truth taus
		Option to enforce the pdgID of the parent particle to be of type Mother.**/
		TruthParticleContainer* getAllFinalTaus(const TruthParticleContainer *mcpartTES, int Mother=-1);

		/** provides the number of charged daughters (pions) in tau decays, does not include pions that come from kaons.
		Will include charged  kaons if UseKaon=true*/
		int nProngTruth(const HepMC::GenParticle* hadTau, bool UseKaon=true );
		int nProngTruth(const xAOD::TruthParticle* hadTau, bool UseKaon=true );
		int nProngTruthTau(const xAOD::TruthParticle* hadTau, bool UseKaon=true );
		int nProngTruth(const TruthParticle* hadTau, bool UseKaon=true ) { return nProngTruth( (const HepMC::GenParticle*)hadTau->genParticle(), UseKaon); }

		/** method to count no. of neutral pions */
		int numNeutPion(const HepMC::GenParticle* hadTau );
		int numNeutPion(const xAOD::TruthParticle* hadTau);
		int numNeutPion( const TruthParticle* hadTau) { return numNeutPion( (const HepMC::GenParticle*)hadTau->genParticle() ); }

		/** method to count no. of neutral pions and get the pointer to pi+, rho+, a1+ */
		const TruthParticle* nPi0Truth( const TruthParticle* hadTau, int &nPi0 );

		/** method to get a pointer to the intermediate daughter (either the pion, rho, or a1) */
		const TruthParticle* GetIntermDaughter(const TruthParticle* hadTau );

		/** determines whether ``Daughter'' is a hadronic daughter of the decay of the ``TrueTau'',
		(where status specifies intermediate daughter (Status==2) or final state daughter (Status==1))*/
		bool isHadTauDaughter(const HepMC::GenParticle* TruthTau,const HepMC::GenParticle* Daughter, int Status);
		bool isHadTauDaughter(const TruthParticle* TruthTau,const TruthParticle* TruthDaughter, int Status) { return isHadTauDaughter( (const HepMC::GenParticle*) TruthTau->genParticle(), (const HepMC::GenParticle*) TruthDaughter->genParticle(), Status); }
		bool isHadTauDaughter(const HepMC::GenEvent* TrueTauCollection, const HepMC::GenParticle* Daughter, int Status);
		bool isHadTauDaughter(const TruthParticleContainer *TrueTauCollection,const TruthParticle* TruthDaughter, int Status);

		/** determines whether ``Daughter'' is a final stable daughter of the decay of the ``TrueTau'' */
		bool isHadTauFinalDaughter(const HepMC::GenParticle* TruthTau,const HepMC::GenParticle* Daughter) { return isHadTauDaughter(TruthTau,Daughter,1); }
		bool isHadTauFinalDaughter(const TruthParticle* TruthTau,const TruthParticle* Daughter)           { return isHadTauDaughter(TruthTau,Daughter,1); }
		bool isHadTauFinalDaughter(const  HepMC::GenEvent* TrueTauCollection, const HepMC::GenParticle* Daughter)      {  return isHadTauDaughter(TrueTauCollection, Daughter, 1); }
		bool isHadTauFinalDaughter(const TruthParticleContainer *TrueTauCollection,const TruthParticle* Daughter)      {  return isHadTauDaughter(TrueTauCollection, Daughter, 1); }

		/** determines whether ``Daughter'' is an intermediate  daughter of the decay of the ``TrueTau'' */
		bool isHadTauIntermDaughter(const HepMC::GenParticle* TruthTau,const HepMC::GenParticle* Daughter) { return isHadTauDaughter(TruthTau,Daughter,2); }
		bool isHadTauIntermDaughter(const TruthParticle* TruthTau,const TruthParticle* Daughter)           { return isHadTauDaughter(TruthTau,Daughter,2); }
		bool isHadTauIntermDaughter(const HepMC::GenEvent* TrueTauCollection,const HepMC::GenParticle* Daughter)         { return isHadTauDaughter(TrueTauCollection, Daughter, 2); }
		bool isHadTauIntermDaughter(const TruthParticleContainer *TrueTauCollection, const TruthParticle* Daughter)      { return isHadTauDaughter(TrueTauCollection, Daughter, 2); }

		/** returns a collection of the true intermediate daughters  (Status==2) or final daughters (Status==2)
		of the true hadronic tau*/
		TruthParticleContainer* getTrueHadDaughters(const TruthParticle *TrueTau, const TruthParticleContainer *mcpartTES, int Status);

		/** returns a collection of the true  final daughters of the true hadronic tau*/
		TruthParticleContainer* getFinalTrueHadDaughters(const TruthParticle *TrueTau, const TruthParticleContainer *mcpartTES)  { return getTrueHadDaughters(TrueTau, mcpartTES, 1); }

		/** returns a collection of the true intermediate daughters of the true hadronic tau*/
		TruthParticleContainer* getIntermTrueHadDaughters(const TruthParticle *TrueTau, const TruthParticleContainer *mcpartTES) { return getTrueHadDaughters(TrueTau, mcpartTES, 2); }

		/** returns a collection of the true hadronics taus which decay to nProngs */
		TruthParticleContainer* getHadronicnProngTruth(const TruthParticleContainer *mcpartTES, int nProngs);

		/** returns a collection of the true hadronics taus which decay to 1 Prong */
		TruthParticleContainer* getHadronic1ProngTruth(const TruthParticleContainer *mcpartTES) { return getHadronicnProngTruth(mcpartTES, 1); }

		/** returns a collection of the true hadronics taus which decay to 3 Prongs */
		TruthParticleContainer* getHadronic3ProngTruth(const TruthParticleContainer *mcpartTES) { return getHadronicnProngTruth(mcpartTES, 3); }

		/** This method adds the momentum of the daughter neutrinos of the tau, it is called recursively */
		CLHEP::HepLorentzVector sumDaughterNeutrinos( const TruthParticle *part );

		/** Given a hadronic truth tau, calculate the visible Sum */
		CLHEP::HepLorentzVector getTauVisibleSumTruth( const TruthParticle* hadTau );
		TLorentzVector getTauVisibleSumTruth( const xAOD::TruthParticle* hadTau );

		/** This method returns the HepLorentzVector of leading charged pion */
		CLHEP::HepLorentzVector getPiChTruth( const TruthParticle* hadTau );
		//CLHEP::HepLorentzVector getPiChTruth( const xAOD::TruthParticle* hadTau );
};

#endif
