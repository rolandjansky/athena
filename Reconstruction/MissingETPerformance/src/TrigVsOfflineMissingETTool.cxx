/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"

#include "StoreGate/StoreGateSvc.h"

#include "TrigConfL1Data/TriggerItem.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "TrigMissingEtEvent/TrigMissingET.h"
#include "TrigMissingEtEvent/TrigMissingETContainer.h"
#include "MissingETEvent/MissingET.h"

#include "MissingETPerformance/TrigVsOfflineMissingETTool.h"
#include "MissingETPerformance/MissingETData.h"

#include "AthenaKernel/Units.h"

#include "boost/tokenizer.hpp"

#include "TMath.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"

using namespace Trig;
using boost::tokenizer;
using boost::char_separator;

using std::string;
using std::vector;
using Athena::Units::GeV;

namespace metfit {

  // turnOn curve -- begin
  Double_t turnOn(Double_t *x, Double_t *par) {
    /****
                -                                     -
             C |          / {x-A} \         / {x-D} \  |
        y = ---| 1 + ERF | ------- | + ERF | ------- | |
             2 |          \ 1.41B /         \ 1.41E /  |
                -                                     -
        constant (offset) + sum of two Gaussian error functions (L2, EF turnons)
        A and D give the turnOn (expected) values in GeV of L2 and EF respectively.
        The respective standard deviations are given by B and E. C is the normalization
        constant.
        par[0] = A
        par[1] = B
        par[2] = C
        par[3] = D
        par[4] = E

    ****/

    Double_t arg = 0;
    Double_t arg2 = 0;
    Double_t sqroot2 = TMath::Sqrt(2.);

    arg2 = (x[0] - par[3]) / (sqroot2 * par[4]);

    if ( fabs(TMath::Sqrt(x[0])*par[1]) > 0 )
      arg = (x[0] - par[0])/(sqroot2 * par[1]);
    else arg = 1.0e10;

    Double_t fitval = 0.5*par[2]*(1+TMath::Erf(arg)+TMath::Erf(arg2));
    return fitval;
  }
  // turnOn curve -- end

  // efficiency -- begin
  TF1* Efficiency(TH1* hnum, TH1* hden, Double_t *range, Double_t *iPars) {

    // Fit function and declare efficiency histogram

    if(!hnum || !hden) return 0;

    TF1 *fpT = 0;
    TH1F *heff = 0;

    // Load sum of squares of weights
    //hnum->Sumw2();
    //hden->Sumw2();

    // Make a copy of numerator histogram in heff
    heff = (TH1F *) hnum->Clone();

    if(heff) {

      // Divide it by hden. Assume binomial errors
      // http://root.cern.ch/root/htmldoc/src/TH1.cxx.html#hh46dE
      heff->Divide(heff, hden, 1., 1., "B");

      // 5 parameter user-defined "turnOn" fit in Range [range[0], range[1]] GeV
      fpT = new TF1("turnOn",turnOn, range[0], range[1], 5);

      // For the fit to converge faster, provide reasonable
      // initital parameters. The turn on is a sum of two
      // error functions (2x2=4 parameters) and the 
      // normalization (+1 parameter)

      //fpT->SetParameters(22.00, 0.7, 0.6, 20., 2.3);
      fpT->SetParameters(iPars[0],iPars[1],iPars[2],iPars[3],iPars[4]);

      // Fit in the range specified "R", quietly "Q"
      heff->Fit(fpT,"RQ");
    }

    //add this line to avoid heff being wriiwri to the root file
    //delete heff;
    // Return fit function
    return fpT;

  }
  // efficiency -- end

  // Given a fit function, its parameters and a std. deviation, this method computes
  // the 10%, 50% and 90% efficiency turnOn values

  void GetNSigmaValues(TF1 *fitfunc, std::vector<float>& vals, float sigma=2.5) {

    Double_t params_pt[6] = {0.};// , parerrs_pt[6] = {0.};
    Double_t chi2 = 0.;
    int ndof = 1;
    chi2 = fitfunc -> GetChisquare();
    ndof = fitfunc -> GetNDF();
    fitfunc->GetParameters(&params_pt[0]);
    //for(unsigned iT = 0; iT < 5; iT++)
    //  parerrs_pt[iT] = fitfunc->GetParError(iT);

    float xmin1 = params_pt[0] - sigma * params_pt[1]; // [0]==A is the mean value and [1]==B is the sigma
    float xmid1 = params_pt[0];
    float xmax1 = params_pt[0] + sigma * params_pt[1]; // scan mean +/- N*sigma for 10%, 50% and 90% values
    float xmax2 = params_pt[3] + 2.5 * params_pt[4]; // scan mean +/- N*sigma for 10%, 50% and 90% values

    vals.push_back(fitfunc -> Eval(xmin1)); // Store 10% eff
    vals.push_back(fitfunc -> Eval(xmid1)); // Store 50% eff
    vals.push_back(fitfunc -> Eval(xmax1)); // Store 90% eff
    vals.push_back(fitfunc -> Eval(xmax2)); // Store 99% eff
    vals.push_back(chi2); // Store chisquare of fit
    vals.push_back(float(ndof)); // Store dof of fit

  }
} // end of namespace metfit


/*-----------------------------------------------------*/
TrigVsOfflineMissingETTool::TrigVsOfflineMissingETTool(const std::string & type, 
    const std::string & name, const IInterface* parent) : AthAlgTool(type, name, parent),
                                                          m_debuglevel(false)
  /*-----------------------------------------------------*/
{
  declareInterface<TrigVsOfflineMissingETTool>( this );

  // trigger aware monitoring 

  declareProperty("ToolName", m_name="");
  declareProperty("PrintMETTriggerStats", m_printTrigStats=false);
  declareProperty("MakePlotsForAllMETTriggers", m_useAllTriggers=false);
  declareProperty("ComputeTriggerEfficiency", m_doTrigEff=true);
  declareProperty("METTriggers", m_met_triggers);
  declareProperty("METReference", m_met_ref="MET_RefFinal");
  declareProperty("SampleSelectionTriggers", m_selectTriggers);
  declareProperty("DeltaRCut", m_deltaRCut = 0.3); // not yet used
  declareProperty("FolderName",           m_folderName="");

  vector<string> procTrig;

  procTrig.push_back("L1_XE20");  // L1 met triggers
  procTrig.push_back("L1_XE50");
  procTrig.push_back("L1_TE360");

  procTrig.push_back("L2_xe20");  // L2 met triggers
  procTrig.push_back("L2_xe50");
  procTrig.push_back("L2_te360");

  procTrig.push_back("EF_xe20");
  procTrig.push_back("EF_xe50");
  procTrig.push_back("EF_te360"); // EF met triggers

  vector<string> selTrig;

  selTrig.push_back("EF_J10");
  selTrig.push_back("EF_MinBiasSpacePoints");
  selTrig.push_back("EF_mu10");

  if(m_met_triggers.empty()) m_met_triggers = procTrig;
  if(m_selectTriggers.empty()) m_selectTriggers = selTrig;

  m_name = (m_name == "") ? this->name() : m_name;
  size_t pos;
  if( (pos = m_name.find("ToolSvc.")) != std::string::npos ) m_name = m_name.erase(pos,8);
  m_curdir = "/AANT/" + m_folderName + m_name + '/';

}


