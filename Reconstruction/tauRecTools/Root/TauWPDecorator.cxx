/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/TauWPDecorator.h"

#include "AsgDataHandles/ReadDecorHandle.h"

#include <algorithm>
#include "TFile.h"
#include "TH2.h"

//______________________________________________________________________________
TauWPDecorator::TauWPDecorator(const std::string& name) :
  TauRecToolBase(name) {
  declareProperty("UseEleBDT", m_electronMode = false);
  declareProperty("DefineWPs", m_defineWPs = false);
  declareProperty("ScoreName", m_scoreName = "");
  declareProperty("NewScoreName", m_scoreNameTrans = "");
  
  declareProperty("flatteningFile0Prong", m_file0p = "");
  declareProperty("flatteningFile1Prong", m_file1p = "");
  declareProperty("flatteningFile3Prong", m_file3p = "");
  
  declareProperty("CutEnumVals", m_EDMWPs);
  declareProperty("SigEff0P", m_EDMWPEffs0p);
  declareProperty("SigEff1P", m_EDMWPEffs1p);
  declareProperty("SigEff3P", m_EDMWPEffs3p);

  declareProperty("DecorWPNames", m_decorWPs);
  declareProperty("DecorWPCutEffs0P", m_decorWPEffs0p);
  declareProperty("DecorWPCutEffs1P", m_decorWPEffs1p);
  declareProperty("DecorWPCutEffs3P", m_decorWPEffs3p);
}

//______________________________________________________________________________
TauWPDecorator::~TauWPDecorator() {
}

