/**
 * @file HelperFunctions.h
 * @author Guillermo Hamity (ghamity@cern.ch)
 * @author: Dirk Duschinger
 * @brief 
 * @date 2021-02-19
 * 
 * @copyright Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 * 
 */

#ifndef TAUANALYSISTOOLS_HELPERFUNCTIONS_H
#define TAUANALYSISTOOLS_HELPERFUNCTIONS_H

/*
  mail: dirk.duschinger@cern.ch
  documentation in: ../README.rst
                    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/tags/TauAnalysisTools-<tag>/README.rst
		    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/README.rst
*/

// stl include(s)
#include <vector>
#include <string>
#include <sstream>

// ROOT include(s)
#include "TEnv.h"

// EDM include(s):
#include "xAODTau/TauJet.h"
#include "xAODTau/TauxAODHelpers.h"
#include "xAODTau/DiTauJet.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"

// Local include(s):
#include "TauAnalysisTools/Enums.h"

namespace TauAnalysisTools
{
void split(const std::string& sInput, const char cDelim, std::vector<std::string>& vOut);
void split(TEnv& rEnv, const std::string& sIn, const char cDelim, std::vector<std::string>& vOut);
void split(TEnv& rEnv, const std::string& sIn, const char cDelim, std::vector<size_t>& vOut);
void split(TEnv& rEnv, const std::string& sIn, const char cDelim, std::vector<int>& vOut);
void split(TEnv& rEnv, const std::string& sIn, const char cDelim, std::vector<float>& vOut);
void split(TEnv& rEnv, const std::string& sIn, const char cDelim, std::vector<double>& vOut);
/** return tau pt in GeV*/
double tauPt(const xAOD::TauJet& xTau);
/** return tau P in GeV*/
double tauP(const xAOD::TauJet& xTau);
/** return tau eta*/
double tauEta(const xAOD::TauJet& xTau);
/** return absolute tau eta*/
double tauAbsEta(const xAOD::TauJet& xTau);
/** return MVA based tau pt in GeV*/
double finalTauPt(const xAOD::TauJet& xTau);
/** return MVA based tau eta*/
double finalTauEta(const xAOD::TauJet& xTau);
/** return MVA based absolute tau eta*/
double finalTauAbsEta(const xAOD::TauJet& xTau);
/** return MVA based tau P in GeV*/
double finalTauP(const xAOD::TauJet& xTau);
/** return leading charge tau track eta*/
double tauLeadTrackEta(const xAOD::TauJet& xTau);
/** return truth match tau pt in GeV (if hadronic truth tau match)*/
double truthTauPt(const xAOD::TauJet& xTau);
/** return truth match tau eta (if hadronic truth tau match)*/
double truthTauAbsEta(const xAOD::TauJet& xTau);
/** return truth decay mode (if hadronic truth tau match)*/
double truthDecayMode(const xAOD::TauJet& xTau);
const xAOD::TruthParticle* getTruth(const xAOD::TauJet& xTau);
/**
 * @brief Get the Truth Decay Mode from TruthTau particle
 * 
 * Counts p0 and charged Kaons and pions
 * @param xTruthTau TruthParticle
 * @return xAOD::TauJetParameters::DecayMode 
 * @note Method returns Mode_Error if TruthParticle is not decorated as hadronic TruthTau
 */
xAOD::TauJetParameters::DecayMode getTruthDecayMode(const xAOD::TruthParticle& xTruthTau);
/** Get the Truth Decay Mode from TruthTau particle.
 * check hadronic truth match and get getTruthDecayMode from truth particle
 */
xAOD::TauJetParameters::DecayMode getTruthDecayMode(const xAOD::TauJet& xTau);
/**
 * @brief Count truth matched decay particles of a particular PDGID
 * 
 * @param xTruthTau TruthTau
 * @param iPdgId PDGID to count over
 * @param bCompareAbsoluteValues true to ignore pdgid charge
 * @return int 
 */
int getNTauDecayParticles(const xAOD::TruthParticle& xTruthTau, int iPdgId, bool bCompareAbsoluteValues);
/** returns true if last line in file is empty or the line starts with the number sign */
bool testFileForEOFContainsCharacters(std::string sFileName);
void createPi0Vectors(const xAOD::TauJet* xTau, std::vector<TLorentzVector>& vPi0s);
void correctedPi0Vectors(const xAOD::TauJet* xTau, std::vector<TLorentzVector>& correctedPi0s, TLorentzVector& TauP4);
/** return charged and neutral daughters of truth tau */
void truthHadrons(const xAOD::TruthParticle* xTruthTau, std::vector<const xAOD::TruthParticle*>& vChargedHadrons, std::vector<const xAOD::TruthParticle*>& vNeutralHadrons);
/** return charged and neutral daughters of hadronic truth matched taujet */
void truthHadrons(const xAOD::TauJet* xTau, std::vector<const xAOD::TruthParticle*>& vChargedHadrons, std::vector<const xAOD::TruthParticle*>& vNeutralHadrons);
/** return TauJet match type*/
e_TruthMatchedParticleType getTruthParticleType(const xAOD::TauJet& xTau);
/** return DiTauJet match type*/
e_TruthMatchedParticleType getTruthParticleType(const xAOD::DiTauJet& xDiTau); 
double average_mu(const xAOD::TauJet& xTau);
void set_mu(unsigned int mu);
}

#endif // not TAUANALYSISTOOLS_HELPERFUNCTIONS_H
