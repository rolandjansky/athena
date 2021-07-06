/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Class handling the output of the MissingMassCalculator
// author Michael Huebner <michael.huebner@no.spam.cern.ch>

// Local include(s):
#include "DiTauMassTools/MissingMassOutput.h"

using namespace DiTauMassTools;

// Default Constructor
MissingMassOutput::MissingMassOutput(){
}

// Default Desctructor
MissingMassOutput::~MissingMassOutput(){
}

//------- clearing output
void MissingMassOutput::ClearOutput(bool fUseVerbose) {

  if(fUseVerbose == 1){ Info("DiTauMassTools", "MissingMassCalculator::ClearOutput()"); }
  m_FitStatus=0;

  for (int imeth=0; imeth<MMCFitMethodV2::MAX; ++imeth)
    {
      if(fUseVerbose == 1){ Info("DiTauMassTools", "%s", ("MissingMassCalculator::ClearOutput(): clearing for method "+std::to_string(imeth)).c_str()); }
      m_FitSignificance[imeth] = -1.0;
      m_FittedMass[imeth] = 0.0;
      m_nuvec1[imeth].SetPxPyPzE(0.0,0.0,0.0,0.0);
      m_objvec1[imeth].SetPxPyPzE(0.0,0.0,0.0,0.0);
      m_nuvec2[imeth].SetPxPyPzE(0.0,0.0,0.0,0.0);
      m_objvec2[imeth].SetPxPyPzE(0.0,0.0,0.0,0.0);
      m_totalvec[imeth].SetPxPyPzE(0.0,0.0,0.0,0.0);
      m_FittedMetVec[imeth].Set(0.0,0.0);
    }

  m_RMS2MPV = 0.0;

  return;
}


// return fit status
int MissingMassOutput::GetFitStatus() const {
  return m_FitStatus;
}

// returns fit significance
double MissingMassOutput::GetFitSignificance(int fitcode) const {
  double signif = -1.0;
  if (fitcode<0 || fitcode >= MMCFitMethodV2::MAX) {
    Error("DiTauMassTools", "%s", ("MissingMassCalculatorV2::GetFitSignificance ERROR ! fitcode="+std::to_string(fitcode)
              +". Should be between 0 and "+std::to_string(MMCFitMethodV2::MAX-1)).c_str());
  } else {
    signif = m_FitSignificance[fitcode];
  }

  return signif;
}

// returns RMS/MPV according to histogram method
double MissingMassOutput::GetRms2Mpv() const {
  return m_RMS2MPV;
}

// returns fitted Mass
double MissingMassOutput::GetFittedMass(int fitcode) const {
  // 0 best parameter space
  // 1 best mass
  // best nu from hist
  double mass = 0.0;
  if (fitcode<0 || fitcode >= MMCFitMethodV2::MAX) {
    Error("DiTauMassTools", "%s", ("MissingMassCalculatorV2::GetFittedMass ERROR ! fitcode="+std::to_string(fitcode)
              +". Should be between 0 and "+std::to_string(MMCFitMethodV2::MAX-1)).c_str());
  } else {
    mass = m_FittedMass[fitcode];
  }

  return mass;
}

// returns upper error on fitted Mass
double MissingMassOutput::GetFittedMassErrorUp(int fitcode) const {
  // 0 best parameter space
  // 1 best mass
  // best nu from hist
  double massUpperError = 0.0;
  if (fitcode<0 || fitcode >= MMCFitMethodV2::MAX) {
    Error("DiTauMassTools", "%s", ("MissingMassCalculatorV2::GetFittedMass ERROR ! fitcode="+std::to_string(fitcode)
              +". Should be between 0 and "+std::to_string(MMCFitMethodV2::MAX-1)).c_str());
  } else {
    massUpperError = m_FittedMassUpperError[fitcode];
  }

  return massUpperError;
}

// returns upper error on fitted Mass
double MissingMassOutput::GetFittedMassErrorLow(int fitcode) const {
  // 0 best parameter space
  // 1 best mass
  // best nu from hist
  double massLowerError = 0.0;
  if (fitcode<0 || fitcode >= MMCFitMethodV2::MAX) {
    Error("DiTauMassTools", "%s", ("MissingMassCalculatorV2::GetFittedMass ERROR ! fitcode="+std::to_string(fitcode)
              +". Should be between 0 and "+std::to_string(MMCFitMethodV2::MAX-1)).c_str());
  } else {
    massLowerError = m_FittedMassLowerError[fitcode];
  }

  return massLowerError;
}

