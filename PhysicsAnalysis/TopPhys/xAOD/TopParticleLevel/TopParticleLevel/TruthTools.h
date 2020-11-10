/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

// Filename: TruthTools.h
// Description:
// Author: Fabian Wilk
// Created: Wed Feb  4 11:04:23 2015

#ifndef _TOP_PARTICLE_LEVEL_TRUTHTOOLS_H_
#define _TOP_PARTICLE_LEVEL_TRUTHTOOLS_H_

#include <ostream>
#include <iostream>
#include <string>

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "TopEvent/EventTools.h"
#include "TopDataPreparation/SampleXsection.h"
#include "xAODBase/IParticle.h"

namespace top {
  namespace truth {
    //Functions to decorate a muon with information on its truth-origin. Particle-level origin added on softmuons if config options SoftMuonAdditionalTruthInfo is True.
    //Its output should be regarded as experimental and thoroughly tested by analyzers before officially including it in the analysis.
    //It is strongly suggested to check its output against the true_origin (calculated by the MCTruthClassifier) to ensure behaviour is as expected.
    //Parton-level added if SoftMuonAdditionalTruthInfo is True and SoftMuonAdditionalTruthInfoCheckPartonOrigin is True.
    //Parton-level currently only usable on pythia8 and herwig7, will be automatically disabled for other PS generators.
    //Some decorations are already written in the default output ntuple, check EventSaverFlatNtuple.cxx
    //Some other are available for usage in custom savers:
    //<const xAOD::TruthParticle*>("truthMuonLink") -> link to the truth muon associated with reco (can be null if not found)
    //<top::LepParticleOriginFlag>("LepParticleOriginFlag") -> flag defined in TopEvent/EventTools.h
    //<top::LepPartonOriginFlag>("LepPartonOriginFlag") -> flag defined in TopEvent/EventTools.h
    //Then several <const xAOD::TruthParticle*>(...) decorations are also added (pointers are 0 when corresponding particle is not available):
    //truthMotherLink -> first direct mother of the truth muon
    //truthFirstNonLeptonMotherLink -> self-explanatory, will be different from truthMotherLink if the muon is from a Tau
    //truthBMotherLink -> first B-hadron encountered when going back through the chain of ancestors
    //truthCMotherLink -> first C-hadron encountered when going back through the chain of ancestors
    //The following are added when using the parton-level flag
    //truthPartonMotherLink -> link to first b/c quark encountered when going back through the chain of ancestors for muons from Heavy Flavour hadrons decays
    //truthTopMotherLink, truthWMotherLink, truthZMotherLink, truthPhotonMotherLink, truthHiggsMotherLink, truthBSMMotherLink -> link to first Top/W/Z/gamma*/Higgs/BSM-particle encountered when going back through the chain of ancestors

    void getTruthMuonHistory(const xAOD::TruthParticle* truth_muon, bool doPartonHistory, SampleXsection::showering shAlgo, bool verbose=false);
    void getTruthMuonPartonHistory(const xAOD::TruthParticle* truthmu, top::LepParticleOriginFlag lepParticleOriginFlag, const xAOD::TruthParticle* truthmu_Bmother, const xAOD::TruthParticle* truthmu_Cmother,  const xAOD::TruthParticle* truthmu_firstNonLeptonMother, SampleXsection::showering shAlgo, bool verbose=false);
    //helper methods
    top::LepParticleOriginFlag getTruthMuonFromTauHistory(const xAOD::TruthParticle* tauMother, const xAOD::TruthParticle* &truthmu_Bmother, const xAOD::TruthParticle* &truthmu_Cmother, const xAOD::TruthParticle* &truthmu_firstNonLeptonMother, bool verbose=false);
    top::LepParticleOriginFlag getTruthMuonFromCharmHistory(const xAOD::TruthParticle* &truthmu_Bmother, const xAOD::TruthParticle* truthmu_Cmother, bool verbose=false);
    
    //function that gets the truth muon associated from a reco muon, runs getTruthMuonHistory on it, and then takes care of propagating the decorations
    void getRecoMuonHistory(const xAOD::Muon* muon, bool doPartonHistory, SampleXsection::showering shAlgo, bool verbose=false);
    
