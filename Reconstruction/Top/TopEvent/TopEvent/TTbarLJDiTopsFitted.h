/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TTBARLJDITOPSFITTED_H
#define TTBARLJDITOPSFITTED_H

#include "TopEvent/TTbarLJDiTops.h"

#include "JetEvent/JetCollection.h"

#include "DataModel/ElementLink.h"

#include "boost/shared_ptr.hpp"
#include "HepMC/GenParticle.h"
#include "FourMomUtils/P4Helpers.h"

#include <vector>
#include <set>
#include <cmath>
#include <iostream>

namespace TopRec {

  // For pointer to rescaled jets
  struct RescaledJetCollectionBase {
    RescaledJetCollectionBase(JetCollection* ptr) : m_rescaledJetCollection(ptr) {;}
    RescaledJetCollectionBase() : m_rescaledJetCollection() {;}
    boost::shared_ptr<JetCollection> m_rescaledJetCollection;
  };

  // For pointer to rescaled lepton
  template<class LeptonCollection> struct RescaledLeptonCollectionBase;
  // specialization for TruthParticleContainers
  template<> struct RescaledLeptonCollectionBase<TruthParticleContainer> {
    RescaledLeptonCollectionBase(TruthParticleContainer* ptr) : m_rescaledLeptonCollection(ptr), m_rescaledGenParticleCollection(0) {;}
    RescaledLeptonCollectionBase() : m_rescaledLeptonCollection(), m_rescaledGenParticleCollection(0) {;}
    HepMC::GenParticle * newGenParticle(const HepMC::GenParticle &genPart) {
      m_rescaledGenParticleCollection.push_back(new HepMC::GenParticle(genPart));
      return m_rescaledGenParticleCollection.back();
    }
    ~RescaledLeptonCollectionBase() {
    }
    boost::shared_ptr<TruthParticleContainer> m_rescaledLeptonCollection;
    std::vector<HepMC::GenParticle*> m_rescaledGenParticleCollection;
  };
  // all other classes
  template<class LeptonCollection>
  struct RescaledLeptonCollectionBase {
    RescaledLeptonCollectionBase(LeptonCollection* ptr) : m_rescaledLeptonCollection(ptr) {;}
    RescaledLeptonCollectionBase() : m_rescaledLeptonCollection() {;}
    boost::shared_ptr<LeptonCollection> m_rescaledLeptonCollection;
  };

  // Scale jet according to different corrections for energy, momentum, eta and phi
  inline Jet * scaledJet(const Jet& initialJet, double scaleE, double scaleP, double deltaEta, double deltaPhi) {
    CLHEP::Hep3Vector vec3(initialJet.px(), initialJet.py(), initialJet.pz());
    vec3.setREtaPhi(vec3.getR()*scaleP, vec3.getEta()+deltaEta, vec3.getPhi()+P4Helpers::deltaPhi(vec3.getPhi()+deltaPhi, vec3.getPhi()));
    return new Jet(CLHEP::HepLorentzVector(vec3, initialJet.e()*scaleE));
  }