/*-----------------------------------------------------*/
TrigVsOfflineMissingETTool::~TrigVsOfflineMissingETTool()
  /*-----------------------------------------------------*/
{
}


/*-----------------------------------------------------*/
StatusCode TrigVsOfflineMissingETTool::CBNT_initialize()
  /*-----------------------------------------------------*/
{

  StatusCode sc = service("THistSvc", m_thistSvc);

  if (sc.isFailure()) {
    msg() << MSG::ERROR
      << "Unable to retrieve pointer to THistSvc"
      << endmsg;
    return sc;
  }


  // init message stream
  m_debuglevel = (msg().level() <= MSG::DEBUG);

  msg() << MSG::DEBUG << "TrigVsOfflineMissingETTool : in initialize()" << endmsg;

  // retrive the trigger decision tool
  sc = m_trigDec.retrieve();
  if ( sc.isFailure() ){
    msg() << MSG::ERROR << "Could not retrieve TrigDecisionTool!" << endmsg;
    return sc;
  }

  // point to the signatures we want to make histos for
  if (m_useAllTriggers) {
    m_l1_met_signatures_tolook = &m_all_l1_met_triggers;
    m_l2_met_signatures_tolook = &m_all_l2_met_triggers;
    m_ef_met_signatures_tolook = &m_all_ef_met_triggers;
  } else {
    m_l1_met_signatures_tolook = &m_l1_met_signatures;
    m_l2_met_signatures_tolook = &m_l2_met_signatures;
    m_ef_met_signatures_tolook = &m_ef_met_signatures;
  }

  sc = bookHistograms();
  return sc;
}

StatusCode TrigVsOfflineMissingETTool::addHistogram(TH1 *h1) {

  if(!h1) return StatusCode::FAILURE;

  StatusCode sc = StatusCode::SUCCESS;
  std::string dir = m_curdir;
  std::string name = h1->GetName();
  std::string reg = dir + name;
  if(m_thistSvc->regHist(reg,h1).isFailure()) {
    msg() << MSG::ERROR << "Couldn't register 1d histogram \"" << name << "\" in " << dir << endmsg;
    sc = StatusCode::FAILURE;
  }
  msg() << MSG::DEBUG << "Registered 1d histogram \"" << name << "\" in " << dir << endmsg;
  return sc;
}

StatusCode TrigVsOfflineMissingETTool::addHistogram(TH2 *h2) {

  if(!h2) return StatusCode::FAILURE;

  StatusCode sc = StatusCode::SUCCESS;
  std::string dir = m_curdir;
  std::string name = h2->GetName();
  std::string reg = dir + name;
  if(m_thistSvc->regHist(reg,h2).isFailure()) {
    msg() << MSG::ERROR << "Couldn't register 2d histogram \"" << name << "\" in " << dir << endmsg;
  }
  msg() << MSG::DEBUG << "Registered 2d histogram \"" << name << "\" in " << dir << endmsg;
  return sc;
}

TH1* TrigVsOfflineMissingETTool::hist(const std::string& hist) {
  std::string dir = m_curdir;
  dir += hist;
  TH1 *h(0);
  if (m_thistSvc->getHist(dir,h).isSuccess()) {
  } else {
    msg() << MSG::ERROR << "Couldn't retrieve 1d histogram \"" << hist << "\" from " << dir << endmsg;
  }
  return h;
}

TH2* TrigVsOfflineMissingETTool::hist2(const std::string& hist) {
  std::string dir = m_curdir;
  dir += hist;
  TH2 *h(0);
  if (m_thistSvc->getHist(dir,h).isSuccess()) {
  } else {
    msg() << MSG::ERROR << "Couldn't retrieve 2d histogram \"" << hist << "\" from " << dir << endmsg;
  }
  return h;
}

std::string& TrigVsOfflineMissingETTool::getDir() {
  return m_curdir;
}

void TrigVsOfflineMissingETTool::setDir(const std::string& grp) {

  m_curdir  = "/AANT/" + m_folderName + m_name + '/';
  std::string add = "";

  // Parse input string and extract folder names
  std::vector<std::string> dirList;
  const char* separator="/";
  typedef  tokenizer<char_separator<char> > Tokenizer;
  Tokenizer tokComp(grp, char_separator<char>(separator));
  Tokenizer::iterator it = tokComp.begin();

  // Set directory to grp (relative to basedir)
  while(it != tokComp.end()) {
    msg() << MSG::DEBUG << *(it) << ";";
    dirList.push_back(*(it++));
  }
  size_t dsize = dirList.size();
  if(!dsize) return ;
  else {
    for(size_t j = 0; j < dsize; j++) {
      if (j == 0 && dirList[j] == "AANT") ;
      else if((/*j == 1 &&*/ dirList[j] == m_name)) ;
      else {
        add += dirList[j];
        add += '/';
      }
    }
  }
  m_curdir += add;
}