    //helper methods
    const xAOD::TruthParticle* getInitialStateParticle(const xAOD::TruthParticle* truthParticle, bool verbose=false);
    const xAOD::TruthParticle* getFirstHFHadronOfSameFlavour(const xAOD::TruthParticle* truthParticle, bool verbose=false);
    const xAOD::TruthParticle* getTruthMuonAssociatedToRecoMuon(const xAOD::Muon* muon);
    //to initialize correctly all decorations to default values
    void initTruthMuonHistoryInfo(const xAOD::TruthParticle* truth_muon, bool doPartonHistory);
    void initRecoMuonHistoryInfo(const xAOD::Muon* muon, bool doPartonHistory);
    //additional method to share common code between the two methods above
    void initCommonMuonHistoryInfo(const xAOD::IParticle* muon, bool doPartonHistory);
    //helper methods to copy info from one muon to another
    void copyTruthMuonHistoryInfo(const xAOD::TruthParticle* tm_origin, const xAOD::TruthParticle* tm_target);
    void copyRecoMuonHistoryInfo(const xAOD::Muon* m_origin, const xAOD::Muon* m_target);
    //additional method to share common code between the two methods above
    void copyCommonMuonHistoryInfo(const xAOD::IParticle*  m_origin, const xAOD::IParticle* m_target);
    
    /*!
     * @brief Prints the decay chain leading up to a certain particle to a
     *  std::ostream.
     * @param part Pointer to xAOD::TruthParticle whose decay origin shall
     *  be printed.
     * @param os The std::ostream into which the decay chain shall be
     *  printed. Defaults to std::cout.
     * @param prefix A string which will be prefixed to the output.
     */
    void printDecayChain(const xAOD::TruthParticle* part,
                         std::ostream& os = std::cout,
                         const std::string& prefix = "");

    /**
     * @brief Check if a certain pdg ID corresponds to a B hadron.
     *  This follows the official PDG Monte-Carlo numbering scheme
     *	(c.f. http://pdg.lbl.gov/2007/reviews/montecarlorpp.pdf) which defines
     *	the PDG ID to be a 7 digit number in the form
     *		+- n_r n_L n_q1 n_q2 n_q3 n_J
     *  where
     *      n_q1 = 0, n_q2 >= n_q3	(for mesons)
     *      n_q1 >= n_q2 >= n_q3	(for baryons)
     *      n_q1 >= n_q2, n_q3 = 0	(for diquarks)
     *  the n_qi denote the flavour of the quark composing the hadron, i.e.
     *      n_q = 1   -> d		n_q = 2   -> u
     *      n_q = 3   -> s		n_q = 4   -> 4
     *      n_q = 5   -> b
     *  The digit \f$n_J\f$ encodes the total angular momentum \f$J\f$ of a
     *  compound system. \f$n_L\f$ is used to distinguish compound objects which
     *  have the same total angular momentum but different spin (\f$S\f$) and orbital
     *  (\f$L\f$) angular momentum numbers. Finally, \f$n_r\f$ is used to label
     *  mesons which are radially exited.
     *
     * @param pdgId The PDG ID to test.
     *
     * @return True if pdg ID corresponds to B hadron, false otherwise.
     */
    inline bool isBHadron(int pdgId) {
      return((510 < pdgId && pdgId < 555) ||  // Check for (normal) Mesons / Diquarks
             (5100 < pdgId && pdgId < 5555) ||// Check for (normal) Baryons
             (pdgId / 100) % 10 == 5 ||   // Check exited Mesons (and Baryons with double beauty)
             (pdgId / 1000) % 10 == 5);   // Check exited Baryons
    }

