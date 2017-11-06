/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#include "CLHEP/Vector/LorentzVector.h"
//#include "CLHEP/Units/SystemOfUnits.h"

//c++
#include <utility>

// root
#include "TFile.h"
#include "TH2.h"
#include "TString.h"

//tau
#include "xAODTau/TauJet.h"
#include "tauRecTools/TauEventData.h"
#include "tauRecTools/TauWPDecorator.h"

#include "xAODEventInfo/EventInfo.h"

//using CLHEP::GeV;

/********************************************************************/
TauWPDecorator::TauWPDecorator(const std::string& name)
  : TauRecToolBase(name)
{
  declareProperty("flatteningFile1Prong", m_file1P = "fitted.pileup_1prong_hlt.root");
  declareProperty("flatteningFile3Prong", m_file3P = "fitted.pileup_multiprongs_hlt.root");

  declareProperty("ScoreName", m_scoreName = "BDTJetScore");
  declareProperty("NewScoreName", m_newScoreName = "BDTJetScoreSigTrans");

  declareProperty("DefineWPs", m_defineWP=false);
  declareProperty("UseEleBDT", m_electronMode=false);
  
  declareProperty("CutEnumVals", m_cut_bits);
  declareProperty("SigEff1P", m_cut_effs_1p);
  declareProperty("SigEff3P", m_cut_effs_3p);

  declareProperty("DecorWPNames", m_decoration_names);
  declareProperty("DecorWPCutEffs1P", m_cut_effs_decoration_1p);
  declareProperty("DecorWPCutEffs3P", m_cut_effs_decoration_3p);

}

/********************************************************************/
TauWPDecorator::~TauWPDecorator() {
}

StatusCode TauWPDecorator::retrieveHistos(int nProng) {

  // Find and open file
  std::string fileName = (nProng == 1) ? m_file1P : m_file3P;
  std::string fullPath = find_file(fileName);
  TFile * myFile = TFile::Open(fullPath.c_str(), "READ");

  if(!myFile || myFile->IsZombie()) {
    ATH_MSG_FATAL("Could not open file " << fullPath.c_str());
  }

  ATH_MSG_INFO("Loading working points from" << fullPath.c_str());

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
      std::unique_ptr<TH2> myLocalGraph1P((TH2*)myGraph->Clone());
      std::unique_ptr<TH2> myLocalGraph3P((TH2*)myGraph->Clone());
      myLocalGraph1P->SetDirectory(0);
      myLocalGraph3P->SetDirectory(0);
      
      if(nProng == 1)
	m_hists1P.push_back(m_pair_t(float(i)/100., std::move(myLocalGraph1P)));
      else
	m_hists3P.push_back(m_pair_t(float(i)/100., std::move(myLocalGraph3P)));
    }
  
  return StatusCode::SUCCESS;  
}

StatusCode TauWPDecorator::storeLimits(int nProng) {

  // Use pointer for simpler access
  std::vector<m_pair_t> *histArray;
  histArray = (nProng == 1) ? &m_hists1P : &m_hists3P;

  // Default values
  m_xmin[nProng] = 1E9;
  m_xmax[nProng] = -1E9;
  m_ymin[nProng] = 1E9;
  m_ymax[nProng] = -1E9;

  // Store limits
  for (unsigned int i=0; i<histArray->size(); i++)
    {
      TH2* myHist = histArray->at(i).second.get();
      m_xmin[nProng] = TMath::Min(myHist->GetXaxis()->GetXmin(), m_xmin[nProng]);
      m_ymin[nProng] = TMath::Min(myHist->GetYaxis()->GetXmin(), m_ymin[nProng]);

      m_xmax[nProng] = TMath::Max(myHist->GetXaxis()->GetBinCenter(myHist->GetNbinsX()), m_xmax[nProng]);
      m_ymax[nProng] = TMath::Max(myHist->GetYaxis()->GetBinCenter(myHist->GetNbinsY()), m_ymin[nProng]);

    }

  //ATH_MSG_WARNING("Final limits " << m_xmin[nProng] << " " << m_xmax[nProng] << " " << m_ymin[nProng] << " " << m_ymax[nProng]);

  return StatusCode::SUCCESS;
}

double TauWPDecorator::transformScore(double score, double cut_lo, double eff_lo, double cut_hi, double eff_hi) {
  double newscore = 1. - eff_lo - (score - cut_lo)/(cut_hi - cut_lo) * (eff_hi - eff_lo);
  //if(reverse) newscore = 1.0 - newscore;
  return newscore;
}
  