/*-----------------------------------------------------*/
StatusCode TrigVsOfflineMissingETTool::bookHistograms()
  /*-----------------------------------------------------*/
{
  if (m_debuglevel) msg() << MSG::DEBUG << "TrigVsOfflineMissingETTool : in book()" << endmsg;

  // get all met triggers
  if(m_useAllTriggers) // get all met trigger items
    getAllMetTriggers();  
  else { // met triggers specified in the input list
    if(m_met_triggers.size()) {
      // check if input met triggers are in trigger configuration
      // put the correct met triggers in m_l*_met_signatures and so on
      checkTriggers(m_met_triggers, true);
    }
  }

  // sample selection triggers specified in the input list
  if(m_selectTriggers.size()) {
    // check if input sample selection triggers are in triggger configuration
    // put the correct met triggers in m_sample_selection_signatures
    checkTriggers(m_selectTriggers, false);
  }



  /** histogram ranges and bins */

  // et
  m_low[0] = -13.5;
  m_high[0] = 601.5;
  m_nbins[0] = 205;

  // sumet
  m_low[1] = -27.;
  m_high[1] = 1803.;
  m_nbins[1] = 305;

  // phi, dPhi
  m_low[2] = -3.1416;
  m_high[2] = 3.1416;
  m_nbins[2] = 64;

  // dEt, dEx, dEy
  m_low[3] = -150.;
  m_high[3] = 150.;
  m_nbins[3] = 60;

  bookHistograms_allStats();

  if (m_l1_met_signatures_tolook->size())
    bookL1Histograms();

  if (m_l2_met_signatures_tolook->size())
    bookL2Histograms();

  if (m_ef_met_signatures_tolook->size())
    bookEFHistograms();

  if (m_doTrigEff)
    bookEffiHistograms();

  return StatusCode::SUCCESS;
} 


/*----------------------------------------------------*/
void TrigVsOfflineMissingETTool::addBasicL1Histograms()
  /*----------------------------------------------------*/
{
  // L1MET
  // et sumet phi histograms
  addHistogram(new TH1F("L1met_et", "et_L1met ; Et", m_nbins[0],m_low[0],m_high[0]));
  addHistogram(new TH1F("L1met_sumet", "sumet_L1met ; sumEt", m_nbins[1],m_low[1],m_high[1]));
  addHistogram(new TH1F("L1met_phi", "phi_L1met ; Phi", m_nbins[2],m_low[2],m_high[2]));
}


/*----------------------------------------------------*/
void TrigVsOfflineMissingETTool::addBasicL2Histograms()
  /*----------------------------------------------------*/
{
  // L2 MET
  // et sumet phi histograms
  addHistogram(new TH1F("L2met_et", "et_L2met ; Et", m_nbins[0],m_low[0],m_high[0]));
  addHistogram(new TH1F("L2met_sumet", "sumet_L2met ; sumEt", m_nbins[1],m_low[1],m_high[1]));
  addHistogram(new TH1F("L2met_phi", "phi_L2met ; Phi", m_nbins[2],m_low[2],m_high[2]));
}


/*----------------------------------------------------*/
void TrigVsOfflineMissingETTool::addBasicEFHistograms()
  /*----------------------------------------------------*/
{
  // EF MET
  // et sumet phi histograms
  addHistogram(new TH1F("EFmet_et", "et_EFmet ; Et", m_nbins[0],m_low[0],m_high[0]));
  addHistogram(new TH1F("EFmet_sumet", "sumet_EFmet ; sumEt", m_nbins[1],m_low[1],m_high[1]));
  addHistogram(new TH1F("EFmet_phi", "phi_EFmet ; Phi", m_nbins[2],m_low[2],m_high[2]));
}


/*----------------------------------------------------*/
void TrigVsOfflineMissingETTool::addBasicRecHistograms()
  /*----------------------------------------------------*/
{
  // RecMET
  // et sumet phi histograms
  addHistogram(new TH1F("Recmet_et", "et_Recmet ; Et", m_nbins[0],m_low[0],m_high[0]));
  addHistogram(new TH1F("Recmet_sumet", "sumet_Recmet ; sumEt", m_nbins[1],m_low[1],m_high[1]));
  addHistogram(new TH1F("Recmet_phi", "phi_Recmet ; Phi", m_nbins[2],m_low[2],m_high[2]));
}


/*----------------------------------------------------*/
void TrigVsOfflineMissingETTool::bookHistograms_allStats()
  /*----------------------------------------------------*/
{
  // histograms with no triggers
  setDir("Summary/allStats");
  addBasicL1Histograms();
  addBasicL2Histograms();
  addBasicEFHistograms();
  addBasicRecHistograms();
}


/*----------------------------------------------------*/
void TrigVsOfflineMissingETTool::bookL1Histograms()
  /*----------------------------------------------------*/
{
  std::string generic_path_trigmetmonitoring = "";

  std::map<std::string,int>::const_iterator it;

  for (it = m_l1_met_signatures_tolook->begin(); it != m_l1_met_signatures_tolook->end(); it++) {

    std::string expert_path_trigmetmonitoring = generic_path_trigmetmonitoring + "/Detail/" + it->first;

    setDir(expert_path_trigmetmonitoring);

    addBasicL1Histograms();
    addBasicRecHistograms();

    setDir(expert_path_trigmetmonitoring);
    // correlation histograms -- L1MET vs RecMET
    addHistogram(new TH2F("L1met_Recmet_EtCor", "EtCor_L1met_Recmet ; RecMET_Et ; L1MET_Et", m_nbins[0],m_low[0],m_high[0], m_nbins[0],m_low[0],m_high[0]));
    addHistogram(new TH2F("L1met_Recmet_PhiCor", "PhiCor_L1met_Recmet ; RecMET_Phi ; L1MET_Phi", m_nbins[2],m_low[2],m_high[2], m_nbins[2],m_low[2],m_high[2]));
    addHistogram(new TH1F("L1met_Recmet_dEt", "dEt_L1met_Recmet ; dEt", m_nbins[3],m_low[3],m_high[3]));
    addHistogram(new TH1F("L1met_Recmet_dPhi", "dPhi_L1met_Recmet ; dPhi", m_nbins[2],m_low[2],m_high[2]));
    addHistogram(new TH1F("L1met_Recmet_dEx", "dEx_L1met_Recmet ; dEx", m_nbins[3],m_low[3],m_high[3]));
    addHistogram(new TH1F("L1met_Recmet_dEy", "dEy_L1met_Recmet ; dEy", m_nbins[3],m_low[3],m_high[3]));

    // trigger efficiency histograms
    addHistogram(new TH1F("L1_et_effi_wrt_RecMET", "et_effi_wrt_RecMET_L1 ; Et", m_nbins[0],m_low[0],m_high[0]));
    addHistogram(new TH1F("L1_phi_effi_wrt_RecMET", "phi_effi_wrt_RecMET_L1 ; Phi", m_nbins[2],m_low[2],m_high[2]));

    if (m_debuglevel) {
      msg() << MSG::DEBUG << "INSIDE TrigVsOfflineMissingETTool : " << expert_path_trigmetmonitoring
        << "/" << " Histograms" << " booked successfully" << endmsg;
    }
  }
}


