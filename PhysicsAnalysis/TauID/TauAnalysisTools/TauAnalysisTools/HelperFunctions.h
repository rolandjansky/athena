/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_HELPERFUNCTIONS_H
#define TAUANALYSISTOOLS_HELPERFUNCTIONS_H

/*
  author: Dirk Duschinger
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
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"

namespace TauAnalysisTools
{
void split(const std::string& sInput, const char cDelim, std::vector<std::string>& vOut);
void split(TEnv& rEnv, const std::string& sIn, const char cDelim, std::vector<std::string>& vOut);
void split(TEnv& rEnv, const std::string& sIn, const char cDelim, std::vector<size_t>& vOut);
void split(TEnv& rEnv, const std::string& sIn, const char cDelim, std::vector<int>& vOut);
void split(TEnv& rEnv, const std::string& sIn, const char cDelim, std::vector<float>& vOut);
void split(TEnv& rEnv, const std::string& sIn, const char cDelim, std::vector<double>& vOut);
double tauPt(const xAOD::TauJet& xTau);
double caloTauPt(const xAOD::TauJet& xTau);
double caloTauEta(const xAOD::TauJet& xTau);
double caloTauAbsEta(const xAOD::TauJet& xTau);
double caloTauP(const xAOD::TauJet& xTau);
double tauP(const xAOD::TauJet& xTau);
double tauEta(const xAOD::TauJet& xTau);
double tauAbsEta(const xAOD::TauJet& xTau);
double tauLeadTrackEta(const xAOD::TauJet& xTau);

bool testFileForEOFContainsCharacters(std::string sFileName);
void createPi0Vectors(const xAOD::TauJet* xTau, std::vector<TLorentzVector>& vPi0s);
void correctedPi0Vectors(const xAOD::TauJet* xTau, std::vector<TLorentzVector>& correctedPi0s, TLorentzVector& TauP4);
void truthHadrons(const xAOD::TruthParticle* xTruthTau, std::vector<const xAOD::TruthParticle*>& vChargedHadrons, std::vector<const xAOD::TruthParticle*>& vNeutralHadrons);
void truthHadrons(const xAOD::TauJet* xTau, std::vector<const xAOD::TruthParticle*>& vChargedHadrons, std::vector<const xAOD::TruthParticle*>& vNeutralHadrons);

}

#endif // not TAUANALYSISTOOLS_HELPERFUNCTIONS_H
