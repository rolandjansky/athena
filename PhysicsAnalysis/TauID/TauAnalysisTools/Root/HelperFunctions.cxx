/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>

// local include(s)
#include "TauAnalysisTools/HelperFunctions.h"
#include "TF1.h"

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
double TauAnalysisTools::caloTauPt(const xAOD::TauJet& xTau)
{
  // return calo based tau pt in GeV
  return xTau.auxdata<float>("ptTauEtaCalib")/1000.;
}

//______________________________________________________________________________
double TauAnalysisTools::caloTauEta(const xAOD::TauJet& xTau)
{
  // return calo based tau eta
  return xTau.auxdata<float>("etaTauEtaCalib");
}

//______________________________________________________________________________
double TauAnalysisTools::caloTauAbsEta(const xAOD::TauJet& xTau)
{
  // return calo based absolute tau eta
  return std::abs(xTau.auxdata<float>("etaTauEtaCalib"));
}

//______________________________________________________________________________
double TauAnalysisTools::caloTauP(const xAOD::TauJet& xTau)
{
  TLorentzVector tlv;
  tlv.SetPtEtaPhiM( xTau.auxdata<float>("ptTauEtaCalib"),
                    xTau.auxdata<float>("etaTauEtaCalib"),
                    xTau.auxdata<float>("phiTauEtaCalib"),
                    xTau.auxdata<float>("mTauEtaCalib") );
  // return tau P in GeV
  return tlv.P()/1000.;
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

  // Since the PFO links as they come out of reconstruction, only correspond to
  // calorimeter clusters, whereas we want the consts_pi0 vectors to correspond
  // to real pi0s, we need to be careful to collect the PFOs correctly to pi0s
  // for the cases where number of pi0s does not match to the decay mode:
  size_t iNumPi0PFO = xTau->nPi0PFOs();

  int iDecayMode = -1;

  if (!(xTau->panTauDetail(PANTAU_DECAYMODE, iDecayMode)))
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
  xTau->panTauDetail(PANTAU_DECAYMODE, iDecayMode);

  if (!(xTau->panTauDetail(PANTAU_DECAYMODE, iDecayMode)))
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
    
    //Get sum of the chargedPFOP4s
    TLorentzVector Sum_ChrgPFOP4;
    std::vector< ElementLink< xAOD::PFOContainer > > finalChrgPFOLinks = xTau->chargedPFOLinks();
    unsigned int NCharged    = finalChrgPFOLinks.size();
    for(unsigned int iPFO=0; iPFO<NCharged; iPFO++) {
      const xAOD::PFO* pfo = finalChrgPFOLinks.at(iPFO).cachedElement();
      Sum_ChrgPFOP4 += pfo->p4();
    }
    
    //Get tau FinalCalib P4
    TLorentzVector FinalCalibP4;
    FinalCalibP4.SetPtEtaPhiM( xTau->auxdata<float>("ptFinalCalib") ,xTau->auxdata<float>("etaFinalCalib") ,xTau->auxdata<float>("phiFinalCalib") ,xTau->auxdata<float>("mFinalCalib") );
    
    //Calculate the difference 3-vector between FinalCalib and Sum of chargedPFOP4
    double px = FinalCalibP4.Px() - Sum_ChrgPFOP4.Px();
    double py = FinalCalibP4.Py() - Sum_ChrgPFOP4.Py();
    double pz = FinalCalibP4.Pz() - Sum_ChrgPFOP4.Pz();

    double p_correctedPi0s = sqrt( pow(px,2.) + pow(py,2.) + pow(pz,2.) );
    double p_vPi0s = Sum_vPi0s.P();

    //Calucate scale factor for the pi0 3-vector momentum
    double X;
    X = p_correctedPi0s/p_vPi0s;

    //Scale the pi0s with X and recalculate the new pi0 energy
    double px_scaled, py_scaled, pz_scaled, e;
    double mPi0 = 134.977;
    for(unsigned int i = 0; i < vPi0s.size() ; i++){
      px_scaled = vPi0s[i].Px() * X;
      py_scaled = vPi0s[i].Py() * X;
      pz_scaled = vPi0s[i].Pz() * X;
      e = sqrt( pow(px_scaled,2.) + pow(py_scaled,2.) + pow(pz_scaled,2.) + pow(mPi0,2.) );

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
    TF1 DeltaRdist;
    DeltaRdist = TF1("DeltaRdist", "pol3", 0, 67500);
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
    correctedPi0_0.SetPtEtaPhiM( correctedPi0s[0].Pt()/cos(0.5*deltaR/sqrt(2.0)), correctedPi0s[0].Eta()+0.5*deltaR/sqrt(2.0), correctedPi0s[0].Phi()+0.5*deltaR/sqrt(2.0), correctedPi0s[0].M() );
    correctedPi0_1.SetPtEtaPhiM( correctedPi0s[1].Pt()/cos(0.5*deltaR/sqrt(2.0)), correctedPi0s[1].Eta()-0.5*deltaR/sqrt(2.0), correctedPi0s[1].Phi()-0.5*deltaR/sqrt(2.0), correctedPi0s[1].M() );

    std::vector<TLorentzVector> AngleCorrectedPi0s;
    AngleCorrectedPi0s.push_back(correctedPi0_0);
    AngleCorrectedPi0s.push_back(correctedPi0_1);

    //Reparametrise: Delta R -> mass of pi0 Cluster
    TLorentzVector PionCluster_angleCorrected = AngleCorrectedPi0s[0]+AngleCorrectedPi0s[1];
    
    double dNewMomentum = std::sqrt(PionCluster_angleCorrected.E()/2 * PionCluster_angleCorrected.E()/2 - PionCluster_angleCorrected.M() / 2. * PionCluster_angleCorrected.M() / 2.);
    correctedPi0s[0].SetVectM(PionCluster_angleCorrected.Vect() * (dNewMomentum / PionCluster_angleCorrected.P()), PionCluster_angleCorrected.M() / 2.);
    correctedPi0s[1] = correctedPi0s[0];
  }



  //Calculate the new tau P4
  std::vector< ElementLink< xAOD::PFOContainer > > finalChrgPFOLinks = xTau->chargedPFOLinks();
  TLorentzVector SumChargedPionP4;
  for(unsigned int iPFO=0; iPFO < finalChrgPFOLinks.size(); iPFO++) {
    const xAOD::PFO* pfo = finalChrgPFOLinks.at(iPFO).cachedElement();
    TauP4 += pfo->p4();
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

  static SG::AuxElement::Accessor<Link_t> accTruthParticleLink("truthParticleLink");
  const Link_t xTruthTauLink = accTruthParticleLink(*xTau);
  const xAOD::TruthParticle* xTruthTau = xTruthTauLink.cachedElement();

  if (xTruthTau!=nullptr && xTruthTau->auxdata<char>("IsHadronicTau"))
  {
    truthHadrons(xTruthTau, vChargedHadrons, vNeutralHadrons);
  }

  return;  
}