/*----------------------------------------------------*/
void TrigVsOfflineMissingETTool::bookL2Histograms()
  /*----------------------------------------------------*/
{
  std::string generic_path_trigmetmonitoring = "";

  std::map<std::string,int>::const_iterator it;

  for (it = m_l2_met_signatures_tolook->begin(); it != m_l2_met_signatures_tolook->end(); it++) {

    std::string expert_path_trigmetmonitoring = generic_path_trigmetmonitoring + "/Detail/" + it->first;
    setDir(expert_path_trigmetmonitoring);

    addBasicL2Histograms();
    addBasicRecHistograms();         

    // correlation histograms -- L2MET vs RecMET
    addHistogram(new TH2F("L2met_Recmet_EtCor", "EtCor_L2met_Recmet ; RecMET_Et ; L2MET_Et", m_nbins[0],m_low[0],m_high[0], m_nbins[0],m_low[0],m_high[0]));
    addHistogram(new TH2F("L2met_Recmet_PhiCor", "PhiCor_L2met_Recmet ; RecMET_Phi ; L2MET_Phi", m_nbins[2],m_low[2],m_high[2], m_nbins[2],m_low[2],m_high[2]));
    addHistogram(new TH1F("L2met_Recmet_dEt", "dEt_L2met_Recmet ; dEt", m_nbins[3],m_low[3],m_high[3]));
    addHistogram(new TH1F("L2met_Recmet_dPhi", "dPhi_L2met_Recmet ; dPhi", m_nbins[2],m_low[2],m_high[2]));
    addHistogram(new TH1F("L2met_Recmet_dEx", "dEx_L2met_Recmet ; dEx", m_nbins[3],m_low[3],m_high[3]));
    addHistogram(new TH1F("L2met_Recmet_dEy", "dEy_L2met_Recmet ; dEy", m_nbins[3],m_low[3],m_high[3]));

    // trigger efficiency histograms
    addHistogram(new TH1F("L2_et_effi_wrt_RecMET", "et_effi_wrt_RecMET_L2 ; Et", m_nbins[0],m_low[0],m_high[0]));
    addHistogram(new TH1F("L2_phi_effi_wrt_RecMET", "phi_effi_wrt_RecMET_L2 ; Phi", m_nbins[2],m_low[2],m_high[2]));

    if (m_debuglevel) {
      msg() << MSG::DEBUG << "INSIDE TrigVsOfflineMissingETTool : " << expert_path_trigmetmonitoring
        << "/" << " Histograms" << " booked successfully" << endmsg;
    }
  }
}


/*----------------------------------------------------*/
void TrigVsOfflineMissingETTool::bookEFHistograms()
  /*----------------------------------------------------*/
{
  std::string generic_path_trigmetmonitoring = "";

  std::map<std::string,int>::const_iterator it;

  for (it = m_ef_met_signatures_tolook->begin(); it != m_ef_met_signatures_tolook->end(); it++) {

    std::string expert_path_trigmetmonitoring = generic_path_trigmetmonitoring + "/Detail/" + it->first;
    setDir(expert_path_trigmetmonitoring);

    addBasicEFHistograms();
    addBasicRecHistograms();

    // correlation histograms -- EFMET vs RecMET

    addHistogram(new TH2F("EFmet_Recmet_EtCor", "EtCor_EFmet_Recmet ; RecMET_Et ; EFMET_Et", m_nbins[0],m_low[0],m_high[0], m_nbins[0],m_low[0],m_high[0]));
    addHistogram(new TH2F("EFmet_Recmet_PhiCor", "PhiCor_EFmet_Recmet ; RecMET_Phi ; EFMET_Phi", m_nbins[2],m_low[2],m_high[2], m_nbins[2],m_low[2],m_high[2]));
    addHistogram(new TH1F("EFmet_Recmet_dEt", "dEt_EFmet_Recmet ; dEt", m_nbins[3],m_low[3],m_high[3]));
    addHistogram(new TH1F("EFmet_Recmet_dPhi", "dPhi_EFmet_Recmet ; dPhi", m_nbins[2],m_low[2],m_high[2]));
    addHistogram(new TH1F("EFmet_Recmet_dEx", "dEx_EFmet_Recmet ; dEx", m_nbins[3],m_low[3],m_high[3]));
    addHistogram(new TH1F("EFmet_Recmet_dEy", "dEy_EFmet_Recmet ; dEy", m_nbins[3],m_low[3],m_high[3]));   

    // trigger efficiency histograms
    addHistogram(new TH1F("EF_et_effi_wrt_RecMET", "et_effi_wrt_RecMET_EF ; Et", m_nbins[0],m_low[0],m_high[0]));
    addHistogram(new TH1F("EF_phi_effi_wrt_RecMET", "phi_effi_wrt_RecMET_EF ; Phi", m_nbins[2],m_low[2],m_high[2]));

    if (m_debuglevel) {
      msg() << MSG::DEBUG << "INSIDE TrigVsOfflineMissingETTool : " << expert_path_trigmetmonitoring
        << "/" << " Histograms" << " booked successfully" << endmsg;
    }
  }
}


/*----------------------------------------------------*/
void TrigVsOfflineMissingETTool::bookEffiHistograms()
  /*----------------------------------------------------*/
{
  std::map<std::string,int>::const_iterator it_s;
  std::map<std::string,int>::const_iterator it;

  for (it_s = m_sample_selection_signatures.begin(); it_s != m_sample_selection_signatures.end(); it_s++) {
    std::string generic_path_trigmetmonitoring = "";
    std::string expert_path_trigmetmonitoring = generic_path_trigmetmonitoring + "/Detail/" + it_s->first;

    setDir(expert_path_trigmetmonitoring);

    addBasicRecHistograms();  

    std::vector<std::map<std::string,int>*> met_signatures_tolook;
    met_signatures_tolook.push_back(m_l1_met_signatures_tolook);
    met_signatures_tolook.push_back(m_l2_met_signatures_tolook);
    met_signatures_tolook.push_back(m_ef_met_signatures_tolook);
    unsigned int size = met_signatures_tolook.size();

    std::string levels[3] = {"L1", "L2", "EF"};

    for (unsigned int i=0; i<size; i++) {
      for (it = met_signatures_tolook[i]->begin(); it != met_signatures_tolook[i]->end(); it++) {
        std::string expert_path_trigmetmonitoring = generic_path_trigmetmonitoring + "/Detail/" + it_s->first + "/" + it->first;

        setDir(expert_path_trigmetmonitoring);

        addBasicRecHistograms();

        // trigger efficiency histograms
        std::string name = levels[i] + "_et_effi_wrt_RecMET";
        std::string title = "et_effi_wrt_RecMET_" + levels[i] + " ; Et";
        addHistogram(new TH1F(name.c_str(), title.c_str(), m_nbins[0],m_low[0],m_high[0]));
        name = levels[i] + "_phi_effi_wrt_RecMET";
        title = "phi_effi_wrt_RecMET_" + levels[i] + " ; Phi";
        addHistogram(new TH1F(name.c_str(), title.c_str(), m_nbins[2],m_low[2],m_high[2]));

        if (m_debuglevel) {
          msg() << MSG::DEBUG << "INSIDE TrigVsOfflineMissingETTool : " << expert_path_trigmetmonitoring
            << "/" << " Histograms" << " booked successfully" << endmsg;
        }
      } // end of loop over l* met signatures to look
    } // end of loop over levels
  } // end of loop over m_selectTriggers
}

