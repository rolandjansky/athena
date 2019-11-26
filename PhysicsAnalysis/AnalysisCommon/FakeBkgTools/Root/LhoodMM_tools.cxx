/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "FakeBkgTools/LhoodMM_tools.h"
#include "FakeBkgTools/FakeBkgInternals.h"
#include "FakeBkgTools/TMinuit_LHMM.h"
#include "FakeBkgTools/Database.h"
#include "PathResolver/PathResolver.h"
#include "TH1.h"
#include "TH2.h"
#include "TMatrixT.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TTreeReader.h"
#include "TKey.h"
#include "TDirectory.h"
#include <chrono>
#include <memory>

#ifndef FAKEBKGTOOLS_ATLAS_ENVIRONMENT
  #define declareProperty(n, p, h) ExtraPropertyManager<LhoodMM_tools>::declareProperty(n, &LhoodMM_tools::p, h)
#endif

using namespace CP;
using FakeBkgTools::Efficiency;
using FakeBkgTools::Uncertainty;
using std::string;
using std::vector;
using Clock=std::chrono::high_resolution_clock;

const int LhoodMM_tools::nLepMax;
const int LhoodMM_tools::maxRank; 

LhoodMM_tools* LhoodMM_tools::m_current_lhoodMM_tool;

LhoodMM_tools::LhoodMM_tools(const std::string& name)  :
  BaseFakeBkgTool(name)
{
 
  //setup fixHistogramNormalization property
  declareProperty("FixHistogramNormalization", m_fixNormalization, "Boolean to determine whether or not histograms are scaled such that their normalization is equal to the fake yield computed for the entire sample (true =  yes, do the scaleing)");

  declareProperty("DoFakeFactorFit",  m_doFakeFactor, "Give results corresponding to the fake factor method rather than the matrix method");

  // set everything to default values
  reset();
}

LhoodMM_tools::~LhoodMM_tools() {

}



FakeBkgTools::Client LhoodMM_tools::clientForDB()
{
    return FakeBkgTools::Client::MATRIX_METHOD;
}


StatusCode LhoodMM_tools::initialize()
{
  return BaseFakeBkgTool::initialize();
}

void LhoodMM_tools::reset() {

  m_nfakes_std = 1.;
  m_nfakes_std_err = 1.;

  m_maxWeight = 1.;

  m_ssfrac = 1.;

  m_printLevel = -1;

  m_theta_tot_start_index = -1;

  m_do_std_perEventWeight = false;

  m_fixNormalization = false;

  m_mmevts_total.clear();

  m_mmevts_internal.clear();
  m_mmevts_internal.resize(nLepMax+1);
  ATH_MSG_VERBOSE("m_mmevts_internal.size() = " << m_mmevts_internal.size());
  ATH_MSG_VERBOSE("nLepMax = " << nLepMax);

  m_real_indices.clear();
  m_real_indices.resize(nLepMax+1);
  m_fake_indices.clear();
  m_fake_indices.resize(nLepMax+1);

  m_maxnlep_loose = 0;

  m_requireSS = false;
  m_requireOS = false;

  m_fsvec.clear();
  m_fsvec.resize(nLepMax+1);

  m_OSfrac.clear();
  m_OSfrac.resize(nLepMax+1);
  for (int ilep = 0; ilep <=nLepMax; ilep++) {
    m_OSfrac[ilep].resize(ilep+1);
  }

  m_doFakeFactor = false;

  m_nrf_mat_vec.clear();
  m_MMmatrix_vec.clear();
  m_nrf_mat_vec.resize(nLepMax+1);
  m_MMmatrix_vec.resize(nLepMax+1);
  m_ntlpred_vec.clear();
  m_ntlpred_vec.resize(nLepMax+1);
  for (int ilep = 0; ilep <= nLepMax; ilep++) {
    m_nrf_mat_vec[ilep] = std::shared_ptr<TMatrixT<double>>(std::make_shared<TMatrixT<double>>(0x1 << ilep,1) );
    m_MMmatrix_vec[ilep] =  std::shared_ptr<TMatrixT<double>>(std::make_shared<TMatrixT<double>> ((0x1 << ilep),(0x1 << ilep)));
    m_ntlpred_vec[ilep] = std::shared_ptr<TMatrixT<double>>(std::make_shared< TMatrixT<double>>(0x1 << ilep,1));
   }

  m_lastSaveIndex = 0;

  m_alreadyMerged = false;

}

StatusCode LhoodMM_tools::setFitType(std::string ft) {
  if (ft == "FF") {
    m_doFakeFactor = true;
    return StatusCode::SUCCESS;
  } else if (ft == "MM") {
    m_doFakeFactor = false;
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_ERROR("Error in  LhoodMM_tools::setFitType: please specify \"MM\" for matrix method or \"FF\" for fake factor method");
    return StatusCode::FAILURE;
  } 
}

