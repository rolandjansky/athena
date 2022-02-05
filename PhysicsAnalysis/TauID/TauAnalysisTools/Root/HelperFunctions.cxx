/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>

// local include(s)
#include "TauAnalysisTools/HelperFunctions.h"
#include "TruthUtils/PIDHelpers.h"
#include "TF1.h"

#ifdef ASGTOOL_ATHENA
#include "CLHEP/Units/SystemOfUnits.h"
using CLHEP::GeV;
#else
#define GeV 1000
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
void TauAnalysisTools::split(TEnv& rEnv, const std::string& sKey, const char cDelim, std::vector<unsigned>& vOut)
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
  return std::abs(xTau.eta());
}

//______________________________________________________________________________
double TauAnalysisTools::finalTauPt(const xAOD::TauJet& xTau)
{
  // return MVA based tau pt in GeV
  return xTau.ptFinalCalib()/GeV;
}

//______________________________________________________________________________
double TauAnalysisTools::finalTauEta(const xAOD::TauJet& xTau)
{
  // return MVA based tau eta
  return xTau.etaFinalCalib();
}

//______________________________________________________________________________
double TauAnalysisTools::finalTauAbsEta(const xAOD::TauJet& xTau)
{
  // return MVA based absolute tau eta
  return std::abs(xTau.etaFinalCalib());
}

//______________________________________________________________________________
double TauAnalysisTools::finalTauP(const xAOD::TauJet& xTau)
{
  // return tau P in GeV
  return xTau.p4(xAOD::TauJetParameters::FinalCalib).P()/GeV;
}