    /**
     * @brief Function to determine whether the input particle was produced
     * in the physical decay of a particle of given PDG ID.
     *
     * The algorithm uses the MC Truth Record.
     * The algorithm works by ascending through the decay chain following
     * the production vertex reference (obtained by the prodVtx() member
     * function of the xAOD::TruthParticle). From the input particle, the
     * algorithm ascends through the chain until the PDG ID of the particle
     * found as first incoming particle is not equal to the PDG ID of the
     * input particle. The last production vertex found during this ascent
     * should then correspond to the physical production vertex of the input
     * particle.
     *
     * @param truthParticle The input particle.
     * @param parentPDGIds A list of PDG IDs which are valid parents
     * @param bOnlyDirect A flag denoting whether "chained matching" is
     *  allowed. In chained matching mode, intermittend particles are
     *  allowed, i.e. for the chain
     *     A -> B -> C
     *  isFrom( C, A ) is true (for A != B != C). If only direct decays are
     *  considered, this is only the case if A == B or B == C. Defaults to false.
     *
     * @return Return true if the input particle was produced in the decay
     *         of a particle which has a PDG ID that is listed in
     *         parentPDGIds. Return false otherwise. Also return false if
     *         the input particle has an invalid production vertex reference.
     */
    bool isFrom(const xAOD::TruthParticle* truthParticle,
                const std::vector<int>& parentPDGIds,
                bool bOnlyDirect = false);

    /**
     * @brief Function to determine whether a given truth particle is a
     * result of a decay chain that started in the decay of a W or Z boson.
     *
     * The algorithm works by ascending through the decay chain following
     * the production vertex reference (obtained by the prodVtx() member
     * function of the xAOD::TruthParticle). From the input particle, the
     * algorithm ascends through the chain until the PDG ID of the particle
     * found as first incoming particle is not equal to the PDG ID of the
     * input particle. The last production vertex found during this ascent
     * should then correspond to the physical production vertex of the input
     * particle.
     *
     * If the PDG ID of the mother particle (i.e. the incoming particle to
     * the truth decay vertex) corresponds to a W / Z boson the algorithm
     * terminates directly, returning true. Otherwise a vertex-based check
     * is employed (which was designed to operate on SHERPA samples, c.f.
     * the JetSimTools-00-02-09 package
     * [svn: /Reconstruction/Jet/JetSimTools]).
     *
     * @param truthParticle The input particle.
     * @param bOnlyDirect A flag denoting whether "chained matching" is
     *  allowed. In chained matching mode, intermittend particles are
     *  allowed, i.e. for the chain
     *     A -> B -> C
     *  isFrom( C, A ) is true (for A != B != C). If only direct decays are
     *  considered, this is only the case if A == B or B == C. Defaults to false.
     *
     * @return Return true if the input particle was created in a physical
     *         decay of a W or Z boson. Otherwise return false. Will also
     *         return false if the prduction vertex of the input particle is
     *         invalid or has zero incoming particles.
     */
    bool isFromWZDecay(const xAOD::TruthParticle* truthParticle,
                       bool bOnlyDirect = false);

    /**
     * @brief Function the check if a particle is a lepton which was
     * produced in the decay of a tau lepton.
     *
     * @param truthParticle The input truth particle which will be checked.
     *
     * @return True if the input particle is a lepton and it has a parent
     *         which was a tau. False otherwise. Will also return false if
     *         the production vertex of the input particle is invalid.
     */
    bool isLeptonFromTau(const xAOD::TruthParticle* truthParticle);

    /**
     * @brief Check whether a given truth particle is a lepton.
     * @param truthParticle The input truth particle to be tested.
     * @return Return true if input is a lepton, false otherwise.
     */
    inline bool isLepton(const xAOD::TruthParticle* truthParticle) {
      return(10 < truthParticle->absPdgId() && truthParticle->absPdgId() < 17);
    }

    /*!
     * @brief Check whether a given truth particle is not produced in the
     *  decay of physical hadrons or their descendents. This must be done in
     *  a recursive manner, i.e. the decay chain must be traversed upwards
     *  until the initial state is found.
     * @param truthParticle The truth particle to be tested.
     * @param bOnlyDirect A flag denoting whether "chained matching" is
     *  allowed. In chained matching mode, intermittend particles are
     *  allowed, i.e. for the chain
     *     A -> B -> C
     *  isFrom( C, A ) is true (for A != B != C). If only direct decays are
     *  considered, this is only the case if A == B or B == C. Defaults to
     *  false.
     * @param bTauIsHadron If true, any intermediate tauons are treated as hadrons.
     * @return True if truthParticle does not descend from a physical
     *  hadron, false otherwise.
     */
    bool isNotFromHadron(const xAOD::TruthParticle* truthParticle,
                         bool bOnlyDirect = false, bool bTauIsHadron = false);
  }
}

#endif /* _TOP_PARTICLE_LEVEL_TRUTHTOOLS_H_ */