  // Scale lepton according to different corrections for energy, momentum, eta and phi
  template<class LeptonCollection>
  inline typename LeptonCollection::base_value_type * scaledLepton(const typename LeptonCollection::base_value_type& initialLepton, double scaleE, double scaleP, double deltaEta, double deltaPhi, RescaledLeptonCollectionBase<LeptonCollection> * );
  // - the version for TruthParticles
  template<> inline TruthParticle * scaledLepton(const TruthParticle& initialTruthParticle, double scaleE, double scaleP, double deltaEta, double deltaPhi, RescaledLeptonCollectionBase<TruthParticleContainer> * rescaledLeptonCollectionBase) {
    CLHEP::Hep3Vector vec3(initialTruthParticle.px(), initialTruthParticle.py(), initialTruthParticle.pz());
    vec3.setREtaPhi(vec3.getR()*scaleP, vec3.getEta()+deltaEta, vec3.getPhi()+P4Helpers::deltaPhi(vec3.getPhi(), vec3.getPhi()+deltaPhi));
    CLHEP::HepLorentzVector lorentzVec(vec3, initialTruthParticle.e()*scaleE);
    return new TruthParticle(rescaledLeptonCollectionBase->newGenParticle(HepMC::GenParticle(lorentzVec, initialTruthParticle.genParticle()->pdg_id(), initialTruthParticle.genParticle()->status(), initialTruthParticle.genParticle()->flow(), initialTruthParticle.genParticle()->polarization())));
  }
  // - the version for all other classes
  template<class LeptonCollection>
  inline typename LeptonCollection::base_value_type * scaledLepton(const typename LeptonCollection::base_value_type& initialLepton, double scaleE, double scaleP, double deltaEta, double deltaPhi, RescaledLeptonCollectionBase<LeptonCollection> * ) {
    CLHEP::Hep3Vector vec3(initialLepton.px(), initialLepton.py(), initialLepton.pz());
    vec3.setREtaPhi(vec3.getR()*scaleP, vec3.getEta()+deltaEta, vec3.getPhi()+P4Helpers::deltaPhi(vec3.getPhi(), vec3.getPhi()+deltaPhi));
    CLHEP::HepLorentzVector lorentzVec(vec3, initialLepton.e()*scaleE);
    typename LeptonCollection::base_value_type * scaledLepton = new typename LeptonCollection::base_value_type();
    scaledLepton->set4Mom(lorentzVec);
    return scaledLepton;
  }

  //====================================================================
  //Helper class for the constructor
  template<class LeptonCollection>
  TTbarLJDiTops<LeptonCollection> makeTTbarLJDiTopsRescaled(const TTbarLJDiTops<LeptonCollection>& ttbarInitial,
							    const double& rescalingHadB_E,
							    const double& rescalingHadB_P,
							    const double& hadB_deltaEta,
							    const double& hadB_deltaPhi,
							    const double& rescalingLepB_E,
							    const double& rescalingLepB_P,
							    const double& lepB_deltaEta,
							    const double& lepB_deltaPhi,
							    const std::vector<double>& rescalingLightJets_E,
							    const std::vector<double>& rescalingLightJets_P,
							    const std::vector<double>& lightJets_deltaEta,
							    const std::vector<double>& lightJets_deltaPhi,
							    const double& rescalingLepton_E,
							    const double& rescalingLepton_P,
							    const double& lepton_deltaEta,
							    const double& lepton_deltaPhi,
							    const double& rescalingEtMissX,
							    const double& rescalingEtMissY,
							    const double& neutrinoPz,
							    JetCollection* rescaledJetCollection,
							    LeptonCollection* rescaledLeptonCollection,
							    RescaledLeptonCollectionBase<LeptonCollection>*	rescaledLeptonCollectionBase)
  {
    JetVector initialJetVec(ttbarInitial.jets());
    JetVector rescaledJetVec;

    //Don't have too, but nicer if we preserve the same jets ordering for
    //the rescaled ttbar than initial ttbar.
    std::vector<size_t> orderInitialJets;

    //Hadronic B
    rescaledJetCollection->push_back(scaledJet(ttbarInitial.htop().jetB(), rescalingHadB_E, rescalingHadB_P, hadB_deltaEta, hadB_deltaPhi));
    orderInitialJets.push_back(ttbarInitial.htop().jetindexB());
    //Leptonic B
    rescaledJetCollection->push_back(scaledJet(ttbarInitial.ltop().jetB(), rescalingLepB_E, rescalingLepB_P, lepB_deltaEta, lepB_deltaPhi));
    orderInitialJets.push_back(ttbarInitial.ltop().jetindexB());
    //Light Jets
    for (size_t i(0); i < rescalingLightJets_E.size(); ++i)
      if (i < ttbarInitial.htop().numJetsW()) {
	rescaledJetCollection->push_back(scaledJet(ttbarInitial.htop().jetW(i), rescalingLightJets_E.at(i), rescalingLightJets_P.at(i), lightJets_deltaEta.at(i), lightJets_deltaPhi.at(i)));
	orderInitialJets.push_back(ttbarInitial.htop().jetIndexesW().at(i));
      }

    //Store the jet in the Vector with same order than ttbar initial
    for (size_t i(0); i < orderInitialJets.size(); ++i)
      for (size_t j(0); j < orderInitialJets.size(); ++j)
	if (orderInitialJets.at(j) == i)
	  rescaledJetVec.push_back(ElementLink<JetCollection>(*rescaledJetCollection,j));

    //Neutrino
    double neutrinoPx = ttbarInitial.ltop().neutrino().px() * rescalingEtMissX;
    double neutrinoPy = ttbarInitial.ltop().neutrino().py() * rescalingEtMissY;
    double neutrinoE = sqrt(neutrinoPx*neutrinoPx+neutrinoPy*neutrinoPy+neutrinoPz*neutrinoPz);
    P4PxPyPzE neutrino = P4PxPyPzE(neutrinoPx,neutrinoPy,neutrinoPz,neutrinoE);

    //Lepton
    rescaledLeptonCollection->push_back(scaledLepton<LeptonCollection>(ttbarInitial.ltop().lepton(), rescalingLepton_E, rescalingLepton_P, lepton_deltaEta, lepton_deltaPhi, rescaledLeptonCollectionBase));

    //Create the rescaled TTbarLJDiTops
    TTbarLJDiTops<LeptonCollection> theTTbarLJDiTopsRescaled(TTbarLJ<LeptonCollection>(ElementLink<LeptonCollection>(*rescaledLeptonCollection, 0), rescaledJetVec), IndexSet(ttbarInitial.htop().jetIndexes().begin(), ttbarInitial.htop().jetIndexes().end()), ttbarInitial.htop().jetindexB(), neutrino, ttbarInitial.ltop().jetindexB());
    return theTTbarLJDiTopsRescaled;
  }