/********************************************************************/
StatusCode TauWPDecorator::initialize() {

  // Open the 1P and 3P files
  ATH_CHECK( retrieveHistos(1) );
  ATH_CHECK( retrieveHistos(3) );
  
  // Store limits for 1P and 3P
  ATH_CHECK( storeLimits(1) );
  ATH_CHECK( storeLimits(3) );

  return StatusCode::SUCCESS;
}

/********************************************************************/
StatusCode TauWPDecorator::execute(xAOD::TauJet& pTau) 
{ 

  // Accessors
  //static SG::AuxElement::ConstAccessor<int> acc_nVertex("NUMVERTICES");
  static const SG::AuxElement::ConstAccessor<float> acc_mu("MU");
  static const SG::AuxElement::ConstAccessor<float> acc_pt("pt");
  static const SG::AuxElement::ConstAccessor<int> acc_numTrack("NUMTRACK");
  static const SG::AuxElement::ConstAccessor<float> acc_absEta("ABS_ETA_LEAD_TRACK");
  SG::AuxElement::ConstAccessor<float> acc_score(m_scoreName);
  SG::AuxElement::Accessor<float> acc_newScore(m_newScoreName);

  // histograms
  std::vector<m_pair_t> *histArray;
  histArray = (acc_numTrack(pTau) == 1) ? &m_hists1P : &m_hists3P;

  // Retrieve tau properties
  int nProng = (acc_numTrack(pTau) == 1) ? 1 : 3;
  double score = (acc_score)(pTau);
  double pt = acc_pt(pTau);
  //  double mu = acc_mu(pTau);

  double y_var = 0.0;
  if(m_electronMode) {
     y_var = std::fabs(acc_absEta(pTau)); 
  } else {
     y_var = acc_mu(pTau);
  }

  // ATH_MSG_VERBOSE("========================================");
  // ATH_MSG_VERBOSE("nProng " << nProng);
  // ATH_MSG_VERBOSE("pT before " << pt);
  // ATH_MSG_VERBOSE("y_var before " << y_var);

  //ATH_MSG_ERROR("xmin=" << m_xmin[nProng] << " xmax=" << m_xmax[nProng]);
  //ATH_MSG_ERROR("ymin=" << m_ymin[nProng] << " ymax=" << m_ymax[nProng]);
  

  ATH_MSG_VERBOSE("pT before " << pt);
  ATH_MSG_VERBOSE("mu before " << y_var);

  // Implement limits
  pt = TMath::Min(m_xmax[nProng], TMath::Max(m_xmin[nProng], pt));
  y_var = TMath::Min(m_ymax[nProng], TMath::Max(m_ymin[nProng], y_var));
  
  ATH_MSG_VERBOSE("pT after " << pt);
  ATH_MSG_VERBOSE("mu after " << y_var);

  // calculate flattened disc
  std::vector<double> cuts(2,0.);
  std::vector<double> effs(2,0.);
  bool gotLow = false;
  bool gotHigh = false;
  
  // Loop over all histograms
  for (unsigned int i=0; i<histArray->size(); i++) {
    TH2* myHist = histArray->at(i).second.get();
    double myCut = myHist->Interpolate(pt, y_var);
    
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
    ATH_MSG_WARNING("Candidate pT/mu/BDTscore " << pt << " " << y_var << " " << score);
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
  (acc_newScore)(pTau) = newscore;
  
  if(m_defineWP) {
    for (u_int Nwp=0; Nwp < m_cut_bits.size(); Nwp++){
      if(acc_numTrack(pTau) == 1) {
	pTau.setIsTau((xAOD::TauJetParameters::IsTauFlag) m_cut_bits[Nwp], newscore > (1-m_cut_effs_1p[Nwp]));
      }
      else{
	pTau.setIsTau((xAOD::TauJetParameters::IsTauFlag) m_cut_bits[Nwp], newscore > (1-m_cut_effs_3p[Nwp]));
      }
    }
    // Decorate other WPs
    for (u_int Nwp=0; Nwp < m_decoration_names.size(); Nwp++){
      if(acc_numTrack(pTau) == 1) {
	pTau.auxdecor<char>(m_decoration_names[Nwp]) = newscore > (1-m_cut_effs_decoration_1p[Nwp]);    
      }
      else {
	pTau.auxdecor<char>(m_decoration_names[Nwp]) = newscore > (1-m_cut_effs_decoration_3p[Nwp]);    
      }      
    }
  }
  
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Finalize
//-----------------------------------------------------------------------------

StatusCode TauWPDecorator::finalize() {
  
  return StatusCode::SUCCESS;
  
}