int MissingMassOutput::GetNTrials() const
{
  return m_NTrials;
}

int MissingMassOutput::GetNSuccesses() const
{
  return m_NSuccesses;
}

int MissingMassOutput::GetNSolutions() const
{
  return m_NSolutions;
}

// sum of weights of all solutions
double MissingMassOutput::GetSumW() const
{
  return m_SumW;
}

// average RMS of solutions in one event
double MissingMassOutput::GetAveSolRMS() const
{
  return m_AveSolRMS;
}

std::shared_ptr<TH1F> MissingMassOutput::GetMassHistogram() const
{
  return m_hMfit_all;
}

std::shared_ptr<TH1F> MissingMassOutput::GetMassHistogramNoWeight() const
{
  return m_hMfit_allNoWeight;
}

// returns neutrino 4-vec
TLorentzVector MissingMassOutput::GetNeutrino4vec(int fitcode, int ind) const
{
  TLorentzVector vec(0.0,0.0,0.0,0.0);
  if (fitcode!=MMCFitMethodV2::MAXW && fitcode!=MMCFitMethodV2::MLNU3P )
    {
      Error("DiTauMassTools", "%s", ("MissingMassCalculatorV2::GetNeutrino4Vec ERROR ! fitcode="+std::to_string(fitcode)
                +". Should be either "+std::to_string(MMCFitMethodV2::MAXW)+" or "+std::to_string(MMCFitMethodV2::MLNU3P)).c_str());
    }
  else if (m_FitStatus>0)
    {
      if(ind==0) vec = m_nuvec1[fitcode];
      else if(ind==1) vec = m_nuvec2[fitcode];
    }
  return vec;
}

// returns neutrino 4-vec
TLorentzVector MissingMassOutput::GetTau4vec(int fitcode, int ind) const
{
  TLorentzVector vec(0.0,0.0,0.0,0.0);
  if (fitcode!=MMCFitMethodV2::MAXW && fitcode!=MMCFitMethodV2::MLNU3P )
    {
      Error("DiTauMassTools", "%s", ("MissingMassCalculatorV2::GetTau4vec ERROR ! fitcode="+std::to_string(fitcode)
                +". Should be either "+std::to_string(MMCFitMethodV2::MAXW)+" or "+std::to_string(MMCFitMethodV2::MLNU3P)).c_str());
    }
  else if (m_FitStatus>0)
    {
      if(ind==0) vec = m_objvec1[fitcode];
      else if(ind==1) vec = m_objvec2[fitcode];
    }
  return vec;
}

// returns 4-vec for resonance
TLorentzVector MissingMassOutput::GetResonanceVec(int fitcode) const {
  TLorentzVector vec(0.0,0.0,0.0,0.0);
  if (fitcode!=MMCFitMethodV2::MAXW && fitcode!=MMCFitMethodV2::MLNU3P )
    {
      Error("DiTauMassTools", "%s", ("MissingMassCalculatorV2::GetResonanceVec ERROR ! fitcode="+std::to_string(fitcode)
                +". Should be either "+std::to_string(MMCFitMethodV2::MAXW)+" or "+std::to_string(MMCFitMethodV2::MLNU3P)).c_str());
    }
  else if (m_FitStatus>0)
    {
      vec = m_objvec1[fitcode]+m_objvec2[fitcode];
    }

  return vec;
}

// returns 2-vec for fitted MET
TVector2 MissingMassOutput::GetFittedMetVec(int fitcode) const {
  TVector2 vec(0.0,0.0);
  if (fitcode!=MMCFitMethodV2::MAXW && fitcode!=MMCFitMethodV2::MLNU3P )
    {
      Error("DiTauMassTools", "%s", ("MissingMassCalculatorV2::GetFittedMetVec ERROR ! fitcode="+std::to_string(fitcode)
                +". Should be either "+std::to_string(MMCFitMethodV2::MAXW)+" or "+std::to_string(MMCFitMethodV2::MLNU3P)).c_str());
    }
  else if (m_FitStatus>0)
    {
      vec=m_FittedMetVec[fitcode];
    }

  return vec;
}