  //====================================================================
  //! Single lepton TTbar with both hadronic top and leptonic top reconstructed
  //! The TTbar is the result of a fitting algorithm that could
  //! 1) rescaled the energy of the jets and EtMiss; (this is the old constructor, which remains for backward compatibility)
  //! 2) rescale energy and momentum of the jets and the lepton,
  //!    provide corrections to the angles eta and phi of these objects,
  //!    and allow for corrections to the EtMiss px and py.
  template<class LeptonCollection>
  class TTbarLJDiTopsFitted : public RescaledJetCollectionBase,
			      public RescaledLeptonCollectionBase<LeptonCollection>,
			      public TTbarLJDiTops<LeptonCollection>
  {
    
    TTbarLJDiTops<LeptonCollection> m_initialTTbar;
 
    double m_rescalingHadB_E;
    double m_rescalingHadB_P;
    double m_hadB_deltaEta;
    double m_hadB_deltaPhi;

    double m_rescalingLepB_E;
    double m_rescalingLepB_P;
    double m_lepB_deltaEta;
    double m_lepB_deltaPhi;

    std::vector<double> m_rescalingLightJets_E;
    std::vector<double> m_rescalingLightJets_P;
    std::vector<double> m_lightJets_deltaEta;
    std::vector<double> m_lightJets_deltaPhi;

    double m_rescalingLepton_E;
    double m_rescalingLepton_P;
    double m_lepton_deltaEta;
    double m_lepton_deltaPhi;

    double m_rescalingEtMiss; // remained for backward compatibility for case 1) -> senseless if m_rescalingEtMissX != m_rescalingEtMissY
    double m_rescalingEtMissX;
    double m_rescalingEtMissY;
    double m_neutrinoPz;

    double m_permutationProbability;
    double m_permutationDiscriminant;

  public:

    //Constructor for case 1)
    TTbarLJDiTopsFitted(const TTbarLJDiTops<LeptonCollection>& ttbarInitial,
			const double& rescalingHadB_E, // uses the same rescaling factor for the momentum, no change of angles (dEta = dPhi = 0)
			const double& rescalingLepB_E, // uses the same rescaling factor for the momentum, no change of angles (dEta = dPhi = 0)
			const std::vector<double>& rescalingLightJets_E, // uses the same rescaling factor for the momentum, no change of angles (dEta = dPhi = 0)
			const double& rescalingEtMiss, // uses the same rescaling factor for the EtMiss x and y component
			const double& neutrinoPz,
			const double& permutationProbability,
			const double& permutationDiscriminant) 

      : RescaledJetCollectionBase(new JetCollection()),
	RescaledLeptonCollectionBase<LeptonCollection>(new LeptonCollection()),
	TTbarLJDiTops<LeptonCollection>(makeTTbarLJDiTopsRescaled<LeptonCollection>(ttbarInitial,
										    rescalingHadB_E, // same rescaling factor for energy
										    rescalingHadB_E, // and momentum
										    0., // dEta = 0
										    0., // dPhi = 0
										    rescalingLepB_E, // same rescaling factor for energy
										    rescalingLepB_E, // and momentum
										    0., // dEta = 0
										    0., // dPhi = 0
										    rescalingLightJets_E, // same rescaling factor for energy
										    rescalingLightJets_E, // and momentum
										    std::vector<double>(ttbarInitial.htop().numJetsW(), 0.), // dEta = 0
										    std::vector<double>(ttbarInitial.htop().numJetsW(), 0.), // dPhi = 0
										    1., // no rescaling of the lepton
										    1., // no rescaling of the lepton
										    0., // dEta = 0
										    0., // dPhi = 0
										    rescalingEtMiss, // same rescaling factor for EtMissX
										    rescalingEtMiss, // and EtMissY
										    neutrinoPz,
										    m_rescaledJetCollection.get(),
										    RescaledLeptonCollectionBase<LeptonCollection>::m_rescaledLeptonCollection.get(),
										    this)),
	m_initialTTbar(ttbarInitial),
	m_rescalingHadB_E(rescalingHadB_E), 
	m_rescalingHadB_P(rescalingHadB_E), // same rescaling factor as for the energy
	m_hadB_deltaEta(0.), // dEta = 0
	m_hadB_deltaPhi(0.), // dPhi = 0
	m_rescalingLepB_E(rescalingLepB_E), 
	m_rescalingLepB_P(rescalingLepB_E), // same rescaling factor as for the energy
	m_lepB_deltaEta(0.), // dEta = 0
	m_lepB_deltaPhi(0.), // dPhi = 0
	m_rescalingLightJets_E(rescalingLightJets_E), 
	m_rescalingLightJets_P(rescalingLightJets_E), // same rescaling factor as for the energy
	m_lightJets_deltaEta(std::vector<double>(ttbarInitial.htop().numJetsW(), 0.)), // dEta = 0
	m_lightJets_deltaPhi(std::vector<double>(ttbarInitial.htop().numJetsW(), 0.)), // dPhi = 0
	m_rescalingLepton_E(1.), // no rescaling of the lepton
	m_rescalingLepton_P(1.), // no rescaling of the lepton
	m_lepton_deltaEta(0.), // dEta = 0
	m_lepton_deltaPhi(0.), // dPhi = 0
	m_rescalingEtMiss(rescalingEtMiss), 
	m_rescalingEtMissX(rescalingEtMiss), // same rescaling factor for EtMissX
	m_rescalingEtMissY(rescalingEtMiss), // and EtMissY
	m_neutrinoPz(neutrinoPz), 
	m_permutationProbability(permutationProbability), 
	m_permutationDiscriminant(permutationDiscriminant) {;}

    //Constructor for case 2)
    TTbarLJDiTopsFitted(const TTbarLJDiTops<LeptonCollection>& ttbarInitial,
			const double& rescalingHadB_E,
			const double& rescalingHadB_P,
			const double& hadB_deltaEta,
			const double& hadB_deltaPhi,
			const double& rescalingLepB_E,
			const double& rescalingLepB_P,
			const double& lepB_deltaEta,
			const double& lepB_deltaPhi,
			const std::vector<double>& rescalingLightJets_E,
			const std::vector<double>& rescalingLightJets_P,
			const std::vector<double>& lightJets_deltaEta,
			const std::vector<double>& lightJets_deltaPhi,
			const double& rescalingLepton_E,
			const double& rescalingLepton_P,
			const double& lepton_deltaEta,
			const double& lepton_deltaPhi,
			const double& rescalingEtMissX,
			const double& rescalingEtMissY,
			const double& neutrinoPz,
			const double& permutationProbability,
			const double& permutationDiscriminant) 

      : RescaledJetCollectionBase(new JetCollection()),
	RescaledLeptonCollectionBase<LeptonCollection>(new LeptonCollection()),
	TTbarLJDiTops<LeptonCollection>(makeTTbarLJDiTopsRescaled<LeptonCollection>(ttbarInitial,
										    rescalingHadB_E,
										    rescalingHadB_P,
										    hadB_deltaEta,
										    hadB_deltaPhi,
										    rescalingLepB_E,
										    rescalingLepB_P,
										    lepB_deltaEta,
										    lepB_deltaPhi,
										    rescalingLightJets_E,
										    rescalingLightJets_P,
										    lightJets_deltaEta,
										    lightJets_deltaPhi,
										    rescalingLepton_E,
										    rescalingLepton_P,
										    lepton_deltaEta,
										    lepton_deltaPhi,
										    rescalingEtMissX,
										    rescalingEtMissY,
										    neutrinoPz,
										    m_rescaledJetCollection.get(),
										    RescaledLeptonCollectionBase<LeptonCollection>::m_rescaledLeptonCollection.get(),
										    this)),
	m_initialTTbar(ttbarInitial),
	m_rescalingHadB_E(rescalingHadB_E), 
	m_rescalingHadB_P(rescalingHadB_P),
	m_hadB_deltaEta(hadB_deltaEta),
	m_hadB_deltaPhi(hadB_deltaPhi),
	m_rescalingLepB_E(rescalingLepB_E), 
	m_rescalingLepB_P(rescalingLepB_P),
	m_lepB_deltaEta(lepB_deltaEta),
	m_lepB_deltaPhi(lepB_deltaPhi),
	m_rescalingLightJets_E(rescalingLightJets_E), 
	m_rescalingLightJets_P(rescalingLightJets_P),
	m_lightJets_deltaEta(lightJets_deltaEta),
	m_lightJets_deltaPhi(lightJets_deltaPhi),
	m_rescalingLepton_E(rescalingLepton_E), 
	m_rescalingLepton_P(rescalingLepton_P),
	m_lepton_deltaEta(lepton_deltaEta),
	m_lepton_deltaPhi(lepton_deltaPhi),
	m_rescalingEtMiss(-999999.), // senseless if m_rescalingEtMissX != m_rescalingEtMissY
	m_rescalingEtMissX(rescalingEtMissX),
	m_rescalingEtMissY(rescalingEtMissY),
	m_neutrinoPz(neutrinoPz), 
	m_permutationProbability(permutationProbability), 
	m_permutationDiscriminant(permutationDiscriminant) {;}

    //! default constructor is needed for POOL and dictionaries
    TTbarLJDiTopsFitted() {}

    //Getters
    const TTbarLJDiTops<LeptonCollection>& initialTTbar() const {return m_initialTTbar;}
    const boost::shared_ptr<JetCollection>& rescaledJetCollection() const {return m_rescaledJetCollection;}
    const boost::shared_ptr<LeptonCollection>& rescaledLeptonCollection() const {return RescaledLeptonCollectionBase<LeptonCollection>::m_rescaledLeptonCollection;}

    const double& rescalingHadB_E() const {return m_rescalingHadB_E;}
    const double& rescalingHadB_P() const {return m_rescalingHadB_P;}
    const double& hadB_deltaEta() const {return m_hadB_deltaEta;}
    const double& hadB_deltaPhi() const {return m_hadB_deltaPhi;}

    const double& rescalingLepB_E() const {return m_rescalingLepB_E;}
    const double& rescalingLepB_P() const {return m_rescalingLepB_P;}
    const double& lepB_deltaEta() const {return m_lepB_deltaEta;}
    const double& lepB_deltaPhi() const {return m_lepB_deltaPhi;}

    const std::vector<double>& rescalingLightJets_E() const {return m_rescalingLightJets_E;}
    const std::vector<double>& rescalingLightJets_P() const {return m_rescalingLightJets_P;}
    const std::vector<double>& lightJets_deltaEta() const {return m_lightJets_deltaEta;}
    const std::vector<double>& lightJets_deltaPhi() const {return m_lightJets_deltaPhi;}

    const double& rescalingLepton_E() const {return m_rescalingLepton_E;}
    const double& rescalingLepton_P() const {return m_rescalingLepton_P;}
    const double& lepton_deltaEta() const {return m_lepton_deltaEta;}
    const double& lepton_deltaPhi() const {return m_lepton_deltaPhi;}

    const double& rescalingEtMissX() const {return m_rescalingEtMissX;}
    const double& rescalingEtMissY() const {return m_rescalingEtMissY;}
    const double& neutrinoPz() const {return m_neutrinoPz;}

    const double& permutationProbability() const {return m_permutationProbability;}
    const double& permutationDiscriminant() const {return m_permutationDiscriminant;}
    
    //Old getters for backward compatibility (for jets: just return the rescaling factor for the energy)
    const double& rescalingHadB() const {return m_rescalingHadB_E;}
    const double& rescalingLepB() const {return m_rescalingLepB_E;}
    const std::vector<double>& rescalingLightsJets() const {return m_rescalingLightJets_E;}
    const double& rescalingEtMiss() const {return m_rescalingEtMiss;}

  };//class TTbarLJDiTopsFitted

