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
#include "AsgTools/AnaToolHandle.h"
#include "GammaORTools/IVGammaORTool.h"

/****************************************************************************//**
*
*   This tool can be used to identify the overlap of a V+jets and V+gamma sample
*   or in fact between most V^n+gamma^m and V^n+gamma^(m-1) samples
*   (e.g. V+gammagamma vs V+gamma or VV+gamma vs VV).
*   It can either decide whether an event is in the overlap region for fixed cuts on photons and lepton
*   or be used to extract relevant information from the event so that the overlap removal (OR) can be performed
*   at a later stage with a simple 'if' statement.
*
*   The OR functions (inOverlap(result), photonPtsOutsideDr(result), or photonPtsOutsideDrs(result)),
*   set the result via reference and return a status code.
*   Usually the tool will take leptons and photons from the current event but this behavior can be
*   overwritten by explicitly setting leptons and photons as additional arguments to the functions.
*
*   Further documented at
*   https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/VGammaORTool
*
*********************************************************************************/

class VGammaORTool: public asg::AsgTool, virtual public IVGammaORTool {
  ASG_TOOL_CLASS(VGammaORTool, IVGammaORTool)
public:
  VGammaORTool(const std::string& name);
  virtual ~VGammaORTool() override;

  /// Determine whether current event is in overlap region (set via reference).
  /// The overlap region is defined by dR_lepton_photon_cut, photon_pT_cut
  /// and many more configurable parameters.
  /// Note that the function returns a status code.
  /// The first agument is the result, the remaining arguments are optional:
  /// * If null pointers are given for leptons or photons, they will be taken from the event
  /// * One can override this behaviour by manually specifying vectors of lepton/photon
  //    four vectors (can be used if the full truth record is not available or broken)
  /// * If lepton or photon origins are given in addition to the lepton four vectors,
  ///   particles from irrelevant origins (e.g. decays) are filtered out
  ///   This can be useful when working with derivations which store the origin but not the
  ///   full truth record
  virtual StatusCode inOverlap(bool& result,
			       const std::vector<TLorentzVector>* leptons=0,
			       const std::vector<TLorentzVector>* photons=0,
			       const std::vector<int>* lepton_origins=0,
			       const std::vector<int>* photon_origins=0) const override;

  /// Determine the pTs of photons outside the dR cut that is configured in tool initialization
  /// (dR_lepton_photon_cut). Calculates a vector of photon pts, ordered by descending pt.
  /// Once these values are calculated they can be stored and used to easily perform OR with arbitrary pT.
  /// Note that the function returns a status code.
  /// The first agument is the result, the remaining arguments are optional
  /// * If null pointers are given for leptons or photons, they will be taken from the event
  /// * One can override this behaviour by manually specifying vectors of lepton/photon
  //    four vectors (can be used if the full truth record is not available or broken)
  /// * If lepton or photon origins are given in addition to the lepton four vectors,
  ///   particles from irrelevant origins (e.g. decays) are filtered out
  ///   This can be useful when working with derivations which store the origin but not the
  ///   full truth record
  virtual StatusCode photonPtsOutsideDr(std::vector<float>& result,
					const std::vector<TLorentzVector>* leptons=0,
					const std::vector<TLorentzVector>* photons=0,
					const std::vector<int>* lepton_origins=0,
					const std::vector<int>* photon_origins=0) const override;

  /// Determine the pTs of photons outside of several dR cuts that are configured in tool initialization
  /// (dR_lepton_photon_cuts). Calculates a mapping between dRs and vector of photon pts (ordered by descending pt).
  /// Once these values are calculated they can be stored and used to easily perform OR with arbitrary pT
  /// and multiple dR cuts. Note that the function returns a status code.
  /// The first agument is the result, the remaining arguments are optional
  /// * If null pointers are given for leptons or photons, they will be taken from the event
  /// * One can override this behaviour by manually specifying vectors of lepton/photon
  //    four vectors (can be used if the full truth record is not available or broken)
  /// * If lepton or photon origins are given in addition to the lepton four vectors,
  ///   particles from irrelevant origins (e.g. decays) are filtered out
  ///   This can be useful when working with derivations which store the origin but not the
  ///   full truth record
  virtual StatusCode photonPtsOutsideDrs(std::map<float, std::vector<float> >& result,
					 const std::vector<TLorentzVector>* leptons=0,
					 const std::vector<TLorentzVector>* photons=0,
					 const std::vector<int>* lepton_origins=0,
					 const std::vector<int>* photon_origins=0) const override;

  /// Function determining whether a photon is frixione isolated from truthParticles
  /// Parameters as defined in https://arxiv.org/pdf/hep-ph/9801442
  virtual bool frixioneIsolated(const xAOD::TruthParticle& photon,
                        const xAOD::TruthParticleContainer& truthParticles,
                        float dR0, float exponent, float epsilon) const override;


  /// Get final state (determined by status and barcode) photons (determined by pdg id) from truthParticleContainer
  /// A minimum pT cut and isolation is applied according to tool configuration
  /// Filter function is applied, only photons from relevant origins are kept
  std::vector<TLorentzVector> getPhotonP4s(const xAOD::TruthParticleContainer& truthParticleContainer) const;

  /// Get final state (determined by status and barcode) leptons (determined by pdg id) from truthParticleContainer
  /// Filter function is applied, only leptons from relevant origins are kept
  std::vector<TLorentzVector> getLeptonP4s(const xAOD::TruthParticleContainer& truthParticleContainer) const;

private:

  // This function performs the actual algorithm needed for OR, if one of the public functions is called
  // It returns a vector of ordered photon pTs for multiple lepton-photon dR cuts
  StatusCode photonPtsOutsideDrs(std::map<float, std::vector<float> >& result,
				 const std::vector<float>& drCuts,
				 const std::vector<TLorentzVector>* leptons=0,
				 const std::vector<TLorentzVector>* photons=0,
				 const std::vector<int>* lepton_origins=0,
				 const std::vector<int>* photon_origins=0) const;

  // Set the right leptons, get them either from user or the current event
  StatusCode setInput(std::vector<TLorentzVector>& leptons_out,
		      std::vector<TLorentzVector>& photons_out,
		      const std::vector<TLorentzVector>* lepton_p4s,
		      const std::vector<TLorentzVector>* photon_p4s,
		      const std::vector<int>* lepton_origins,
		      const std::vector<int>* photon_origins) const;

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
  asg::AnaToolHandle<MCTruthClassifier> m_truthClassifier;

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
