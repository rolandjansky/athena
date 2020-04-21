/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <utility>
#include "TFile.h"
#include "TH2.h"
#include "TString.h"
#include "tauRecTools/TauWPDecorator.h"
#include "xAODEventInfo/EventInfo.h"

/********************************************************************/
TauWPDecorator::TauWPDecorator(const std::string& name) :
  TauRecToolBase(name) {
}

/********************************************************************/
TauWPDecorator::~TauWPDecorator() {
}

StatusCode TauWPDecorator::retrieveHistos(int nProng) {

  // Find and open file
  std::string fileName;
  std::vector<m_pair_t>* histArray = nullptr;
  if (nProng == 0) { 
    fileName = m_file0P;
    histArray = m_hists0P.get();
  }
  else if (nProng == 1) {
    fileName = m_file1P;
    histArray = m_hists1P.get();
  }
  else {
    fileName = m_file3P;
    histArray = m_hists3P.get();
  }

  std::string fullPath = find_file(fileName);
  std::unique_ptr<TFile>  myFile(TFile::Open(fullPath.c_str(), "READ"));

  if(!myFile || myFile->IsZombie()) {
    ATH_MSG_FATAL("Could not open file " << fullPath.c_str());
    return StatusCode::FAILURE;  
  }
  
  ATH_MSG_INFO("Loading working points from " << fullPath.c_str());
  
  // Iterate over working points
  for(int i=0; i<100; i++)
  {
    // Retrieve histogram
    TH2* myGraph = dynamic_cast<TH2*>(myFile->Get(Form("h2_%02d", i)));
    if(!myGraph){
      ATH_MSG_WARNING("Failed to retrieve Graph " << i << " named " << Form("h2_%02d", i));
      continue;
    }
      
    std::unique_ptr<TH2> myLocalGraph(myGraph);
    myLocalGraph->SetDirectory(0);       
    histArray->push_back(m_pair_t(float(i)/100., std::move(myLocalGraph)));
  }
  
  myFile->Close();

  return StatusCode::SUCCESS;  
}

StatusCode TauWPDecorator::storeLimits(int nProng) {

  std::vector<m_pair_t>* histArray = nullptr;
  if (nProng == 0) histArray = m_hists0P.get();
  else if (nProng == 1) histArray = m_hists1P.get();
  else histArray = m_hists3P.get();

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

  return StatusCode::SUCCESS;
}

double TauWPDecorator::transformScore(double score, double cut_lo, double eff_lo, double cut_hi, double eff_hi) {
  double newscore = 1. - eff_lo - (score - cut_lo)/(cut_hi - cut_lo) * (eff_hi - eff_lo);
  return newscore;
}

/********************************************************************/
StatusCode TauWPDecorator::initialize() {

  ATH_CHECK( m_eventInfo.initialize() );

  // 0p is for trigger only
  if(!m_file0P.empty()) {
    m_hists0P = std::make_unique<std::vector<m_pair_t>>();
    ATH_CHECK( retrieveHistos(0) );
    ATH_CHECK( storeLimits(0) );
  }

  m_hists1P = std::make_unique<std::vector<m_pair_t>>();
  ATH_CHECK( retrieveHistos(1) );
  ATH_CHECK( storeLimits(1) );
  
  m_hists3P = std::make_unique<std::vector<m_pair_t>>();
  ATH_CHECK( retrieveHistos(3) );
  ATH_CHECK( storeLimits(3) );  

  return StatusCode::SUCCESS;
}

/********************************************************************/
StatusCode TauWPDecorator::execute(xAOD::TauJet& pTau) 
{ 

  float mu = 0;
  SG::ReadHandle<xAOD::EventInfo> eventinfoInHandle( m_eventInfo );
  if (!eventinfoInHandle.isValid()) {
    ATH_MSG_ERROR( "Could not retrieve HiveDataObj with key " << eventinfoInHandle.key() << ", mu is set to be .0");
  }
  else {
    const xAOD::EventInfo* eventInfo = eventinfoInHandle.cptr();    
    mu = eventInfo->averageInteractionsPerCrossing();
  }

  const SG::AuxElement::ConstAccessor<float> acc_score(m_scoreName);
  SG::AuxElement::Accessor<float> acc_newScore(m_newScoreName);

  // could use detail(TauJetParameters::nChargedTracks) for trigger, but TauIDVarCalculator used nTracks()
  int nProng = pTau.nTracks();

  // WARNING, previous behaviour: use 1p flattening file for 1-track taus, else use 3p flattening file
  // meaning when we don't use a 0p flattening file, 0p falls back to 3p case
  // for now, keep it for backward-compatibility
  if(nProng==0 && !m_hists0P) nProng = 3;
  else if(nProng>=2) nProng = 3;

  std::vector<m_pair_t>* histArray = nullptr;
  if (nProng == 0) histArray = m_hists0P.get();
  else if (nProng == 1) histArray = m_hists1P.get();
  else histArray = m_hists3P.get();
  
 // Retrieve tau properties
  double score = acc_score(pTau);
  double pt = pTau.pt();

  double y_var = 0.0;
  if(m_electronMode) {
     const SG::AuxElement::ConstAccessor<float> acc_absEta("ABS_ETA_LEAD_TRACK");
     y_var = std::abs(acc_absEta(pTau));
  } else {
     y_var = mu;
  }

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
    if(myCut <= score && ((!gotLow) || std::abs(myCut-score) < std::abs(cuts[0]-score))) {
      gotLow = true;
      effs[0] = histArray->at(i).first;
      cuts[0] = myCut;
    }
      
    else if(myCut > score && ((!gotHigh) || std::abs(myCut-score) < std::abs(cuts[1]-score))) {
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
  acc_newScore(pTau) = newscore;
  
  if(m_defineWP) {
    for (u_int Nwp=0; Nwp < m_cut_bits.size(); Nwp++){
      if(nProng == 0) {
	pTau.setIsTau((xAOD::TauJetParameters::IsTauFlag) m_cut_bits[Nwp], newscore > (1-m_cut_effs_0p[Nwp]));
      }
      else if(nProng == 1) {
	pTau.setIsTau((xAOD::TauJetParameters::IsTauFlag) m_cut_bits[Nwp], newscore > (1-m_cut_effs_1p[Nwp]));
      }
      else {
	pTau.setIsTau((xAOD::TauJetParameters::IsTauFlag) m_cut_bits[Nwp], newscore > (1-m_cut_effs_3p[Nwp]));
      }
    }
    // Decorate other WPs
    for (u_int Nwp=0; Nwp < m_decoration_names.size(); Nwp++){
      if(nProng == 0) {
	pTau.auxdecor<char>(m_decoration_names[Nwp]) = newscore > (1-m_cut_effs_decoration_0p[Nwp]);
      }
      else if(nProng == 1) {
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