StatusCode LhoodMM_tools::register1DHistogram(TH1* h1, const float *val)  {

  auto sc = BaseFakeBkgTool::register1DHistogram(h1, val);

  if(sc != StatusCode::SUCCESS) return sc;

  auto currentMap = m_mmevts_1dhisto_map.find(h1);
  if (currentMap ==  m_mmevts_1dhisto_map.end() ) {
    m_mmevts_1dhisto_map[h1];
    ATH_MSG_INFO("Registered a 1D histogram "<<h1->GetName());
  } else {
    ATH_MSG_ERROR("Error in  LhoodMM_tools::register1DHistogram: histogram has already been registered");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
} 

StatusCode LhoodMM_tools::register2DHistogram(TH2* h2, const float *xval, const float *yval) {

  auto sc = BaseFakeBkgTool::register2DHistogram(h2, xval, yval);

  if(sc != StatusCode::SUCCESS) return sc;
    
  auto currentMap = m_mmevts_2dhisto_map.find(h2);
  if (currentMap ==  m_mmevts_2dhisto_map.end() ) {
    m_mmevts_2dhisto_map[h2];
    ATH_MSG_INFO("Registered a 2D histogram"<< h2->GetName());
  } else {
    ATH_MSG_ERROR("histogram has already been registered");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
} 

StatusCode LhoodMM_tools::addEventCustom(const std::vector<bool>& isTight_vals,
                 const std::vector<Efficiency>& realEff_vals,
                 const std::vector<Efficiency>& fakeEff_vals,
                 const std::vector<int>& charges,
                 float extraweight) {

  if (extraweight > m_maxWeight) m_maxWeight = extraweight;

  int nlep = isTight_vals.size();
 
  LhoodMMEvent mmevt(nlep, realEff_vals, fakeEff_vals, isTight_vals, charges, extraweight);
  m_mmevts_total.push_back(mmevt);
  
  // fill mmevts vectors for all registered histograms
  std::map<TH1*, const float*>::iterator map1_iter;
  for (map1_iter = m_values_1dhisto_map.begin(); map1_iter != m_values_1dhisto_map.end(); map1_iter++) {
    const float* val = map1_iter->second;
    LhoodMMEvent mmevt1(nlep, realEff_vals, fakeEff_vals, isTight_vals, charges, extraweight, *val);
    auto histoMap = m_mmevts_1dhisto_map.find(map1_iter->first);
    if (histoMap != m_mmevts_1dhisto_map.end() ) {
      histoMap->second.push_back(mmevt1);
    } else {
      ATH_MSG_ERROR("Can not find entry for 1d histogram"); 
      return StatusCode::FAILURE;
    }
  }
  
  std::map<TH2*, std::pair<const float*, const float*> >::iterator map2_iter;
  for (map2_iter = m_values_2dhisto_map.begin(); map2_iter != m_values_2dhisto_map.end(); map2_iter++) {
    std::pair<const float*, const float*> val = map2_iter->second;
    LhoodMMEvent mmevt2(nlep, realEff_vals, fakeEff_vals, isTight_vals, charges,extraweight, *(val.first), *(val.second));
    
    auto histoMap = m_mmevts_2dhisto_map.find(map2_iter->first);
    if (histoMap != m_mmevts_2dhisto_map.end() ) {
      histoMap->second.push_back(mmevt2);
    } else {
      ATH_MSG_ERROR("Can not find entry for 2d histogram"); 
      return StatusCode::FAILURE;
    }

  }
  return StatusCode::SUCCESS;
  
}

StatusCode LhoodMM_tools::addEventCustom() {

  int nlep = m_particles.size();
  if (nlep == 0) {
    ATH_MSG_WARNING("Attempt to add an event with 0 leptons.  This event will be ignored.");
    return StatusCode::SUCCESS;
  }
  std::vector<bool> isTight_vals;
  std::vector<Efficiency> realEff_vals;
  std::vector<Efficiency> fakeEff_vals;  
  std::vector<int> charges;
  std::vector<FakeBkgTools::ParticleData>::const_iterator particles_it;
  for (particles_it = m_particles.begin(); particles_it != m_particles.end(); particles_it++) { 
    const FakeBkgTools::ParticleData& p = *particles_it;
    isTight_vals.push_back(p.tight);
    realEff_vals.push_back(p.real_efficiency);
    fakeEff_vals.push_back(p.fake_efficiency);
    double r_eff = p.real_efficiency.value(this);
    double f_eff = p.fake_efficiency.value(this);

    if (particles_it == m_particles.begin() ){
    
      for(const std::pair<short unsigned int, FakeBkgTools::Uncertainty> kv : p.real_efficiency.uncertainties)
        {
	  ATH_MSG_DEBUG("real eff uncertainties for first lepton are " <<  kv.second.up << " " << kv.second.down);
        }
for(const std::pair<short unsigned int, FakeBkgTools::Uncertainty> kv : p.fake_efficiency.uncertainties)
        {
	  ATH_MSG_DEBUG("fake eff uncertainties for first lepton are " <<  kv.second.up << " " << kv.second.down);
        }
    }
    charges.push_back(p.charge);
    if ( r_eff < 0.01 && f_eff< 0.01) {
       ATH_MSG_DEBUG("Found bad efficiency values");
    }
  }
  
  return addEventCustom( isTight_vals, realEff_vals,  fakeEff_vals, charges, m_externalWeight);
}

StatusCode LhoodMM_tools::getTotalYield(float& yield, float& statErrUp, float& statErrDown) {

  if (m_progressFileName != "none"  && m_alreadyMerged == false) {
    ATH_CHECK( mergeSubJobs() );
  }
  
  //set output level according to debug flag, also check whether setPrintLevel was called directly
  msgLvl(MSG::DEBUG) ||  msgLvl(MSG::VERBOSE) || (m_printLevel > 0) ?  m_printLevel = 1 : m_printLevel = -1;

  Double_t poserr, negerr;
  
  unsigned minNlep = 1;
  unsigned maxNlep = 1;

  bool reqSameSign = false;
  bool inclusive = true;
  
  minNlep = FakeBkgTools::maxParticles();

  bool atLeastOneValid = false;
  for(unsigned n=1;n<=FakeBkgTools::maxParticles();++n)
  {
    std::string error;
    FakeBkgTools::FinalState fs(1234, n, m_selection, m_process, error);
    if(error.length()) {
      if(!atLeastOneValid && n==FakeBkgTools::maxParticles()) {
    ATH_MSG_ERROR(error); // if they all failed
    return StatusCode::FAILURE;
      }
      continue;
    }
    atLeastOneValid = true;
    for(unsigned c=0;c<(1u<<n);++c) // loop on all possible n-particles tight/loose combinations; there are 2^n of them
      {
    if(!fs.selection[c]) continue; // check if that particular combination is among the ones allowed        
    unsigned nTight = FakeBkgTools::FSBitset(c).count(); // if yes, count how many tight leptons it contains
    if(nTight < minNlep) minNlep = nTight; // minNlep set to "number of tight leptons in the combination with the least number fo tight leptons"
    if (nTight > maxNlep) maxNlep = nTight;
    inclusive = inclusive || (nTight>minNlep); // inclusive set to "whether there is at least one allowed combination with more tight leptons than the minimum"
      }
    reqSameSign = reqSameSign || fs.hasSS(); // this should in principle not be used standalone, but fs.accept() should be used instead
  }

  // now check to make sure that minNlep is consistent with the smallest
  // number of fake leptons specified with the process string
  unsigned minNlep_proc = maxNlep;
  unsigned maxNlep_proc = 0;
  for (unsigned  n=minNlep;n<=maxNlep;++n)  {
    bool success;
    auto fs = getCachedFinalState(n, m_selection, m_process, success);

    if (success) {
      for(unsigned c=0;c<(1u<<n);++c) // loop on all possible n-particles tight/loose combinations; there are 2^n of them
    {
      FakeBkgTools::FSBitset fakes = c;
      FakeBkgTools::FSBitset reals = 0;
      FakeBkgTools::FSBitset tights = 0;      
      for (unsigned ibit = 0; ibit < n; ibit++) {
        reals.set(ibit, ~fakes[ibit]);
        tights.set(ibit, 1);
      }

      if (fs.accept_process(n, reals, tights) ) {
        ATH_MSG_VERBOSE("Accepted a process for n = " << n);
        if(n < minNlep_proc) minNlep_proc = n; // minNlep set to "number of tight leptons in the combination with the least number fo tight leptons"
        if (n > maxNlep_proc) {
          maxNlep_proc = n;
        }
        ATH_MSG_VERBOSE("maxNlep_proc = " << maxNlep_proc);
        break;
      }
    }
    } else {
      ATH_MSG_ERROR("Selection and/or process strings unable to be parsed, or incompatible with each other");
      return StatusCode::FAILURE;
    }
  }
  minNlep = minNlep_proc;
  maxNlep = maxNlep_proc;


  if (reqSameSign) m_ssfrac = 0.5;

  inclusive = false;

  m_minnlep = minNlep;
  m_maxnlep = maxNlep;

  ATH_MSG_VERBOSE("m_mmevts_total.size() = " << m_mmevts_total.size());
  ATH_MSG_VERBOSE("minNlep, maxNlep = " << minNlep << " " << maxNlep);

  yield = nfakes(m_mmevts_total, &poserr,&negerr);

  statErrUp = poserr;
  statErrDown = -negerr;

  return fillHistograms();
}

StatusCode LhoodMM_tools::setup(const vector<LhoodMMEvent>& mmevts) {

  m_current_lhoodMM_tool = this;

  m_mmevts_internal.clear();
  m_mmevts_internal.resize(nLepMax+1);

  m_maxnlep_loose = 0;

  m_coeffs.clear();

  m_coeffs.resize(nLepMax+1);

  m_event_cat.clear();
  m_event_cat.resize(nLepMax+1);

  m_event_sumw2.clear();
  m_event_sumw2.resize(nLepMax+1);

  for (int ilep = 0; ilep <= nLepMax; ilep++) {
    m_event_cat[ilep].clear();
    m_event_sumw2[ilep].clear();
    m_coeffs[ilep].clear();
    m_coeffs[ilep].resize((0x1 << (ilep+1)) );
    for (int ientry = 0; ientry < (0x1 << (ilep+1)); ientry++) {
      m_event_cat.at(ilep).push_back(0.);
      m_event_sumw2.at(ilep).push_back(0.);
      m_coeffs[ilep][ientry].resize((0x1 << (ilep+1)) );
      for (int jentry = 0; jentry < (0x1 << (ilep+1)); jentry++) {
    m_coeffs[ilep][ientry][jentry] = 0.;
      }
    }
  }

  vector<LhoodMMEvent>::const_iterator mmiter = mmevts.begin();
  
  float dilep_SSfrac_num = 0, dilep_SSfrac_denom = 0;
  vector < vector<double> > OSfrac_num;
  vector < vector <int> >  OSfrac_denom;
  OSfrac_num.resize(nLepMax+1);
  OSfrac_denom.resize(nLepMax+1);
  for (int ilep = 0; ilep <= nLepMax; ilep++) {
    OSfrac_num[ilep].resize(ilep+1);
    OSfrac_denom[ilep].resize(ilep+1);
  }

  ATH_MSG_VERBOSE("Total nevents: " << mmevts.size());

  for (mmiter = mmevts.begin(); mmiter != mmevts.end(); mmiter++) {
    int nlep = (*mmiter).nlep();
    if (nlep > m_maxnlep_loose) {
      m_maxnlep_loose = nlep;
    }
    if (nlep <= nLepMax) {
      // if we have a dilepton event, look to see whether SS or OS is
      // required.  If so, and the event has the wrong charge configuration, 
      // reject it!  
      if (nlep == 2) {
    dilep_SSfrac_denom++;
    if ((*mmiter).charge(0) ==  (*mmiter).charge(1)) {
      dilep_SSfrac_num++;
    }
      }

      //check to see if there is at least one OS pair...
      for (int icomb = 0; icomb < (0x1 << nlep); icomb++) {
    int totcharge = 0;
    std::bitset<nLepMax> tights(icomb);
    int ntight = tights.count();
    ATH_MSG_VERBOSE("Setting OSfrac_denom[" << ntight << "][" << nlep);
    ATH_MSG_VERBOSE("size is " << OSfrac_denom[ntight].size());
    OSfrac_denom[nlep][ntight]++;

    for (int jlep = 0; jlep < nlep; jlep++) {
      if (tights[jlep]) {
        totcharge += (*mmiter).charge(jlep);
      }
    }
    ATH_MSG_VERBOSE("Setting OSfrac_num[" << ntight << "][" << nlep);
    if ((TMath::Abs(totcharge) < ntight) || ntight == 1) OSfrac_num[nlep][ntight]++;
      }

      ATH_MSG_VERBOSE("Adding event to m_mmevts_internal[" << nlep-1 << "]; ");
      m_mmevts_internal.at(nlep-1).push_back(*mmiter);
      ATH_MSG_VERBOSE("new size is " <<  m_mmevts_internal.at(nlep-1).size());
      unsigned int catIndex = 0;
      for (int jlep = 0; jlep < nlep; jlep++) {
    catIndex += (!(*mmiter).isTight(jlep)) << (nlep-jlep-1);
      }
      double weight = (*mmiter).weight();
      m_event_cat.at(nlep-1).at(catIndex)+=weight;
      m_event_sumw2.at(nlep-1).at(catIndex)+=weight*weight;
    }  else {
      ATH_MSG_ERROR( "Error: can only handle " << nLepMax << " leptons; you tried " << nlep);
      return StatusCode::FAILURE;
    }
  } 

  m_dilep_SSfrac = dilep_SSfrac_num/dilep_SSfrac_denom;
  for (int ilep = 1; ilep <= m_maxnlep_loose; ilep++) {
    for (int jlep = 1; jlep <= ilep; jlep++) {
      if (OSfrac_denom[ilep][jlep] > 0) {
    m_OSfrac[ilep][jlep]  = OSfrac_num[ilep][jlep]/OSfrac_denom[ilep][jlep];
      } else {
    m_OSfrac[ilep][jlep]  = 1.;
      }
    }
  }
  return StatusCode::SUCCESS;
}

  
double LhoodMM_tools::logPoisson(double obs, double pred) {
  
  double f =  -(obs*TMath::Log(pred)-pred);
  if (obs > 0) f += obs*TMath::Log(obs)-obs;
  return f;
}

#ifdef FAKEBKGTOOLS_ATLAS_ENVIRONMENT
    #define ASG_MSG_VERBOSE(x) do { if ( m_current_lhoodMM_tool->msgLvl(MSG::VERBOSE)) m_current_lhoodMM_tool->msg(MSG::VERBOSE) <<  x << endmsg; } while(0)
#else
    #define ASG_MSG_VERBOSE(x) do { if(m_current_lhoodMM_tool->msgLvl(MSG::VERBOSE)) std::cout << x << std::endl; } while (0)      
#endif

void LhoodMM_tools::fcn_minnlep_maxnlep(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{

  LhoodMM_tools* l = m_current_lhoodMM_tool;

  const bool verbose = (l->m_printLevel > 0);

  int minnlep = l->m_minnlep;
  int maxnlep_loose = l->m_maxnlep_loose;

  Double_t f_tot = 0.;
  
  Double_t rsq = par[0];

  Double_t sinterm_tot = 1.;

  int theta_nlep_index = 1 + maxnlep_loose -minnlep;
  for (int ilep = minnlep; ilep <= maxnlep_loose; ilep++) {
    theta_nlep_index += l->m_real_indices[ilep].size();
  }

  Double_t pars_thisnlep[maxRank];  // a bit of a waste of memory, but avoids compiler warnings for variable-     

  if(verbose) ASG_MSG_VERBOSE("theta_nlep_index = " << theta_nlep_index);

  int real_index = 1;
  for (int ilep = minnlep; ilep <= maxnlep_loose; ilep++) {
    l->m_curr_nlep = ilep;
    Int_t npar_thisnlep = 0x1 << ilep;

    int theta_tot_index = l->m_theta_tot_start_index+ilep-minnlep; 
    if(verbose) ASG_MSG_VERBOSE("theta_tot_index, npar = " << theta_tot_index << " " << npar);
    if(verbose) ASG_MSG_VERBOSE("sinterm_tot, par[theta_tot_index] = " << sinterm_tot << " " <<  par[theta_tot_index]); 
    if (l->m_maxnlep_loose - minnlep > 0) {
      double costerm;
      if (ilep < l->m_maxnlep_loose ) {
    costerm = TMath::Cos(par[theta_tot_index]);
      } else {
    costerm = 1.;
      }
      if(verbose) ASG_MSG_VERBOSE("r sinterm_tot costerm = " << rsq <<" " << sinterm_tot << " " << costerm);
      pars_thisnlep[0] = rsq*sinterm_tot*costerm*sinterm_tot*costerm;
      sinterm_tot *= TMath::Sin(par[theta_tot_index]);
    } else {
      pars_thisnlep[0] = rsq;
    }
    if(verbose) ASG_MSG_VERBOSE("pars_thisnlep[0] = " <<  pars_thisnlep[0]);
    int par_index = 1;
    for (unsigned ipar = 0; ipar < l->m_real_indices[ilep].size(); ipar++) {
      pars_thisnlep[par_index] = par[real_index+ipar];
      if(verbose) ASG_MSG_VERBOSE("pars_thisnlep[" << par_index << "] = " <<  pars_thisnlep[par_index]);
      par_index++;
    }

    for (unsigned ipar = 0; ipar < l->m_fake_indices[ilep].size()-1; ipar++) {
      pars_thisnlep[par_index+ipar] = par[theta_nlep_index];
      if(verbose) ASG_MSG_VERBOSE("pars_thisnlep[" << par_index+ipar <<"] = " <<  pars_thisnlep[par_index+ipar]);
      theta_nlep_index++;
    }
    if (l->m_mmevts_internal[ilep-1].size() > 0) {
      fcn_nlep(npar_thisnlep, gin, f, pars_thisnlep, iflag);
      f_tot += f;
    }
    real_index += l->m_real_indices[ilep].size();
  }

  f = f_tot;
}

void LhoodMM_tools::fcn_nlep(Int_t &npar, Double_t *, Double_t &f, Double_t *par, Int_t)
{

  LhoodMM_tools* l = m_current_lhoodMM_tool;

  const bool verbose = (l->m_printLevel > 0);

  int nlep = l->m_curr_nlep;
  int rank = 0x1 << nlep;

  if(verbose) {
    ASG_MSG_VERBOSE("npar = " << npar);
    for (int ipar = 0; ipar < npar; ipar++) {
      ASG_MSG_VERBOSE("Parameter " << ipar <<  " = " << par[ipar]);
    }
  }

  if (l->m_mmevts_internal.at(nlep-1).size() == 0) {
    f = par[0] + par[1];
    return;
  }
  
  if(verbose) {
    std::string txt = "testing variable transform: angle pars in fcn: ";
    for (int i = 0; i < npar; i++) {
    txt += std::to_string(par[i]) + " ";
    }
    ASG_MSG_VERBOSE(txt);
  }

  // remaining parameters are the angles used to divide up the fakes contribution
  
  std::shared_ptr<TMatrixT<double>> nrf, MMmatrix, ntlpred; 
  nrf = l->m_nrf_mat_vec[nlep];
  if(verbose) {
    ASG_MSG_VERBOSE("What does nrf look like?");
    nrf->Print();
  }
  MMmatrix = l->m_MMmatrix_vec[nlep];
  ntlpred = l->m_ntlpred_vec[nlep];

  (*nrf)(0,0) = par[0];
  double rsq = TMath::Abs(par[0]);
  if(verbose) ASG_MSG_VERBOSE("rsq = " << rsq);
  int rsize = l->m_real_indices[nlep].size();
  int fsize = l->m_fake_indices[nlep].size();

  for (int ipar = 0; ipar < rsize; ipar++) {
    ASG_MSG_VERBOSE("In fcn, setting real par " << l->m_real_indices[nlep][ipar] << " to " <<  par[ipar+1]); 
    (*nrf)(l->m_real_indices[nlep][ipar], 0) = par[ipar+1];
  }
  double sinterm = 1.;

  if(verbose) ASG_MSG_VERBOSE("nrf[0] = " << (*nrf)(0,0));

  for (int ipar = 0; ipar < fsize; ipar++) {
    if(verbose) ASG_MSG_VERBOSE("r, sinterm, par[ipar+1] " << rsq << " " << sinterm << " " << par[ipar+1]);
    if (ipar <  fsize-1 ) {
      double costerm = TMath::Cos(par[rsize+ipar+1]);
      if(verbose) ASG_MSG_VERBOSE("for setting fake parameter, sinterm = " << sinterm << " par index = " << l->m_real_indices[nlep].size()+ipar+1);
      (*nrf)(l->m_fake_indices[nlep][ipar],0) = rsq*sinterm*costerm*sinterm*costerm;
    } else {
      (*nrf)(l->m_fake_indices[nlep][ipar],0) = rsq*sinterm*sinterm;
    }
    
    ASG_MSG_VERBOSE("nrf[" << ipar << "] = " << (*nrf)(ipar,0));
    ASG_MSG_VERBOSE("In fcn, setting fake par " << l->m_fake_indices[nlep][ipar] << " to " <<  (*nrf)(l->m_fake_indices[nlep][ipar],0)); 
   
    sinterm *= TMath::Sin(par[rsize+ipar+1]);
  }
  
  ASG_MSG_VERBOSE("Testing variable transform: nrf in fcn: ");
  if(verbose) {
    nrf->Print();
  }

  *ntlpred = (*MMmatrix)*(*nrf);
 
  ASG_MSG_VERBOSE("Printing the matrix in fcn");
  if(verbose) {
    MMmatrix->Print();
    nrf->Print();
  }
  f = 0;
  int ipar_start;
  if (l->m_doFakeFactor) {
    ipar_start = 1;
  } else {
    ipar_start = 0;
  }

  for (int ipar = ipar_start; ipar < rank; ipar++) {
    if(verbose) ASG_MSG_VERBOSE("Comparing parameter " << ipar << ": " << l->m_event_cat.at(nlep-1).at(ipar) << " to " << (*ntlpred)(ipar,0));
    // following scaling for weighted events motivated by arXiv:1309.1287 
    int nobs = l->m_event_cat.at(nlep-1).at(ipar);
    double s = 1.;
    if (nobs > 0) {
      s= l->m_event_sumw2.at(nlep-1).at(ipar)/nobs;
    }
    if (verbose) ASG_MSG_VERBOSE("s = " << s); 
    f += logPoisson(1.*nobs/s, (*ntlpred)(ipar,0)/s);
    if (verbose)  ASG_MSG_VERBOSE("f = " << f);
  }

}


double LhoodMM_tools::nfakes( const vector<LhoodMMEvent>& mmevts, Double_t *poserr, Double_t *negerr) {
  
  if (mmevts.size() == 0) {
    *poserr = 0.;
    *negerr = 0.;
    return 0.;
  }

  m_fitStatus = 0;

  m_minnlep = nLepMax;
  m_maxnlep = 0;

  m_requireSS = false;
  m_requireOS = false;

  for (int ilep = 1; ilep <= nLepMax; ilep++) {
    string error;
    // possible issue here -- reassigning vector elements?
    m_fsvec[ilep].reset(new FakeBkgTools::FinalState(0, ilep, m_selection, m_process, error));
    if (error.size() > 0) continue; // unable to parse selection
    if (m_fsvec[ilep]->hasSS() ) {
      m_requireSS = true;
    }
    if (m_fsvec[ilep]->hasOS() ) {
      m_requireOS = true;
    } 
    m_fsvec[ilep]->setSS(false);
    m_fsvec[ilep]->setOS(false);

    // charges will only give the charges of the leptons in the most recently
    // added event.  That's why the tricks with the hasSS, hasOS, setSS, and
    // setOS functions are played above (so we don't actually have to care 
    // about the values here 
    auto charges = m_fsvec[ilep]->retrieveCharges(m_particles);  
  
    for (int icomb = 0; icomb < (0x1 << ilep); icomb++) {
      FakeBkgTools::FSBitset tights(icomb);
        ATH_MSG_VERBOSE("ilep " << ilep << " (0x1 << ilep) " << std::hex << (0x1 << ilep) << " icomb " << std::hex << icomb << std::dec);
        ATH_MSG_VERBOSE("tights = " << std::hex << tights << std::dec);
        ATH_MSG_VERBOSE("charges = " << std::hex << charges << std::dec);

      if (m_fsvec[ilep]->accept_selection(tights, charges)) {
    int nlep = tights.count();
    ATH_MSG_VERBOSE("tights = " << std::hex << tights << std::dec << " nlep = " << nlep);
    if (nlep > m_maxnlep) m_maxnlep = nlep;
    if (nlep < m_minnlep) m_minnlep = nlep;
      }
    }
     
  }

  ATH_MSG_VERBOSE("m_minnlep, m_maxnlep = " << m_minnlep << " " << m_maxnlep);
  int minNlep_proc = m_maxnlep;
  int maxNlep_proc = 0;
  for (int  n=m_minnlep;n<=m_maxnlep;++n)  {
    for(unsigned c=0;c<(1u<<n);++c) // loop on all possible n-particles tight/loose combinations; there are 2^n of them
      {
    FakeBkgTools::FSBitset fakes = c;
    FakeBkgTools::FSBitset reals = 0;
    FakeBkgTools::FSBitset tights = 0;      
    for (int ibit = 0; ibit < n; ibit++) {
      reals.set(ibit, ~fakes[ibit]);
      tights.set(ibit, 1);
    }
    if (m_fsvec[n]->accept_process(n, reals, tights) ) {
      if(n < minNlep_proc)  minNlep_proc = n; // minNlep set to "number of tight leptons in the combination with the least number fo tight leptons"
      if (n > maxNlep_proc) {
        maxNlep_proc = n;
      }
      ATH_MSG_VERBOSE("maxNlep_proc = "<< maxNlep_proc);
      break;
    }
      }
  }

  m_minnlep = minNlep_proc;
  m_maxnlep = maxNlep_proc;

  if(setup(mmevts) != StatusCode::SUCCESS) return 0.;

  if (m_maxnlep > m_maxnlep_loose) {
    m_maxnlep = m_maxnlep_loose;
  }

  if (m_maxnlep_loose < m_minnlep) {
    ATH_MSG_WARNING("Selection requires more leptons than are in any event in the sample; returning nfakes = 0 +/- 0");
    *poserr = 0.;
    *negerr = 0.;
    return 0.;
  }

  ATH_MSG_VERBOSE("m_minnlep, m_maxnlep = " << m_minnlep << " " << m_maxnlep);

  double nfake_fit, nfake_fitErr;
  
  int npar = 0;
  for (int ilep = m_minnlep; ilep <= m_maxnlep_loose; ilep++) {
    npar += 0x1 << ilep;
  }

  TMinuit_LHMM* lhoodFit = new TMinuit_LHMM(npar);
  lhoodFit->SetPrintLevel(m_printLevel);
  lhoodFit->SetFCN(fcn_minnlep_maxnlep);


  Double_t arglist[10];
  Int_t ierflg = 0;

  arglist[0] = 0.5;
  lhoodFit->mnexcm("SET ERR", arglist ,1,ierflg);;

  m_nfakes_std = 0;
  m_nfakes_std_err = 0;

  ATH_MSG_VERBOSE("About to initialize parameters");

  vector<double> init_pars;
  init_pars.resize(npar);
  int index = 0;
  vector< vector <double> > loc_init_pars;
  loc_init_pars.resize(m_maxnlep_loose +1 );
 
  for (int ilep = m_minnlep; ilep <=m_maxnlep_loose; ilep++) {
     loc_init_pars[ilep].resize(0x1 << ilep);
    get_init_pars(loc_init_pars[ilep], ilep);
    for (int ipar = 0; ipar < (0x1 << ilep); ipar++) {
      init_pars[index+ipar] = loc_init_pars[ilep][ipar];
    }
    index+= 0x1 << ilep;
  }

  Double_t step = TMath::Max(loc_init_pars[m_minnlep][0]/1000,0.001);
  ATH_MSG_VERBOSE("parameters initialized OK");
  ATH_MSG_VERBOSE("m_nfakes_std_err = " << m_nfakes_std_err);

  m_nfakes_std_err = TMath::Sqrt(m_nfakes_std_err);

  vector<TString> parameterName;
  vector<int> nreal_start_indices;
  nreal_start_indices.resize(m_maxnlep_loose+1);
  parameterName.push_back("nfake_tot");
  TString sreal = "nreal";
  for (int ilep = m_minnlep; ilep<= m_maxnlep_loose; ilep++) {
    char tmpchar[20];
    sprintf(tmpchar, "_%i", ilep);
    TString tmpstr = sreal;
    tmpstr.Append(tmpchar);
    for (unsigned isublep = 0; isublep < m_real_indices[ilep].size(); isublep++) {
      TString locsreal = tmpstr;
      char tmpchar2[20];
      sprintf(tmpchar2, "_%i", isublep);
      locsreal.Append(tmpchar2);
      parameterName.push_back(locsreal);
      if (isublep == 0) {
        nreal_start_indices[ilep] = (parameterName.size());
        ATH_MSG_VERBOSE("nreal_start_indices[" << ilep << "] = " << nreal_start_indices[ilep]);
      }
    }
  }
  
  TString stheta_tot = "theta_tot";
  for (int ilep = m_minnlep; ilep<m_maxnlep_loose; ilep++) {
    char tmpchar[20];
    sprintf(tmpchar, "_%i", ilep-m_minnlep);
    TString tmpstr = stheta_tot;
    tmpstr.Append(tmpchar);
    parameterName.push_back(tmpstr);
  }

  for (int ilep = m_minnlep; ilep <= m_maxnlep_loose; ilep++) {
    TString stheta = "theta_";
    char tmpchar[20];
    sprintf(tmpchar, "%i", ilep);
    TString tmpstr = stheta;
    tmpstr.Append(tmpchar);
    tmpstr.Append("_");
    ATH_MSG_VERBOSE("How many fake indices?" << m_fake_indices[ilep].size());
    for(int jlep = 0; jlep < (int)m_fake_indices[ilep].size() - 1; jlep++) {
      TString locstheta = tmpstr;
      char tmpchar2[20];
      sprintf(tmpchar2, "%i", jlep);
      locstheta.Append(tmpchar2);
      parameterName.push_back(locstheta);
    }
    
  }

  vector<double> theta_tot;
  ATH_MSG_VERBOSE("m_maxnlep_loose = " << m_maxnlep_loose << " m_minnlep = " << m_minnlep);
  theta_tot.resize(m_maxnlep_loose-m_minnlep);
  ATH_MSG_VERBOSE("theta_tot.size() = " << theta_tot.size());
  ATH_MSG_VERBOSE("m_nfakes_std = " << m_nfakes_std);
  if (m_nfakes_std > 0.) {
    double sinterm = 1.;
    int theta_index = 0;
    for (int ilep = m_minnlep; ilep < m_maxnlep_loose; ilep++ ){ 
      ATH_MSG_VERBOSE("nfakes for nlep = " << ilep << " used to find theta_tot = " << loc_init_pars[ilep][0]);
      theta_tot[theta_index] = TMath::ACos(TMath::Sqrt(TMath::Max(loc_init_pars[ilep][0],0.)/(m_nfakes_std))/sinterm);
      if (TMath::IsNaN( theta_tot[theta_index] ) ) {
    theta_tot[theta_index] = _piover4; 
      }
      sinterm *= TMath::Sin(theta_tot[theta_index]);
      theta_index++;
    }
  } else {
    int theta_index = 0;
    for (int ilep = m_minnlep; ilep < m_maxnlep_loose; ilep++ ){
      theta_tot[theta_index] = _piover4; 
      theta_index++;
    }
  }

  ATH_MSG_VERBOSE("About to set upper limits");
  vector<double> upper_limits;
  upper_limits.resize(npar);

  upper_limits[0] =   TMath::Max(5.*m_maxWeight*mmevts.size(), 0.01);

  int real_index = 1;
  for (int ilep = m_minnlep; ilep <= m_maxnlep_loose; ilep++) {
    for (unsigned isublep = 0; isublep < m_real_indices[ilep].size(); isublep++) {
      upper_limits[real_index] = 5*m_maxWeight*m_mmevts_internal.at(ilep-1).size(); 
      real_index++;
    }
  }
  
  for (int ipar = real_index; ipar < npar; ipar++) {
    upper_limits[ipar] = _piover2; 
  }
  
  //re-organize from "per-lepton" parameters to global parameters
  vector<double> init_par_values;
  init_par_values.resize(npar);
  ATH_MSG_VERBOSE("Setting parameter 0 to " << TMath::Max(m_nfakes_std,0.5));
  init_par_values[0] =  TMath::Max(m_nfakes_std,0.);
  int init_index = 1;
  int glob_index = 1;
  for (int ilep= m_minnlep; ilep <= m_maxnlep_loose; ilep++) {
    for (unsigned isublep = 0; isublep < m_real_indices[ilep].size(); isublep++) {
      ATH_MSG_VERBOSE("Setting parameter " << glob_index << " to " <<  init_pars[init_index+isublep]);
      init_par_values[glob_index] = init_pars[init_index+isublep];
      glob_index++;
    }
    init_index+= pow(2,ilep);
  }

  m_theta_tot_start_index = 1;
  for (int ilep = m_minnlep; ilep <= m_maxnlep_loose; ilep++) {
    m_theta_tot_start_index += m_real_indices[ilep].size();
  }
  vector<int> theta_start_indices;
  theta_start_indices.resize(m_maxnlep_loose+1);
  theta_start_indices[m_minnlep] =  m_theta_tot_start_index+m_maxnlep_loose - m_minnlep;
  for (int i = m_minnlep+1; i <= m_maxnlep_loose; i++) {
    theta_start_indices[i] = theta_start_indices[i-1] +  m_fake_indices[i-1].size() - 1;
  }

  for (int ilep= m_minnlep; ilep < m_maxnlep_loose; ilep++) {
    ATH_MSG_VERBOSE("Setting parameter " << m_theta_tot_start_index+ilep-m_minnlep  << " to " << theta_tot[ilep-m_minnlep]);
    init_par_values[m_theta_tot_start_index+ilep-m_minnlep] = theta_tot[ilep-m_minnlep];
  }

  ATH_MSG_VERBOSE( "all done!");

  int currpar = m_theta_tot_start_index+m_maxnlep_loose-m_minnlep;
  index = 1;
  for (int ilep= m_minnlep; ilep <= m_maxnlep_loose; ilep++) {
    for (unsigned ipar = 0; ipar < m_fake_indices[ilep].size() - 1; ipar++) {
      ATH_MSG_VERBOSE("Setting parameter " << currpar  << " to " <<  init_pars[index+m_real_indices[ilep].size()+ipar]);
      init_par_values[currpar] = init_pars[index+m_real_indices[ilep].size()+ipar];
      currpar++;
    }
    index+= 0x1 << ilep;
  }
  int ipar;
  for (ipar = 0; ipar < npar; ipar++) {
    lhoodFit->mnparm(ipar, parameterName[ipar], init_par_values[ipar], step, 0., upper_limits[ipar], ierflg);
  }

  ATH_MSG_VERBOSE("About to fix some parameters");
  ATH_MSG_VERBOSE("m_minnlep = " << m_minnlep);
  // account for case where there may be no leptons of a given multiplicity by
  // fixing the parameters that are relevant to that multiplicity
  for (int ilep = m_minnlep; ilep <= m_maxnlep_loose; ilep++) {
    if (m_mmevts_internal[ilep-1].size() ==  0) {
      // start with the nreal parameters
      for (unsigned ipar = nreal_start_indices[ilep]; ipar < nreal_start_indices[ilep] + m_real_indices[ilep].size(); ipar++) {
    arglist[0] = ipar;
    arglist[1] = 0.;
    lhoodFit->mnexcm("SET PAR", arglist, 2, ierflg);
    lhoodFit->mnexcm("FIX PAR", arglist, 1, ierflg);
      }
      //now the theta_tot angle associated with this multiplicity
      arglist[0] = m_theta_tot_start_index + ilep - m_minnlep +1;
      arglist[1] = _piover2; 
      lhoodFit->mnexcm("SET PAR", arglist, 2, ierflg);
      lhoodFit->mnexcm("FIX PAR", arglist, 1, ierflg);

      //now all the angle parameters for this lepton multiplicity
      for (unsigned ipar = theta_start_indices[ilep]+1; ipar <  theta_start_indices[ilep] + m_fake_indices[ilep].size() ; ipar++) {
    arglist[0] = ipar;
    arglist[1] = 0.;
    lhoodFit->mnexcm("SET PAR", arglist, 2, ierflg);
    lhoodFit->mnexcm("FIX PAR", arglist, 1, ierflg);
      }
    }
    index += (0x1 << ilep) - 2;
  }

  arglist[0] = 5000;
  arglist[1] = 1.0;
  lhoodFit->mnexcm("MIGRAD", arglist ,2,ierflg);
  ATH_MSG_VERBOSE("ierflg = " << ierflg);
  lhoodFit->mnimpr();

  Double_t amin, edm, errdef;
  Int_t nvpar, nparx, icstat;
  lhoodFit->mnstat(amin,edm,errdef,nvpar,nparx,icstat);
  
  lhoodFit->GetParameter(0, nfake_fit, nfake_fitErr);
  
  Double_t gcc;
  
  if (poserr && negerr) { 
    arglist[0] = 500;
    arglist[1] = 1.0;
    lhoodFit->mnexcm("MINOS", arglist ,2,ierflg);
    lhoodFit->mnerrs(0, *poserr, *negerr, nfake_fitErr, gcc);

    // re-retrieve fit values, since MINOS might have found a new minimum
    lhoodFit->mnstat(amin,edm,errdef,nvpar,nparx,icstat);    
    lhoodFit->GetParameter(0, nfake_fit, nfake_fitErr);

    // check that MINOS succeeded. If not, fix it...

     if ( *poserr < 1.e-5) {
      *poserr = fixPosErr(nfake_fit, lhoodFit);
    } 
    if (*negerr > -1.e-5 ) {
      *negerr = fixNegErr(nfake_fit, lhoodFit);
    }
    //resort to parabolic errors if all else fails
    if (*negerr > -1.e-5) {
      *negerr = -nfake_fitErr;
      // prevent negative error from extending below 0
      if (nfake_fit + *negerr < 0.) {
    *negerr = -nfake_fit;
      }
    }

    if (*poserr < 1.e-5) {
      *poserr = nfake_fitErr;
    }
    
  }
  
  delete lhoodFit;

  return nfake_fit; 

}

StatusCode LhoodMM_tools::fillHistograms() {

  StatusCode status = StatusCode::SUCCESS;
  std::map<TH1*, std::vector<LhoodMMEvent>>::iterator map1_iter;
  for (map1_iter=m_mmevts_1dhisto_map.begin(); map1_iter != m_mmevts_1dhisto_map.end(); map1_iter++) {
    std::vector<LhoodMMEvent>& currVec = map1_iter->second;
    TH1* histogram = map1_iter->first;
    status = fillHisto_internal(currVec, histogram);
  }

  std::map<TH2*, std::vector<LhoodMMEvent>>::iterator map2_iter;
  for (map2_iter=m_mmevts_2dhisto_map.begin(); map2_iter != m_mmevts_2dhisto_map.end(); map2_iter++) {
    std::vector<LhoodMMEvent>& currVec = map2_iter->second;
    TH2* histogram = (TH2*)map2_iter->first;
    status = fillHisto_internal(currVec, histogram);
    ATH_MSG_VERBOSE("Inside fill code, mean = " << histogram->ProjectionX()->GetMean());
  }
  return status;
}

StatusCode LhoodMM_tools::fillHisto_internal(const vector<LhoodMMEvent>& mmevts, TH1* h) {

  // If fixNormalization is set to true, the fake yield for the entire sample 
  // will be calculated first, and then the histogram will be scaled at the end 
  // so that its normalization matches this yield.  Otherwise, the sum of the 
  // bin entries is not guaranteed to match yield calculated on the sample
  // as a whole.

  // As a general rule of thumb, if the histogram being created is the key one
  // for the analysis (e.g. used to set limits) then it's best to keep 
  // fixNormalization false.  On the other hand, if the analysis is based on
  // total yield, and the histogram is a control/validation plot, it's best
  // to set fixNormalization to true.

  bool is2d = false;
  if (h->GetNbinsY() > 1) is2d = true;
    
  vector<vector<LhoodMMEvent> > mmevts_vec;
  int nbins = h->GetNcells();

  mmevts_vec.resize(nbins);

  vector<LhoodMMEvent>::const_iterator mmevt_iter;

  for (mmevt_iter = mmevts.begin(); mmevt_iter != mmevts.end(); mmevt_iter++) {
    int bin;
    if (is2d) {
      bin = h->FindBin((*mmevt_iter).aux(), (*mmevt_iter).aux2());
    } else {
      bin = h->FindBin((*mmevt_iter).aux());
    }
    mmevts_vec[bin].push_back(*mmevt_iter);
  }

  ATH_MSG_VERBOSE("nbins = " << nbins);
  Double_t nf, poserr, negerr, shift = 0.;
  for (int ibin = 0; ibin < nbins; ibin++) {
  ATH_MSG_VERBOSE("Filling bin " << ibin << " with " << mmevts_vec[ibin].size() << " events");
    if (mmevts_vec[ibin].size() > 0) {
      ATH_MSG_VERBOSE("first event has " << mmevts_vec[ibin][0].nlep() << " leptons ");

      nf = nfakes(mmevts_vec[ibin], &poserr, &negerr);
      h->SetBinContent(ibin, nf+shift);
      if (TMath::IsNaN(h->GetBinContent(ibin))) {
    h->SetBinContent(ibin,0.);
    h->SetBinError(ibin, 0.);
      } else {
    h->SetBinError(ibin,TMath::Max(poserr,-negerr));
      }
    } else {
      h->SetBinContent(ibin,0.);
      h->SetBinError(ibin, 0.);
    }
    ATH_MSG_VERBOSE("Result is " << h->GetBinContent(ibin) << " +/- " << h->GetBinError(ibin));
  }

  if (m_fixNormalization) {
    double poserr, negerr;
    double totFakes;
    
    totFakes = nfakes(mmevts, &poserr, &negerr);

    // find total content of histogram, including under- and overflows
    double totHistContent  = 0.;
    for (int ibin = 0; ibin < nbins; ibin++) {
      totHistContent += h->GetBinContent(ibin);
    }
    
    ATH_MSG_VERBOSE("totHistContent = " << totHistContent);
    ATH_MSG_VERBOSE("sum of weights = " << h->GetSumOfWeights());

    double scaleFactor;
    if (totHistContent > 0.) {
      scaleFactor = totFakes/totHistContent;
    } else {
      scaleFactor = 1.;
    }
    for (int ibin = 1; ibin <= nbins; ibin++) {
      h->SetBinContent(ibin, scaleFactor*h->GetBinContent(ibin));
      h->SetBinError(ibin, scaleFactor*h->GetBinError(ibin));
    }   
  }
  return StatusCode::SUCCESS;
}


double LhoodMM_tools::nfakes_std(double *error) {

  *error = m_nfakes_std_err;
  return m_nfakes_std;
}

double LhoodMM_tools::nfakes_std_perEventWeight(double *error) {

  *error = m_nfakes_std_err_perEventWeight;
  return m_nfakes_std_perEventWeight;
}

void LhoodMM_tools::get_init_pars(vector<double> &init_pars, int nlep) {

  ATH_MSG_VERBOSE("Setting initial parameters for nlep = " << nlep);

  vector<double> nrf;
  nrf.resize(0x1 <<nlep);
  get_analytic(nrf, nlep);
  m_real_indices[nlep].clear();
  m_fake_indices[nlep].clear();
  
  std::string txt = "Testing variable transform: Initial nrf: ";
  for (auto i = nrf.begin(); i != nrf.end(); ++i)
    txt += std::to_string(*i) + ' ';
  ATH_MSG_VERBOSE(txt);

  vector<double> init_angles;

  double nfakes_std_thisnlep = 0;
  FakeBkgTools::FSBitset tights = 0;      
  for (int ibit = 0; ibit < nlep; ibit++) {
    tights.set(ibit, 1);
  }
  
  for (int ipar = 0; ipar < 0x1 <<nlep; ipar++) {
    FakeBkgTools::FSBitset fakes = ipar;
    FakeBkgTools::FSBitset reals;
    for (int ibit = 0; ibit < nlep; ibit++) {
      reals.set(ibit, ~fakes[ibit]);
    }
    if (m_fsvec[nlep]->accept_process(nlep, reals, tights) ) {
      ATH_MSG_VERBOSE("accepted " << ipar);
      nfakes_std_thisnlep += nrf[ipar];
      m_fake_indices[nlep].push_back(ipar);
    } else {
      m_real_indices[nlep].push_back(ipar);
    }
  }

  init_pars[0] = nfakes_std_thisnlep;
  for (unsigned ipar = 1; ipar <= m_real_indices[nlep].size(); ipar++) {
    init_pars[ipar] = nrf[m_real_indices[nlep][ipar-1]];
  }

  if (nfakes_std_thisnlep > 0.5) {
    double sinterm = 1.;
    int ifake = 0;
    for (int ipar = m_real_indices[nlep].size()+1; ipar < (0x1 << nlep); ipar++) {
      init_pars[ipar] = TMath::ACos(TMath::Sqrt(TMath::Max(nrf[m_fake_indices[nlep][ifake] ], 0.)/(nfakes_std_thisnlep))/sinterm);
    sinterm *= TMath::Sin(init_pars[ipar]);
    ifake++;
    }
  } else {
    for (int ipar = m_real_indices[nlep].size()+1; ipar < (0x1 << nlep); ipar++) {
      init_pars[ipar] =  _piover4;  
    }
  }

  txt = "testing variable transform: Initial pars: ";
  for (int i = 0; i < (0x1 << nlep); i++) {
    txt += std::to_string(init_pars[i]) + " ";
  }
  ATH_MSG_VERBOSE(txt);

  // fix any nan's...
  for (int ipar = 2; ipar < (0x1 << nlep); ipar++) {
    if (TMath::IsNaN(init_pars[ipar])) {
      init_pars[ipar] = 0.;
    }
    ATH_MSG_VERBOSE("Setting angle parameter " << ipar << " to " <<  init_pars[ipar]);
  }
}


void LhoodMM_tools::get_analytic(vector<double>& nrf, const int nlep) {

  ATH_MSG_VERBOSE("just getting started with nlep = "  << nlep);

  ATH_MSG_VERBOSE("m_minnlepreg = " << m_minnlep << " m_maxnlep = " << m_maxnlep);
  int nevents = m_mmevts_internal.at(nlep-1).size();
  ATH_MSG_VERBOSE("nevents = " << nevents);

  if (nevents == 0) return;

  const int rank = 0x1 << nlep;
  const int nnormterms = pow(4, nlep);

  
  std::vector<std::vector<double>> coeff(rank, vector<double> (rank));
  std::vector<std::vector<double>> coeff_num(rank, vector<double> (rank));
  std::vector<double> coeff_denom(rank);

  ATH_MSG_VERBOSE("rank is " << rank);

  for (int irf = 0; irf < rank; irf++) {
    coeff_denom[irf] = 0.;
    for (int itl = 0; itl < rank; itl++) {
      coeff_num[irf][itl] = 0.;
    }
  }

  ATH_MSG_VERBOSE("Initialized coeff numerator and denominator vectors");

  std::vector<double> normterms(nnormterms);
  for (int inorm = 0; inorm < nnormterms; inorm++) {
    normterms[inorm] = 0.;
  }

  ATH_MSG_VERBOSE("initialized normterm vector");


  for (vector<LhoodMMEvent>::iterator mmevt_iter = m_mmevts_internal.at(nlep-1).begin(); mmevt_iter != m_mmevts_internal.at(nlep-1).end(); mmevt_iter++) {

    
    std::vector<std::vector<double>> vals(2, std::vector<double>(nlep));
    for (int ilep = 0; ilep < nlep; ilep++) {
      // invert lepton pt ordering here since my notation assigns the LSB to
      // the lowest-pt lepton
      if (m_doFakeFactor) {
    vals[0][nlep-ilep-1] = 1.;
      } else {
	vals[0][nlep-ilep-1] =  (*mmevt_iter).realEff(ilep, this);
      }
      vals[1][nlep-ilep-1] = (*mmevt_iter).fakeEff(ilep, this);
      ATH_MSG_VERBOSE("Real and fake efficiencies for lepton " << ilep << ": " << vals[0][nlep-ilep-1] << " " << vals[1][nlep-ilep-1]);
    }

    double curr_coeff_num;
    for (int irf = 0; irf < rank; irf++) {
      for (int itl = 0; itl < rank; itl++) {
    curr_coeff_num = 1.;
       for (int ilep = 0; ilep < nlep; ilep++) {
      if (itl & (0x1 << ilep) ) {
        if (irf & (0x1 << ilep)) {
          curr_coeff_num*=(1.-vals[1][ilep]);
        } else {
          curr_coeff_num*=(1.-vals[0][ilep]);
        }
      } else {
        if (irf & (0x1 << ilep) ) {
          curr_coeff_num*=vals[1][ilep];
        } else {
          curr_coeff_num*=vals[0][ilep];
        }
      }
    }
    ATH_MSG_VERBOSE("setting normterm " << (itl<<nlep) + irf);
    normterms[(itl<<nlep) + irf] += curr_coeff_num;
    coeff_num[itl][irf] += curr_coeff_num;
      }
    }
  }

  ATH_MSG_VERBOSE("All set! ");
  
  FakeBkgTools::FSBitset charges =  m_fsvec[nlep]->retrieveCharges(m_particles); 
  for (int irf = 0; irf < rank; irf++) {
    // add up all the relevant terms in the denominator to translate the 
    // loose sample counts to tight sample counts within the required range of
    // tight lepton multiplicity
    coeff_denom[irf] = 0.;
    coeff_denom[0] = coeff_num[0][0];  // don't care about this one, but it can't be 0
    float chargefactor ;
    FakeBkgTools::FSBitset fakes = irf;
    FakeBkgTools::FSBitset reals;
    for (int ibit = 0; ibit < nlep; ibit++) {
      reals.set(ibit, ~fakes[ibit]);
    }
    for (int itl = 0; itl < rank; itl++) {
      chargefactor = 1.0;
      FakeBkgTools::FSBitset antitights = itl;
      FakeBkgTools::FSBitset tights = 0;      
      for (int ibit = 0; ibit < nlep; ibit++) {
    tights.set(ibit, ~antitights[ibit]);
      }
      ATH_MSG_VERBOSE("tights " << tights);
      ATH_MSG_VERBOSE("reals "  << reals);
      ATH_MSG_VERBOSE("charges " << charges);
      if (m_fsvec[nlep]->accept_selection(tights, charges)
      &&  m_fsvec[nlep]->accept_process(nlep, reals, tights) ) {
    ATH_MSG_VERBOSE("Accepted in LhoodMM_tools " << irf);
    ATH_MSG_VERBOSE("index is " << (itl<<nlep) + irf);
    ATH_MSG_VERBOSE("Adding " << normterms[(itl<<nlep) + irf] << " to " << irf);
    // assume that dilepton fakes are evenly split between OS and SS
    if (nlep > 2 && tights.count() == 2) {
      if (m_requireOS || m_requireSS) {
        chargefactor = 0.5;
        ATH_MSG_VERBOSE("Setting SSfactor ");
      }
    }
    if (nlep == 2 && tights.count() == 2) {
      if (m_requireOS || m_requireSS) {
        chargefactor = m_dilep_SSfrac;
      }
    }
    if (m_requireOS) {
      chargefactor = m_OSfrac[nlep][tights.count()];
    }
    ATH_MSG_VERBOSE("chargefactor = " << chargefactor << " for nlep = " << nlep);
    ATH_MSG_VERBOSE("normterm = " << normterms[(itl<<nlep) + irf]);
    coeff_denom[irf] += chargefactor*normterms[(itl<<nlep) + irf];
    ATH_MSG_VERBOSE("coeff_denom[" << irf << "] = " << coeff_denom[irf]);
      }
    }
    
    // The following are "don't care" terms, but need to have non-zero
    // denominators 
    if (coeff_denom[irf] == 0.) {
      coeff_denom[irf] =  coeff_num[0][irf];
    }

    for (int itl = 0; itl < rank; itl++) {
      ATH_MSG_VERBOSE("coeff_denom[" << irf << "] = " << coeff_denom[irf]);
      m_coeffs[nlep][itl][irf] = coeff_num[itl][irf]/coeff_denom[irf]; 
    }
  }

    ATH_MSG_VERBOSE("About to do matrix stuff");
    std::shared_ptr<TMatrixT<double>> MMmatrix;
    MMmatrix = m_MMmatrix_vec[nlep];
 
    for (int i = 0; i < rank; i++) {
      for (int j = 0; j < rank; j++) {
    ATH_MSG_VERBOSE(i << " " << j << " " << coeff_num[i][j] << " " << coeff_denom[j] << " " <<  m_coeffs[nlep][i][j]);
    (*MMmatrix)(i,j) =  m_coeffs[nlep][i][j];
      }
    }

    const bool verbose = this->msgLvl(MSG::VERBOSE);
    ATH_MSG_VERBOSE("Printing the matrix in get_analytic ");
    if(verbose) MMmatrix->Print();

    TMatrixT<double>  MMmatrix_inv(rank,rank);
    MMmatrix_inv = *MMmatrix;
    MMmatrix_inv.Invert();
    
    TMatrixT<double> MMmatrix_sqr = MMmatrix_inv;
    for (int i = 0; i < rank; i++) {
      for (int j = 0; j < rank; j++) {
    MMmatrix_sqr(i,j) *= MMmatrix_sqr[i][j];
      }
    }

    TMatrixT<double> nevents_mat(rank,1), nfake_mat(rank,1), nfake_err_mat(rank,1);
    for (int i = 0; i < rank; i++) {
      nevents_mat(i,0) = m_event_cat.at(nlep-1).at(i);
    }
    
    ATH_MSG_VERBOSE("Printing nevents matrix");
    if(verbose) nevents_mat.Print();

    nfake_mat = MMmatrix_inv*nevents_mat;

    ATH_MSG_VERBOSE("MMmatrix:");
    if(verbose) MMmatrix->Print();
    ATH_MSG_VERBOSE("nevents_lll_mat:");
    if(verbose) nevents_mat.Print();
    ATH_MSG_VERBOSE("nfake_mat:");
    if(verbose) nfake_mat.Print();


    nfake_err_mat =  MMmatrix_sqr*nevents_mat;
    
    int n_proc_acc = 0;
    for (int ipar = 0;  ipar < (0x1 <<nlep) ; ipar++) {
      nrf[ipar] = nfake_mat(ipar, 0);
      ATH_MSG_VERBOSE("nrf[" << ipar << "] = " << nrf[ipar]);
      FakeBkgTools::FSBitset fakes = ipar;
      FakeBkgTools::FSBitset reals;
      FakeBkgTools::FSBitset tights = 0;      
      for (int ibit = 0; ibit < nlep; ibit++) {
    tights.set(ibit, 1);
    reals.set(ibit, ~fakes[ibit]);
      }
      if (m_fsvec[nlep]->accept_process(nlep, reals, tights) ) {
    ATH_MSG_VERBOSE("Adding " <<  nfake_mat(ipar,0) << " to m_nfakes_std");
    n_proc_acc++;
    m_nfakes_std += nfake_mat(ipar,0);
    m_nfakes_std_err += nfake_err_mat(ipar, 0);
      }
    }

    ATH_MSG_VERBOSE("Accepted " << n_proc_acc << " processes for nlep = " << nlep);
    ATH_MSG_VERBOSE("m_nfakes_std = " << m_nfakes_std);
    
}

double LhoodMM_tools::fixPosErr(double n_fake_fit, TMinuit_LHMM* lhoodFit) {

  ATH_MSG_VERBOSE("Trying to fix bad positive error");

  // get current value of -lnL
  Double_t f_from_fit, junk;
  Int_t ijunk;

    // do a binary search to find real value of positive error
  double n_fake_guess_hi = TMath::Max(n_fake_fit*5,1.);
  double n_fake_guess_lo = n_fake_fit;
  double n_fake_guess = n_fake_guess_hi;
  double f_with_guess;

  bool errFound = 0;
  bool stopSearch = 0;

  double convergeCriteria = 0.01;

  int nfake_tot_index = 1;

  Double_t arglist[10];

  int ierflg;

  arglist[0] = nfake_tot_index;
  arglist[1] = n_fake_fit;
  lhoodFit->mnexcm("SET PAR", arglist, 2, ierflg);
  lhoodFit->mnexcm("FIX PAR", arglist, 1, ierflg);
  
  arglist[0] = 10000;
  arglist[1] = 1.0;
  lhoodFit->mnexcm("MIGRAD", arglist ,2,ierflg);
  lhoodFit->mnimpr();

  lhoodFit->mnstat(f_from_fit, junk, junk, ijunk, ijunk, ijunk);
  ATH_MSG_VERBOSE("f_from_fit = " << f_from_fit);

  while (!stopSearch) {
    // fit with n_fake_tot fixed to guess value
    arglist[0] = nfake_tot_index;
    arglist[1] = n_fake_guess;
     lhoodFit->mnexcm("SET PAR", arglist, 2, ierflg);
     lhoodFit->mnexcm("FIX PAR", arglist, 1, ierflg);

    arglist[0] = 10000;
    arglist[1] = 1.0;
    lhoodFit->mnexcm("MIGRAD", arglist ,2,ierflg);
    lhoodFit->mnimpr();

    lhoodFit->mnstat(f_with_guess, junk, junk, ijunk, ijunk, ijunk);

    ATH_MSG_VERBOSE("nlep, n_fake_guess, n_fake_guess_lo, n_fake_guesss_hi, f_from_fit, f_with_guess: " << "?" << " " << n_fake_guess << " " <<  n_fake_guess_lo << " " <<  n_fake_guess_hi << " " << f_from_fit << " " <<  f_with_guess);

    if (TMath::IsNaN(f_with_guess)) {
      f_with_guess = f_from_fit + 1.;
    }
    if ((f_with_guess - f_from_fit) > 0.5) {
      n_fake_guess_hi = n_fake_guess;
    } else {
      n_fake_guess_lo = n_fake_guess;
    }

    n_fake_guess = 0.5*(n_fake_guess_lo+n_fake_guess_hi);
    
    ATH_MSG_VERBOSE( "n_fake_guess_lo, hi = " <<  n_fake_guess_hi << " " << n_fake_guess_lo);
    if ((n_fake_guess_hi - n_fake_guess_lo)/n_fake_guess_hi < convergeCriteria) {
      stopSearch = 1;
      ATH_MSG_VERBOSE("(n_fake_guess_lo,  n_fake_fit = " << n_fake_guess_lo << " " <<  n_fake_fit);
      if (n_fake_guess_lo > n_fake_fit) {
    errFound = 1;
      }
    
    }
  }

  // reset nfakes to value found from original fit, so it's read out properly
  // later
  arglist[0] = nfake_tot_index;
  arglist[1] = n_fake_fit;
  lhoodFit->mnexcm("SET PAR", arglist, 2, ierflg);
  
  if (errFound) {
    return n_fake_guess - n_fake_fit;
  } else {
    ATH_MSG_VERBOSE("Setting fitStatus to 1 in fixPosErr");
    m_fitStatus = 1;
    return -1.;
  }
}
  
double LhoodMM_tools::fixNegErr(double n_fake_fit, TMinuit_LHMM* lhoodFit) {

  ATH_MSG_VERBOSE("Trying to fix bad negative error");

  // get current value of -lnL
  Double_t f_from_fit, junk;
  Int_t ijunk;

  lhoodFit->mnstat(f_from_fit, junk, junk, ijunk, ijunk, ijunk);

  // do a binary search to find real value of positive error
  double n_fake_guess_hi = n_fake_fit;
  double n_fake_guess_lo = 0.;
  double n_fake_guess = n_fake_guess_lo;
  double f_with_guess;

  bool errFound = 0;
  bool stopSearch = 0;
  double convergeCriteria = 0.01;
  double min_n_fake_guess = 0.05;

  int nfake_tot_index = 1;

  Double_t arglist[10];

  int ierflg;

  arglist[0] = nfake_tot_index;
  arglist[1] = n_fake_fit;
  lhoodFit->mnexcm("SET PAR", arglist, 2, ierflg);
  lhoodFit->mnexcm("FIX PAR", arglist, 1, ierflg);
  
  arglist[0] = 10000;
  arglist[1] = 1.0;
  lhoodFit->mnexcm("MIGRAD", arglist ,2,ierflg);
  lhoodFit->mnimpr();

  lhoodFit->mnstat(f_from_fit, junk, junk, ijunk, ijunk, ijunk);

  while (!stopSearch) {
    // fit with n_fake_tot fixed to guess value

    
    arglist[0] = nfake_tot_index;
    arglist[1] = n_fake_guess;
     lhoodFit->mnexcm("SET PAR", arglist, 2, ierflg);
     lhoodFit->mnexcm("FIX PAR", arglist, 1, ierflg);

    arglist[0] = 10000;
    arglist[1] = 1.0;
    lhoodFit->mnexcm("MIGRAD", arglist ,2,ierflg);
    lhoodFit->mnimpr();
    lhoodFit->mnstat(f_with_guess, junk, junk, ijunk, ijunk, ijunk);

    ATH_MSG_VERBOSE("nlep, n_fake_guess, n_fake_guess_lo, n_fake_guesss_hi, f_from_fit, f_with_guess: " << "?" << " " << n_fake_guess << " " <<  n_fake_guess_lo << " " <<  n_fake_guess_hi << " " << f_from_fit << " " <<  f_with_guess);

    if ((f_with_guess - f_from_fit) > 0.5) {
      n_fake_guess_lo = n_fake_guess;
    } else {
      n_fake_guess_hi = n_fake_guess;
    }
    n_fake_guess = 0.5*(n_fake_guess_lo+n_fake_guess_hi);

    if (((n_fake_guess_hi - n_fake_guess_lo)/n_fake_guess_hi < convergeCriteria) || (n_fake_guess_hi < min_n_fake_guess) ) {
      stopSearch = 1;
      if (n_fake_guess_hi < n_fake_fit) {
    errFound = 1;
      }
    }
  }

  // reset nfakes to value found from original fit, so it's read out properly
  // later
  arglist[0] = nfake_tot_index;
  arglist[1] = n_fake_fit;
  lhoodFit->mnexcm("SET PAR", arglist, 2, ierflg);
  
  if (errFound) {
    return n_fake_guess - n_fake_fit;
  } else {
    ATH_MSG_VERBOSE("Setting fitStatus to 1 in fixNegErr");
    m_fitStatus = 1;
    return 1.;
  }
}

StatusCode LhoodMM_tools::saveProgress(TDirectory* dir) {

  ATH_MSG_VERBOSE("Saving progress");

  TTree *t = new TTree("LhoodMM_progress", "Stores current info from LhoodMM_toos");

  Int_t nlep;
  
  Float_t weight;
  Float_t rnominal[nLepMax];
  Float_t fnominal[nLepMax]; 
  Bool_t tight[nLepMax];
  Int_t charge[nLepMax];

  std::vector<std::vector<UShort_t> > r_systUID, f_systUID;
  std::vector<std::vector<float> > r_systUp, r_systDown,  f_systUp, f_systDown;


  t->Branch("nlep", &nlep, "nlep/I");
  t->Branch("weight", &weight, "weight/F");
  t->Branch("rnominal", rnominal, "rnominal[nlep]");
  t->Branch("fnominal", fnominal, "fnominal[nlep]");
  t->Branch("tight", tight, "tight[nlep]/O");
  t->Branch("charge", charge, "charge[nlep]/I");
  t->Branch("r_systUID", &r_systUID);    
  t->Branch("r_systUp", &r_systUp);
  t->Branch("r_systDown", &r_systDown);
  t->Branch("f_systUID", &f_systUID);    
  t->Branch("f_systUp", &f_systUp);
  t->Branch("f_systDown", &f_systDown);

  dir->cd();
  dir->mkdir("histos_1d");
  dir->cd("histos_1d");
  
  std::vector<float*> h1_val_addrs; 
  std::map<TH1*, std::vector<LhoodMMEvent>>::iterator map1_iter;
  int i1dhist = 0;
  for (map1_iter=m_mmevts_1dhisto_map.begin(); map1_iter != m_mmevts_1dhisto_map.end(); map1_iter++) {
    ATH_MSG_VERBOSE("Trying to write a 1d histo");
    TH1* histogram = (TH1*)map1_iter->first->Clone();
    histogram->Reset(); // just want to store empty histograms
    histogram->Write();

    auto val_map_iter = m_values_1dhisto_map.find(map1_iter->first);
    float* f = new float;
    h1_val_addrs.push_back(f);
    if (val_map_iter != m_values_1dhisto_map.end() ) {
      string histvarname = "h1d_val";
      histvarname += std::to_string(i1dhist);
      i1dhist++;
      string histvartype = histvarname+"/F";
      t->Branch(histvarname.c_str(), f, histvartype.c_str());      
    } else {
      ATH_MSG_ERROR("Could not find entry for histogram " << map1_iter->first);
      return StatusCode::FAILURE;
    }

  }

  dir->cd("..");
  dir->mkdir("histos_2d");
  dir->cd("histos_2d");

  std::vector<float*> h2_valx_addrs, h2_valy_addrs; 
  int i2dhist = 0;
  std::map<TH2*, std::vector<LhoodMMEvent>>::iterator map2_iter;
  for (map2_iter=m_mmevts_2dhisto_map.begin(); map2_iter != m_mmevts_2dhisto_map.end(); map2_iter++) {
    ATH_MSG_VERBOSE("Trying to write a 2d histo");
    TH2* histogram = (TH2*)map2_iter->first->Clone();
    histogram->Reset(); // just want to store empty histograms
    histogram->Write();

    auto val_map_iter = m_values_2dhisto_map.find(map2_iter->first);
    float* fx = new float;
    h2_valx_addrs.push_back(fx);
    float* fy = new float;
    h2_valy_addrs.push_back(fy);
    if (val_map_iter != m_values_2dhisto_map.end() ) {
      string histvarname = "h2d_valx";
      histvarname += std::to_string(i2dhist);
      string histvartype = histvarname+"/F";
      t->Branch(histvarname.c_str(), fx, histvartype.c_str());      
      histvarname = "h2d_valy";
      histvarname += std::to_string(i2dhist);
      i2dhist++;
      t->Branch(histvarname.c_str(), fy, histvartype.c_str());   
    } else {
      ATH_MSG_ERROR("Could not find entry for histogram " << map1_iter->first);
      return StatusCode::FAILURE;
    }
 
 }

  for (unsigned ievt = m_lastSaveIndex; ievt< m_mmevts_total.size(); ievt++) {

    r_systUID.clear();
    f_systUID.clear();
    r_systUp.clear();
    f_systUp.clear();
    r_systDown.clear();
    f_systDown.clear();

    LhoodMMEvent m = m_mmevts_total[ievt];
    nlep = m.nlep();
    weight = m.weight();

    for (int i = 0; i<nlep; i++) {
      rnominal[i] = m.realEff(i);
      fnominal[i] = m.fakeEff(i);
      tight[i] = m.isTight(i);
      charge[i] = m.charge(i);
      boost::container::flat_map<uint16_t, FakeBkgTools::Uncertainty> r =  m.realEffObj(i).uncertainties;
      std::vector<UShort_t> r_systUIDvec = std::vector<UShort_t>(); 
      std::vector<float> r_systUpvec = std::vector<float>(); 
      std::vector<float> r_systDownvec = std::vector<float>(); 
      for (const auto syst : r ) {
	r_systUIDvec.push_back(syst.first);
	r_systUpvec.push_back(syst.second.up);
	r_systDownvec.push_back(syst.second.down);
      } 
      r_systUID.push_back(r_systUIDvec);
      r_systUp.push_back(r_systUpvec);
      r_systDown.push_back(r_systDownvec);

      boost::container::flat_map<uint16_t, FakeBkgTools::Uncertainty> f =  m.fakeEffObj(i).uncertainties;
      std::vector<UShort_t> f_systUIDvec = std::vector<UShort_t>(); 
      std::vector<float> f_systUpvec = std::vector<float>(); 
      std::vector<float> f_systDownvec = std::vector<float>(); 
      for (const auto& syst : f) {
	f_systUIDvec.push_back(syst.first);
	f_systUpvec.push_back(syst.second.up);
	f_systDownvec.push_back(syst.second.down);
      } 
      f_systUID.push_back(f_systUIDvec);
      f_systUp.push_back(f_systUpvec);
      f_systDown.push_back(f_systDownvec);
    }

    int ih1var = 0;
    auto map1_iter = m_mmevts_1dhisto_map.begin();
    for (; map1_iter != m_mmevts_1dhisto_map.end(); map1_iter++) {
      float *h1_var_addr = h1_val_addrs[ih1var];
      auto mmevts_vec = map1_iter->second;
      *h1_var_addr = mmevts_vec[ievt].aux();
      ih1var++;
    }

    int ih2var = 0;
    auto map2_iter = m_mmevts_2dhisto_map.begin();
    for (; map2_iter != m_mmevts_2dhisto_map.end(); map2_iter++) {
      float *h2_varx_addr = h2_valx_addrs[ih2var];
      float *h2_vary_addr = h2_valy_addrs[ih2var];
      auto mmevts_vec = map2_iter->second;
      *h2_varx_addr = mmevts_vec[ievt].aux();
      *h2_vary_addr = mmevts_vec[ievt].aux2();
      ih2var++;
    }

    t->Fill();
  }
  m_lastSaveIndex = m_mmevts_total.size();
  dir->cd();
  t->Write();

  return StatusCode::SUCCESS;
}

StatusCode LhoodMM_tools::mergeSubJobs() {

  ATH_MSG_VERBOSE("Merging sub jobs");
  m_alreadyMerged = true;
  std::string filename = PathResolverFindDataFile(m_progressFileName);
  TFile* fin = new TFile(filename.c_str());
  if (fin == nullptr) {
    ATH_MSG_ERROR("Unable to open merged input file " << filename );
    return StatusCode::FAILURE;
  }
  if(m_progressFileDirectory.length()) {
    if(!fin->cd(m_progressFileDirectory.c_str())) {
      ATH_MSG_ERROR("Unable to find the directory " << m_progressFileDirectory << " inside the file " << filename);
      return StatusCode::FAILURE;
    }
  }

  fin->cd("histos_1d");
  TIter nextkey(gDirectory->GetListOfKeys());
  vector<float*> h1_val_addrs, h2_valx_addrs, h2_valy_addrs;
  while (TKey* key = (TKey*)nextkey()) {
    TH1F* h = (TH1F*)key->ReadObj()->Clone();
    h->SetDirectory(0);
    float* x = new float;
    h1_val_addrs.push_back(x);
    register1DHistogram(h, x);
  }

  fin->cd();
  fin->cd("histos_2d");
  nextkey = gDirectory->GetListOfKeys();
  while (TKey* key = (TKey*)nextkey()) {
    TH2F* h = (TH2F*)key->ReadObj()->Clone();
    h->SetDirectory(0);
    float* x = new float;
    float* y = new float;
    h2_valx_addrs.push_back(x);
    h2_valx_addrs.push_back(y);
    register2DHistogram(h, x, y);
  }


  TTree *t = (TTree*)fin->Get("LhoodMM_progress");
  if (t == nullptr) {
    ATH_MSG_ERROR("Unable to find LhoodMM_progress tree in " << filename);
    return StatusCode::FAILURE;
  }

  Int_t nlep;
  Float_t weight;

  Float_t rnominal[nLepMax];
  Float_t fnominal[nLepMax]; 
  Bool_t tight[nLepMax];
  Int_t charge[nLepMax];

  std::vector<std::vector<UShort_t> > *r_systUID, *f_systUID;
  std::vector<std::vector<float> > *r_systUp, *r_systDown,  *f_systUp, *f_systDown;

  r_systUID= new std::vector<std::vector<UShort_t> >;
  f_systUID= new std::vector<std::vector<UShort_t> >;
  r_systUp = new std::vector<std::vector<float> >;
  r_systDown = new std::vector<std::vector<float> >;
  f_systUp = new std::vector<std::vector<float> >;
  f_systDown = new std::vector<std::vector<float> >;
  
  t->SetBranchAddress("nlep", &nlep);
  t->SetBranchAddress("weight", &weight);
  t->SetBranchAddress("rnominal", rnominal);
  t->SetBranchAddress("fnominal", fnominal);
  t->SetBranchAddress("tight", tight);
  t->SetBranchAddress("charge", charge);

  t->SetBranchAddress("r_systUID", &r_systUID);    
  t->SetBranchAddress("r_systUp", &r_systUp);
  t->SetBranchAddress("r_systDown", &r_systDown);
  t->SetBranchAddress("f_systUID", &f_systUID);    
  t->SetBranchAddress("f_systUp", &f_systUp);
  t->SetBranchAddress("f_systDown", &f_systDown);

  int i1dhist = 0;
  for (auto map1_iter= m_values_1dhisto_map.begin(); map1_iter !=   m_values_1dhisto_map.end(); map1_iter++) {
    string histvarname = "h1d_val";
    histvarname += std::to_string(i1dhist);
    i1dhist++;
    t->SetBranchAddress(histvarname.c_str(), (float*)map1_iter->second);      
  }

  int i2dhist = 0;
  for (auto map2_iter=m_values_2dhisto_map.begin(); map2_iter != m_values_2dhisto_map.end(); map2_iter++) {
    string histvarname = "h2d_valx";
    histvarname += std::to_string(i2dhist);
    string histvartype = histvarname+"/F";
    t->SetBranchAddress(histvarname.c_str(), (float*)map2_iter->second.first);      
    histvarname = "h2d_valy";
    histvarname += std::to_string(i2dhist);
    i2dhist++;
    t->SetBranchAddress(histvarname.c_str(), (float*)map2_iter->second.second);   
  }

  Int_t nentries = (Int_t)t->GetEntries();
  for (Int_t ievt = 0; ievt < nentries; ievt++) {

    vector<xAOD::IParticle> leptons;
    vector<FakeBkgTools::ParticleData> leptons_data;

    t->GetEntry(ievt);

    std::vector<Bool_t> isTightvec;
    std::vector<Efficiency> rvec;
    std::vector<Efficiency> fvec;
    std::vector<Int_t> chargevec;

    for (int ilep = 0; ilep < nlep; ilep++) {
      // currently don't store type and pt for leptons in LhoodMMEvent, so
      // this information is not available.  Will that cause a problem?
      isTightvec.push_back(tight[ilep]);
      Efficiency reff, feff;
      reff.nominal = rnominal[ilep];
      feff.nominal = fnominal[ilep];
      
      
      for (unsigned irsys = 0; irsys < (*r_systUID)[ilep].size(); irsys++) {
	UShort_t id = (*r_systUID)[ilep][irsys];
	float up = (*r_systUp)[ilep][irsys];
	float down = (*r_systDown)[ilep][irsys];
	Uncertainty u;
	u.up = up;
	u.down = down;
	reff.uncertainties.insert(std::make_pair(id, u));
      }

      for (unsigned ifsys = 0; ifsys < (*f_systUID)[ilep].size(); ifsys++) {
	UShort_t id = (*f_systUID)[ilep][ifsys];
	float up = (*f_systUp)[ilep][ifsys];
	float down = (*f_systDown)[ilep][ifsys];
	Uncertainty u;
	u.up = up;
	u.down = down;
	feff.uncertainties.insert(std::make_pair(id, u));
      }
     
      rvec.push_back(reff);
      fvec.push_back(feff);
      chargevec.push_back(charge[ilep]);
    }

    addEventCustom(isTightvec, rvec, fvec, chargevec, weight);

  }
  
  delete r_systUID;
  delete f_systUID;
  delete r_systUp;
  delete f_systUp;
  delete r_systDown;
  delete f_systDown;

  delete t;

  return StatusCode::SUCCESS;
}


