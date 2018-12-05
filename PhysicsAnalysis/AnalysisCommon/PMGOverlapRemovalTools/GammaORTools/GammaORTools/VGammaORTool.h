/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GAMMAORTOOLS_VGAMMAORTOOL_H
#define GAMMAORTOOLS_VGAMMAORTOOL_H

#include <vector>
#include <map>
#include <string>
#include "boost/current_function.hpp"
#include "MCTruthClassifier/MCTruthClassifier.h"
#include <xAODTruth/TruthParticleContainer.h>
#include <xAODTruth/TruthParticle.h>
#include "AsgTools/AsgTool.h"
#include "GammaORTools/IVGammaORTool.h"

/*******************************************************************************
 +++ Overview
   This tool can be used to identify the overlap of a V+jets and V+gamma sample
   or in fact between most V^n+gamma^m and V^n+gamma^(m-1) samples
   (e.g. V+gammagamma vs V+gamma or VV+gamma vs VV).
   It can either decide whether an event is in the overlap region for fixed cuts on photons and lepton
   or be used to extract relevant information from the event so that the overlap removal (OR) can be performed
   at a later stage with a simple 'if' statement.

   Currently documented at
   https://twiki.cern.ch/twiki/bin/view/Main/VGammaORTool

 ********************************************************************************/

class VGammaORTool: public asg::AsgTool, virtual public IVGammaORTool {
  ASG_TOOL_CLASS(VGammaORTool, IVGammaORTool)
public:
  VGammaORTool(const std::string& name, const std::vector<float>& photon_pT_cuts={});
  virtual ~VGammaORTool() override;

  /// Update the truth leptons and photons used to perform the OR
  /// * If null pointers are given for leptons or photons, they will be taken from the event
  /// *  One can override this behaviour by manually specifying vectors
  ///   (this can be used if the full truth record is not available or broken)
  /// * If lepton or photon origins are given in addition to the lepton four vectors,
  ///   particles from irrelevant origins (e.g. decays) are filtered out
  ///   This can be useful when working with derivations which store the origin but not the
  ///   full truth record
  virtual StatusCode updateInput(const std::vector<TLorentzVector>* leptons=0,
				 const std::vector<TLorentzVector>* photons=0,
				 const std::vector<int>* lepton_origins=0,
				 const std::vector<int>* photon_origins=0) override;

  /// Determine whether current event is in overlap region
  /// The overlap region is defined by dR_lepton_photon_cut, photon_pT_cut
  /// and many more configurable parameters
  virtual bool inOverlap() const override;

  /// Determine the pTs of photons outside the dR cut that is configured in tool initialization
  /// (dR_lepton_photon_cut)
  /// Once these values are calculated they can be stored and used to easily perform OR with arbitrary pT
  virtual std::vector<float> photonPtsOutsideDr() const override;

  /// Determine the pTs of photons outside of several dR cuts that are configured in tool initialization
  /// (dR_lepton_photon_cuts)
  /// Once these values are calculated they can be stored and used to easily perform OR with arbitrary pT
  /// and multiple dR cuts
  virtual std::map<float, std::vector<float> > photonPtsOutsideDrs() const override;

  /// Function determining whether a photon is frixione isolated from truthParticles
  /// Parameters as defined in https://arxiv.org/pdf/hep-ph/9801442
  virtual bool frixioneIsolated(const xAOD::TruthParticle& photon,
                        const xAOD::TruthParticleContainer& truthParticles,
                        float dR0, float exponent, float epsilon) const override;

  /// Get final state (determined by status and barcode) photons (determined by pdg id) from truthParticleContainer
  /// A minimum pT cut and isolation is applied according to tool configuration
  /// Filter function is applied, only photons from relevant origins are kept
  virtual std::vector<TLorentzVector> getPhotonP4s(const xAOD::TruthParticleContainer& truthParticleContainer) const override;

  /// Get final state (determined by status and barcode) leptons (determined by pdg id) from truthParticleContainer
  /// Filter function is applied, only leptons from relevant origins are kept
  virtual std::vector<TLorentzVector> getLeptonP4s(const xAOD::TruthParticleContainer& truthParticleContainer) const override;

 private:

  // This function performs the actual algorithm needed for OR, if one of the public functions is called
  // It returns a vector of ordered photon pTs for multiple lepton-photon dR cuts
  std::map<float, std::vector<float> > photonPtsOutsideDrs(const std::vector<float>& drCuts) const;