//______________________________________________________________________________
double TauAnalysisTools::tauLeadTrackEta(const xAOD::TauJet& xTau)
{
  // return leading charge tau track eta
  double dTrackEta = 0.;
  double dTrackMaxPt = 0.;
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
double TauAnalysisTools::truthTauPt(const xAOD::TauJet& xTau)
{
  // return truth tau Pt in GeV
  const xAOD::TruthParticle* xTruthTau = getTruth(xTau);

  // if there is a truth tau return pT, otherwise return 0 (getTruth will print an error)
  if (xTruthTau!=nullptr && xTruthTau->auxdata<char>("IsHadronicTau"))
    return xTruthTau->pt()/GeV;
  else
    return 0.;
}

//______________________________________________________________________________
double TauAnalysisTools::truthTauAbsEta(const xAOD::TauJet& xTau)
{
  // return truth tau absolute eta
  const xAOD::TruthParticle* xTruthTau = getTruth(xTau);

  // if there is a truth tau return absolute eta, otherwise return -5 (getTruth will print an error)
  if (xTruthTau!=nullptr && xTruthTau->auxdata<char>("IsHadronicTau"))
    return std::abs(xTruthTau->eta());
  else
    return -5.;
}

//______________________________________________________________________________
double TauAnalysisTools::truthDecayMode(const xAOD::TauJet& xTau)
{
  // return truth tau decay mode.
  int iDecayMode = getTruthDecayMode(xTau);
  return static_cast<double>(iDecayMode);
}

//______________________________________________________________________________
const xAOD::TruthParticle* TauAnalysisTools::getTruth(const xAOD::TauJet& xTau)
{
  typedef ElementLink< xAOD::TruthParticleContainer > Link_t;
  if (!xTau.isAvailable< Link_t >("truthParticleLink"))
  {
    Error("TauAnalysisTools::getTruth", "No truth match information available. Please run TauTruthMatchingTool first");
  }

  static const SG::AuxElement::ConstAccessor<Link_t> accTruthParticleLink("truthParticleLink");
  const Link_t xTruthTauLink = accTruthParticleLink(xTau);
  const xAOD::TruthParticle* xTruthTau = xTruthTauLink.cachedElement();

  return xTruthTau;
}


//______________________________________________________________________________
xAOD::TauJetParameters::DecayMode TauAnalysisTools::getTruthDecayMode(const xAOD::TauJet& xTau)
{
  const xAOD::TruthParticle* xTruthTau = getTruth(xTau);

  if (xTruthTau!=nullptr && xTruthTau->auxdata<char>("IsHadronicTau"))
    return getTruthDecayMode(*xTruthTau);
  else
    return xAOD::TauJetParameters::Mode_Error;
}

//______________________________________________________________________________
xAOD::TauJetParameters::DecayMode TauAnalysisTools::getTruthDecayMode(const xAOD::TruthParticle& xTruthTau)
{
  if (!(xTruthTau.isAvailable<size_t>("numCharged")))
  {
    Warning("TauAnalysisTools::getTruthDecayMode", "passed truth particle is not a truth tau, return Mode_Error");
    return xAOD::TauJetParameters::Mode_Error;
  }

  int iCharged = getNTauDecayParticles(xTruthTau,MC::PID::PIPLUS, true) + getNTauDecayParticles(xTruthTau,MC::PID::KPLUS, true);
  int iNeutral = getNTauDecayParticles(xTruthTau,MC::PID::PI0, true);
  if (iCharged == 1)
  {
    if (iNeutral == 0) return xAOD::TauJetParameters::DecayMode::Mode_1p0n;
    if (iNeutral == 1) return xAOD::TauJetParameters::DecayMode::Mode_1p1n;
    if (iNeutral >= 2) return xAOD::TauJetParameters::DecayMode::Mode_1pXn;
  }
  else if (iCharged == 3)
  {
    if (iNeutral == 0) return xAOD::TauJetParameters::DecayMode::Mode_3p0n;
    if (iNeutral >= 1) return xAOD::TauJetParameters::DecayMode::Mode_3pXn;
  }

  if (iCharged == 2 or iCharged == 4 or iCharged == 5)
    return xAOD::TauJetParameters::DecayMode::Mode_Other;
  if (iCharged == 0 or iCharged >=6)
    return xAOD::TauJetParameters::DecayMode::Mode_NotSet;

  // if you got here, something should have gone wrong
  return xAOD::TauJetParameters::DecayMode::Mode_Error;
}

//______________________________________________________________________________
int TauAnalysisTools::getNTauDecayParticles(const xAOD::TruthParticle& xTruthTau, int iPdgId, bool bCompareAbsoluteValues)
{
  int iNum = 0;
  if (!xTruthTau.isAvailable<std::vector<int>>("DecayModeVector"))
  {
    Warning("TauAnalysisTools::getNTauDecayParticles", "passed truth particle is not a truth tau, return 0");
    return 0;
  }

  static const SG::AuxElement::ConstAccessor<std::vector<int> > accDecayModeVector("DecayModeVector");
  for(auto iPdgId2 : accDecayModeVector(xTruthTau))
    if (!bCompareAbsoluteValues)
    {
      if (iPdgId2 == iPdgId) iNum++;
    }
    else
    {
      if (std::abs(iPdgId2) == std::abs(iPdgId)) iNum++;
    }
  return iNum;
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

  // Since the PFO links as they come out of reconstruction, only correspond to
  // calorimeter clusters, whereas we want the consts_pi0 vectors to correspond
  // to real pi0s, we need to be careful to collect the PFOs correctly to pi0s
  // for the cases where number of pi0s does not match to the decay mode:
  size_t iNumPi0PFO = xTau->nPi0PFOs();

  int iDecayMode = -1;

  if (!(xTau->panTauDetail(xAOD::TauJetParameters::PanTauDetails::PanTau_DecayMode, iDecayMode)))
  {
    std::cerr <<"Failed to retrieve panTauDetail decay mode\n";
    return;
  }

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
    double dNewMomentum = std::sqrt(vPi0s[0].E() / 2 * vPi0s[0].E() / 2 - vPi0s[0].M() / 2. * vPi0s[0].M() / 2.);
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


//______________________________________________________________________________
void TauAnalysisTools::correctedPi0Vectors(const xAOD::TauJet* xTau, std::vector<TLorentzVector>& correctedPi0s, TLorentzVector& TauP4){
  //reset the pi0s
  correctedPi0s.clear();

  int iDecayMode = -1;  

  if (!(xTau->panTauDetail(xAOD::TauJetParameters::PanTauDetails::PanTau_DecayMode, iDecayMode)))
  {
    std::cerr <<"Failed to retrieve panTauDetail decay mode\n";
    return;
  }

  //Reading in the pi0 vector from createPi0Vectors
  std::vector<TLorentzVector> vPi0s;
  createPi0Vectors(xTau,vPi0s);

  if(iDecayMode == xAOD::TauJetParameters::DecayMode::Mode_1p1n || iDecayMode == xAOD::TauJetParameters::DecayMode::Mode_1pXn || iDecayMode == xAOD::TauJetParameters::DecayMode::Mode_3pXn){  
    //Adding up Pi0 P4s from createPi0Vectors
    TLorentzVector Sum_vPi0s;
    for(unsigned int i = 0; i < vPi0s.size() ; i++){
      Sum_vPi0s += vPi0s[i];
    }
    
    //Get sum of the chargedPFO (i.e. tau track) p4
    TLorentzVector Sum_ChrgPFOP4;
    for(const xAOD::TauTrack* track : xTau->tracks()) {
      Sum_ChrgPFOP4 += track->p4();
    }
    
    //Get tau FinalCalib P4 (explicitly requiring p4(xAOD::TauJetParameters::TauCalibType::FinalCalib) should be superfluous, as FinalCalib is the default p4)
    TLorentzVector FinalCalibP4 = xTau->p4();
    
    //Calculate the difference 3-vector between FinalCalib and Sum of chargedPFOP4
    double px = FinalCalibP4.Px() - Sum_ChrgPFOP4.Px();
    double py = FinalCalibP4.Py() - Sum_ChrgPFOP4.Py();
    double pz = FinalCalibP4.Pz() - Sum_ChrgPFOP4.Pz();

    double p_correctedPi0s = std::sqrt( std::pow(px,2.) + std::pow(py,2.) + std::pow(pz,2.) );
    double p_vPi0s = Sum_vPi0s.P();

    //Calucate scale factor for the pi0 3-vector momentum
    double X = p_correctedPi0s/p_vPi0s;

    //Scale the pi0s with X and recalculate the new pi0 energy
    double px_scaled, py_scaled, pz_scaled, e;
    double mPi0 = 134.977;
    for(unsigned int i = 0; i < vPi0s.size() ; i++){
      px_scaled = vPi0s[i].Px() * X;
      py_scaled = vPi0s[i].Py() * X;
      pz_scaled = vPi0s[i].Pz() * X;
      e = std::sqrt( std::pow(px_scaled,2.) + std::pow(py_scaled,2.) + std::pow(pz_scaled,2.) + std::pow(mPi0,2.) );

      //Append the corrected pi0P4 to correctedPi0s
      TLorentzVector P4_correctedPi0s;
      P4_correctedPi0s.SetPxPyPzE(px_scaled,py_scaled,pz_scaled,e);
      correctedPi0s.push_back(P4_correctedPi0s);
    }
  }else{
    correctedPi0s = vPi0s;
  }

  //Correct angles between pi0s for 1pXn decays with 1 cluster
  if(iDecayMode == xAOD::TauJetParameters::DecayMode::Mode_1pXn && xTau->nPi0PFOs() == 1){

    //Get Function of Delta R between the two Pi0s
    TF1 DeltaRdist("DeltaRdist", "pol3", 0, 67500);
    DeltaRdist.SetParameter(0, 0.07924);
    DeltaRdist.SetParameter(1, -2.078/1000000.);
    DeltaRdist.SetParameter(2,  2.619/100000000000.);
    DeltaRdist.SetParameter(3, -1.238/10000000000000000.);
  
    //Get Sum of pi0 P4.Pt()
    TLorentzVector SumPi0_P4;
    for( unsigned int i = 0 ; i < correctedPi0s.size() ; i++){
      SumPi0_P4 += correctedPi0s[i];
    }

    float SumPi0_pt = SumPi0_P4.Pt();

    //Get delta R value (mean of true DeltaR distribution)
    float deltaR;
    if(SumPi0_pt >= 67500){
      deltaR = 0.020; // = DeltaRdist.Eval(67500);
    } else{
      deltaR = DeltaRdist.Eval(SumPi0_pt);
    }

    TLorentzVector correctedPi0_0, correctedPi0_1;
    correctedPi0_0.SetPtEtaPhiM( correctedPi0s[0].Pt()/cos(0.5*deltaR/std::sqrt(2.0)), correctedPi0s[0].Eta()+0.5*deltaR/std::sqrt(2.0), correctedPi0s[0].Phi()+0.5*deltaR/std::sqrt(2.0), correctedPi0s[0].M() );
    correctedPi0_1.SetPtEtaPhiM( correctedPi0s[1].Pt()/cos(0.5*deltaR/std::sqrt(2.0)), correctedPi0s[1].Eta()-0.5*deltaR/std::sqrt(2.0), correctedPi0s[1].Phi()-0.5*deltaR/std::sqrt(2.0), correctedPi0s[1].M() );

    std::vector<TLorentzVector> AngleCorrectedPi0s;
    AngleCorrectedPi0s.push_back(correctedPi0_0);
    AngleCorrectedPi0s.push_back(correctedPi0_1);

    //Reparametrise: Delta R -> mass of pi0 Cluster
    TLorentzVector PionCluster_angleCorrected = AngleCorrectedPi0s[0]+AngleCorrectedPi0s[1];
    
    double dNewMomentum = std::sqrt(PionCluster_angleCorrected.E()/2. * PionCluster_angleCorrected.E()/2. - PionCluster_angleCorrected.M() / 2. * PionCluster_angleCorrected.M() / 2.);
    correctedPi0s[0].SetVectM(PionCluster_angleCorrected.Vect() * (dNewMomentum / PionCluster_angleCorrected.P()), PionCluster_angleCorrected.M() / 2.);
    correctedPi0s[1] = correctedPi0s[0];
  }

  //Calculate the new tau P4
  for(const xAOD::TauTrack* track : xTau->tracks()) {
    TauP4 += track->p4();
  }

  for(unsigned int iPi0=0; iPi0 < correctedPi0s.size(); iPi0++) {
    TauP4 += correctedPi0s[iPi0];
  }

}

//______________________________________________________________________________
void TauAnalysisTools::truthHadrons(const xAOD::TruthParticle* xTruthTau, std::vector<const xAOD::TruthParticle*>& vChargedHadrons, std::vector<const xAOD::TruthParticle*>& vNeutralHadrons)
{
  vChargedHadrons.clear();
  vNeutralHadrons.clear();

  // skip this tau if it has no decay vertex
  if ( !xTruthTau->hasDecayVtx() )
  {
    Warning("TauAnalysisTools::truthHadrons", "Passed truth particle has no decay vertex.");
    return;
  }

  // get vertex and check if it is valid
  const xAOD::TruthVertex* xDecayVertex = xTruthTau->decayVtx();
  if (!xDecayVertex)
  {
    Warning("TauAnalysisTools::truthHadrons", "Passed truth particle has no valid decay vertex.");
    return;
  }

  // loop over outgoing particles 
  for ( size_t iOutgoingParticle = 0; iOutgoingParticle < xDecayVertex->nOutgoingParticles(); ++iOutgoingParticle )
  {
    const xAOD::TruthParticle* xTruthDaughter = xDecayVertex->outgoingParticle(iOutgoingParticle);
    if (!xTruthDaughter)
    {
      Warning("TauAnalysisTools::truthHadrons", "Truth daughter of tau decay was not found. Please ensure that this container has the full tau decay information or produce the TruthTaus container in AtlasDerivation.\nInformation on how to do this can be found here:\nhttps://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/TauPreRecommendations2015#Accessing_Tau_Truth_Information");
      return;
    }

    // if tau decays into tau this is not a proper tau decay
    if ( xTruthDaughter->isTau() )
    {
      Warning("TauAnalysisTools::truthHadrons", "Tau decays into a tau itself. Skip this decay");
      return;
    }

    // ignore electrons, muons and neutrinos
    if (xTruthDaughter->isElectron() or xTruthDaughter->isMuon() or xTruthDaughter->isNeutrino())
      continue;

    if (xTruthDaughter->isCharged())
      vChargedHadrons.push_back(xTruthDaughter);
    else 
      vNeutralHadrons.push_back(xTruthDaughter);
  }

  return;
  
}

//______________________________________________________________________________
void TauAnalysisTools::truthHadrons(const xAOD::TauJet* xTau, std::vector<const xAOD::TruthParticle*>& vChargedHadrons, std::vector<const xAOD::TruthParticle*>& vNeutralHadrons)
{
  vChargedHadrons.clear();
  vNeutralHadrons.clear();

  // check if reco tau is a truth hadronic tau
  typedef ElementLink< xAOD::TruthParticleContainer > Link_t;
  if (!xTau->isAvailable< Link_t >("truthParticleLink"))
  {
    Error("TauAnalysisTools::truthHadrons", "No truth match information available. Please run TauTruthMatchingTool first");
  }

  static const SG::AuxElement::ConstAccessor<Link_t> accTruthParticleLink("truthParticleLink");
  const Link_t xTruthTauLink = accTruthParticleLink(*xTau);
  const xAOD::TruthParticle* xTruthTau = xTruthTauLink.cachedElement();

  if (xTruthTau!=nullptr && xTruthTau->auxdata<char>("IsHadronicTau"))
  {
    truthHadrons(xTruthTau, vChargedHadrons, vNeutralHadrons);
  }

  return;  
}

//______________________________________________________________________________
e_TruthMatchedParticleType TauAnalysisTools::getTruthParticleType(const xAOD::TauJet& xTau)
{
  typedef ElementLink< xAOD::TruthParticleContainer > Link_t;
  if (!xTau.isAvailable< Link_t >("truthParticleLink"))
    Error("TauAnalysisTools::getTruthParticleType", "No truth match information available. Please run TauTruthMatchingTool first.");

  const xAOD::TruthParticle* xTruthParticle = xAOD::TauHelpers::getTruthParticle(&xTau);
  if (xTruthParticle)
  {
    if (xTruthParticle->isTau())
      {
      static const SG::AuxElement::ConstAccessor<char> accIsHadronicTau("IsHadronicTau");
      if ((bool)accIsHadronicTau(*xTruthParticle))
        return TruthHadronicTau;
      else
        return TruthLeptonicTau;
    }
    if (xTruthParticle->isMuon())
      return TruthMuon;
    if (xTruthParticle->isElectron())
      return TruthElectron;
  }
  
  // TODO: use const xAOD::Jet* xTruthJet = xAOD::TauHelpers::getLink<xAOD::Jet>(&xTau, "truthJetLink");
  // currently it is unavailable as templated class is not in icc file
  static const SG::AuxElement::ConstAccessor< ElementLink< xAOD::JetContainer > > accTruthJetLink("truthJetLink");
  const ElementLink< xAOD::JetContainer > lTruthParticleLink = accTruthJetLink(xTau);
  if (lTruthParticleLink.isValid())
    return TruthJet;

  return Unknown;
}

e_TruthMatchedParticleType TauAnalysisTools::getTruthParticleType(const xAOD::DiTauJet& xDiTau)
{
  if (!xDiTau.isAvailable<char>("IsTruthHadronic"))
    Error("TauAnalysisTools::getTruthParticleType", "No truth match information available. Please run DiTauTruthMatchingTool first");
  static const SG::AuxElement::ConstAccessor<char> accIsTruthHadronic("IsTruthHadronic");

  e_TruthMatchedParticleType eTruthMatchedParticleType = Unknown;

  if (accIsTruthHadronic(xDiTau))
    eTruthMatchedParticleType = TruthHadronicDiTau;

  return eTruthMatchedParticleType;
}
// This double is needed to save the average/actual mu for the y-axis in CommonEfficiencyTool.
// The new trigger systematics (from tag 00-03-14 onwards) use mu dependent values.
// The functions average_mu() and set_mu() are also needed to support this.
double l_Mu;

//______________________________________________________________________________
double TauAnalysisTools::average_mu(const xAOD::TauJet& /*xTau*/)
{
  return l_Mu;
}

//______________________________________________________________________________
void TauAnalysisTools::set_mu(unsigned int mu)
{
  l_Mu=mu;
}
