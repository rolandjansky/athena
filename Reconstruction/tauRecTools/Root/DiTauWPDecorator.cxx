/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// root
#include "TFile.h"
#include "TH2.h"
#include "TString.h"

#include "tauRecTools/DiTauWPDecorator.h"
#include "PathResolver/PathResolver.h"

using namespace tauRecTools;

/********************************************************************/
DiTauWPDecorator::DiTauWPDecorator(const std::string& name) :
  AsgTool(name),
  m_accScore(0),
  m_accNewScore(0)
{
  declareProperty("flatteningFile", m_sFile = "");

  declareProperty("ScoreName", m_sScoreName = "JetBDT");
  declareProperty("NewScoreName", m_sNewScoreName = "JetBDTFlat");

  declareProperty("XVarName", m_sXVar = "pt");
  declareProperty("YVarName", m_sYVar = "DeltaR");
    
  declareProperty("SigEff", m_vfCutEffsDecoration);
  declareProperty("WPNames", m_vsDecorationNames);
  declareProperty("DiTauDecayChannel", m_sDecayChannel="HadEl");
}

/********************************************************************/
DiTauWPDecorator::~DiTauWPDecorator() {
}

StatusCode DiTauWPDecorator::retrieveHistos() {

  // Find and open file
  std::string weights_file = PathResolverFindCalibFile(m_sFile);
  TFile * myFile = TFile::Open(weights_file.c_str(), "READ");

  if(!myFile || myFile->IsZombie()) {
    ATH_MSG_FATAL("Could not open file " << m_sFile.c_str());
  }

  ATH_MSG_INFO("Loading working points from" << m_sFile.c_str());

  // Iterate over working points
  for(int i=0; i<100; i++)
  {
    // Retrieve histogram
    TH2* myGraph = (TH2*)myFile->Get(Form("h2_%02d", i));
    if(!myGraph){
      ATH_MSG_WARNING("Failed to retrieve Graph " << i << " named " << Form("h2_%02d", i));
      continue;
    }

    // Clone histogram and store locally
    TH2* myLocalGraph = (TH2*)myGraph->Clone();
    myLocalGraph->SetDirectory(0);
    m_vpHists.push_back(m_pair_t(float(i)/100., myLocalGraph));

  }
  
  return StatusCode::SUCCESS;  
}

StatusCode DiTauWPDecorator::storeLimits() {

  // Use pointer for simpler access
  std::vector<m_pair_t> *histArray;
  histArray = &m_vpHists;

  // Default values
  m_dXmin = 1E9;
  m_dXmax = -1E9;
  m_dYmin = 1E9;
  m_dYmax = -1E9;

  // Store limits
  for (unsigned int i=0; i<histArray->size(); i++)
  {
    TH2* myHist = histArray->at(i).second;
      
    m_dXmin = TMath::Min(myHist->GetXaxis()->GetXmin(), m_dXmin);
    m_dYmin = TMath::Min(myHist->GetYaxis()->GetXmin(), m_dYmin);

    m_dXmax = TMath::Max(myHist->GetXaxis()->GetBinCenter(myHist->GetNbinsX()), m_dXmax);
    m_dYmax = TMath::Max(myHist->GetYaxis()->GetBinCenter(myHist->GetNbinsY()), m_dYmin);

  }

  return StatusCode::SUCCESS;
}

double DiTauWPDecorator::transformScore(double score, double cut_lo, double eff_lo, double cut_hi, double eff_hi) {
  double newscore = 1. - eff_lo - (score - cut_lo)/(cut_hi - cut_lo) * (eff_hi - eff_lo);
  return newscore;
}
  
/********************************************************************/
StatusCode DiTauWPDecorator::initialize() {

  // Open the 1P and 3P files
  ATH_CHECK( retrieveHistos() );
  
  // Store limits for 1P and 3P
  ATH_CHECK( storeLimits() );

  m_accX = new SG::AuxElement::ConstAccessor<float>(m_sXVar);
  m_accY = new SG::AuxElement::ConstAccessor<float>(m_sYVar);
  
  m_accScore = new SG::AuxElement::ConstAccessor<double>(m_sScoreName);
  m_accNewScore = new SG::AuxElement::Decorator<double>(m_sNewScoreName);


  
  return StatusCode::SUCCESS;
}