StatusCode TrigVsOfflineMissingETTool::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode TrigVsOfflineMissingETTool::execute(MissingETData *data) {

  /** check that the input and the output containers are defined */
  StatusCode sc = StatusCode::SUCCESS;

  msg() << MSG::DEBUG << "execute() has been called" << endmsg;

  trigger_decision();
  sc = trigVsOfflineMissingETPlots(data);

  return sc;
}


// Trigger decision function - called by fillHistograms() on event by event
/*----------------------------------------------------*/
void TrigVsOfflineMissingETTool::trigger_decision()
  /*----------------------------------------------------*/
{
  // fill maps of met triggers to look with trigger statistics

  std::vector<std::map<std::string,int>*> met_signatures_tolook;
  met_signatures_tolook.push_back(m_l1_met_signatures_tolook);
  met_signatures_tolook.push_back(m_l2_met_signatures_tolook);
  met_signatures_tolook.push_back(m_ef_met_signatures_tolook);

  if(m_printTrigStats) {
    std::map<std::string,int>::iterator iter;
    for (unsigned int i = 0; i<met_signatures_tolook.size(); i++) {
      for ( iter = met_signatures_tolook[i]->begin(); iter != met_signatures_tolook[i]->end(); ++iter) {
        std::string name = iter->first;
        if (m_trigDec->isPassed(name)) {
          iter->second +=1;
        }
      }
    } 
  }
}