  //====================================================================
  typedef TTbarLJDiTopsFitted<ElectronContainer>         TTbarEJDiTopsFitted;
  typedef TTbarLJDiTopsFitted<Analysis::MuonContainer>   TTbarMuJDiTopsFitted;
  typedef TTbarLJDiTopsFitted<Analysis::TauJetContainer> TTbarTauJDiTopsFitted;
  typedef TTbarLJDiTopsFitted<TruthParticleContainer>    TTbarTruthJDiTopsFitted;
  typedef TTbarLJDiTopsFitted<JetCollection>    TTbarJeJDiTopsFitted;
  //====================================================================
  //To do sorting of the vector using different criteria
  struct BiggestProbabilitySorting {
    template<class LeptonCollection>
    bool operator() (const TTbarLJDiTopsFitted<LeptonCollection>& firstTTbarLJDiTopsFitted, const TTbarLJDiTopsFitted<LeptonCollection>& secondTTbarLJDiTopsFitted) {
      return firstTTbarLJDiTopsFitted.permutationProbability() > secondTTbarLJDiTopsFitted.permutationProbability();
    }
  };//BiggestProbabilitySorting

  struct BiggestDiscriminantSorting {
    template<class LeptonCollection>
    bool operator() (const TTbarLJDiTopsFitted<LeptonCollection>& firstTTbarLJDiTopsFitted, const TTbarLJDiTopsFitted<LeptonCollection>& secondTTbarLJDiTopsFitted) {
      return firstTTbarLJDiTopsFitted.permutationDiscriminant() > secondTTbarLJDiTopsFitted.permutationDiscriminant();
    }
  };//BiggestDiscriminantSorting

  struct SmallestDiscriminantSorting {
    template<class LeptonCollection>
    bool operator() (const TTbarLJDiTopsFitted<LeptonCollection>& firstTTbarLJDiTopsFitted, const TTbarLJDiTopsFitted<LeptonCollection>& secondTTbarLJDiTopsFitted) { 
      return firstTTbarLJDiTopsFitted.permutationDiscriminant() < secondTTbarLJDiTopsFitted.permutationDiscriminant();
    }
  };//SmallestDiscriminantSorting
} // namespace TopRec

#endif //TTBARLJDITOPSFITTED_H