  // Filter out photons whose origin is among the veto origins (typically photons from hadron decays)
  std::vector<TLorentzVector> filterPhotonOrigins(const std::vector<TLorentzVector>&, const std::vector<int>&) const;

  // Filter out leptons whose origin is among the veto origins (typically photons from hadron decays)
  // Return list containing all leptons from prefered origins (from hard process) first
  // As fallback leptons from origins not explictly vetoed (e.g. unkown origins) are added at the end of the list
  std::vector<TLorentzVector> filterLeptonOrigins(const std::vector<TLorentzVector>&, const std::vector<int>&) const;

  // Check whether a lepton originates from tau decay 
  bool isFromTau(const xAOD::TruthParticle& lepton, int nRecursions=0) const;
  
  // Internal helper function to check wheter photon pTs are compatible with configured overlap cuts
  bool checkPhotonPts(const std::vector<float>& photon_pts) const;

  // Implementation of Eq 3.4 in  https://arxiv.org/pdf/hep-ph/9801442
  float frixioneFunc(float dR, float dR0, float exponent, float epsilon) const;

  // MCTruthClassifier
  // Is pointer because otherwise non-const function m_truthClassifier.particleTruthClassifier would spoil constness
  MCTruthClassifier* m_truthClassifier;

  // Makes sure input lepton and photons are set before overlap functions are called
  bool m_setInput=false;

  // stores photons used in OR
  std::vector<TLorentzVector> m_photons;

  // stores leptons used in OR
  std::vector<TLorentzVector> m_leptons;

  // CONFIGURATION =================================================

  // Maximum barcode considered for final state particles (exclude particles from Geant)
  int m_max_barcode;

  // Number of leptons expected in process (e.g. 2 for Z+gamma, 1 for W+gamma, 4 for ZZ+gamma)
  // If this is set to a negative number, all possible leptons are considered
  // Per default this includes leptons from unkown origins, so it is more robuts to set a positive value
  // for m_n_leptons as a upper limit
  int m_n_leptons;
  // Lepton flavours considered,  signed pdgIds  (only relevant if leptons are not provided by user)
  // If possible, restricting flavors can increase the robustness
  std::vector<int> m_lepton_pdgIds;
  // Lepton origins preferred when searching prompt leptons
  // These origins should be associated with the hard process
  // Origin codes are defined here:
  // https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/MCTruthClassifier/trunk/MCTruthClassifier/MCTruthClassifierDefs.h
  std::vector<int> m_preferred_lepton_origins;
  // Lepton origins of leptons never used as prompt
  // These should be e.g. hadron origins
  std::vector<int> m_lepton_veto_origins;

  // Photon origins vetoed when searching prompt leptons
  // These should be photons from hadron/tau decays
  // Origin codes are defined here:
  // https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/MCTruthClassifier/trunk/MCTruthClassifier/MCTruthClassifierDefs.h
  std::vector<int> m_veto_photon_origins;

    // p_T (y) cut(s) used in inOverlap function -- only 1 for Vgamma OR, 2 for Vgammagamma
  std::vector<float> m_photon_pT_cuts;
  // delta R (l,y) cut for used in inOverlap and photonPtsOutsideDr function
  float m_dR_lepton_photon_cut;
  // List of dR cuts for which the highest photon pT will be determined by photonPtsOutsideDrs function
  std::vector<float> m_dR_lepton_photon_cuts;

  // Require photons to be Frixione isolated (https://arxiv.org/abs/hep-ph/9801442)?
  bool m_use_gamma_iso;
  // Isolation cone used for Frixione isolation
  float m_frixione_dR;
  // exponent in Eq 3.4 in https://arxiv.org/pdf/hep-ph/9801442
  float m_frixione_exponent;
  // epsilon in Eq 3.4 in https://arxiv.org/pdf/hep-ph/9801442
  float m_frixione_epsilon;
  // Which stable particles are *not* considered in isolation? Usually leptons and photons.
  std::vector<int> m_abs_pdgids_excluded_from_iso;

  // Minimum photon pT considered in algorithm -- increase speeds up algorithm as less photons need to be classified
  float m_min_considered_photon_pT;

  // Name of truth particle collection
  std::string m_truthparticle_collection_name;
};



#endif //> !GAMMAORTOOLS_VGAMMAORTOOL_H