// met function - called by fill() on event by event
/*----------------------------------------------------*/
StatusCode TrigVsOfflineMissingETTool::trigVsOfflineMissingETPlots(MissingETData *data)
  /*----------------------------------------------------*/
{
  if (m_debuglevel) msg() << MSG::DEBUG << "in met()" << endmsg;

  StatusCode sc = StatusCode::SUCCESS;

  std::string allStatsMonGroup = "Summary/allStats";
  std::string expertpartialMonGroup = "Detail/";

  std::map<std::string,int>::const_iterator it;


  // Retrieve offline MET
  // fill histograms for offline MET
  const MissingET *missET_rec = 0;
  double et_RecMET=0., sumet_RecMET=0., ex_RecMET=0., ey_RecMET=0., phi_RecMET=0.;
  missET_rec = data->getMissingETbyName(m_met_ref); // data->refFinal();
  if (!missET_rec){
    msg() << MSG::DEBUG << "Could not retrieve Reconstructed MET term" << endmsg;
  } else {
    et_RecMET = (missET_rec->et())/GeV;
    sumet_RecMET = (missET_rec->sumet())/GeV;
    ex_RecMET = (missET_rec->etx())/GeV;
    ey_RecMET = (missET_rec->ety())/GeV;
    phi_RecMET = missET_rec->phi();
    msg() << MSG::DEBUG << "Rec MET: "<< et_RecMET << " Rec SumEt: " << sumet_RecMET << endmsg;  

    setDir(allStatsMonGroup);
    hist("Recmet_et")->Fill(et_RecMET);
    hist("Recmet_sumet")->Fill(sumet_RecMET);
    hist("Recmet_phi")->Fill(phi_RecMET);

    std::map<std::string,int>::const_iterator it_s;
    std::vector<std::map<std::string,int>*> met_signatures_tolook;
    met_signatures_tolook.push_back(m_l1_met_signatures_tolook);
    met_signatures_tolook.push_back(m_l2_met_signatures_tolook);
    met_signatures_tolook.push_back(m_ef_met_signatures_tolook);
    unsigned int size = met_signatures_tolook.size();

    for (it_s = m_sample_selection_signatures.begin(); it_s != m_sample_selection_signatures.end(); it_s++) {
      if (m_trigDec->isPassed(it_s->first)) {
        std::string tmp_MonGroup = expertpartialMonGroup + it_s->first;
        std::string tmp_partialMonGroup = tmp_MonGroup + "/";
        setDir(tmp_MonGroup);
        hist("Recmet_et")->Fill(et_RecMET);
        hist("Recmet_sumet")->Fill(sumet_RecMET);
        hist("Recmet_phi")->Fill(phi_RecMET);

        for (unsigned int i=0; i<size; i++) {
          for (it = met_signatures_tolook[i]->begin(); it != met_signatures_tolook[i]->end(); it++) {
            std::string name = it->first;
            if (m_trigDec->isPassed(name)) {
              setDir(tmp_partialMonGroup + name);
              hist("Recmet_et")->Fill(et_RecMET);
              hist("Recmet_sumet")->Fill(sumet_RecMET);
              hist("Recmet_phi")->Fill(phi_RecMET);
            }
          }
        }
      }
    }
  }


  // Retrieve the L1 met roi
  // fill et, sumet, phi histograms
  // fill correlation histograms
  const LVL1_ROI *lvl1_roi = 0;
  lvl1_roi = data->getL1ROI();
  if( !lvl1_roi ) {
    msg() << MSG::DEBUG
      << "No LVL1 ROI found in TDS"
      << endmsg;
  }
  else {
    LVL1_ROI::energysums_type L1METROI = lvl1_roi->getEnergySumROIs();
    int l1metroi_size = L1METROI.size();
    if (l1metroi_size > 1)
      msg() << MSG::DEBUG << "Number of L1METROI's " << l1metroi_size << ", should be 1!" << endmsg;
    if (m_debuglevel)
      msg() << MSG::DEBUG << " Number of L1METROI's " << l1metroi_size
        << endmsg;

    LVL1_ROI::energysums_type::const_iterator it_L1 = L1METROI.begin();
    LVL1_ROI::energysums_type::const_iterator it_e_L1 = L1METROI.end();

    if (l1metroi_size == 0) { 
      msg() << MSG::DEBUG << "Could not retrieve L1 term" << endmsg;
    } else{
      for( ; it_L1 != it_e_L1; it_L1++) {
        double ex = it_L1->getEnergyX();
        double ey = it_L1->getEnergyY();
        double et = sqrt(ex*ex + ey*ey);
        double sumet = (it_L1->getEnergyT());
        double phi = atan2f(ey, ex);

        //double et = (fabs(it_L1->getEnergyT()))/GeV;
        //double sumet = (it_L1->getThrPatSummET())/GeV;
        //double ex = (it_L1->getExMiss())/GeV;
        //double ey = (it_L1->getEyMiss())/GeV;
        //double phi = atan2(ey, ex);
        msg() << MSG::DEBUG << "L1 MET: " << et << " L1 SumEt: " << sumet << endmsg;

        setDir(allStatsMonGroup);
        hist("L1met_et")->Fill(et);
        hist("L1met_sumet")->Fill(sumet);
        hist("L1met_phi")->Fill(phi);

        for (it = m_l1_met_signatures_tolook->begin(); it != m_l1_met_signatures_tolook->end(); it++) {
          std::string name = it->first;
          if(m_trigDec->isPassed(name)) {
            setDir(expertpartialMonGroup+name);
            hist("L1met_et")->Fill(et);
            hist("L1met_sumet")->Fill(sumet);
            hist("L1met_phi")->Fill(phi);
            hist("Recmet_et")->Fill(et_RecMET);
            hist("Recmet_sumet")->Fill(sumet_RecMET);
            hist("Recmet_phi")->Fill(phi_RecMET);
            // correlation with RecMET
            hist2("L1met_Recmet_EtCor")->Fill(et_RecMET,et);
            hist2("L1met_Recmet_PhiCor")->Fill(phi_RecMET,phi);
            hist("L1met_Recmet_dEt")->Fill(et-et_RecMET);
            hist("L1met_Recmet_dPhi")->Fill(phi-phi_RecMET);
            hist("L1met_Recmet_dEx")->Fill(ex-ex_RecMET);
            hist("L1met_Recmet_dEy")->Fill(ey-ey_RecMET); 
          }
        }
      }
    }
  }


  // hlt met
  const TrigMissingETContainer* trigMETcont=0;


  // fill histograms for L2
  const TrigMissingET *missETL2=0;
  trigMETcont = data->getL2MET();
  if (!trigMETcont) {
    msg() << MSG::DEBUG << "Could not retrieve L2 term : trigMETcont = 0" << endmsg;
  } else {
    TrigMissingETContainer::const_iterator trigMETfirst  = trigMETcont->begin();
    TrigMissingETContainer::const_iterator trigMETlast = trigMETcont->end();
    if (trigMETfirst != trigMETlast)
      missETL2 = *trigMETfirst;
  }

  if (!missETL2) { 
    msg() << MSG::DEBUG << "Could not retrieve L2 term" << endmsg;
  } else {
    double et = (missETL2->et())/GeV;
    double sumet = (missETL2->sumEt())/GeV;
    double ex = (missETL2->ex())/GeV;
    double ey = (missETL2->ey())/GeV;
    double phi = atan2(ey, ex);
    msg() << MSG::DEBUG << "HLT L2 MET: "<< et << " HLT L2 SumEt: " << sumet << endmsg;

    setDir(allStatsMonGroup);
    hist("L2met_et")->Fill(et);
    hist("L2met_sumet")->Fill(sumet);
    hist("L2met_phi")->Fill(phi);

    for (it = m_l2_met_signatures_tolook->begin(); it != m_l2_met_signatures_tolook->end(); it++) {
      std::string name = it->first;

      if (m_trigDec->isPassed(name)) {
        setDir(expertpartialMonGroup+name);
        hist("L2met_et")->Fill(et);
        hist("L2met_sumet")->Fill(sumet);
        hist("L2met_phi")->Fill(phi);
        hist("Recmet_et")->Fill(et_RecMET);
        hist("Recmet_sumet")->Fill(sumet_RecMET);
        hist("Recmet_phi")->Fill(phi_RecMET);
        // correlation with RecMET
        hist2("L2met_Recmet_EtCor")->Fill(et_RecMET,et);
        hist2("L2met_Recmet_PhiCor")->Fill(phi_RecMET,phi);
        hist("L2met_Recmet_dEt")->Fill(et-et_RecMET);
        hist("L2met_Recmet_dPhi")->Fill(phi-phi_RecMET);
        hist("L2met_Recmet_dEx")->Fill(ex-ex_RecMET);
        hist("L2met_Recmet_dEy")->Fill(ey-ey_RecMET);
      }
    }
  }


  // fill histograms for EF
  const TrigMissingET *missETEF = 0;
  trigMETcont = 0;
  trigMETcont = data->getEFMET_default(); //, METTags[1]);
  if (!trigMETcont){
    msg() << MSG::DEBUG << "Could not retrieve EF term : trigMETcont = 0" << endmsg;
  } else {
    TrigMissingETContainer::const_iterator trigMETfirst  = trigMETcont->begin();
    TrigMissingETContainer::const_iterator trigMETlast = trigMETcont->end();
    if (trigMETfirst != trigMETlast) 
      missETEF = *(trigMETcont->begin());
  }

  if (!missETEF) { 
    msg() << MSG::DEBUG << "Could not retrieve EF term" << endmsg;
  } else{
    double et = (missETEF->et())/GeV;
    double sumet = (missETEF->sumEt())/GeV;
    double ex = (missETEF->ex())/GeV;
    double ey = (missETEF->ey())/GeV;
    double phi = atan2(ey,ex);
    msg() << MSG::DEBUG << "HLT EF MET: "<< et << " HLT EF SumEt: " << sumet << endmsg;
    setDir(allStatsMonGroup);
    hist("EFmet_et")->Fill(et);
    hist("EFmet_sumet")->Fill(sumet);
    hist("EFmet_phi")->Fill(phi);
    for (it = m_ef_met_signatures_tolook->begin(); it != m_ef_met_signatures_tolook->end(); it++) {
      std::string name = it->first;
      if (m_trigDec->isPassed(name)) {
        setDir(expertpartialMonGroup+name);
        hist("EFmet_et")->Fill(et);
        hist("EFmet_sumet")->Fill(sumet);
        hist("EFmet_phi")->Fill(phi);
        hist("Recmet_et")->Fill(et_RecMET);
        hist("Recmet_sumet")->Fill(sumet_RecMET);
        hist("Recmet_phi")->Fill(phi_RecMET);
        // correlation with RecMET
        hist2("EFmet_Recmet_EtCor")->Fill(et_RecMET,et);
        hist2("EFmet_Recmet_PhiCor")->Fill(phi_RecMET,phi);
        hist("EFmet_Recmet_dEt")->Fill(et-et_RecMET);
        hist("EFmet_Recmet_dPhi")->Fill(phi-phi_RecMET);
        hist("EFmet_Recmet_dEx")->Fill(ex-ex_RecMET);
        hist("EFmet_Recmet_dEy")->Fill(ey-ey_RecMET);
      }
    }
  }

  return sc;
}