/********************************************************************/
StatusCode DiTauWPDecorator::execute(const xAOD::DiTauJet& xDiTau) 
{ 
  // histograms
  std::vector<m_pair_t> *histArray = &m_vpHists;

  // Retrieve tau properties
  double score = (*m_accScore)(xDiTau);
  double xvar = (*m_accX)(xDiTau);
  double yvar = (*m_accY)(xDiTau);
  
  ATH_MSG_VERBOSE("xvar: " << m_sXVar << " before " << xvar);
  ATH_MSG_VERBOSE("yvar: " << m_sYVar << " before " << yvar);

  // Implement limits
  xvar = TMath::Min(m_dXmax, TMath::Max(m_dXmin, xvar));
  yvar = TMath::Min(m_dYmax, TMath::Max(m_dYmin, yvar));
  
  ATH_MSG_VERBOSE("xvar: " << m_sXVar << " after " << xvar);
  ATH_MSG_VERBOSE("yvar: " << m_sYVar << " after " << yvar);

  // calculate flattened disc
  std::vector<double> cuts(2,0.);
  std::vector<double> effs(2,0.);
  bool gotLow = false;
  bool gotHigh = false;
  
  // Loop over all histograms
  for (unsigned int i=0; i<histArray->size(); i++) {
    TH2* myHist = histArray->at(i).second;
    double myCut = myHist->Interpolate(xvar, yvar);
    
    // Find upper and lower cuts
    if(myCut <= score && ((!gotLow) || fabs(myCut-score) < fabs(cuts[0]-score))) {
      gotLow = true;
      effs[0] = histArray->at(i).first;
      cuts[0] = myCut;
    }
      
    else if(myCut > score && ((!gotHigh) || fabs(myCut-score) < fabs(cuts[1]-score))) {
      gotHigh = true;
      effs[1] = histArray->at(i).first;
      cuts[1] = myCut;
    }

    if(gotLow && gotHigh){
      ATH_MSG_VERBOSE("break @ " << myHist->GetName());
      break;
    }
  }

  ATH_MSG_VERBOSE("BDTScore " << score);
  ATH_MSG_VERBOSE("CutLow/High " << cuts[0] << " " << cuts[1]);
  ATH_MSG_VERBOSE("EffLow/High " << effs[0] << " " << effs[1]);
  ATH_MSG_VERBOSE("gotHigh=" << gotHigh << " gotLow=" << gotLow);

  if(!gotLow && !gotHigh) {
    ATH_MSG_WARNING("Must have at least upper or lower efficiency boundary!");
    ATH_MSG_WARNING("Candidate xvar/yvar/BDTscore " << xvar << " " << yvar << " " << score);
  }

  // Upper & Lower Boundaries
  if(!gotHigh) {
    cuts[1] = 1.01;
    effs[1] = 0.0;
  }
  if(!gotLow) {
    cuts[0] = -1.01;
    effs[0] = 1.0;
  }

  // Evaluate and decorate new score
  double newscore = -1111.;
  // Score higher than default upper boundary (rare)
  if( score > cuts[1] )      newscore = cuts[0];
  // Score lower than default lower boundary (rare)            
  else if( score < cuts[0] ) newscore = cuts[1];
  // Score inside boundaries            
  else {
    newscore = transformScore(score, cuts[0], effs[0], cuts[1], effs[1]);
  }

  ATH_MSG_VERBOSE("newScore=" << newscore);
  
  (*m_accNewScore)(xDiTau) = newscore;

  // Decorate other WPs
  for (u_int Nwp=0; Nwp < m_vsDecorationNames.size(); Nwp++){
    xDiTau.auxdecor<char>(m_vsDecorationNames[Nwp]) = newscore > (1-m_vfCutEffsDecoration[Nwp]);    
  }
    
  return StatusCode::SUCCESS;
}
