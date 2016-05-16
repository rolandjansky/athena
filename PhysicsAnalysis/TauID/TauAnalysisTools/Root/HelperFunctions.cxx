/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>

// local include(s)
#include "TauAnalysisTools/HelperFunctions.h"

#ifdef XAODTAU_VERSIONS_TAUJET_V3_H
xAOD::TauJetParameters::PanTauDetails PANTAU_DECAYMODE=xAOD::TauJetParameters::PanTau_DecayMode;
#else
xAOD::TauJetParameters::PanTauDetails PANTAU_DECAYMODE=xAOD::TauJetParameters::pantau_CellBasedInput_DecayMode;
#endif

using namespace TauAnalysisTools;

//______________________________________________________________________________
void TauAnalysisTools::split(const std::string& sInput, const char cDelim, std::vector<std::string>& vOut)
{
  std::stringstream sSS(sInput);
  std::string sItem;
  while (std::getline(sSS, sItem, cDelim))
    vOut.push_back(sItem);
}

//______________________________________________________________________________
void TauAnalysisTools::split(TEnv& rEnv, const std::string& sKey, const char cDelim, std::vector<std::string>& vOut)
{
  std::stringstream sSS(rEnv.GetValue(sKey.c_str(),""));
  std::string sItem;
  while (std::getline(sSS, sItem, cDelim))
    vOut.push_back(sItem);
}

//______________________________________________________________________________
void TauAnalysisTools::split(TEnv& rEnv, const std::string& sKey, const char cDelim, std::vector<size_t>& vOut)
{
  std::stringstream sSS(rEnv.GetValue(sKey.c_str(),""));
  std::string sItem;
  while (std::getline(sSS, sItem, cDelim))
    vOut.push_back(size_t(stoi(sItem)));
}

//______________________________________________________________________________
void TauAnalysisTools::split(TEnv& rEnv, const std::string& sKey, const char cDelim, std::vector<int>& vOut)
{
  std::stringstream sSS(rEnv.GetValue(sKey.c_str(),""));
  std::string sItem;
  while (std::getline(sSS, sItem, cDelim))
    vOut.push_back(stoi(sItem));
}

//______________________________________________________________________________
void TauAnalysisTools::split(TEnv& rEnv, const std::string& sKey, const char cDelim, std::vector<float>& vOut)
{
  std::stringstream sSS(rEnv.GetValue(sKey.c_str(),""));
  std::string sItem;
  while (std::getline(sSS, sItem, cDelim))
    vOut.push_back(stof(sItem));
}

//______________________________________________________________________________
void TauAnalysisTools::split(TEnv& rEnv, const std::string& sKey, const char cDelim, std::vector<double>& vOut)
{
  std::stringstream sSS(rEnv.GetValue(sKey.c_str(),""));
  std::string sItem;
  while (std::getline(sSS, sItem, cDelim))
    vOut.push_back(stod(sItem));
}

//______________________________________________________________________________
double TauAnalysisTools::tauPt(const xAOD::TauJet& xTau)
{
  // return tau pt in GeV
  return xTau.pt()/1000.;
}

//______________________________________________________________________________
double TauAnalysisTools::tauP(const xAOD::TauJet& xTau)
{
  // return tau P in GeV
  return xTau.p4().P()/1000.;
}

//______________________________________________________________________________
double TauAnalysisTools::tauEta(const xAOD::TauJet& xTau)
{
  // return tau eta
  return xTau.eta();
}

//______________________________________________________________________________
double TauAnalysisTools::tauAbsEta(const xAOD::TauJet& xTau)
{
  // return absolute tau eta
  return fabs(xTau.eta());
}

//______________________________________________________________________________
double TauAnalysisTools::tauLeadTrackEta(const xAOD::TauJet& xTau)
{
  // return lead tau track eta
  double dTrackEta = 0;
  double dTrackMaxPt = 0;
  for( unsigned int iNumTrack = 0; iNumTrack < xTau.nTracks(); iNumTrack++)
  {
    if (xTau.track(iNumTrack)->pt() > dTrackMaxPt)
    {
      dTrackMaxPt = xTau.track(iNumTrack)->pt();
      dTrackEta = xTau.track(iNumTrack)->eta();
    }
  }
  return dTrackEta;
}