/*---------------------------------------------------------*/
StatusCode TrigVsOfflineMissingETTool::finalize()
  /*---------------------------------------------------------*/
{
  if (m_debuglevel) msg() << MSG::DEBUG << "TrigVsOfflineMissingETTool : in procHistograms()" << endmsg;
  msg() << MSG::DEBUG << "finalize() has been called" << endmsg;

  std::map<std::string, int>::const_iterator it_s;
  std::map<std::string, int>::const_iterator it;

  TH1 *Recmet_et_denom;
  TH1 *Recmet_phi_denom;
  TH1 *Recmet_et_num;
  TH1 *Recmet_phi_num;

  if(m_printTrigStats)
    printMetTriggerStats();

  //double ranges[2] = {0.,1000.};
  //double pars[5] = {22.00,0.7,0.6,20.,2.3};

  setDir("Summary/allStats");
  Recmet_et_denom = hist("Recmet_et");
  Recmet_phi_denom = hist("Recmet_phi");
  Recmet_et_denom->Sumw2();
  Recmet_phi_denom->Sumw2();

  for(it = m_l1_met_signatures_tolook->begin(); it != m_l1_met_signatures_tolook->end(); it++) {
    setDir(std::string("Detail/")+it->first);
    Recmet_et_num = hist("Recmet_et");
    Recmet_phi_num = hist("Recmet_phi");
    Recmet_et_num->Sumw2();
    Recmet_phi_num->Sumw2();
    hist("L1_et_effi_wrt_RecMET")->Divide(Recmet_et_num, Recmet_et_denom, 1.0, 1.0, "B");
    hist("L1_phi_effi_wrt_RecMET")->Divide(Recmet_phi_num, Recmet_phi_denom, 1.0, 1.0, "B");

    //metfit::Efficiency(hist("Recmet_et"), hist("Recmet_et", "Summary/allStats"), ranges, pars);
  }

  for(it = m_l2_met_signatures_tolook->begin(); it != m_l2_met_signatures_tolook->end(); it++) {
    setDir(std::string("Detail/")+it->first);
    Recmet_et_num = hist("Recmet_et");
    Recmet_phi_num = hist("Recmet_phi");
    Recmet_et_num->Sumw2();
    Recmet_phi_num->Sumw2();
    hist("L2_et_effi_wrt_RecMET")->Divide(Recmet_et_num, Recmet_et_denom, 1.0, 1.0, "B");
    hist("L2_phi_effi_wrt_RecMET")->Divide(Recmet_phi_num, Recmet_phi_denom, 1.0, 1.0, "B");

    //metfit::Efficiency(hist("Recmet_et"), hist("Recmet_et", "Summary/allStats"), ranges, pars);
  }

  for(it = m_ef_met_signatures_tolook->begin(); it != m_ef_met_signatures_tolook->end(); it++) {
    setDir(std::string("Detail/")+it->first);
    Recmet_et_num = hist("Recmet_et");
    Recmet_phi_num = hist("Recmet_phi");
    Recmet_et_num->Sumw2();
    Recmet_phi_num->Sumw2();
    hist("EF_et_effi_wrt_RecMET")->Divide(Recmet_et_num, Recmet_et_denom, 1.0, 1.0, "B");
    hist("EF_phi_effi_wrt_RecMET")->Divide(Recmet_phi_num, Recmet_phi_denom, 1.0, 1.0, "B");

    //metfit::Efficiency(hist("Recmet_et"), hist("Recmet_et", "Summary/allStats"), ranges, pars);
  }

  for(it_s = m_sample_selection_signatures.begin(); it_s != m_sample_selection_signatures.end(); it_s++) {
    setDir(std::string("Detail/")+it_s->first);
    Recmet_et_denom = hist("Recmet_et");
    Recmet_phi_denom = hist("Recmet_phi");
    Recmet_et_denom->Sumw2();
    Recmet_phi_denom->Sumw2();

    for(it = m_l1_met_signatures_tolook->begin(); it != m_l1_met_signatures_tolook->end(); it++) {
      setDir(std::string("Detail/")+it_s->first+std::string("/")+it->first);
      Recmet_et_num = hist("Recmet_et");
      Recmet_phi_num = hist("Recmet_phi");
      Recmet_et_num->Sumw2();
      Recmet_phi_num->Sumw2();
      hist("L1_et_effi_wrt_RecMET")->Divide(hist("Recmet_et"), Recmet_et_denom, 1.0, 1.0, "B");
      hist("L1_phi_effi_wrt_RecMET")->Divide(hist("Recmet_phi"), Recmet_phi_denom, 1.0, 1.0, "B");
    }

    for(it = m_l2_met_signatures_tolook->begin(); it != m_l2_met_signatures_tolook->end(); it++) {
      setDir(std::string("Detail/")+it_s->first+std::string("/")+it->first);
      Recmet_et_num = hist("Recmet_et");
      Recmet_phi_num = hist("Recmet_phi");
      Recmet_et_num->Sumw2();
      Recmet_phi_num->Sumw2();
      hist("L2_et_effi_wrt_RecMET")->Divide(Recmet_et_num, Recmet_et_denom, 1.0, 1.0, "B");
      hist("L2_phi_effi_wrt_RecMET")->Divide(Recmet_phi_num, Recmet_phi_denom, 1.0, 1.0, "B");
    }

    for(it = m_ef_met_signatures_tolook->begin(); it != m_ef_met_signatures_tolook->end(); it++) {
      setDir(std::string("Detail/")+it_s->first+std::string("/")+it->first);
      Recmet_et_num = hist("Recmet_et");
      Recmet_phi_num = hist("Recmet_phi");
      Recmet_et_num->Sumw2();
      Recmet_phi_num->Sumw2();
      hist("EF_et_effi_wrt_RecMET")->Divide(Recmet_et_num, Recmet_et_denom, 1.0, 1.0, "B");
      hist("EF_phi_effi_wrt_RecMET")->Divide(Recmet_phi_num, Recmet_phi_denom, 1.0, 1.0, "B");
    }
  }


  return StatusCode::SUCCESS;
}


