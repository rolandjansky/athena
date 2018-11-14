/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GAMMAORTOOLS_IVGAMMAORTOOL_H
#define GAMMAORTOOLS_IVGAMMAORTOOL_H

#include <vector>
#include <map>

#include <xAODTruth/TruthParticleContainer.h>
#include <xAODTruth/TruthParticle.h>
#include "AsgTools/IAsgTool.h"

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

class IVGammaORTool: virtual public asg::IAsgTool {
public:
  ASG_TOOL_INTERFACE(IVGammaORTool)
  virtual ~IVGammaORTool(){};

  // Determine whether current event is in overlap region
  // Overlap region is defined by dR_lepton_photon_cut, photon_pT_cut
  // There are many more parameters to steer details of OR, see bottom
  // The truth photons and leptons required to perform OR are taken from the event analyzed
  virtual bool inOverlap() const=0;
  // Determine whether current event is in overlap region
  // Use leptons provided by user
  virtual bool inOverlap(const std::vector<TLorentzVector>& leptons) const=0;
  // Determine whether current event is in overlap region
  // Use leptons and photons provided by user
  virtual bool inOverlap(const std::vector<TLorentzVector>& leptons,
                 const std::vector<TLorentzVector>& photons) const=0;
  // Determine whether current event is in overlap region
  // Use leptons, photons, and photon origins provided by user
  // Tool will determine which photons are from relevant origins
  // Can be useful for derivations in which the truth record is missing but the MCTruthClassifier result is stored
  virtual bool inOverlap(const std::vector<TLorentzVector>& leptons,
                 const std::vector<TLorentzVector>& photons,
                 const std::vector<int>& photon_origins) const=0;
  // Determine whether current event is in overlap region
  // Use leptons, lepton origins, photons, and photon origins provided by user
  virtual bool inOverlap(const std::vector<TLorentzVector>& leptons,
                 const std::vector<int>& lepton_origins,
                 const std::vector<TLorentzVector>& photons,
                 const std::vector<int>& photon_origins) const=0;

  // Determine the pTs of photons outsidedR cuts that are configured in tool initialization
  // (dR_lepton_photon_cut)
  // Once these values are calculated they can be stored and used to easily perform OR with arbitrary pT
  virtual std::vector<float> photonPtsOutsideDr() const=0;
  // Determine the pTs of photons outsidedR cuts that are configured in tool initialization
  // (dR_lepton_photon_cut)
  // Use leptons provided by user
  virtual std::vector<float> photonPtsOutsideDr(const std::vector<TLorentzVector>& leptons) const=0;
  // Determine the pTs of photons outsidedR cuts that are configured in tool initialization
  // (dR_lepton_photon_cut)
  // Use leptons and photons provided by user
  virtual std::vector<float> photonPtsOutsideDr(const std::vector<TLorentzVector>& leptons,
						 const std::vector<TLorentzVector>& photons) const=0;
  // Determine the pTs of photons outsidedR cuts that are configured in tool initialization
  // (dR_lepton_photon_cut)
  // Use leptons, photons, and photon origins provided by user
  // Tool will determine which photons are from relevant origins
  // Can be useful for derivations in which the truth record is missing but the MCTruthClassifier result is stored
  virtual std::vector<float> photonPtsOutsideDr(const std::vector<TLorentzVector>& leptons,
                 const std::vector<TLorentzVector>& photons,
                 const std::vector<int>& photon_origins) const=0;
  // Determine the pTs of photons outsidedR cuts that are configured in tool initialization
  // (dR_lepton_photon_cut)
  // Use leptons, lepton origins, photons, and photon origins provided by user
  virtual std::vector<float> photonPtsOutsideDr(const std::vector<TLorentzVector>& leptons,
                 const std::vector<int>& lepton_origins,
                 const std::vector<TLorentzVector>& photons,
                 const std::vector<int>& photon_origins) const=0;

  // Determine the pTs of photons outside of several dR cuts that are configured in tool initialization
  // (dR_lepton_photon_cuts)
  // Once these values are calculated they can be stored and used to easily perform OR with arbitrary pT
  // and multiple dR cuts
  // There are many more parameters to steer details of OR, see bottom
  // The truth photons and leptons required to perform OR are taken from the event analyzed
  virtual std::map<float, std::vector<float> > photonPtsOutsideDrs() const=0;
  // Determine the pTs of photons outside of several dR cuts that are configured in tool initialization
  // (dR_lepton_photon_cuts)
  // Use leptons provided by user
  virtual std::map<float, std::vector<float> > photonPtsOutsideDrs(const std::vector<TLorentzVector>& photons) const=0;
  // Determine the pTs of photons outside of several dR cuts that are configured in tool initialization
  // (dR_lepton_photon_cuts)
  // Use leptons and photons provided by user
  virtual std::map<float, std::vector<float> > photonPtsOutsideDrs(const std::vector<TLorentzVector>& leptons,
                                                             const std::vector<TLorentzVector>& photons) const=0;
  // Determine the pTs of photons outside of several dR cuts that are configured in tool initialization
  // (dR_lepton_photon_cuts)
  // Use leptons, photons, and photon origins provided by user
  // Tool will determine which photons are from relevant origins
  // Can be useful for derivations in which the truth record is missing but the MCTruthClassifier result is stored
  virtual std::map<float, std::vector<float> > photonPtsOutsideDrs(const std::vector<TLorentzVector>& leptons,
                                                             const std::vector<TLorentzVector>& photons,
                                                             const std::vector<int>& photon_origins) const=0;
  // Determine the pTs of photons outside of several dR cuts that are configured in tool initialization
  // (dR_lepton_photon_cuts)
  // Determine whether current event is in overlap region
  // Use leptons, lepton origins, photons, and photon origins provided by user
  // Can be useful for derivations in which the truth record is missing but the MCTruthClassifier result is stored
  // Note: the first n_leptons leptons from good origins will be used,
  // the lepton vector should be sorted with this in mind
  virtual std::map<float, std::vector<float> > photonPtsOutsideDrs(const std::vector<TLorentzVector>& leptons,
                                                             const std::vector<int>& lepton_origins,
                                                             const std::vector<TLorentzVector>& photons,
                                                             const std::vector<int>& photon_origins) const=0;

  // Function determining whether a photon is frixione isolated from truthParticles
  // Parameters as defined in https://arxiv.org/pdf/hep-ph/9801442
  virtual bool frixioneIsolated(const xAOD::TruthParticle& photon,
                        const xAOD::TruthParticleContainer& truthParticles,
                        float dR0, float exponent, float epsilon) const=0;

  // Get final state (determined by status and barcode) photons (determined by pdg id) from truthParticleContainer
  // A minimum pT cut and isolation is applied according to tool configuration
  // Filter function is applied, only photons from relevant origins are kept
  virtual std::vector<TLorentzVector> getPhotonP4s(const xAOD::TruthParticleContainer& truthParticleContainer) const=0;
  // Get final state (determined by status and barcode) leptons (determined by pdg id) from truthParticleContainer
  // Filter function is applied, only leptons from relevant origins are kept
  virtual std::vector<TLorentzVector> getLeptonP4s(const xAOD::TruthParticleContainer& truthParticleContainer) const=0;
};



#endif //> !GAMMAORTOOLS_IVGAMMAORTOOL_H
