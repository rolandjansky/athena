/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Class handling the input of the MissingMassCalculator
// author Michael Huebner <michael.huebner@no.spam.cern.ch>

// Local include(s):
#include "DiTauMassTools/MissingMassInput.h"

using namespace DiTauMassTools;

// Default Constructor
MissingMassInput::MissingMassInput(){
}

// Default Desctructor
MissingMassInput::~MissingMassInput(){
}

//------- clearing input
void MissingMassInput::ClearInput(){

  if(m_fUseVerbose == 1){ Info("DiTauMassTools", "MissingMassCalculator::ClearInput()"); }

  m_MetVec.Set(0.,0.);
  m_vistau1.SetPtEtaPhiM(0.,0.,0.,0.);
  m_vistau2.SetPtEtaPhiM(0.,0.,0.,0.);

  m_type_visTau1 = 0;
  m_type_visTau2 = 0;
  m_Nprong_tau1 = 0;
  m_Nprong_tau2 = 0;
  m_dataType = 0;
  m_Njet25 = 0;

  m_METcovphi = 0.;
  m_METsigmaP = 0.;
  m_METsigmaL = 0.;
  m_SumEt = 0.;
  m_DelPhiTT = 0.;
  m_MHtSigma1 = 0.;
  m_MHtSigma2 = 0.;
  m_MHtGaussFr = 0.;
  m_HtOffset = 0.;

  m_jet4vecs.clear();

  m_LFVmode = -1;

  return;
}


// Printout of final results
void MissingMassInput::PrintInputInfo() {
  if(m_fUseVerbose!=1) return;

  Info("DiTauMassTools", "%s", ("met_x="+std::to_string(m_MetVec.Px())+" met_y="+std::to_string(m_MetVec.Py())+" MET="+std::to_string(m_MetVec.Mod())+" met_phi="+std::to_string(m_MetVec.Phi())).c_str());
  Info("DiTauMassTools", "%s", ("sumEt="+std::to_string(m_SumEt)+" METsigmaP="+std::to_string(m_METsigmaP)+" METsigmaL="+std::to_string(m_METsigmaL)+" METcovphi="+std::to_string(m_METcovphi)).c_str());
  //Info("DiTauMassTools", "%s", (" Njet25="+std::to_string(Njet25)+" allowUseHT="+std::to_string(prob->GetAllowUseHT())+" useHT="+std::to_string(prob->GetUseHT())).c_str());

  Info("DiTauMassTools", "%s", ("MHtSigma1="+std::to_string(m_MHtSigma1)+" MHtSigma2="+std::to_string(m_MHtSigma2)
            +" MHtGaussFr="+std::to_string(m_MHtGaussFr)
            +" HtOffset="+std::to_string(m_HtOffset)).c_str());

  Info("DiTauMassTools", "%s", ("1st visible tau: type="+std::to_string(m_type_visTau1)+" Nprong="+std::to_string(m_Nprong_tau1)+"  P="+std::to_string(m_vistau1.P())+" Pt="+std::to_string(m_vistau1.Pt())
           +" Eta="+std::to_string(m_vistau1.Eta())+" Phi="+std::to_string(m_vistau1.Phi())+" M="+std::to_string(m_vistau1.M())).c_str());
  Info("DiTauMassTools", "%s", ("2nd visible tau: type="+std::to_string(m_type_visTau2)+" Nprong="+std::to_string(m_Nprong_tau2)+"  P="+std::to_string(m_vistau2.P())+" Pt="+std::to_string(m_vistau2.Pt())
           +" Eta="+std::to_string(m_vistau2.Eta())+" Phi="+std::to_string(m_vistau2.Phi())+" M="+std::to_string(m_vistau2.M())).c_str());

  if(m_jet4vecs.size()>0)
    {
      for(unsigned int i=0; i<m_jet4vecs.size(); i++)
        {
          Info("DiTauMassTools", "%s", ("Printing jets: jet "+std::to_string(i)+" E="+std::to_string(m_jet4vecs[i].E())+" Pt="
                   +std::to_string(m_jet4vecs[i].Pt())+" Phi="+std::to_string(m_jet4vecs[i].Phi())+" Eta="+std::to_string(m_jet4vecs[i].Eta())).c_str());
        }
    }
  return;
}



// input is sumEt after electrons and taus have been removed
// data_code=0 for data and =1 for MC
void MissingMassInput::SetMetScanParamsUE(double sumEt, double phi_scan, int data_code) {
  m_METcovphi = phi_scan;
  if(sumEt>2.0*m_beamEnergy) sumEt = sumEt/1000.0; // it's likely that sumEt was entered in MeV; this fix won't work only for a very small fraction of events
  double sigma = 1.0;
  double sigmaSyst = 0.10; // 10% systematics for now (be conservative)
  double METresScale = 0.7; // using inclusive number for winter 2012
  if(data_code==1) METresScale=0.7; // use the same for data & MC
  METresScale = METresScale*(1.0+m_METresSyst*sigmaSyst);
  // MET resolution can't be perfect in presence of other objects (i.e., electrons, jets, taus), so assume minSumEt = 5.0 for now
  sigma = sumEt>pow(3.0/METresScale,2) ? METresScale*sqrt(sumEt) : 3.0; // assume that MET resolution can't be better than 3 GeV
  m_METsigmaP = sigma;
  m_METsigmaL = sigma;
  m_SumEt = sumEt;
  m_dataType = data_code; // Sasha added on 09/26/11
  return;
}

// ---- input number of jets with Et>25 GeV
void MissingMassInput::SetNjet25(int val) {
  if(val>-1) m_Njet25 = val;
  return;
}
// ----- input SumEt
void MissingMassInput::SetSumEt(double sumEt) {
  m_SumEt = sumEt;
  return;
}
// ----- input vis Tau type
void MissingMassInput::SetVisTauType(int i, int tautype) {
  if(i==0) m_type_visTau1 = tautype;
  if(i==1) m_type_visTau2 = tautype;
  return;
}
// ----- input vis Tau vectors
void MissingMassInput::SetVisTauVec(int i, const TLorentzVector & vec) {
  if(m_fUseVerbose==1) { Info("DiTauMassTools", "%s", ("Seting input "+std::to_string(i)+" to pT="+std::to_string(vec.Pt())).c_str()); }
  if(i==0) m_vistau1 = vec;
  if(i==1) m_vistau2 = vec;
  return;
}
// ---- input Met vector
void MissingMassInput::SetMetVec(const TVector2 & vec) {
  m_MetVec = vec;
  return;
}

// not used but might be helpful?
// ----- input data type
void MissingMassInput::SetIsData(int val) {
  if(val==0 || val==1) m_dataType = val;
  return;
}
// ----- input vis Tau N-prong
void MissingMassInput::SetNprong(int i, int nprong) {
  if(i==0) m_Nprong_tau1 = nprong;
  if(i==1) m_Nprong_tau2 = nprong;
  return;
}