//______________________________________________________________________________
StatusCode TauWPDecorator::retrieveHistos(int nProng) {
  // Find and open file
  std::string fileName;
  std::shared_ptr<std::vector<m_pair_t>> histArray = nullptr;
  if (nProng == 0) { 
    fileName = m_file0p;
    histArray = m_hists0p;
  }
  else if (nProng == 1) {
    fileName = m_file1p;
    histArray = m_hists1p;
  }
  else {
    fileName = m_file3p;
    histArray = m_hists3p;
  }

  std::string fullPath = find_file(fileName);
  std::unique_ptr<TFile> file(TFile::Open(fullPath.c_str(), "READ"));

  if (!file || file->IsZombie()) {
    ATH_MSG_FATAL("Could not open file " << fullPath.c_str());
    return StatusCode::FAILURE;  
  }
  
  ATH_MSG_INFO("Loading working points from " << fullPath.c_str());
  
  // Iterate over working points
  for (int i = 0; i < 100; ++i) {
    // Retrieve histogram
    TH2* graph = dynamic_cast<TH2*>(file->Get(Form("h2_%02d", i)));
    if (!graph){
      ATH_MSG_WARNING("Failed to retrieve Graph " << i << " named " << Form("h2_%02d", i));
      continue;
    }
    graph->SetDirectory(0);       
    std::shared_ptr<TH2> sharedGraph(graph);
    histArray->push_back(m_pair_t(float(i)/100., std::move(sharedGraph)));
  }
  
  file->Close();
  
  if (histArray->size() == 0) {
    ATH_MSG_ERROR("There is no histograms for " << nProng << "-prong taus");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;  
}

//______________________________________________________________________________
StatusCode TauWPDecorator::storeLimits(int nProng) {
  std::shared_ptr<std::vector<m_pair_t>> histArray = nullptr;
  if (nProng == 0) {
    histArray = m_hists0p;
  }
  else if (nProng == 1) {
    histArray = m_hists1p;
  }
  else {
    histArray = m_hists3p;
  }

  std::shared_ptr<TH2> firstHist = histArray->at(0).second;
  m_xMin[nProng] = firstHist->GetXaxis()->GetXmin();
  m_xMax[nProng] = firstHist->GetXaxis()->GetBinCenter(firstHist->GetNbinsX());
  m_yMin[nProng] = firstHist->GetYaxis()->GetXmin();
  m_yMax[nProng] = firstHist->GetYaxis()->GetBinCenter(firstHist->GetNbinsY());

  // Check all the histograms have the same limits
  for (size_t i = 1; i < histArray->size(); ++i) {
    std::shared_ptr<TH2> hist = histArray->at(i).second;
  
    double xMin = hist->GetXaxis()->GetXmin();
    double xMax = hist->GetXaxis()->GetBinCenter(firstHist->GetNbinsX());
    double yMin = hist->GetYaxis()->GetXmin();
    double yMax = hist->GetYaxis()->GetBinCenter(firstHist->GetNbinsY());
   
    if (std::abs(m_xMin[nProng] - xMin) > 1e-5 ||
        std::abs(m_xMax[nProng] - xMax) > 1e-5 ||
        std::abs(m_yMin[nProng] - yMin) > 1e-5 ||
        std::abs(m_yMax[nProng] - yMax) > 1e-5) {
      ATH_MSG_WARNING("The " << i << " th histogram has different limit");
    } 
  }

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
double TauWPDecorator::transformScore(double score, double cutLow, double effLow, double cutHigh, double effHigh) const {
  double efficiency = effLow + (score - cutLow)/(cutHigh - cutLow) * (effHigh - effLow);
  double scoreTrans = 1.0 - efficiency;
  return scoreTrans;
}

//______________________________________________________________________________
StatusCode TauWPDecorator::initialize() {

  ATH_CHECK( m_aveIntPerXKey.initialize() );

  // 0p is for trigger only
  if (!m_file0p.empty()) {
    m_hists0p = std::make_shared<std::vector<m_pair_t>>();
    ATH_CHECK(retrieveHistos(0));
    ATH_CHECK(storeLimits(0));
  }
  
  // 1p and 3p file must be provided
  if (m_file1p.empty() || m_file3p.empty()) {
    ATH_MSG_ERROR("1p/3p flatterning file is not provided !");
    return StatusCode::FAILURE;
  }

  m_hists1p = std::make_shared<std::vector<m_pair_t>>();
  ATH_CHECK(retrieveHistos(1));
  ATH_CHECK(storeLimits(1));
  
  m_hists3p = std::make_shared<std::vector<m_pair_t>>();
  ATH_CHECK(retrieveHistos(3));
  ATH_CHECK(storeLimits(3));  
    
  for (size_t wpIndex=0; wpIndex < m_decorWPs.size(); ++wpIndex) {
    const std::string& name = m_decorWPs[wpIndex];
    m_charDecors.emplace_back(SG::AuxElement::Decorator<char>(name));
  }

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauWPDecorator::execute(xAOD::TauJet& pTau) const { 
  // obtain the dependent variables of the efficiency 
  // -- x variable is tau pt
  double xVariable = pTau.pt();
  
  // -- y variable is |eta| of leading track in electron mode, and pileup in other cases
  double yVariable = 0.0;
  if (m_electronMode) {
     const SG::AuxElement::ConstAccessor<float> acc_absEta("ABS_ETA_LEAD_TRACK");
     yVariable = std::abs(acc_absEta(pTau));
  } 
  else {
    SG::ReadDecorHandle<xAOD::EventInfo, float> eventInfoDecorHandle( m_aveIntPerXKey );
    if (!eventInfoDecorHandle.isPresent()) {
      ATH_MSG_WARNING ( "EventInfo decoration not available! Will set yVariable=0." );
    }
    else {
      yVariable = eventInfoDecorHandle(0);
    } 
  }

  int nProng = pTau.nTracks();
  
  // nProng >=2 is treated as 3 prong 
  // 0 prong is treated as 3 prong when 0 prong calibraction file is not provided
  if (nProng==0 && !m_hists0p) {
    nProng = 3;
  }
  else if (nProng>=2) {
    nProng = 3;
  }

  // make sure the dependent variables are within the range of calibration histograms
  ATH_MSG_DEBUG("original pT:\t" << xVariable);
  if (m_electronMode) {
    ATH_MSG_DEBUG("original |eta|:\t" << yVariable);
  }
  else {
    ATH_MSG_DEBUG("original mu:\t" << yVariable);
  }

  xVariable = std::min(m_xMax.at(nProng), std::max(m_xMin.at(nProng), xVariable));
  yVariable = std::min(m_yMax.at(nProng), std::max(m_yMin.at(nProng), yVariable));
  
  ATH_MSG_DEBUG("final pT:\t" << xVariable);
  if (m_electronMode) {
    ATH_MSG_DEBUG("final |eta|:\t" << yVariable);
  }
  else {
    ATH_MSG_DEBUG("final mu:\t" << yVariable);
  }

  std::shared_ptr<std::vector<m_pair_t>> histArray = nullptr;
  if (nProng == 0) histArray = m_hists0p;
  else if (nProng == 1) histArray = m_hists1p;
  else histArray = m_hists3p;
  
  std::array<double, 2> cuts = {-1.01, 1.01}; // lower and upper bounday of the score
  std::array<double, 2> effs = {1.0, 0.0}; // efficiency corresponding to the score cut
  bool gotLow = false; // whether lower bounday is found
  bool gotHigh = false; // whether upper bounday is found
  
  const SG::AuxElement::ConstAccessor<float> acc_score(m_scoreName);
  double score = acc_score(pTau); // original score (BDT/RNN)
  
  // Loop over all histograms to find the lower and upper bounary of the score and corresponding efficiency
  for (unsigned int i = 0; i < histArray->size(); ++i) {
    std::shared_ptr<TH2> myHist = histArray->at(i).second;
    double myCut = myHist->Interpolate(xVariable, yVariable);
    
    if (myCut <= score && ((!gotLow) || std::abs(myCut-score) < std::abs(cuts[0]-score))) {
      gotLow = true;
      effs[0] = histArray->at(i).first;
      cuts[0] = myCut;
    }
    else if (myCut > score && ((!gotHigh) || std::abs(myCut-score) < std::abs(cuts[1]-score))) {
      gotHigh = true;
      effs[1] = histArray->at(i).first;
      cuts[1] = myCut;
    }

    if (gotLow && gotHigh){
      ATH_MSG_VERBOSE("break @ " << myHist->GetName());
      break;
    }
  }

  double scoreTrans = -1111.; // flattened score
  if (score > cuts[1]) { // should not happen
    scoreTrans = 1 - effs[1];
  }
  else if (score < cuts[0]) { // score is -9999 when BDT/RNN fails
    scoreTrans = 1 - effs[0];
  }
  else {
    scoreTrans = transformScore(score, cuts[0], effs[0], cuts[1], effs[1]);
  }
  SG::AuxElement::Accessor<float> acc_scoreTrans(m_scoreNameTrans);
  acc_scoreTrans(pTau) = scoreTrans;
  
  if(m_defineWPs) {
    // WPs in EDM
    for (size_t wpIndex=0; wpIndex < m_EDMWPs.size(); ++wpIndex) {
      if(nProng == 0) {
        pTau.setIsTau((xAOD::TauJetParameters::IsTauFlag) m_EDMWPs[wpIndex], scoreTrans > (1-m_EDMWPEffs0p[wpIndex]));
      }
      else if(nProng == 1) {
        pTau.setIsTau((xAOD::TauJetParameters::IsTauFlag) m_EDMWPs[wpIndex], scoreTrans > (1-m_EDMWPEffs1p[wpIndex]));
      }
      else {
        pTau.setIsTau((xAOD::TauJetParameters::IsTauFlag) m_EDMWPs[wpIndex], scoreTrans > (1-m_EDMWPEffs3p[wpIndex]));
      }
    }
    // Decorate other WPs
    for (size_t wpIndex=0; wpIndex < m_decorWPs.size(); ++wpIndex) {
      const SG::AuxElement::Decorator<char>& decorator = m_charDecors[wpIndex];

      if(nProng == 0) {
        decorator(pTau) = scoreTrans > (1-m_decorWPEffs0p[wpIndex]);
      }
      else if(nProng == 1) {
        decorator(pTau) = scoreTrans > (1-m_decorWPEffs1p[wpIndex]);    
      }
      else {
        decorator(pTau) = scoreTrans > (1-m_decorWPEffs3p[wpIndex]);    
      }
    }
  }
  
  return StatusCode::SUCCESS;
}
