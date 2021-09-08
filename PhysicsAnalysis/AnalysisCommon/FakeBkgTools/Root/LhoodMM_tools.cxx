/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "FakeBkgTools/LhoodMM_tools.h"
#include "FakeBkgTools/LhoodMMFitInfo.h"
#include "FakeBkgTools/FakeBkgInternals.h"
#include "FakeBkgTools/TMinuit_LHMM.h"
#include "FakeBkgTools/Database.h"
#include "PathResolver/PathResolver.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TMatrixT.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
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

  m_unlimitedSystematicVariations = true;

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

  m_global_fitInfo.reset();

  m_prevSave = false;

  m_nfakes_std = 1.;
  m_nfakes_std_err = 1.;

  m_maxWeight = 1.;

  m_printLevel = -1;

  m_theta_tot_start_index = -1;

  m_do_std_perEventWeight = false;

  m_fixNormalization = false;

  m_coeffs.clear();
  m_coeffs.resize(nLepMax);

  for (int ilep = 0; ilep < nLepMax; ilep++) {
    m_coeffs[ilep].clear();
    m_coeffs[ilep].resize((0x1 << (ilep+1)) );
    for (int ientry = 0; ientry < (0x1 << (ilep+1)); ientry++) {
      m_coeffs[ilep][ientry].resize((0x1 << (ilep+1)) );
      for (int jentry = 0; jentry < (0x1 << (ilep+1)); jentry++) {
      	m_coeffs[ilep][ientry][jentry] =0.;
      }
    }
  }
 
  m_real_indices.clear();
  m_real_indices.resize(nLepMax);
  m_fake_indices.clear();
  m_fake_indices.resize(nLepMax);

  m_maxnlep_loose = 0;

  m_requireSS = false;
  m_requireOS = false;

  m_fsvec.clear();
  m_fsvec.resize(nLepMax);

  m_OSfrac.clear();
  m_OSfrac.resize(nLepMax);

  for (int ilep = 0; ilep <nLepMax; ilep++) {
    m_OSfrac[ilep].resize(ilep);
  }

  m_doFakeFactor = false;

  m_nrf_mat_vec.clear();
  m_MMmatrix_vec.clear();
  m_nrf_mat_vec.resize(nLepMax);
  m_MMmatrix_vec.resize(nLepMax);
  m_ntlpred_vec.clear();
  m_ntlpred_vec.resize(nLepMax);
  for (int ilep = 0; ilep < nLepMax; ilep++) {
    m_nrf_mat_vec[ilep] = std::shared_ptr<TMatrixT<double>>(std::make_shared<TMatrixT<double>>(0x1 << (ilep+1),1) );
    m_MMmatrix_vec[ilep] =  std::shared_ptr<TMatrixT<double>>(std::make_shared<TMatrixT<double>> ((0x1 << (ilep+1)),(0x1 << (ilep+1))));
    m_ntlpred_vec[ilep] = std::shared_ptr<TMatrixT<double>>(std::make_shared< TMatrixT<double>>(0x1 << (ilep+1),1));
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

  
  auto currentMap = m_fitInfo_1dhisto_map.find(h1);
  if (currentMap ==  m_fitInfo_1dhisto_map.end() ) {
    m_fitInfo_1dhisto_map[h1];
    // size all the vectors appropriately
    int ncells = h1->GetNcells();
    auto *fitinfovec =  &m_fitInfo_1dhisto_map.find(h1)->second;
    fitinfovec->resize(ncells);
    ATH_MSG_VERBOSE("Just resized fitinfovec");
    for (int icell = 0; icell < ncells; icell++) {
      LhoodMMFitInfo* fitInfo =  &(fitinfovec->at(icell));
      fitInfo->reset();
    }

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
    
  auto currentMap = m_fitInfo_2dhisto_map.find(h2);
  if (currentMap ==  m_fitInfo_2dhisto_map.end() ) {
    m_fitInfo_2dhisto_map[h2];
    // size all the vectors appropriately
    int ncells = h2->GetNcells();
    auto *fitinfovec =  &m_fitInfo_2dhisto_map.find(h2)->second;
    fitinfovec->resize(ncells);
    for (int icell = 0; icell < ncells; icell++) {
      LhoodMMFitInfo* fitInfo =  &(fitinfovec->at(icell));
      fitInfo->reset();
    }
    ATH_MSG_INFO("Registered a 2D histogram "<<h2->GetName());
  } else {
    ATH_MSG_ERROR("Error in  LhoodMM_tools::register2DHistogram: histogram has already been registered");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
} 

StatusCode LhoodMM_tools::register3DHistogram(TH3* h3, const float *xval, const float *yval, const float *zval) {

  auto sc = BaseFakeBkgTool::register3DHistogram(h3, xval, yval, zval);

  if(sc != StatusCode::SUCCESS) return sc;
    
  auto currentMap = m_fitInfo_3dhisto_map.find(h3);
  if (currentMap ==  m_fitInfo_3dhisto_map.end() ) {
    m_fitInfo_3dhisto_map[h3];
    // size all the vectors appropriately
    int ncells = h3->GetNcells();
    auto *fitinfovec =  &m_fitInfo_3dhisto_map.find(h3)->second;
    fitinfovec->resize(ncells);
    for (int icell = 0; icell < ncells; icell++) {
      LhoodMMFitInfo* fitInfo =  &(fitinfovec->at(icell));
      fitInfo->reset();
    }
    ATH_MSG_INFO("Registered a 3D histogram "<<h3->GetName());
  } else {
    ATH_MSG_ERROR("Error in  LhoodMM_tools::register3DHistogram: histogram has already been registered");
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

  m_needToResize = false;
  if (nlep > m_maxnlep_loose) {
    m_maxnlep_loose = nlep;
    m_needToResize = true;
  }

  LhoodMMEvent mmevt(nlep, realEff_vals, fakeEff_vals, isTight_vals, charges, extraweight);
  
  ATH_CHECK(incrementMatrices(mmevt));
   
  
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
  
  m_current_fitInfo = &m_global_fitInfo;
  yield = nfakes(&poserr,&negerr);

  ATH_MSG_DEBUG("Leaving getTotalYield with yield = " << yield);

  statErrUp = poserr;
  statErrDown = -negerr;

  return fillHistograms();
}

StatusCode LhoodMM_tools::incrementMatrices(const LhoodMMEvent& mmevt) {

  int nlep = mmevt.nlep();
  //increment global matrix
  if (m_needToResize) {
    m_global_fitInfo.resizeVectors(nlep);
  }

  ATH_CHECK(incrementOneMatrixSet(m_global_fitInfo, mmevt));

  // increment matrix terms for all registered histograms
  // if size needs to increase, this must be done for all bins...

  for (auto map1_iter = m_values_1dhisto_map.begin(); map1_iter != m_values_1dhisto_map.end(); map1_iter++) {
    const float* val = map1_iter->second;
    TH1* h = map1_iter->first;
    auto histoMap = m_fitInfo_1dhisto_map.find(map1_iter->first);
    if (histoMap != m_fitInfo_1dhisto_map.end() ) {
      int icell;
      LhoodMMFitInfo *fitInfo;
      if (m_needToResize) {
	for (icell = 0; icell<h->GetNcells(); icell++) {
	  fitInfo = &histoMap->second[icell];
	  fitInfo->resizeVectors(nlep);
	}
      }
      icell = h->FindBin(*val);
      ATH_MSG_VERBOSE("fitInfo vector size is " << histoMap->second.size());
      fitInfo = &histoMap->second[icell];
      ATH_MSG_VERBOSE("icell is " << icell);
      ATH_MSG_VERBOSE("fitInfo totEvents is " << fitInfo->totEvents);
      ATH_MSG_VERBOSE("need to resize? " << m_needToResize);
      incrementOneMatrixSet(*fitInfo, mmevt);
      ATH_MSG_VERBOSE("need to resize now? " << m_needToResize);
    } else {
      ATH_MSG_ERROR("Can not find entry for 1d histogram"); 
      return StatusCode::FAILURE;
    }
  }
  
  std::map<TH2*, std::pair<const float*, const float*> >::iterator map2_iter;
  for (map2_iter = m_values_2dhisto_map.begin(); map2_iter != m_values_2dhisto_map.end(); map2_iter++) {
    std::pair<const float*, const float*> val = map2_iter->second;
    TH2* h = map2_iter->first;
 
    auto histoMap = m_fitInfo_2dhisto_map.find(map2_iter->first);
    if (histoMap != m_fitInfo_2dhisto_map.end() ) {
      int icell;
      LhoodMMFitInfo *fitInfo;
      if (m_needToResize) {
	for (icell = 0; icell<h->GetNcells(); icell++) {
	  fitInfo = &histoMap->second[icell];
	  fitInfo->resizeVectors(nlep);
	}
      }
      icell = h->FindBin(*(val.first), *(val.second));
      fitInfo = &histoMap->second[icell];
      incrementOneMatrixSet(*fitInfo, mmevt);
    } else {
      ATH_MSG_ERROR("Can not find entry for 2d histogram"); 
      return StatusCode::FAILURE;
    }

  }

  std::map<TH3*, std::tuple<const float*, const float*, const float*> >::iterator map3_iter;
  for (map3_iter = m_values_3dhisto_map.begin(); map3_iter != m_values_3dhisto_map.end(); map3_iter++) {
    std::tuple<const float*, const float*, const float*> val = map3_iter->second;
    TH3* h = map3_iter->first;
    auto histoMap = m_fitInfo_3dhisto_map.find(map3_iter->first);
    if (histoMap != m_fitInfo_3dhisto_map.end() ) {
      int icell;
      LhoodMMFitInfo *fitInfo;
      if (m_needToResize) {
        for (icell = 0; icell<h->GetNcells(); icell++) {
          fitInfo = &histoMap->second[icell];
          fitInfo->resizeVectors(nlep);
        }
      }
      icell = h->FindBin(*(std::get<0>(val)), *(std::get<1>(val)), *(std::get<2>(val)) );
      fitInfo = &histoMap->second[icell];
      incrementOneMatrixSet(*fitInfo, mmevt);
    } else {
      ATH_MSG_ERROR("Can not find entry for 3d histogram"); 
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}
  
StatusCode LhoodMM_tools::incrementOneMatrixSet(LhoodMMFitInfo& fitInfo,
						const LhoodMMEvent& mmevt) {

  int nlep = mmevt.nlep();
  int lepidx = nlep-1;
  int rank = 0x1 << nlep;

  fitInfo.totEvents++;
  fitInfo.eventCount[lepidx]++;


  if (nlep <= nLepMax) {
    ATH_MSG_VERBOSE("In incrementMatrices, how many uncertainties? " << mmevt.realEffObj(0).uncertainties.size());
    ATH_MSG_VERBOSE("In incrementMatrices, m_totEvents = " << fitInfo.totEvents);

    unsigned int catIndex = 0;
    for (int jlep = 0; jlep < nlep; jlep++) {
      catIndex += (!mmevt.isTight(jlep)) << jlep;
    }
    double weight = mmevt.weight();

    ATH_MSG_VERBOSE("event_cat.size() = " << fitInfo.event_cat.size());
    ATH_MSG_VERBOSE("event_sumw2.size() = " << fitInfo.event_sumw2.size());

    ATH_MSG_VERBOSE("nlep= " << nlep);
    ATH_MSG_VERBOSE("catIndex= " << catIndex);
    ATH_MSG_VERBOSE("fitInfo.event_cat.at(nlep-1).size() = " << fitInfo.event_cat.at(lepidx).size());

    fitInfo.event_cat[lepidx][catIndex] +=weight;
    fitInfo.event_sumw2[lepidx][catIndex]+=weight*weight;

    ATH_MSG_VERBOSE("Done updating event_cat and event_sumw2");
  
    //check to see if there is at least one OS pair...
    for (int icomb = 0; icomb < (0x1 << nlep); icomb++) {
      int totcharge = 0;
      std::bitset<nLepMax+1> tights(icomb);
      int ntight = tights.count();
      
      for (int jlep = 0; jlep < nlep; jlep++) {
	if (tights[jlep]) {
	  totcharge += mmevt.charge(jlep);
	}
      }

      ATH_MSG_VERBOSE("Setting OSfrac_denom[" << lepidx << "][" << ntight << "]");
      ATH_MSG_VERBOSE("size is " << fitInfo.OSfrac_denom.size() );
      ATH_MSG_VERBOSE("size is " << fitInfo.OSfrac_denom[lepidx].size() );
      ATH_MSG_VERBOSE("weight is " << weight  );
      fitInfo.OSfrac_denom[lepidx][ntight]+=weight;
      ATH_MSG_VERBOSE("Setting OSfrac_num[" << lepidx << "][" << ntight << "]");
      if ((TMath::Abs(totcharge) < ntight) || ntight == 1) fitInfo.OSfrac_num[lepidx][ntight]+=weight;
    }
  
  
    std::vector<std::vector<FakeBkgTools::Efficiency>> vals(2, std::vector<FakeBkgTools::Efficiency>(nlep));
    for (int ilep = 0; ilep < nlep; ilep++) {
      ATH_MSG_VERBOSE("1 vals[0].size() = " << vals[0].size());
      ATH_MSG_VERBOSE("getting efficiency values for lepton " << ilep);
      ATH_MSG_VERBOSE("how many leptons are there? " << mmevt.nlep());
      ATH_MSG_VERBOSE("nlep-ilep-1 = " << nlep-ilep-1);
      ATH_MSG_VERBOSE("2 vals[0].size() = " << vals[0].size());
      ATH_MSG_VERBOSE("vals[1].size() = " << vals[1].size());
      vals[0][ilep] = mmevt.realEffObj(ilep);
      if (m_doFakeFactor) {
	vals[0][ilep].setToConst(1.0);
      }
      vals[1][ilep] = mmevt.fakeEffObj(ilep);
      ATH_MSG_VERBOSE("Real and fake efficiencies for lepton " << ilep << ": " << vals[0][nlep-ilep-1].value(this) << " " << vals[1][nlep-ilep-1].value(this));
      ATH_MSG_VERBOSE("this is surely harmless");
      ATH_MSG_VERBOSE("3 vals[0].size() = " << vals[0].size());
    }
    
    FakeBkgTools::Efficiency curr_coeff_num;
    ATH_MSG_VERBOSE("how many uncertainties in curr_coeff_num? " << curr_coeff_num.uncertainties.size());
    for (int irf = 0; irf < rank; irf++) {
      for (int itl = 0; itl < rank; itl++) {
	curr_coeff_num.setToConst(1.);
	for (int ilep = 0; ilep < nlep; ilep++) {
	  if (itl & (0x1 << ilep) ) {
	    if (irf & (0x1 << ilep)) {
	      FakeBkgTools::Efficiency tmpEff = vals[1][ilep];
	      curr_coeff_num.multiply(tmpEff.subFromOne());
	    } else {
	      FakeBkgTools::Efficiency tmpEff = vals[0][ilep];
	      curr_coeff_num.multiply(tmpEff.subFromOne());
	    }
	  } else {
	    if (irf & (0x1 << ilep) ) {
	      curr_coeff_num.multiply(vals[1][ilep]);
	    } else {
	      curr_coeff_num.multiply(vals[0][ilep]);
	    }
	  }
	}
	ATH_MSG_VERBOSE("about to set m_coeffs_num[" << lepidx << "][" << itl << "][" << irf << "]");
	ATH_MSG_VERBOSE("setting normterm " << (itl<<nlep) + irf);
	fitInfo.normterms[lepidx][(itl<<nlep) + irf].add(curr_coeff_num);
	fitInfo.coeffs_num[lepidx][itl][irf].add(curr_coeff_num);
	ATH_MSG_VERBOSE("how many uncertainties in coeffs_num? " << fitInfo.coeffs_num[lepidx][itl][irf].uncertainties.size());
	ATH_MSG_VERBOSE("done setting normterm ");
      }
    }
    return StatusCode::SUCCESS;
  }  else {
    ATH_MSG_ERROR( "Error: can only handle " << nLepMax << " leptons; you tried " << nlep);
    return StatusCode::FAILURE;
  }
  
}

StatusCode LhoodMM_tools::setup() {

  m_current_lhoodMM_tool = this;

  for (int ilep = 0; ilep < m_maxnlep_loose; ilep++) {
    for (int jlep = 0; jlep < ilep; jlep++) {
      if (m_current_fitInfo->OSfrac_denom[ilep][jlep] > 0) {
	m_OSfrac[ilep][jlep]  = m_current_fitInfo->OSfrac_num[ilep][jlep]/m_current_fitInfo->OSfrac_denom[ilep][jlep];
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

  if (verbose) {
    ASG_MSG_VERBOSE("Parameters input to fcn_minnlep_maxnlep:" );
    for (int ipar = 0; ipar < npar; ipar++) {
      ASG_MSG_VERBOSE(ipar << ": " << par[ipar]); 
    }
  }
  int minnlep = l->m_minnlep;
  int maxnlep_loose = l->m_maxnlep_loose;

  Double_t f_tot = 0.;
  
  Double_t rsq = par[0];

  Double_t sinterm_tot = 1.;

  int theta_nlep_index = 1 + maxnlep_loose -minnlep;
  ASG_MSG_VERBOSE("theta_nlep_index initial = " << theta_nlep_index);
  for (int ilep = minnlep; ilep <= maxnlep_loose; ilep++) {
    theta_nlep_index += l->m_real_indices[ilep-1].size();
    ASG_MSG_VERBOSE("theta_nlep_index for ilep = " << ilep << " = " << theta_nlep_index);
  }

  Double_t pars_thisnlep[maxRank];  // a bit of a waste of memory, but avoids compiler warnings for variable-     

  if(verbose) ASG_MSG_VERBOSE("theta_nlep_index = " << theta_nlep_index);

  int real_index = 1;
  for (int ilep = minnlep; ilep <= maxnlep_loose; ilep++) {
    if (l->m_current_fitInfo->eventCount[ilep-1] == 0) {
      ASG_MSG_VERBOSE("m_real_indices[" << ilep-1 << "].size() = " << l->m_real_indices[ilep-1].size());
      real_index += l->m_real_indices[ilep-1].size();
      for (int ipar = 0; ipar < (int)l->m_fake_indices[ilep-1].size()-1; ipar++) {
	theta_nlep_index++;
      }
      continue;
    }
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
    ASG_MSG_VERBOSE("m_real_indices[ilep-1].size() = " <<  l->m_real_indices[ilep-1].size());
    for (unsigned ipar = 0; ipar < l->m_real_indices[ilep-1].size(); ipar++) {
      pars_thisnlep[par_index] = par[real_index+ipar];
      if(verbose) ASG_MSG_VERBOSE("r pars_thisnlep[" << par_index << "] = " <<  pars_thisnlep[par_index]);
      par_index++;
    }
    
    for (int ipar = 0; ipar < (int)l->m_fake_indices[ilep-1].size()-1; ipar++) {
      ASG_MSG_VERBOSE("theta_nlep_index = " << theta_nlep_index );
      pars_thisnlep[par_index+ipar] = par[theta_nlep_index];
      if(verbose) ASG_MSG_VERBOSE("f pars_thisnlep[" << par_index+ipar <<"] = " <<  pars_thisnlep[par_index+ipar]);
      theta_nlep_index++;
    }
    fcn_nlep(npar_thisnlep, gin, f, pars_thisnlep, iflag);
    f_tot += f;
    
    real_index += l->m_real_indices[ilep-1].size();
  }
  
  f = f_tot;
}

void LhoodMM_tools::fcn_nlep(Int_t &npar, Double_t *, Double_t &f, Double_t *par, Int_t)
{

  LhoodMM_tools* l = m_current_lhoodMM_tool;

  const bool verbose = (l->m_printLevel > 0);

  int nlep = l->m_curr_nlep;
  int lepidx = nlep-1;
  int rank = 0x1 << nlep;

  if(verbose) {
    ASG_MSG_VERBOSE("npar = " << npar);
    for (int ipar = 0; ipar < npar; ipar++) {
      ASG_MSG_VERBOSE("Parameter " << ipar <<  " = " << par[ipar]);
    }
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
  nrf = l->m_nrf_mat_vec[lepidx];
  if(verbose) {
    ASG_MSG_VERBOSE("What does nrf look like?");
    nrf->Print();
  }
  MMmatrix = l->m_MMmatrix_vec[lepidx];
  ntlpred = l->m_ntlpred_vec[lepidx];

  (*nrf)(0,0) = par[0];
  double rsq = TMath::Abs(par[0]);
  if(verbose) ASG_MSG_VERBOSE("rsq = " << rsq);
  int rsize = l->m_real_indices[lepidx].size();
  int fsize = l->m_fake_indices[lepidx].size();

  for (int ipar = 0; ipar < rsize; ipar++) {
    ASG_MSG_VERBOSE("In fcn, setting real par " << l->m_real_indices[lepidx][ipar] << " to " <<  par[ipar+1]); 
    (*nrf)(l->m_real_indices[lepidx][ipar], 0) = par[ipar+1];
  }
  double sinterm = 1.;

  if(verbose) ASG_MSG_VERBOSE("nrf[0] = " << (*nrf)(0,0));

  for (int ipar = 0; ipar < fsize; ipar++) {
    if(verbose) ASG_MSG_VERBOSE("r, sinterm, par[ipar+1] " << rsq << " " << sinterm << " " << par[ipar+1]);
    if (ipar <  fsize-1 ) {
      double costerm = TMath::Cos(par[rsize+ipar+1]);
      if(verbose) ASG_MSG_VERBOSE("for setting fake parameter, sinterm = " << sinterm << " par index = " << l->m_real_indices[lepidx].size()+ipar+1);
      (*nrf)(l->m_fake_indices[lepidx][ipar],0) = rsq*sinterm*costerm*sinterm*costerm;
    } else {
      (*nrf)(l->m_fake_indices[lepidx][ipar],0) = rsq*sinterm*sinterm;
    }
    
    ASG_MSG_VERBOSE("nrf[" << ipar << "] = " << (*nrf)(ipar,0));
    ASG_MSG_VERBOSE("In fcn, setting fake par " << l->m_fake_indices[lepidx][ipar] << " to " <<  (*nrf)(l->m_fake_indices[lepidx][ipar],0)); 
   
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
    if(verbose) ASG_MSG_VERBOSE("Comparing parameter " << ipar << ": " << l->m_current_fitInfo->event_cat.at(lepidx).at(ipar) << " to " << (*ntlpred)(ipar,0));
    // following scaling for weighted events motivated by arXiv:1309.1287 
    int nobs = l->m_current_fitInfo->event_cat.at(lepidx).at(ipar);
    double s = 1.;
    if (nobs > 0) {
      s= l->m_current_fitInfo->event_sumw2.at(lepidx).at(ipar)/nobs;
    }
    if (verbose) ASG_MSG_VERBOSE("s = " << s); 
    f += logPoisson(1.*nobs/s, (*ntlpred)(ipar,0)/s);
    if (verbose)  ASG_MSG_VERBOSE("f = " << f);
  }

}


double LhoodMM_tools::nfakes(Double_t *poserr, Double_t *negerr) {
  
  if (m_current_fitInfo->totEvents == 0) {
    *poserr = 0.;
    *negerr = 0.;
    return 0.;
  }

  m_fitStatus = 0;

  m_minnlep = nLepMax;
  m_maxnlep = 0;

  m_requireSS = false;
  m_requireOS = false;

  for (int ilep = 0; ilep < nLepMax; ilep++) {
    string error;
    // possible issue here -- reassigning vector elements?
    m_fsvec[ilep].reset(new FakeBkgTools::FinalState(0, ilep+1, m_selection, m_process, error));
    if (error.size() > 0) {
      ATH_MSG_VERBOSE("Unable to parse selection " << m_selection << " with process " << m_process << " with " << ilep+1 << " leptons. Error is " << error);
      continue; // unable to parse selection
    }
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
    // about the values here) 
    auto charges = m_fsvec[ilep]->retrieveCharges(m_particles);  
  
    for (int icomb = 0; icomb < (0x1 << (ilep+1)); icomb++) {
      FakeBkgTools::FSBitset tights(icomb);
      ATH_MSG_VERBOSE("ilep " << ilep << " (0x1 << ilep) " << std::hex << (0x1 << ilep) << " icomb " << std::hex << icomb << std::dec);
      ATH_MSG_VERBOSE("tights = " << std::hex << tights << std::dec);
      ATH_MSG_VERBOSE("charges = " << std::hex << charges << std::dec);

      if (m_fsvec[ilep]->accept_selection(tights, charges)) {
	int nlep = ilep+1; 
	ATH_MSG_VERBOSE("tights = " << std::hex << tights << std::dec << " nlep = " << nlep);
	if (nlep > m_maxnlep) m_maxnlep = nlep;
	if (nlep < m_minnlep) m_minnlep = nlep;
      }
    }
     
  }

  //protect against selection strings that don't explicitly require at least
  //one lepton
  if (m_minnlep < 1) m_minnlep = 1;

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
	if (m_fsvec[n-1]->accept_process(n, reals, tights) ) {
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

  if(setup() != StatusCode::SUCCESS) return 0.;

  if (m_maxnlep > m_maxnlep_loose) {
    m_maxnlep = m_maxnlep_loose;
  }

  if (m_maxnlep_loose < m_minnlep) {
    ATH_MSG_WARNING("Selection requires more leptons than are in any event in the sample; returning nfakes = 0 +/- 0 " << m_maxnlep_loose);
    *poserr = 0.;
    *negerr = 0.;
    return 0.;
  }

  ATH_MSG_VERBOSE("In nfakes, m_minnlep, m_maxnlep, m_maxnlep_loose = " << m_minnlep << " " << m_maxnlep << " " << m_maxnlep_loose);

  double nfake_fit, nfake_fitErr;
  
  int npar = 0;
  for (int ilep = m_minnlep; ilep <= m_maxnlep_loose; ilep++) {
    npar += 0x1 << ilep;
  }

  ATH_MSG_VERBOSE("In nfakes, npar = " << npar);

  std::unique_ptr<TMinuit_LHMM> lhoodFit(new TMinuit_LHMM(npar));
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
  loc_init_pars.resize(m_maxnlep_loose);
 
  for (int ilep = m_minnlep; ilep <=m_maxnlep_loose; ilep++) {
    loc_init_pars[ilep-1].resize(0x1 << ilep);
    get_init_pars(loc_init_pars[ilep-1], ilep);
    for (int ipar = 0; ipar < (0x1 << ilep); ipar++) {
      init_pars[index+ipar] = loc_init_pars[ilep-1][ipar];
    }
    index+= 0x1 << ilep;
  }

  Double_t step = TMath::Max(loc_init_pars[m_minnlep-1][0]/1000,0.001);
  ATH_MSG_VERBOSE("parameters initialized OK");
  ATH_MSG_VERBOSE("m_nfakes_std_err = " << m_nfakes_std_err);

  m_nfakes_std_err = TMath::Sqrt(m_nfakes_std_err);
  
  vector<TString> parameterName;
  vector<int> nreal_start_indices;
  nreal_start_indices.resize(m_maxnlep_loose);
  parameterName.push_back("nfake_tot");
  TString sreal = "nreal";
  for (int ilep = m_minnlep; ilep<= m_maxnlep_loose; ilep++) {
    char tmpchar[20];
    sprintf(tmpchar, "_%i", ilep);
    TString tmpstr = sreal;
    tmpstr.Append(tmpchar);
    for (unsigned isublep = 0; isublep < m_real_indices[ilep-1].size(); isublep++) {
      TString locsreal = tmpstr;
      char tmpchar2[20];
      sprintf(tmpchar2, "_%i", isublep);
      locsreal.Append(tmpchar2);
      parameterName.push_back(locsreal);
      if (isublep == 0) {
        nreal_start_indices[ilep-1] = (parameterName.size());
        ATH_MSG_VERBOSE("nreal_start_indices[" << ilep-1 << "] = " << nreal_start_indices[ilep-1]);
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
    ATH_MSG_VERBOSE("How many fake indices?" << m_fake_indices[ilep-1].size());
    for(int jlep = 0; jlep < (int)m_fake_indices[ilep-1].size() - 1; jlep++) {
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
      ATH_MSG_VERBOSE("nfakes for nlep = " << ilep << " used to find theta_tot = " << loc_init_pars[ilep-1][0]);
      theta_tot[theta_index] = TMath::ACos(TMath::Sqrt(TMath::Max(loc_init_pars[ilep-1][0],0.)/(m_nfakes_std))/sinterm);
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

  upper_limits[0] =   TMath::Max(5.*m_maxWeight*m_current_fitInfo->totEvents, 0.01);

  int real_index = 1;
  for (int ilep = m_minnlep; ilep <= m_maxnlep_loose; ilep++) {
    for (unsigned isublep = 0; isublep < m_real_indices[ilep-1].size(); isublep++) {
      upper_limits[real_index] = 5*m_maxWeight*m_current_fitInfo->eventCount[ilep-1]; 
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
    for (unsigned isublep = 0; isublep < m_real_indices[ilep-1].size(); isublep++) {     
      ATH_MSG_VERBOSE("Setting parameter " << glob_index << " to " <<  init_pars[init_index+isublep]);
      init_par_values[glob_index] = init_pars[init_index+isublep];
      glob_index++;
    }
    init_index+= pow(2,ilep);
  }

  m_theta_tot_start_index = 1;
  for (int ilep = m_minnlep; ilep <= m_maxnlep_loose; ilep++) {
    m_theta_tot_start_index += m_real_indices[ilep-1].size();
  }
  vector<int> theta_start_indices;
  theta_start_indices.resize(m_maxnlep_loose);
  theta_start_indices[m_minnlep-1] =  m_theta_tot_start_index+m_maxnlep_loose - m_minnlep;
  for (int i = m_minnlep; i < m_maxnlep_loose; i++) {
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
    for (int ipar = 0; ipar < (int)m_fake_indices[ilep-1].size() - 1; ipar++) {
      ATH_MSG_VERBOSE("Setting parameter " << currpar  << " to " <<  init_pars[index+m_real_indices[ilep-1].size()+ipar]);
      init_par_values[currpar] = init_pars[index+m_real_indices[ilep-1].size()+ipar];
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
  ATH_MSG_VERBOSE("m_maxnlep_loose = " << m_maxnlep_loose);
  // account for case where there may be no leptons of a given multiplicity 
  // (or no fake leptons allowed by the process string) by
  // fixing the parameters that are relevant to that multiplicity.
  // Also check that at least one lepton multiplicity is valid

  int nGoodLeptonMult = 0;

  for (int ilep = m_minnlep; ilep <= m_maxnlep_loose; ilep++) {
    if (m_current_fitInfo->eventCount[ilep-1] ==  0  ||
	m_fake_indices[ilep-1].size() == 0) {
      // start with the nreal parameters
      for (unsigned ipar = nreal_start_indices[ilep-1]; ipar < nreal_start_indices[ilep-1] + m_real_indices[ilep-1].size(); ipar++) {
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
      for (unsigned ipar = theta_start_indices[ilep-1]+1; ipar <  theta_start_indices[ilep-1] + m_fake_indices[ilep-1].size() ; ipar++) {
	arglist[0] = ipar;
	arglist[1] = 0.;
	lhoodFit->mnexcm("SET PAR", arglist, 2, ierflg);
	lhoodFit->mnexcm("FIX PAR", arglist, 1, ierflg);
      }
    } else {
      nGoodLeptonMult++;
    }
    index += (0x1 << ilep) - 2;
  }

  if (nGoodLeptonMult == 0) {
    ATH_MSG_VERBOSE("No possible fake contribution for any lepton multiplicity");
    *poserr = 0;
    *negerr = 0;
    return 0;
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
      *poserr = fixPosErr(nfake_fit, lhoodFit.get());
    } 
    if (*negerr > -1.e-5 ) {
      *negerr = fixNegErr(nfake_fit, lhoodFit.get());
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
  
  return nfake_fit; 

}

StatusCode LhoodMM_tools::fillHistograms() {

  StatusCode status = StatusCode::SUCCESS;

  for (auto map1_iter=m_fitInfo_1dhisto_map.begin(); map1_iter != m_fitInfo_1dhisto_map.end(); map1_iter++) {
    auto & fitInfo_vec = map1_iter->second;
    TH1* histogram = map1_iter->first;
    status = fillHisto_internal(fitInfo_vec, histogram);
  }

  for (auto map2_iter=m_fitInfo_2dhisto_map.begin(); map2_iter != m_fitInfo_2dhisto_map.end(); map2_iter++) {
    auto& fitInfo_vec = map2_iter->second;
    TH2* histogram = (TH2*)map2_iter->first;
    status = fillHisto_internal(fitInfo_vec, histogram);
    ATH_MSG_VERBOSE("Inside fill code, mean = " << histogram->ProjectionX()->GetMean());
  }

  for (auto map3_iter=m_fitInfo_3dhisto_map.begin(); map3_iter != m_fitInfo_3dhisto_map.end(); map3_iter++) {
    auto& fitInfo_vec = map3_iter->second;
    TH3* histogram = (TH3*)map3_iter->first;
    status = fillHisto_internal(fitInfo_vec, histogram);
    ATH_MSG_VERBOSE("Inside fill code, mean = " << histogram->ProjectionX()->GetMean());
  }

  return status;
}

StatusCode LhoodMM_tools::fillHisto_internal(const std::vector< LhoodMMFitInfo>& fitInfo_vec, TH1* h) {

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

  int nbins = h->GetNcells();

  ATH_MSG_VERBOSE("nbins = " << nbins);
  Double_t nf, poserr, negerr, shift = 0.;

  for (int ibin = 0; ibin < nbins; ibin++) {

    m_current_fitInfo = &fitInfo_vec[ibin];
    int totEvents =  m_current_fitInfo->totEvents;
    if (totEvents > 0) {
      ATH_MSG_VERBOSE("Filling bin " << ibin << " with " << totEvents << " events");
      nf = nfakes(&poserr, &negerr);
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
    
    m_current_fitInfo = &m_global_fitInfo;
    totFakes = nfakes(&poserr, &negerr);
    
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

  int lepidx = nlep-1;

  vector<double> nrf;
  nrf.resize(0x1 <<nlep);
  get_analytic(nrf, nlep);
  m_real_indices[lepidx].clear();
  m_fake_indices[lepidx].clear();
  
  std::string txt = "Testing variable transform: Initial nrf: ";
  for (auto i = nrf.begin(); i != nrf.end(); ++i)
    txt += std::to_string(*i) + ' ';
  ATH_MSG_VERBOSE(txt);

  vector<double> init_angles;

  double nfakes_std_thisnlep = 0;
  
  ATH_MSG_VERBOSE("m_fsvec.size() = " << m_fsvec.size() );
  for (int ipar = 0; ipar < 0x1 <<nlep; ipar++) {
    ATH_MSG_VERBOSE("ipar = " << ipar );
    FakeBkgTools::FSBitset fakes = ipar;
    FakeBkgTools::FSBitset reals;
    for (int ibit = 0; ibit < nlep; ibit++) {
      reals.set(ibit, ~fakes[ibit]);
    }
    ATH_MSG_VERBOSE("reals set" );
    bool countsAsFake = false;
    for (int jpar = 0; jpar <  0x1 <<nlep; jpar++) {
      FakeBkgTools::FSBitset tights = jpar;
      for (int kpar = 0; kpar < 0x1 <<nlep; kpar++) {
	FakeBkgTools::FSBitset charges = kpar;
	if (!countsAsFake && 
	    m_fsvec[lepidx]->accept_process(nlep, reals, tights) && 
	    m_fsvec[lepidx]->accept_selection(tights, charges) ) {
	  ATH_MSG_VERBOSE("accepted " << ipar);
	  nfakes_std_thisnlep += nrf[ipar];
	  m_fake_indices[lepidx].push_back(ipar);
	  countsAsFake = true;
	  break;
	}
      }
    }
    if (!countsAsFake) {
      ATH_MSG_VERBOSE("trying to push onto m_real_indices");
      ATH_MSG_VERBOSE("m_real_indices.size() = " << m_real_indices.size());
      m_real_indices[lepidx].push_back(ipar);
    }
  }

  init_pars[0] = nfakes_std_thisnlep;
  for (unsigned ipar = 1; ipar <= m_real_indices[lepidx].size(); ipar++) {
    init_pars[ipar] = nrf[m_real_indices[lepidx][ipar-1]];
  }

  if (nfakes_std_thisnlep > 0.5) {
    double sinterm = 1.;
    int ifake = 0;
    for (int ipar = m_real_indices[lepidx].size()+1; ipar < (0x1 << nlep); ipar++) {
      init_pars[ipar] = TMath::ACos(TMath::Sqrt(TMath::Max(nrf[m_fake_indices[lepidx][ifake] ], 0.)/(nfakes_std_thisnlep))/sinterm);
      sinterm *= TMath::Sin(init_pars[ipar]);
      ifake++;
    }
  } else {
    for (int ipar = m_real_indices[lepidx].size()+1; ipar < (0x1 << nlep); ipar++) {
      init_pars[ipar] =  _piover4;  
    }
  }

  if (m_printLevel > 0) {
    txt = "testing variable transform: Initial pars: ";
    for (int i = 0; i < (0x1 << nlep); i++) {
      txt += std::to_string(init_pars[i]) + " ";
    }
    ATH_MSG_VERBOSE(txt);
  }

  // fix any nan's...
  for (int ipar = 2; ipar < (0x1 << nlep); ipar++) {
    if (TMath::IsNaN(init_pars[ipar])) {
      init_pars[ipar] = 0.;
    }
    ATH_MSG_VERBOSE("Setting angle parameter " << ipar << " to " <<  init_pars[ipar]);
  }
}


void LhoodMM_tools::get_analytic(vector<double>& nrf, const int nlep) {

  m_perfectFit = true;
  ATH_MSG_VERBOSE("just getting started with nlep = "  << nlep);

  ATH_MSG_VERBOSE("m_minnlepreg = " << m_minnlep << " m_maxnlep = " << m_maxnlep);


  int lepidx = nlep-1;

  if (m_current_fitInfo->eventCount[lepidx] == 0) return;

  const int rank = 0x1 << nlep;

  std::vector<FakeBkgTools::Efficiency> coeff_denom(rank);

  ATH_MSG_VERBOSE("All set! ");
  
  FakeBkgTools::FSBitset charges =  m_fsvec[lepidx]->retrieveCharges(m_particles); 
  for (int irf = 0; irf < rank; irf++) {
    // add up all the relevant terms in the denominator to translate the 
    // loose sample counts to tight sample counts within the required range of
    // tight lepton multiplicity
    coeff_denom[irf].setToConst(0.);
    coeff_denom[0] = m_current_fitInfo->coeffs_num[lepidx][0][0];  // don't care about this one, but it can't be 0
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
      if (m_fsvec[lepidx]->accept_selection(tights, charges)
      	  &&  m_fsvec[lepidx]->accept_process(nlep, reals, tights) ) {
	ATH_MSG_VERBOSE("Accepted in LhoodMM_tools " << irf);
	ATH_MSG_VERBOSE("index is " << (itl<<nlep) + irf);
	ATH_MSG_VERBOSE("Adding " << m_current_fitInfo->normterms[lepidx][(itl<<nlep) + irf].value(this) << " to " << irf);
	// assume that dilepton fakes are evenly split between OS and SS
	if (nlep > 2 && tights.count() == 2) {
	  if (m_requireOS || m_requireSS) {
	    chargefactor = 0.5;
	    ATH_MSG_VERBOSE("Setting SSfactor ");
	  }
	}
	if (nlep == 2 && tights.count() == 2) {
	  if (m_requireOS || m_requireSS) {
	    chargefactor = m_OSfrac[2][2];
	  }
	}
	if (m_requireOS) {
	  chargefactor = m_OSfrac[lepidx][tights.count()];
	}
	ATH_MSG_VERBOSE("chargefactor = " << chargefactor << " for nlep = " << nlep);
	ATH_MSG_VERBOSE("normterm = " << m_current_fitInfo->normterms[lepidx][(itl<<nlep) + irf].value(this));
	FakeBkgTools::Efficiency tmpEff = m_current_fitInfo->normterms[lepidx][(itl<<nlep) + irf];
	tmpEff.multiply(chargefactor);
	ATH_MSG_VERBOSE("how much space? " << coeff_denom.size());
	ATH_MSG_VERBOSE("irf =  " << irf);
	coeff_denom[irf].add(tmpEff);
	ATH_MSG_VERBOSE("m_normterms[" << nlep << "][" << (itl<<nlep) + irf << "]= " << m_current_fitInfo->normterms[lepidx][(itl<<nlep) + irf].value(this));
	ATH_MSG_VERBOSE("chargefactor = " << chargefactor);	
	ATH_MSG_VERBOSE("coeff_denom[" << irf << "] = " << coeff_denom[irf].value(this));
      }
    }
    
    // The following are "don't care" terms, but need to have non-zero
    // denominators 
     if (coeff_denom[irf].nominal == 0.) {
       coeff_denom[irf] =  m_current_fitInfo->coeffs_num[lepidx][0][irf];
     }

    for (int itl = 0; itl < rank; itl++) {
      ATH_MSG_VERBOSE("coeff_denom[" << irf << "] = " << coeff_denom[irf].value(this));
      m_coeffs[lepidx][itl][irf] = m_current_fitInfo->coeffs_num[lepidx][itl][irf].value(this)/coeff_denom[irf].value(this); 
    }
  }

  ATH_MSG_VERBOSE("About to do matrix stuff");
  std::shared_ptr<TMatrixT<double>> MMmatrix;
  MMmatrix = m_MMmatrix_vec[lepidx];
  
  for (int i = 0; i < rank; i++) {
    for (int j = 0; j < rank; j++) {
      ATH_MSG_VERBOSE(i << " " << j << " " << m_current_fitInfo->coeffs_num[lepidx][i][j].value(this) << " " << coeff_denom[j].value(this) << " " <<  m_coeffs[lepidx][i][j]);
      (*MMmatrix)(i,j) =  m_coeffs[lepidx][i][j];
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
    nevents_mat(i,0) = m_current_fitInfo->event_cat.at(lepidx).at(i);
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
    if (nrf[ipar] < 0) m_perfectFit = false;
    ATH_MSG_VERBOSE("nrf[" << ipar << "] = " << nrf[ipar]);
    FakeBkgTools::FSBitset fakes = ipar;
    FakeBkgTools::FSBitset reals;
    FakeBkgTools::FSBitset tights = 0;      
    for (int ibit = 0; ibit < nlep; ibit++) {
      tights.set(ibit, 1);
      reals.set(ibit, ~fakes[ibit]);
    }
    if (m_fsvec[lepidx]->accept_process(nlep, reals, tights) && m_fsvec[lepidx]->accept_selection(tights, charges)) {
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

  if (m_prevSave) {
    ATH_MSG_ERROR("Multiple calls to saveProgress are not supported");
    return StatusCode::FAILURE;
  }
  
  m_prevSave = true;
  
  std::unique_ptr<TTree> t(new TTree("LhoodMM_progress", "Stores current info from LhoodMM_toos"));
  
  std::unique_ptr<TTree> t_nlep(new TTree("LhoodMM_progress_nlep", "Stores minimum and maximum lepton multiplicities"));
  
  auto fitInfoBranch = t->Branch("glb_fitInfo", &m_global_fitInfo);
  ATH_MSG_VERBOSE("Branch split level is " << fitInfoBranch->GetSplitLevel() );
  t->Branch("fitInfo_1dhisto_map", &m_fitInfo_1dhisto_map);
  t->Branch("fitInfo_2dhisto_map", &m_fitInfo_2dhisto_map);
  t->Branch("fitInfo_3dhisto_map", &m_fitInfo_3dhisto_map);

  t_nlep->Branch("maxnlep", &m_maxnlep_loose);  

  ATH_MSG_VERBOSE("Filling tree...");
  t->Fill();
  t_nlep->Fill();
  dir->cd();
  t->Write();
  t_nlep->Write();

  return StatusCode::SUCCESS;
}

StatusCode LhoodMM_tools::mergeSubJobs() {

  ATH_MSG_VERBOSE("Merging sub jobs");

  m_alreadyMerged = true;
  std::string filename = PathResolverFindDataFile(m_progressFileName);
  std::unique_ptr<TFile> fin( new TFile(filename.c_str()));
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

  std::string prefix = (m_progressFileDirectory.length()? "/" + m_progressFileDirectory + "/" : "");
  
  TTree *t_nlep = (TTree*)fin->Get((prefix + "LhoodMM_progress_nlep").c_str());
  if (t_nlep == nullptr) {
    ATH_MSG_ERROR("Unable to find LhoodMM_progress_nlep tree in " << filename);
    return StatusCode::FAILURE;
  }

  int merged_maxnlep, merged_maxnlep_prev = -1;
  t_nlep->SetBranchAddress("maxnlep", &merged_maxnlep);  

  TTree *t = (TTree*)fin->Get((prefix + "LhoodMM_progress").c_str());
  if (t == nullptr) {
    ATH_MSG_ERROR("Unable to find LhoodMM_progress tree in " << filename);
    return StatusCode::FAILURE;
  }
  
  // check that we are not trying to merge subjobs with inconsistent lepton
  // multiplicities
  Int_t nentries = (Int_t)t_nlep->GetEntries();
  for (Int_t ievt = 0; ievt < nentries; ievt++) {
    t_nlep->GetEntry(ievt);
    if (ievt > 0 && (merged_maxnlep != merged_maxnlep_prev)) {
      ATH_MSG_ERROR("Attempting to merge files with different lepton multiplicities.  This is not supported.");
      return StatusCode::FAILURE;
    }
    merged_maxnlep_prev = merged_maxnlep;  
  }

  m_maxnlep_loose = merged_maxnlep;

  std::unique_ptr<LhoodMMFitInfo> tmpFitInfo(new LhoodMMFitInfo);

  tmpFitInfo->resizeVectors(m_maxnlep_loose);
  m_global_fitInfo.resizeVectors(m_maxnlep_loose);
  
  LhoodMMFitInfo* fitInfoPtr = tmpFitInfo.get();
  t->SetBranchAddress("glb_fitInfo", &fitInfoPtr );

  ATH_MSG_VERBOSE("About to add LhoodMMFitInfos");

 // prepare to merge any histograms

  std::unique_ptr<std::map<TH1*, std::vector< LhoodMMFitInfo > > > tmp_fitInfo_1dhisto_map(new std::map<TH1*, std::vector< LhoodMMFitInfo > >);
  auto *tmp_fitInfo_1dhisto_map_ptr =  tmp_fitInfo_1dhisto_map.get(); 
  t->SetBranchAddress("fitInfo_1dhisto_map", &tmp_fitInfo_1dhisto_map_ptr);
  std::unique_ptr<std::map<TH2*, std::vector< LhoodMMFitInfo > > > tmp_fitInfo_2dhisto_map(new std::map<TH2*, std::vector< LhoodMMFitInfo > >);
  auto *tmp_fitInfo_2dhisto_map_ptr =  tmp_fitInfo_2dhisto_map.get(); 
  t->SetBranchAddress("fitInfo_2dhisto_map", &tmp_fitInfo_2dhisto_map_ptr);
  std::map<TH3*, std::vector< LhoodMMFitInfo > > *tmp_fitInfo_3dhisto_map =  new std::map<TH3*, std::vector< LhoodMMFitInfo > >;
  t->SetBranchAddress("fitInfo_3dhisto_map", &tmp_fitInfo_3dhisto_map);

  nentries = (Int_t)t->GetEntries();
  for (Int_t ievt = 0; ievt < nentries; ievt++) {
    t->GetEntry(ievt);
    ATH_MSG_VERBOSE("Adding LhoodMMFitInto with " << (*tmpFitInfo).totEvents << " events, with m_maxnlep_loose = " <<  m_maxnlep_loose );    
    m_global_fitInfo.add(*tmpFitInfo, m_maxnlep_loose);
    for (auto& hm :  m_fitInfo_1dhisto_map) {
      TH1F* histogram = (TH1F*)hm.first;
      std::string hname = histogram->GetName();
      for (auto& im: *tmp_fitInfo_1dhisto_map) {
	ATH_MSG_VERBOSE("Found a matching histogram");
	TH1F* ihistogram = (TH1F*)im.first;
	std::string iname = ihistogram->GetName();
	if (hname == iname) {
	  int ncells = histogram->GetNcells();
	  for (int icell = 0; icell<ncells; icell++) {
	    hm.second[icell].resizeVectors(m_maxnlep_loose);
	    hm.second[icell].add(im.second[icell], m_maxnlep_loose);
	  }
	}
      }
    }
    for (auto& hm :  m_fitInfo_2dhisto_map) {
      TH1F* histogram = (TH1F*)hm.first;
      std::string hname = histogram->GetName();
      for (auto& im: *tmp_fitInfo_2dhisto_map) {
	ATH_MSG_VERBOSE("Found a matching histogram");
	TH1F* ihistogram = (TH1F*)im.first;
	std::string iname = ihistogram->GetName();
	if (hname == iname) {
	  int ncells = histogram->GetNcells();
	  for (int icell = 0; icell<ncells; icell++) {
	    hm.second[icell].resizeVectors(m_maxnlep_loose);
	    hm.second[icell].add(im.second[icell], m_maxnlep_loose);
	  }
	}
      }
    }
    for(auto& hm : m_fitInfo_3dhisto_map){
      TH1F* histogram = (TH1F*)hm.first;
      std::string hname = histogram->GetName();
      for (auto& im: *tmp_fitInfo_3dhisto_map){
        ATH_MSG_VERBOSE("Found a matching histogram");
        TH1F* ihistogram = (TH1F*)im.first;
        std::string iname = ihistogram->GetName();
        if (hname == iname) {
          int ncells = histogram->GetNcells();
          for (int icell = 0; icell<ncells; icell++) {
            hm.second[icell].resizeVectors(m_maxnlep_loose);
            hm.second[icell].add(im.second[icell], m_maxnlep_loose);
          }
        }
      }
    }
    
    ATH_MSG_VERBOSE("Added " << ievt);

  }
 
  
  ATH_MSG_VERBOSE("Merged totEvents is " << m_global_fitInfo.totEvents);
  for (int ilep = 0; ilep < m_maxnlep_loose; ilep++) {
    ATH_MSG_VERBOSE("Merged  event count is " << m_global_fitInfo.eventCount[ilep]);
  }

  return StatusCode::SUCCESS;
}