//______________________________________________________________________________
bool TauAnalysisTools::testFileForEOFContainsCharacters(std::string sFileName)
{
  // returns true if last line in file is empty or the line starts with the
  // number sign #

  std::ifstream fInputFile;
  fInputFile.open(sFileName);
  if(!fInputFile.is_open())
    return true;

  fInputFile.seekg(-1,fInputFile.end);

  bool bKeepLooping = true;
  while(bKeepLooping)
  {
    char ch;
    fInputFile.get(ch);

    if((int)fInputFile.tellg() <= 1)
    {
      fInputFile.seekg(0);
      bKeepLooping = false;
    }
    else if(ch == '\n')
      bKeepLooping = false;
    else
      fInputFile.seekg(-2,fInputFile.cur);
  }

  std::string sLastLine;
  getline(fInputFile,sLastLine);
  fInputFile.close();

  return (sLastLine.size() == 0 or sLastLine[0] == '#');
}

//______________________________________________________________________________
void TauAnalysisTools::createPi0Vectors(const xAOD::TauJet* xTau, std::vector<TLorentzVector>& vPi0s)
{
  // reset the pi0s
  vPi0s.clear();

  // // not needed
  // xAOD::Vertex xPrimaryVertex = *xTau->vertex();

  // Since the PFO links as they come out of reconstruction, only correspond to
  // calorimeter clusters, whereas we want the consts_pi0 vectors to correspond
  // to real pi0s, we need to be careful to collect the PFOs correctly to pi0s
  // for the cases where number of pi0s does not match to the decay mode:
  size_t iNumPi0PFO = xTau->nPi0PFOs();

  int iDecayMode = -1;
  xTau->panTauDetail(PANTAU_DECAYMODE, iDecayMode);

  if (iDecayMode == xAOD::TauJetParameters::DecayMode::Mode_1p1n && iNumPi0PFO > 1)
  {
    // TODO: find out if the pi0 mass is defined elsewhere in atlas code!
    // float fMassPi0 = 134.98;
    float fMassPi0Squared = 18219.6004;

    // combine both photons (with 0 mass from Pantau) to one pi0 vector:
    const xAOD::PFO* xPfo1 = xTau->pi0PFO(0);
    const xAOD::PFO* xPfo2 = xTau->pi0PFO(1);
    vPi0s.push_back(xPfo1->p4() + xPfo2->p4());

    // re-set the mass to one pi0:
    double dNewMomentum = std::sqrt(vPi0s[0].E() * vPi0s[0].E() - fMassPi0Squared);
    vPi0s[0].SetPxPyPzE(vPi0s[0].Vect().Unit().Px() * dNewMomentum,
                        vPi0s[0].Vect().Unit().Py() * dNewMomentum,
                        vPi0s[0].Vect().Unit().Pz() * dNewMomentum,
                        vPi0s[0].E());
  }
  else if (iDecayMode == xAOD::TauJetParameters::DecayMode::Mode_1pXn && iNumPi0PFO == 1)
  {
    // make a single pi0 from a PFO that contains two pi0s:
    const xAOD::PFO* xPfo = xTau->pi0PFO(0);
    // add the 2-pi0 vector preliminarily to the pi0vector:
    vPi0s.push_back(xPfo->p4());

    // re-set the mass back to one pi0:
    double dNewMomentum = std::sqrt(vPi0s[0].E() * vPi0s[0].E() - vPi0s[0].M() / 2. * vPi0s[0].M() / 2.);
    vPi0s[0].SetVectM(vPi0s[0].Vect() * (dNewMomentum / vPi0s[0].P()), vPi0s[0].M() / 2.);

    // create another pi0 from the same vector:
    vPi0s.push_back(vPi0s[0]);
  }
  else
  {
    // if it's not any of the special cases above then just collect the PFOs:
    for (size_t iPFO = 0; iPFO < iNumPi0PFO; iPFO++)
    {
      vPi0s.push_back(xTau->pi0PFO(iPFO)->p4());
    }
  }
}

