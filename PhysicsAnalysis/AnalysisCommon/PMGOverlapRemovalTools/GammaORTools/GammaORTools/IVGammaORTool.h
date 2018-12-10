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

class IVGammaORTool: virtual public asg::IAsgTool {
public:
  ASG_TOOL_INTERFACE(IVGammaORTool)

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
			       const std::vector<int>* photon_origins=0) const =0;

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
					const std::vector<int>* photon_origins=0) const=0;

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
					 const std::vector<int>* photon_origins=0) const=0;

  /// Function determining whether a photon is frixione isolated from truthParticles
  /// Parameters as defined in https://arxiv.org/pdf/hep-ph/9801442
  virtual bool frixioneIsolated(const xAOD::TruthParticle& photon,
				const xAOD::TruthParticleContainer& truthParticles,
				float dR0, float exponent, float epsilon) const=0;
};



#endif //> !GAMMAORTOOLS_IVGAMMAORTOOL_H
