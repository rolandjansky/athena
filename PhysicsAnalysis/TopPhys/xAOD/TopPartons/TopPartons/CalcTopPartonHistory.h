/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

// $Id: CalcTopPartonHistory.h 800464 2017-03-13 18:06:24Z tpelzer $
#ifndef ANALYSISTOP_TOPPARTONS_CALCTOPPARTONHISTORY_H
#define ANALYSISTOP_TOPPARTONS_CALCTOPPARTONHISTORY_H

/**
 * @author John Morris <john.morris@cern.ch>
 * @author Silvestre Marino Romano <sromanos@cern.ch>
 * @author Samuel Calvet <scalvet@cern.ch>
 *
 * @brief CalcTopPartonHistory
 *   Determine the history of the top partons
 *
 * $Revision: 800464 $
 * $Date: 2017-03-13 18:06:24 +0000 (Mon, 13 Mar 2017) $
 *
 **/

// system include(s):
#include <memory>
#include <vector>

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AthContainers/DataVector.h"
#include "AthContainers/AuxElement.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "TopPartons/PartonHistory.h"

// forward declaration(s):
namespace top {
  class TopConfig;
}

namespace top {
  class CalcTopPartonHistory: public asg::AsgTool {
  public:
    explicit CalcTopPartonHistory(const std::string& name);
    virtual ~CalcTopPartonHistory() {};

    CalcTopPartonHistory(const CalcTopPartonHistory& rhs) = delete;
    CalcTopPartonHistory(CalcTopPartonHistory&& rhs) = delete;
    CalcTopPartonHistory& operator = (const CalcTopPartonHistory& rhs) = delete;

    /** Store the four-momentum of the post-FSR top or anti-top found using statusCodes
    *   This would only work if there is at most one "true" top of each charge (i.e. won't work for SS tops or 4 tops)
    *   This code was adapted from the 7TeV parton-level differential ttbar routine:
    * https://svnweb.cern.ch/trac/atlasphys-top/browser/Physics/Top/Software/MCvalidation/Rivet/Rivet2.X/trunk/routines/ATLAS_2014_I1304289/ATLAS_2014_I1304289.cc
    */
    bool topAfterFSR_SC(const xAOD::TruthParticleContainer* truthParticles, int start,
                        TLorentzVector& top_afterFSR_SC_p4);

    ///Store the four-momentum of several particles in the top decay chain
    bool topWb(const xAOD::TruthParticleContainer* truthParticles, int start, TLorentzVector& t_beforeFSR_p4,
               TLorentzVector& t_afterFSR_p4, TLorentzVector& W_p4, TLorentzVector& b_p4, TLorentzVector& Wdecay1_p4,
               int& Wdecay1_pdgId, TLorentzVector& Wdecay2_p4, int& Wdecay2_pdgId);
    bool topWq(const xAOD::TruthParticleContainer* truthParticles, int start, TLorentzVector& t_beforeFSR_p4,
               TLorentzVector& t_afterFSR_p4, TLorentzVector& W_p4, TLorentzVector& q_p4, int& q_pdgId,
               TLorentzVector& Wdecay1_p4, int& Wdecay1_pdgId, TLorentzVector& Wdecay2_p4, int& Wdecay2_pdgId);

    ///Store the four-momentum of b (not from tops_ before and after FSR
    bool b(const xAOD::TruthParticleContainer* truthParticles, TLorentzVector& b_beforeFSR, TLorentzVector& b_afterFSR);

    ///Store the four-momentum of several particles in the W decay chain
    bool Wlv(const xAOD::TruthParticleContainer* truthParticles, TLorentzVector& W_p4, TLorentzVector& Wdecay1_p4,
             int& Wdecay1_pdgId, TLorentzVector& Wdecay2_p4, int& Wdecay2_pdgId);

    /// Store the four-momentum of several particles in W decay chain for W that is NOT from top in Wt ST events
    bool Wt_W(const xAOD::TruthParticleContainer* truthParticles, TLorentzVector& W_p4, int& W_pdgId,
              TLorentzVector& Wdecay1_p4, int& Wdecay1_pdgId, TLorentzVector& Wdecay2_p4, int& Wdecay2_pdgId);

    /// Store the four-momentum of b quark that is NOT from top in Wt(b) ST events
    bool Wt_b(const xAOD::TruthParticleContainer* truthParticles, TLorentzVector& b_beforeFSR,
              TLorentzVector& b_afterFSR, int& b_pdgId);

    /// Store the four-momentum of photon coming from virtual top in ttgamma events
    bool topPhWb(const xAOD::TruthParticleContainer* truthParticles, int topId, TLorentzVector& t_beforeFSR_p4,
                 TLorentzVector& t_afterFSR_p4, TLorentzVector& Ph_p4, TLorentzVector& W_p4, TLorentzVector& b_p4,
                 TLorentzVector& Wdecay1_p4, int& Wdecay1_pdgId, TLorentzVector& Wdecay2_p4, int& Wdecay2_pdgId,
                 bool& has_ph, int& BranchType, int& IniPartonType, bool& missingTop);

    ///Return particle after FSR (before the decay vertex)
    const xAOD::TruthParticle* findAfterFSR(const xAOD::TruthParticle* particle);

    ///Return true when particle is a top before FSR
    bool hasParticleIdenticalParent(const xAOD::TruthParticle* particle);
    
    virtual StatusCode execute();
  protected:
    std::shared_ptr<top::TopConfig> m_config;

    void fillEtaBranch(xAOD::PartonHistory* partonHistory, std:: string branchName, TLorentzVector& tlv);
    
    /** used to build container from multiple collections
    *   in DAOD_PHYS we don't have the TruthParticles collection, so we have to build a TruthParticleContainer (named out_contName) by merging several collections; this is stored in the evtStore
    *   this method has to use some tricks, like the helper m_tempParticles ConstDataVector, due to the desing of DataVector, see https://twiki.cern.ch/twiki/bin/view/AtlasComputing/DataVector
    */
    StatusCode buildContainerFromMultipleCollections(const std::vector<std::string> &collections, const std::string& out_contName);
    
    /** currently in DAOD_PHYS TruthTop have links to Ws from the TruthBoson collection, which have no link to their decay products;
    *   we have therefore to associate the W from the TruthBoson collections to those in the TruthBosonsWithDecayParticles collection.
    *   This method will use the helper method decorateCollectionWithLinksToAnotherCollection to decorate bosons in the TruthBoson collection with 
    *   "AT_linkToTruthBosonsWithDecayParticles", which is a link to the same bosons in the TruthBosonsWithDecayParticles collection
    */
    StatusCode linkBosonCollections();
    
    ///helper method to handle retriveing the truth particle linked in the decoration of another particle
    const xAOD::TruthParticle* getTruthParticleLinkedFromDecoration(const xAOD::TruthParticle* part, const std::string &decorationName);
    
  private:
    /**helper method currently used in DAOD_PHYS to link particles from a given collection to the same particles included in another collection;
    *  needed because particles may be duplicated in different collections, but their navigation links may only be there in some of them...
    */
    StatusCode decorateCollectionWithLinksToAnotherCollection(const std::string &collectionToDecorate, const std::string &collectionToLink, const std::string &nameOfDecoration);
    
  };
}

#endif