/*-----------------------------------------------------*/
void TrigVsOfflineMissingETTool::getAllMetTriggers()
  /*-----------------------------------------------------*/
{
  // clear the list from previous run
  m_all_l1_met_triggers.clear();
  m_all_l2_met_triggers.clear();
  m_all_ef_met_triggers.clear();

  // store L1 MET trigger items in map
  const std::vector<std::string> TrigL1Items = m_trigDec->getListOfTriggers("L1_.*");
  std::vector<std::string>::const_iterator iter;
  for (iter = TrigL1Items.begin(); iter != TrigL1Items.end(); ++iter) {
    std::string L1name = *iter;
    std::string tmp_trigType = L1name.substr(0,5);
    msg() << MSG::INFO  << "Trigger Item " << L1name  << " defined " << endmsg;
    // store L1 MET trigger item names
    if (tmp_trigType=="L1_TE" || tmp_trigType=="L1_XE")
      m_all_l1_met_triggers.insert(std::map<std::string,int>::value_type(L1name,0));
  }

  // store HLT MET trigger items in map
  const std::vector<std::string> TrigHLTItems = m_trigDec->getListOfTriggers("L2_.*|EF_.*");
  std::vector<std::string>::const_iterator chain;
  for (chain= TrigHLTItems.begin(); chain!= TrigHLTItems.end(); ++chain) {
    std::string HLTname = *chain;
    float prescale = m_trigDec->getPrescale(*chain);
    // store HLT MET trigger item names divided in L2 and EF
    std::string tmp_trigType =  HLTname.substr(0,5);
    msg() << MSG::INFO  << "Trigger Item " << HLTname  << " defined with prescale " << prescale << " , " << tmp_trigType<< endmsg;
    if (tmp_trigType=="L2_te" || tmp_trigType=="L2_xe") {
      m_all_l2_met_triggers.insert(std::map<std::string,int>::value_type(HLTname,0));
    } else if (tmp_trigType=="EF_te" || tmp_trigType=="EF_xe") {
      m_all_ef_met_triggers.insert(std::map<std::string,int>::value_type(HLTname,0));
    }
  }
}


/*-----------------------------------------------------*/
void TrigVsOfflineMissingETTool::checkTriggers(std::vector<std::string>& triggers,
    bool isInputMetTriggers)
/*-----------------------------------------------------*/
{
  // these iterators we need for checking the requested trigger exist
  const std::vector<std::string>TrigL1Items = m_trigDec->getListOfTriggers("L1_.*");
  std::vector<std::string>::const_iterator iter;
  const std::vector<std::string> TrigHLTItems = m_trigDec->getListOfTriggers("L2_.*|EF_.*");
  std::vector<std::string>::const_iterator chain;

  for(unsigned int it = 0; it < triggers.size(); ++it) { // Loop over trigger items
    std::string item = triggers[it];
    std::string tmp_trigLevel =  item.substr(0,3);
    if(m_debuglevel)
      msg() << MSG::DEBUG  << "signature " << item << " requested" << endmsg;
    // check your trigger is defined in the menu
    bool triggerFound = false;
    if (tmp_trigLevel == "L1_") {
      for (iter=TrigL1Items.begin(); iter!=TrigL1Items.end(); ++iter) {
        std::string L1name = *iter;
        if (L1name == item) {
          triggerFound=true;
          if (isInputMetTriggers)
            m_l1_met_signatures.insert(std::map<std::string,int>::value_type(item,0));
        }
      }
      if (!triggerFound) {
        msg() << MSG::DEBUG  << "L1 signature " << item << " requested but not found in the menu" << endmsg;
      }
    } 
    else if (tmp_trigLevel == "L2_" || tmp_trigLevel == "EF_") {
      for (chain=TrigHLTItems.begin(); chain!=TrigHLTItems.end(); ++chain) {
        std::string HLTname = *chain;
        if (HLTname == item) {
          triggerFound=true;
          if (isInputMetTriggers) {
            if (tmp_trigLevel == "L2_") {
              m_l2_met_signatures.insert(std::map<std::string,int>::value_type(item,0));
            }
            else if (tmp_trigLevel == "EF_") {
              m_ef_met_signatures.insert(std::map<std::string,int>::value_type(item,0));
            }
          }
        }
      }
      if (!triggerFound)
        msg() << MSG::DEBUG << "HLT signature " << item << " requested but not found in the menu" << endmsg;
    }
    if(triggerFound && !isInputMetTriggers)
      m_sample_selection_signatures.insert(std::map<std::string,int>::value_type(item,0));
  }
}


/*-----------------------------------------------------*/
void TrigVsOfflineMissingETTool::printMetTriggerStats()
  /*-----------------------------------------------------*/
{
  // L1
  msg() << MSG::INFO  << "Level-1 Met Triggers to look: " << m_l1_met_signatures_tolook->size() << endmsg;
  std::map<std::string,int>::iterator iter;
  for (iter = m_l1_met_signatures_tolook->begin(); iter!= m_l1_met_signatures_tolook->end(); ++iter) {
    msg() << MSG::INFO  << "Events passing " << iter->first << ": " << iter->second << endmsg;
  }

  // L2
  msg() << MSG::INFO  << "Level-2 Met Triggers to look: " << m_l2_met_signatures_tolook->size()<<endmsg;
  for (iter = m_l2_met_signatures_tolook->begin(); iter != m_l2_met_signatures_tolook->end(); ++iter) {
    msg() << MSG::INFO  << "Events passing " << iter->first << ": " << iter->second << endmsg;
  }

  // EF
  msg() << MSG::INFO  << "EF Met Triggers to look: "  << m_ef_met_signatures_tolook->size() << endmsg;
  for (iter = m_ef_met_signatures_tolook->begin(); iter != m_ef_met_signatures_tolook->end(); ++iter) {
    msg() << MSG::INFO  << "Events passing " << iter->first << ": " << iter->second << endmsg;
  }
}
