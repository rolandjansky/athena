/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//----------------------------------------------------------------------
//
//  This program provides a toy MC model of a set of pseudoexperiments.  It
//  is intended to explore the statistical properties of the fake lepton
//  background estimate for a given set of experiemental conditions (number
//  of events, number of leptons required, typical values of the real and
//  fake efficiencies, etc.)
//
//----------------------------------------------------------------------

#include <getopt.h>
#include "FakeBkgTools/FakeBkgInternals.h"
#include "FakeBkgTools/ApplyFakeFactor.h"
#include "FakeBkgTools/AsymptMatrixTool.h"
#include "FakeBkgTools/LhoodMM_tools.h"

#include "xAODEgamma/Electron.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TRandom3.h"
#include "TSystem.h"
#include <unistd.h>
#include <map>
#include <chrono>

struct fbtTestToyMC_config {
  unsigned nevents;
  unsigned ncases;
  unsigned minnbaseline;
  unsigned maxnbaseline;
  float realeff_mean;
  float fakeeff_mean;
  float eff_spread;
  float eff_delta_with_pt;
  std::string selection;
  std::string process;
  bool test_save;
  bool test_merge;
  std::string saveFileNameBase;
  std::string mergeFileNameBase;
  std::string outputdirname;
  bool verbose;
  bool test_histo;
  bool test_systematics;
  bool poisson_fluctuations;
};

using namespace FakeBkgTools;
using namespace std;

TH1F  *h_realeff_e, *h_fakeeff_e,  *h_realeff_mu, *h_fakeeff_mu;
TFile *rootEffFile;

void initialize(CP::BaseFakeBkgTool& tool, const std::vector<std::string>& input, const std::string& selection, const std::string& process, bool verbose);
void writeXML(const string& name, int type);
void writeROOT(const string& name, int type, float realeff_mean, float fakeeff_mean, float eff_spread, float eff_delta_with_pt);
void setupEfficiencies();
void lookupEfficiencies(xAOD::IParticle& lepton, ParticleData& lepton_data);

void parseArguments(int argc, char *argv[], fbtTestToyMC_config &config);
void setupSystBranches(const char* baseName, 
		       CP::SystematicVariation sysvar, 
		       float &weight,
		       float &weight_err,
		       std::map<CP::SystematicVariation, float> &syst_map, 
		       std::map<CP::SystematicVariation, float> &syst_err_map,
		       TTree* ntuple);

void setupSystBranchesAsym(const char* baseName, 
			   CP::SystematicVariation sysvar, 
			   float &weight,
			   float &weight_poserr,
			   float &weight_negerr,	   
			   std::map<CP::SystematicVariation, float> &syst_map, 
			   std::map<CP::SystematicVariation, float> &syst_poserr_map,
			   std::map<CP::SystematicVariation, float> &syst_negerr_map,
			   TTree* ntuple);

std::unique_ptr<TFile> openRootFile(fbtTestToyMC_config &config);

void  doMerge( std::vector<std::string> input, std::string name, fbtTestToyMC_config &config, TH1F* h_lep_pt, float &lep_pt, TH1F* h_lep_eta, float &lep_eta, TH2F* h_lep_pt_eta, float &fakes, float &poserr, float &negerr, int icase);
  
void Loop(fbtTestToyMC_config config);

double comboProb(vector<FakeBkgTools::ParticleData> leptons_data, std::bitset<64> tights, std::bitset<64> reals);

void usage();

double totWeight_std; 
double err_std;
double weight_lepfakes_2tight;
double weight_lepfakes_3tight;
double weight_lepfakes_2tight_1loose; 

bool fitFailed;

string rootEffFileName;

//additional variables that don't correspond to Tree branches

Double_t n_fake_lhood;

Double_t n_fake_lhood_tot_poserr;
Double_t n_fake_lhood_tot_negerr;

std::ofstream *f_stdneg_lhood_pos;
std::ofstream *f_stdpos_lhood_0;

const int nSave = 4; // number of subjobs to split into when testing saveProgress

int main(int argc, char *argv[]){

  fbtTestToyMC_config config;

  // defaults
  config.nevents = 100;
  config.ncases = 100;
  config.realeff_mean = 0.90;
  config.fakeeff_mean = 0.20;
  config.eff_spread = 0.10;
  config.eff_delta_with_pt = 0.;
  config.minnbaseline = 1;
  config.maxnbaseline = 4;
  config.test_save = false;
  config.saveFileNameBase = "saveProgress";
  config.mergeFileNameBase = "saveProgress";
  config.test_merge = false;
  config.test_histo = false;
  config.test_systematics = false;
  config.poisson_fluctuations = false;
  config.verbose = false;
  config.selection = ">=1T";
  config.process = ">=1F[T]";

  if (config.verbose) cout << "maxnbaseline = " << config.maxnbaseline << endl;

  parseArguments(argc, argv, config);

  Loop(config);

}

void Loop(fbtTestToyMC_config config)
{
   //Open an output file
  if (config.verbose) cout << "maxnbaseline = " << config.maxnbaseline << endl;

  std::unique_ptr<TFile> f_out = openRootFile(config);

  int nevents_thiscase;

  // create output ntuple
  TTree *ntuple = new TTree("FakeBkg", "Variables from toy MC");
  ntuple->Branch("nevents", &nevents_thiscase, "nevents/I");
  Int_t nevents_sel;
  ntuple->Branch("nevents_sel", &nevents_sel, "nevents_sel/I");
  Float_t fake_lep_frac;
  ntuple->Branch("fake_lep_frac", &fake_lep_frac, "fake_lep_frac/F");
  Float_t true_fakes;
  ntuple->Branch("true_fakes", &true_fakes,"true_fakes/F");
  Float_t asm_fakes;
  ntuple->Branch("asm_fakes", &asm_fakes,"asm_fakes/F");
  Float_t asm_err;
  ntuple->Branch("asm_err", &asm_err,"asm_err/F");
  Float_t fkf_fakes;
  ntuple->Branch("fkf_fakes", &fkf_fakes,"fkf_fakes/F");
  Float_t fkf_err;
  ntuple->Branch("fkf_err", &fkf_err,"fkf_err/F");
  Float_t lhoodMM_fakes;
  ntuple->Branch("lhoodMM_fakes", &lhoodMM_fakes,"lhoodMM_fakes/F");
  Float_t lhoodMM_poserr;
  ntuple->Branch("lhoodMM_poserr", &lhoodMM_poserr,"lhoodMM_poserr/F");
  Float_t lhoodMM_negerr;
  ntuple->Branch("lhoodMM_negerr", &lhoodMM_negerr,"lhoodMM_negerr/F");
  Float_t lhoodFF_fakes;
  ntuple->Branch("lhoodFF_fakes", &lhoodFF_fakes,"lhoodFF_fakes/F");
  Float_t lhoodFF_poserr;
  ntuple->Branch("lhoodFF_poserr", &lhoodFF_poserr,"lhoodFF_poserr/F");
  Float_t lhoodFF_negerr;
  ntuple->Branch("lhoodFF_negerr", &lhoodFF_negerr,"lhoodFF_negerr/F");
  

  map<CP::SystematicVariation,float> lhoodMM_weight_map, lhoodMM_poserr_map, lhoodMM_negerr_map;
  map<CP::SystematicVariation,float> lhoodFF_weight_map, lhoodFF_poserr_map, lhoodFF_negerr_map;
  map<CP::SystematicVariation,float> asm_weight_map, asm_err_map;
  map<CP::SystematicVariation,float> fkf_weight_map, fkf_err_map;

  TRandom3 rand(242868252);
    
  Double_t realeff_spread = config.eff_spread;
  Double_t fakeeff_spread = config.eff_spread;

  std::vector<bool> isTight;
  std::vector<std::string> input = { config.outputdirname+"efficiencies_full" };
  
  if(false) // switch to test either XML or ROOT input
    {
      // not implemented yet!
      //      input.back().append(".xml");
      //      writeXML(input.back(), 0);
    }
  else
    {
	input.back().append(".root");
	writeROOT(input.back(), 0, config.realeff_mean, config.fakeeff_mean, config.eff_spread, config.eff_delta_with_pt);
	rootEffFileName = input.back();
	setupEfficiencies();
    }
 

  for (unsigned icase = 0; icase < config.ncases; icase++) {
 
    nevents_sel = 0;

    TH1F* h_lep_pt_lhoodMM = 0;
    TH1F* h_lep_eta_lhoodMM = 0;
    TH2F* h_lep_pt_eta_lhoodMM = 0;
    TH1F* h_lep_pt_lhoodFF = 0;
    TH1F* h_lep_eta_lhoodFF = 0;
    TH2F* h_lep_pt_eta_lhoodFF = 0;
    TH1F* h_lep_pt_asm = 0;
    TH1F* h_lep_eta_asm = 0;
    TH2F* h_lep_pt_eta_asm = 0;
    TH1F* h_lep_pt_fkf = 0;
    TH1F* h_lep_eta_fkf = 0;
    TH2F* h_lep_pt_eta_fkf = 0;

    if (config.test_histo) {
      std::string hname = "lep_pt_lhoodMM_"+to_string(icase);
      h_lep_pt_lhoodMM =  new TH1F(hname.c_str(), hname.c_str(), 10, 0, 100);
      hname = "lep_eta_lhoodMM_"+to_string(icase);
      h_lep_eta_lhoodMM = new TH1F(hname.c_str(), hname.c_str(), 10, -5, 5);
      hname = "lep_pt_eta_lhoodMM_"+to_string(icase);
      h_lep_pt_eta_lhoodMM =  new TH2F(hname.c_str(), hname.c_str(), 10, 0, 100, 10, -5, 5);
      hname = "lep_pt_lhoodFF_"+to_string(icase);
      h_lep_pt_lhoodFF = new TH1F(hname.c_str(), hname.c_str(), 10, 0, 100);
      hname = "lep_eta_lhoodFF_"+to_string(icase);
      h_lep_eta_lhoodFF = new TH1F(hname.c_str(), hname.c_str(), 10, -5, 5);
      hname = "lep_pt_eta_lhoodFF_"+to_string(icase);
      h_lep_pt_eta_lhoodFF =  new TH2F(hname.c_str(), hname.c_str(), 10, 0, 100, 10, -5, 5);
      hname = "lep_pt_asm_"+to_string(icase);
      h_lep_pt_asm = new TH1F(hname.c_str(), hname.c_str(), 10, 0, 100);
      hname = "lep_eta_asm_"+to_string(icase);
      h_lep_eta_asm = new TH1F(hname.c_str(), hname.c_str(), 10, -5, 5);
      hname = "lep_pt_eta_asm_"+to_string(icase);
      h_lep_pt_eta_asm =  new TH2F(hname.c_str(), hname.c_str(), 10, 0, 100, 10, -5, 5);
      hname = "lep_pt_fkf_"+to_string(icase);
      h_lep_pt_fkf = new TH1F(hname.c_str(), hname.c_str(), 10, 0, 100);
      hname = "lep_eta_fkf_"+to_string(icase);
      h_lep_eta_fkf = new TH1F(hname.c_str(), hname.c_str(), 10, -5, 5);
      hname = "lep_pt_eta_fkf_"+to_string(icase);
      h_lep_pt_eta_fkf =  new TH2F(hname.c_str(), hname.c_str(), 10, 0, 100, 10, -5, 5);
    }

    float lep_pt, lep_eta;
  
    if (!config.test_merge) {

      cout << "Starting case " << icase << endl;
      CP::LhoodMM_tools lhmTool("LhoodMM_tools");
      // arrays of tool instances to test saveProgress
      std::vector<CP::LhoodMM_tools*> lhmTool_sav;
      CP::LhoodMM_tools lhmTool_FF("LhoodFF_tools");
      std::vector<CP::LhoodMM_tools*> lhmTool_FF_sav;
      CP::AsymptMatrixTool asmTool("AsymptMatrixTool");
      std::vector<CP::AsymptMatrixTool*> asmTool_sav;
      CP::ApplyFakeFactor fkfTool("fkfTool");
      std::vector<CP::ApplyFakeFactor*> fkfTool_sav;
      
      float asmYield = 0;
      float asmErr = 0;
      float fkfYield = 0;
      float fkfErr = 0;
      if (config.test_histo) {
	lhmTool.register1DHistogram(h_lep_pt_lhoodMM, &lep_pt);
	lhmTool.register1DHistogram(h_lep_eta_lhoodMM, &lep_eta);
	lhmTool.register2DHistogram(h_lep_pt_eta_lhoodMM, &lep_pt, &lep_eta);
	lhmTool_FF.register1DHistogram(h_lep_pt_lhoodFF, &lep_pt);
	lhmTool_FF.register1DHistogram(h_lep_eta_lhoodFF, &lep_eta);
	lhmTool_FF.register2DHistogram(h_lep_pt_eta_lhoodFF, &lep_pt, &lep_eta);
	fkfTool.register1DHistogram(h_lep_pt_fkf, &lep_pt);
	fkfTool.register1DHistogram(h_lep_eta_fkf, &lep_eta);
	fkfTool.register2DHistogram(h_lep_pt_eta_fkf, &lep_pt, &lep_eta);
	asmTool.register1DHistogram(h_lep_pt_asm, &lep_pt);
	asmTool.register1DHistogram(h_lep_eta_asm, &lep_eta);
	asmTool.register2DHistogram(h_lep_pt_eta_asm, &lep_pt, &lep_eta);
      }

      initialize(asmTool, input, config.selection, config.process, config.verbose);
      initialize(lhmTool, input, config.selection, config.process, config.verbose); 
      initialize(lhmTool_FF, input, config.selection, config.process, config.verbose); 
      lhmTool_FF.setProperty("DoFakeFactorFit", true);

      initialize(fkfTool, input, config.selection, config.process, config.verbose);

      if (config.test_save) {
	for (int iSave = 0; iSave <= nSave; iSave++) {
	  std:: string toolName = "LhoodMM_tools_save_" + std::to_string(icase) + "_" +  std::to_string(iSave);
	  CP::LhoodMM_tools *lhmTool_is = new CP::LhoodMM_tools(toolName.c_str());
	  lhmTool_sav.push_back(lhmTool_is);
	  initialize(*lhmTool_sav[iSave], input, config.selection, config.process, config.verbose);
	  if (config.test_histo) {
	    lhmTool_is->register1DHistogram(h_lep_pt_lhoodMM, &lep_pt);
	    lhmTool_is->register1DHistogram(h_lep_eta_lhoodMM, &lep_eta);
	    lhmTool_is->register2DHistogram(h_lep_pt_eta_lhoodMM, &lep_pt, &lep_eta);	
	  }
	  toolName = "LhoodMM_tools_FF_save_" + std::to_string(icase) + "_" + std::to_string(iSave);
	  CP::LhoodMM_tools *lhmTool_FF_is = new CP::LhoodMM_tools(toolName.c_str());
	  lhmTool_FF_sav.push_back(lhmTool_FF_is);
	  initialize(*lhmTool_FF_sav[iSave], input, config.selection, config.process, config.verbose);
	  lhmTool_FF_sav[iSave]->setProperty("DoFakeFactorFit", true);
	  if (config.test_histo) {
	    lhmTool_FF_is->register1DHistogram(h_lep_pt_lhoodFF, &lep_pt);
	    lhmTool_FF_is->register1DHistogram(h_lep_eta_lhoodFF, &lep_eta);
	    lhmTool_FF_is->register2DHistogram(h_lep_pt_eta_lhoodFF, &lep_pt, &lep_eta);	
	  }

	  toolName = "AsymptMatrixTool_save_" + std::to_string(icase) + "_" + std::to_string(iSave);
	  CP::AsymptMatrixTool *asmTool_is = new CP::AsymptMatrixTool(toolName.c_str()); if (config.test_histo) {
	    asmTool_is->register1DHistogram(h_lep_pt_asm, &lep_pt);
	    asmTool_is->register1DHistogram(h_lep_eta_asm, &lep_eta);
	    asmTool_is->register2DHistogram(h_lep_pt_eta_asm, &lep_pt, &lep_eta);	
	  }
	  asmTool_sav.push_back(asmTool_is);
	  initialize(*asmTool_sav[iSave], input, config.selection, config.process, config.verbose);
	  
	  toolName = "ApplyFakeFactor_save_" + std::to_string(icase) + "_" + std::to_string(iSave);
	  CP::ApplyFakeFactor *fkfTool_is = new CP::ApplyFakeFactor(toolName.c_str());
	  fkfTool_sav.push_back(fkfTool_is);
	  initialize(*fkfTool_sav[iSave], input, config.selection, config.process, config.verbose);
	  if (config.test_histo) {
	    fkfTool_is->register1DHistogram(h_lep_pt_fkf, &lep_pt);
	    fkfTool_is->register1DHistogram(h_lep_eta_fkf, &lep_eta);
	    fkfTool_is->register2DHistogram(h_lep_pt_eta_fkf, &lep_pt, &lep_eta);	
	  }
	}
      }


      Double_t realeff_mean_thiscase = rand.Gaus(config.realeff_mean, realeff_spread);
      if (realeff_mean_thiscase > 0.99) realeff_mean_thiscase = 0.99;
      Double_t fakeeff_mean_thiscase = rand.Gaus(config.fakeeff_mean, fakeeff_spread);
      if (fakeeff_mean_thiscase < 0.01) fakeeff_mean_thiscase = 0.01;
      if (fakeeff_mean_thiscase > realeff_mean_thiscase) fakeeff_mean_thiscase = realeff_mean_thiscase-0.01;
      
      fake_lep_frac = rand.Uniform();  // fraction of fake leptons in the loose sample
      
      totWeight_std = 0.;
      err_std = 0;
      n_fake_lhood = 0.;
      n_fake_lhood_tot_poserr = 0.;
      n_fake_lhood_tot_negerr = 0.;
      
      std::vector<LhoodMMEvent> mmevts;
      
      true_fakes = 0; 
 
      std::vector<double> realeff, fakeeff;
      
      vector<FinalState*> fs;
      FinalState* this_fs;
      for (unsigned ilep = 0; ilep <= config.maxnbaseline; ilep++) {
	string error;
	this_fs = new FinalState(0, ilep, config.selection, config.process, error);
	fs.push_back(this_fs);
      }
      
      if (config.poisson_fluctuations) {
	nevents_thiscase = rand.Poisson(config.nevents);
      } else {
	nevents_thiscase = config.nevents;
      }

      // need two passes to simulate subtraction of real lepton contribution
      // in fake factor method with a statistically-independent sample
      for (int pass =0; pass<2; pass++) {

	int savIndex = 0;

 
	int nEventsMultFactor = 1;
	if (pass > 0) nEventsMultFactor = 10;  // simulates higher-stats "MC" for removal of real contribution to fake factor result
	for (Long64_t ievt=0; ievt<nevents_thiscase*nEventsMultFactor;ievt++) {

	  float nlep_frac = 1./(config.maxnbaseline-config.minnbaseline+1);
	  Int_t nlep_select = config.minnbaseline+rand.Uniform()/nlep_frac;
 
	  Int_t nReal = 0;
	  Int_t nTight = 0;
	
	  float extraweight = 1.0;
	
	  realeff.clear();
	  fakeeff.clear();
	
	  xAOD::IParticleContainer leptons(SG::VIEW_ELEMENTS);
	  vector<FakeBkgTools::ParticleData> leptons_data;
	
	  vector<bool> lep_real;

	  // set up an ordered vector of lepton pts 
	  vector<float> lep_pts;
	  for (int ilep = 0; ilep < nlep_select; ilep++) {
	    lep_pts.push_back(100*rand.Uniform());
	  }
	  // sort in descending order
	  sort(lep_pts.begin(), lep_pts.end(), std::greater<float>());

	  for (int ilep = 0; ilep < nlep_select; ilep++) {
	    xAOD::Electron* lepton =  new xAOD::Electron;   // for toy MC the lepton flavor doesn't matter
	    // assign the lepton as real or fake
	    bool isReal;
	    if (rand.Uniform() > fake_lep_frac) {
	      isReal = true;
	      nReal++;
	    } else {
	      isReal = false;
	    }
	    
	    lep_real.push_back(isReal);
	    lepton->makePrivateStore();
	    lepton->setCharge(rand.Uniform() > 0.5 ? 1 : -1 );
	    
	    lep_pt = lep_pts[ilep];
	    lep_eta = -5. + 10*rand.Uniform();
	    lepton->setP4( 1000*lep_pt, lep_eta, 0, 0.511);
	    
	    FakeBkgTools::ParticleData lepton_data;
	    lookupEfficiencies(*lepton, lepton_data);
	    // decide if lepton is tight or not
	    if (isReal) {
	      if (rand.Uniform() < lepton_data.real_efficiency.nominal) {
		lepton->auxdata<char>("Tight") = true;
		nTight++;
	      } else {
		lepton->auxdata<char>("Tight") = false;
	      }
	    } else {
	      if (rand.Uniform() < lepton_data.fake_efficiency.nominal) {
		lepton->auxdata<char>("Tight") = true;
		nTight++;
	      } else {
		lepton->auxdata<char>("Tight") = false;
	      }
	    }
	    leptons.push_back(static_cast<xAOD::IParticle*>(lepton));
	    
	    leptons_data.push_back(lepton_data);
	  } 
	  
	  if (pass == 0) {
	    asmTool.addEvent(leptons, extraweight);
	    fkfTool.addEvent(leptons, extraweight);
	    lhmTool.addEvent(leptons, extraweight);
	    lhmTool_FF.addEvent(leptons, extraweight);
	    if (config.test_save) {
	      lhmTool_sav[savIndex]->addEvent(leptons, extraweight);
	      lhmTool_FF_sav[savIndex]->addEvent(leptons, extraweight);
	      asmTool_sav[savIndex]->addEvent(leptons, extraweight);
	      fkfTool_sav[savIndex]->addEvent(leptons, extraweight);
	    }
	    
	  }

	  bool all_real = true;
	  if (pass == 1) {
	    for (int ilep = 0; ilep < nlep_select; ilep++) {
	      if (!lep_real[ilep]) {
		all_real = false;
	      }
	    }
	  }

	  //	  fkfTool.addEvent(leptons, extraweight);
	  if (pass == 1 && all_real) {
	    fkfTool.addEvent(leptons, -extraweight/nEventsMultFactor);
	    // probably not the fully correct way to do it for the LhoodMM,
	    // but might be close enough
	    lhmTool_FF.addEvent(leptons, -extraweight/nEventsMultFactor);
	 
	    if (config.test_save) {
	      fkfTool_sav[savIndex]->addEvent(leptons, -extraweight/nEventsMultFactor);
	      lhmTool_FF_sav[savIndex]->addEvent(leptons, -extraweight/nEventsMultFactor);
	    }
	  }

	  // determine the expected number of fake lepton events
	  // start by looping over all possible number of tight leptons
	  vector<int> lep_indices;
	  std::bitset<64> tights, reals, charges;
	  for (int ilep = 0; ilep < nlep_select; ilep++) {
	    lep_indices.push_back(ilep);
	    if (lep_real[ilep]) reals.set(ilep);
	  }

	  if (pass == 0) {
	    for (long comb = 0; comb < (1<<nlep_select); ++comb) {
	      tights = std::bitset<64>(comb);
	      if (fs[nlep_select]->accept_selection(tights, charges)) {
		if (fs[nlep_select]->accept_process(nlep_select, reals, tights)) {
		  true_fakes += extraweight*comboProb(leptons_data, tights, reals);
		}
	      }
	    }
	    // now see how many events actually passed the required selection
	    tights.reset();
	    for (int ilep = 0; ilep < nlep_select; ilep++) {
	      if (leptons[ilep]->auxdata<char>("Tight")) tights.set(ilep);
	    }
	    if (fs[nlep_select]->accept_selection(tights,charges) ) {
	      nevents_sel += extraweight;
	    }
	 
	    

	    float wgt;
	    if(asmTool.getEventWeight(wgt, config.selection, config.process) != StatusCode::SUCCESS) { cout << "ERROR: AsymptMatrixTool::getEventWeight() failed\n"; exit(2); }
	    asmYield += wgt;
	    asmErr += wgt*wgt;

	    if(fkfTool.getEventWeight(wgt, config.selection, config.process) != StatusCode::SUCCESS) { cout << "ERROR: ApplyFakeFactor::getEventWeight() failed\n"; exit(2); }
	    fkfYield += wgt;
	    fkfErr += wgt*wgt;
	  } else {
	    // this is where the subtraction of the real contribution is simulated
	    if (all_real) {
	      float wgt;
	      if(fkfTool.getEventWeight(wgt, config.selection, config.process) != StatusCode::SUCCESS) { cout << "ERROR: ApplyFakeFactor::getEventWeight() failed\n"; exit(2); }
	      fkfYield -= wgt/nEventsMultFactor;
	      fkfErr += wgt*wgt/(nEventsMultFactor);
	    }
	  }
	
	  if (config.test_save) {
	    if (pass == 0) {
	      if (ievt > 0 &&  (((nSave*ievt)%nevents_thiscase ==0) || ievt == nevents_thiscase -1) ) {
		string saveFileName = config.saveFileNameBase;
		saveFileName+=  "_lhm_"+to_string(icase)+"_"+to_string(savIndex)+".root";
		std::unique_ptr<TFile> saveFile(TFile::Open(saveFileName.c_str(), "RECREATE"));
		lhmTool_sav[savIndex]->saveProgress(saveFile->mkdir("fakes"));
		saveFile->Close();

		saveFileName =  config.saveFileNameBase+"_asm_"+to_string(icase)+"_"+to_string(savIndex)+".root";
		saveFile = std::make_unique<TFile>(saveFileName.c_str(), "RECREATE");
		asmTool_sav[savIndex]->saveProgress(saveFile.get());
		saveFile->Close();

		savIndex++;
	      }
	    } else {
	      if (ievt > 0 &&  (((nSave*ievt)%(nevents_thiscase*nEventsMultFactor) ==0) || ievt == nevents_thiscase*nEventsMultFactor -1) ) {
		string saveFileName = config.saveFileNameBase;

		saveFileName =  config.saveFileNameBase+"_lhf_"+to_string(icase)+"_"+to_string(savIndex)+".root";
		std::unique_ptr<TFile> saveFile(TFile::Open(saveFileName.c_str(), "RECREATE"));
		lhmTool_FF_sav[savIndex]->saveProgress(saveFile->mkdir("fakes"));
		saveFile->Close();

		saveFileName =  config.saveFileNameBase+"_fkf_"+to_string(icase)+"_"+to_string(savIndex)+".root";
		saveFile = std::make_unique<TFile>(saveFileName.c_str(), "RECREATE");
		fkfTool_sav[savIndex]->saveProgress(saveFile.get());
		saveFile->Close();
		savIndex++;

		float nfakes_tmp, err_tmp;
		fkfTool.getTotalYield(nfakes_tmp, err_tmp, err_tmp);
	      }
	    }
	  }

	  for (xAOD::IParticleContainer::iterator it = leptons.begin(); it != leptons.end(); it++) {
	    if (*it != nullptr) delete *it;
	  }
	  leptons.clear();
	}
      }
      asm_err = sqrt(asmErr);
      asm_fakes = asmYield;
      if (asmTool.getTotalYield(asmYield, asmErr, asmErr) !=  StatusCode::SUCCESS) { cout << "ERROR: AsymptMatrixTool::getTotalYield() failed\n"; exit(2); }


      if(lhmTool.getTotalYield(lhoodMM_fakes, lhoodMM_poserr, lhoodMM_negerr) != StatusCode::SUCCESS) { cout << "ERROR: LhoodMM_tools::getTotalYield() failed\n"; exit(2); }

      if(lhmTool_FF.getTotalYield(lhoodFF_fakes, lhoodFF_poserr, lhoodFF_negerr) != StatusCode::SUCCESS) { cout << "ERROR: LhoodMM_tools::getTotalYield() failed\n"; exit(2); }

      if (fkfTool.getTotalYield(fkfYield, fkfErr, fkfErr) !=  StatusCode::SUCCESS) { cout << "ERROR: AsymptMatrixTool::getTotalYield() failed\n"; exit(2); }
      fkf_err = fkfErr;
      fkf_fakes = fkfYield;          
      
      if (config.test_systematics) {
	auto sysvars = asmTool.affectingSystematics();
	std::string systBrName, systBrNameF;

	for(auto& sysvar : sysvars)
	  {

	    if (config.verbose) asmTool.getSystDescriptor().printUncertaintyDescription(sysvar);

	    float asm_syst_weight, asm_syst_err;
	    float fkf_syst_weight, fkf_syst_err;
	    float lhoodMM_syst_weight, lhoodMM_syst_poserr, lhoodMM_syst_negerr;
	    float lhoodFF_syst_weight, lhoodFF_syst_poserr, lhoodFF_syst_negerr;
 
	    if (icase == 0) {
	      setupSystBranches("asm", sysvar, asm_syst_weight, asm_syst_err, asm_weight_map, asm_err_map, ntuple);
	      setupSystBranches("fkf", sysvar, fkf_syst_weight, fkf_syst_err, fkf_weight_map, fkf_err_map, ntuple);
	      setupSystBranchesAsym("lhoodMM", sysvar, lhoodMM_syst_weight, lhoodMM_syst_poserr, lhoodMM_syst_negerr, lhoodMM_weight_map, lhoodMM_poserr_map, lhoodMM_negerr_map, ntuple);
	      setupSystBranchesAsym("lhoodFF", sysvar, lhoodFF_syst_weight, lhoodFF_syst_poserr, lhoodFF_syst_negerr, lhoodFF_weight_map, lhoodFF_poserr_map, lhoodFF_negerr_map, ntuple);

	    }
	    asmTool.applySystematicVariation({sysvar});
	    asmTool.getTotalYield(asm_weight_map.find(sysvar)->second, 
				  asm_err_map.find(sysvar)->second,
				  asm_err_map.find(sysvar)->second);

	    fkfTool.applySystematicVariation({sysvar});
	    fkfTool.getTotalYield(fkf_weight_map.find(sysvar)->second, 
				  fkf_err_map.find(sysvar)->second,
				  fkf_err_map.find(sysvar)->second);
	    lhmTool.applySystematicVariation({sysvar});
	    lhmTool.getTotalYield(lhoodMM_weight_map.find(sysvar)->second, 
				  lhoodMM_poserr_map.find(sysvar)->second,
				  lhoodMM_negerr_map.find(sysvar)->second);
	    lhmTool_FF.applySystematicVariation({sysvar});
	    lhmTool_FF.getTotalYield(lhoodFF_weight_map.find(sysvar)->second, 
				  lhoodFF_poserr_map.find(sysvar)->second,
				  lhoodFF_negerr_map.find(sysvar)->second);
	  }
      }
    
      
      cout << "OUTPUT true_fakes = " << true_fakes << endl;
      cout << "OUTPUT nfakes for lhoodMM = " << lhoodMM_fakes << " +" << lhoodMM_poserr << " -" <<  lhoodMM_negerr <<  endl;
      cout << "OUTPUT nfakes for lhoodFF = " << lhoodFF_fakes << " +" << lhoodFF_poserr << " -" <<  lhoodFF_negerr <<  endl;
      cout << "OUTPUT nfakes for asm = " << asm_fakes << " +- " << asm_err <<  endl;
      cout << "OUTPUT nfakes for fkf = " << fkf_fakes << " +- " << fkf_err <<  endl;
      cout << "OUTPUT true_fakes = " << true_fakes << endl;
    } else {

      doMerge(input, "lhm", config, h_lep_pt_lhoodMM, lep_pt, h_lep_eta_lhoodMM, lep_eta, h_lep_pt_eta_lhoodMM, lhoodMM_fakes, lhoodMM_poserr, lhoodMM_negerr, icase);
      doMerge(input, "lhf", config, h_lep_pt_lhoodFF, lep_pt, h_lep_eta_lhoodFF, lep_eta, h_lep_pt_eta_lhoodFF, lhoodFF_fakes, lhoodFF_poserr, lhoodFF_negerr, icase);
      doMerge(input, "asm", config, h_lep_pt_asm, lep_pt, h_lep_eta_asm, lep_eta, h_lep_pt_eta_asm, asm_fakes, asm_err, asm_err, icase);
      doMerge(input, "fkf", config, h_lep_pt_fkf, lep_pt, h_lep_eta_fkf, lep_eta, h_lep_pt_eta_fkf, fkf_fakes, fkf_err, fkf_err, icase);

    }
     
    ntuple->Fill();

    f_out->cd();
    if (config.test_histo) {
      h_lep_pt_lhoodMM->Write();
      h_lep_eta_lhoodMM->Write();
      h_lep_pt_eta_lhoodMM->Write();
      h_lep_pt_lhoodFF->Write();
      h_lep_eta_lhoodFF->Write();
      h_lep_pt_eta_lhoodFF->Write();
      h_lep_pt_asm->Write();
      h_lep_eta_asm->Write();
      h_lep_pt_eta_asm->Write();
      h_lep_pt_fkf->Write();
      h_lep_eta_fkf->Write();
      h_lep_pt_eta_fkf->Write();
      
      delete h_lep_pt_lhoodMM;
      delete h_lep_eta_lhoodMM;
      delete h_lep_pt_eta_lhoodMM;
      delete h_lep_pt_lhoodFF;
      delete h_lep_eta_lhoodFF;
      delete h_lep_pt_eta_lhoodFF;
      delete h_lep_pt_asm;
      delete h_lep_eta_asm;
      delete h_lep_pt_eta_asm;
      delete h_lep_pt_fkf;
      delete h_lep_eta_fkf;
      delete h_lep_pt_eta_fkf;


    }
  }

  f_out->Write();
  f_out->Close();

}

void writeROOT(const string& name, int type, float realeff_mean, float fakeeff_mean, float eff_spread, float eff_delta_with_pt)
{
  TRandom3 rnd(235789);

  int nbin = 100;

  std::unique_ptr<TFile> file(TFile::Open(name.c_str(), "RECREATE"));

  if(type == 0)
    {
      TH1D hElFake("FakeEfficiency_el_pt","FakeEfficiency_el_pt", nbin, 0., 1.*nbin);
      TH1D hMuFake("FakeEfficiency_mu_pt","FakeEfficiency_mu_pt", nbin, 0., 1.*nbin);
      TH1D hElReal("RealEfficiency_el_pt","RealEfficiency_el_pt", nbin, 0., 1.*nbin);
      TH1D hMuReal("RealEfficiency_mu_pt","RealEfficiency_mu_pt", nbin, 0., 1.*nbin);

      TH1D hElFake_bigSyst("FakeEfficiency_el_pt__bigSyst","FakeEfficiency_el_pt__bigSyst", nbin, 0., 1.*nbin);
      TH1D hElFake_smallSyst("FakeEfficiency_el_pt__smallSyst","FakeEfficiency_el_pt__smallSyst", nbin, 0., 1.*nbin);
      TH1D hMuFake_bigSyst("FakeEfficiency_mu_pt__bigSyst","FakeEfficiency_mu_pt__bigSyst", nbin, 0., 1.*nbin);
      TH1D hMuFake_smallSyst("FakeEfficiency_mu_pt__smallSyst","FakeEfficiency_mu_pt__smallSyst", nbin, 0., 1.*nbin);
      TH1D hElReal_bigSyst("RealEfficiency_el_pt__bigSyst","RealEfficiency_el_pt__bigSyst", nbin, 0., 1.*nbin);
      TH1D hMuReal_bigSyst("RealEfficiency_mu_pt__bigSyst","RealEfficiency_mu_pt__bigSyst", nbin, 0., 1.*nbin);

      for (int ibin = 1; ibin <= nbin; ibin++) {
	Double_t realeff = TMath::Min(rnd.Gaus(realeff_mean, eff_spread)+eff_delta_with_pt*(ibin-nbin/2), 0.99);
	Double_t fakeeff = TMath::Max(rnd.Gaus(fakeeff_mean, eff_spread)-eff_delta_with_pt*(ibin-nbin/2), 0.01);
    
	float minrfdiff = 0.10;
	if (realeff - fakeeff < minrfdiff) {
	  if (realeff > minrfdiff) {
	    fakeeff = realeff - minrfdiff;
	  } else {
	    realeff = realeff + 0.10;
	  }
	}
	
	// sanity checks on the efficiencies
	if (realeff < 0 || realeff > 1.) cout << "ERROR: Bad real efficiency value: " << realeff << endl;
        if (fakeeff < 0 || fakeeff > 1.) cout << "ERROR: Bad fake efficiency value: " << fakeeff << endl;
	if ((realeff - fakeeff) < minrfdiff) cout << "ERROR: Too small difference between real and fake efficiencies: " << realeff << " " << fakeeff << endl;
 
	hElFake.SetBinContent(ibin, fakeeff);
	hElFake.SetBinError(ibin, eff_spread);
	//hElFake.SetBinError(ibin, 0.);
	hMuFake.SetBinContent(ibin, fakeeff);
	hMuFake.SetBinError(ibin, eff_spread);
	//hMuFake.SetBinError(ibin, 0.);
	hElReal.SetBinContent(ibin, realeff);
       	hElReal.SetBinError(ibin, eff_spread);
	//hElReal.SetBinError(ibin, 0.);
	hMuReal.SetBinContent(ibin, realeff);
	hMuReal.SetBinError(ibin, eff_spread);
	//hMuReal.SetBinError(ibin, 0.);

	hElFake_bigSyst.SetBinContent(ibin, 0.20*hElFake.GetBinContent(ibin));
	hElFake_smallSyst.SetBinContent(ibin, 0.02*hElFake.GetBinContent(ibin));
	hMuFake_bigSyst.SetBinContent(ibin, 0.20*hMuFake.GetBinContent(ibin));
	hMuFake_smallSyst.SetBinContent(ibin, 0.02*hMuFake.GetBinContent(ibin));
	hElReal_bigSyst.SetBinContent(ibin, 0.20*hElReal.GetBinContent(ibin));
	hMuReal_bigSyst.SetBinContent(ibin, 0.20*hMuReal.GetBinContent(ibin));

      }

      file->cd();
      hElFake.Write();
      hElReal.Write();
      hMuFake.Write();
      hMuReal.Write();
      hElFake_bigSyst.Write();
      hElReal_bigSyst.Write(); 
      hElFake_smallSyst.Write();
      hMuFake_bigSyst.Write();
      hMuFake_smallSyst.Write();
      hMuReal_bigSyst.Write();
    }
  file->Close();
}

void parseArguments(int argc, char *argv[], fbtTestToyMC_config &config) {
  static struct option long_options[] = 
    {
      {"ncases",  required_argument, 0, 'c'},
      {"nevents", required_argument, 0, 'e'},
      {"minnb", required_argument, 0, 'm'},
      {"maxnb", required_argument, 0, 'n'},
      {"rmean", required_argument, 0, 'r'},
      {"fmean", required_argument, 0, 'f'},
      {"effsigma", required_argument, 0, 's'},
      {"effdeltawithpt", required_argument, 0, 'd'},
      {"sel", required_argument, 0, 'l'},
      {"proc", required_argument, 0, 'p'},
      {"test_save", no_argument, 0, 'S'},
      {"test_merge", required_argument, 0, 'M'},
      {"test_histo", required_argument, 0, 'H'},
      {"test_systematics", no_argument, 0, 'E'},
      {"verbose", no_argument, 0, 'v'},
      {"poisson", no_argument, 0, 'P'},
      {"help", no_argument, 0, 'h'},
      {0,0,0,0}
    };

  char c;
  //  int optind;
  int longindex = 0;

  while ((c = getopt_long(argc, argv, "c:e:m:n:r:f:s:d:l:p:S:M:HEPvh", long_options, &longindex)) != -1) {
    switch(c) {
    case 'c':
      config.ncases = atoi(optarg);
      break;
    case 'e':
      config.nevents = atoi(optarg);
      break;
    case 'r':
      config.realeff_mean = atof(optarg);
      break;
    case 'f':
      config.fakeeff_mean = atof(optarg);
      break;
    case 's':
      config.eff_spread = atof(optarg);
      break;
    case 'd':
      config.eff_delta_with_pt = atof(optarg);
      break;
    case 'l':
      config.selection = optarg;
      break;
    case 'p':
      config.process = optarg;
      break;
    case 'm':
      config.minnbaseline = atoi(optarg);
      break;
    case 'n':
      config.maxnbaseline = atoi(optarg);
      break;
    case 'S':
      config.test_save = true;
      config.saveFileNameBase = optarg;
      break;
    case 'M':
      config.test_merge = true;
      config.mergeFileNameBase = optarg;
      break;
    case 'H':
      config.test_histo = true;
      break;
    case 'E':
      config.test_systematics = true;
      break;
    case 'v':
      config.verbose = true;
      break;
    case 'P':
      config.poisson_fluctuations = true;
      break;
    case 'h':
      usage();
      exit(0);
    case '?':
      usage();
      abort();
    default:
      abort();
    }
  }
  
  // remove any \ from selection and process strings 
  size_t pos;

  pos = config.selection.find("\"");
  while (pos != string::npos) {
    config.selection.replace(pos, 1, "");
    pos = config.selection.find("\"");
  }
  pos = config.process.find("\"");
  while (pos != string::npos) {
    config.process.replace(pos, 1, "");
    pos = config.process.find("\"");
  }
}

std::unique_ptr<TFile> openRootFile(fbtTestToyMC_config &config) {
  string outputdirname;
  string rootfilename;
  outputdirname = "FakeBkgTools_toy_MC_nevents_";
  outputdirname += to_string(config.nevents);
  outputdirname += "_ncases_";
  outputdirname += to_string(config.ncases);
  outputdirname += "_minnbaseline_";
  outputdirname += to_string(config.minnbaseline);
  outputdirname += "_maxnbaseline_";
  outputdirname += to_string(config.maxnbaseline);
  outputdirname += "_realeff_";
  outputdirname += to_string(config.realeff_mean);
  outputdirname += "_fakeeff_";
  outputdirname += to_string(config.fakeeff_mean);
  outputdirname += "_effspread_";
  outputdirname += to_string(config.eff_spread);
  outputdirname += "_selection_";
  outputdirname += config.selection;
  outputdirname += "_process_";
  outputdirname += config.process;
  if (config.poisson_fluctuations) {
    outputdirname += "_poisson_";
  }

  //now replace things like >, < from selection and process strings to get a 
  //legal directory name
  size_t pos;
  pos = outputdirname.find(">=");
  while (pos != string::npos) {
    outputdirname.replace(pos, 2, "ge");
    pos = outputdirname.find(">=");
  }
  pos = outputdirname.find("<=");
  while (pos != string::npos) {
    outputdirname.replace(pos, 2, "le");
    pos = outputdirname.find("<=");
  }
  pos = outputdirname.find(">");
  while (pos != string::npos) {
    outputdirname.replace(pos, 1, "gt");
    pos = outputdirname.find(">");
  }
  pos = outputdirname.find("<");
  while (pos != string::npos) {
    outputdirname.replace(pos, 1, "lt");
    pos = outputdirname.find("<");
  }
  pos = outputdirname.find("=");
  while (pos != string::npos) {
    outputdirname.replace(pos, 1, "eq");
    pos = outputdirname.find("=");
  }
  pos = outputdirname.find(",");
  while (pos != string::npos) {
    outputdirname.replace(pos, 1, "");
    pos = outputdirname.find(",");
  }
  pos = outputdirname.find("[");
  while (pos != string::npos) {
    outputdirname.replace(pos, 1, "");
    pos = outputdirname.find("[");
  }
  pos = outputdirname.find("]");
  while (pos != string::npos) {
    outputdirname.replace(pos, 1, "");
    pos = outputdirname.find("]");
  }
  pos = outputdirname.find("\"");
  while (pos != string::npos) {
    outputdirname.replace(pos, 1, "");
    pos = outputdirname.find("\"");
  }
  pos = outputdirname.find("\\");
  while (pos != string::npos) {
    outputdirname.replace(pos, 1, "");
    pos = outputdirname.find("\\");
  }

  gSystem->mkdir(outputdirname.c_str());
  rootfilename = outputdirname+"/output.root";
  
  config.outputdirname = outputdirname;

  std::unique_ptr<TFile> f_out(TFile::Open(rootfilename.c_str(),"RECREATE"));

  return f_out;
}

void initialize(CP::BaseFakeBkgTool& tool, const std::vector<std::string>& input, const std::string& selection, const std::string& process, bool verbose)
{
    tool.setProperty("InputFiles", input);
    tool.setProperty("EnergyUnit", "GeV");
    tool.setProperty("Selection", selection);
    tool.setProperty("Process", process);
	if (verbose) {
	  tool.setProperty("OutputLevel", MSG::VERBOSE);
	} else {
	  tool.setProperty("OutputLevel", MSG::INFO);
	}
        tool.setProperty("ConvertWhenMissing", true);
        if(tool.initialize() != StatusCode::SUCCESS)
        {
                cout << "ERROR: unable to initialize tool " << endl;;
                exit(1);
        }
}

void setupEfficiencies() {
  rootEffFile =  new TFile(rootEffFileName.c_str());
  if (rootEffFile == 0) {
    cout << "Um, no ROOT file!" << endl;
  }

  h_realeff_e = (TH1F*)rootEffFile->Get("RealEfficiency_el_pt");
  h_fakeeff_e = (TH1F*)rootEffFile->Get("FakeEfficiency_el_pt");
  h_realeff_mu = (TH1F*)rootEffFile->Get("RealEfficiency_mu_pt");
  h_fakeeff_mu = (TH1F*)rootEffFile->Get("FakeEfficiency_mu_pt");    

  h_realeff_e->SetDirectory(0);
  h_fakeeff_e->SetDirectory(0);
  h_realeff_mu->SetDirectory(0);
  h_fakeeff_mu->SetDirectory(0);

  if (h_realeff_e == 0) cout << "No real e" << endl;
  if (h_fakeeff_e == 0) cout << "No fake e" << endl;
  if (h_realeff_mu == 0) cout << "No real mu" << endl;
  if (h_fakeeff_mu == 0) cout << "No fake mu" << endl;

  rootEffFile->Close();
  delete rootEffFile;
}

void lookupEfficiencies(xAOD::IParticle& lepton, FakeBkgTools::ParticleData& lepton_data) {

  if (h_realeff_e == 0) cout << "No real e" << endl;
  if (h_fakeeff_e == 0) cout << "No fake e" << endl;
  if (h_realeff_mu == 0) cout << "No real mu" << endl;
  if (h_fakeeff_mu == 0) cout << "No fake mu" << endl;
  if (lepton.type() == xAOD::Type::Electron) {
    lepton_data.real_efficiency.nominal = h_realeff_e->GetBinContent(h_realeff_e->FindBin(lepton.pt()/1000.));
    lepton_data.fake_efficiency.nominal = h_fakeeff_e->GetBinContent(h_fakeeff_e->FindBin(lepton.pt()/1000.));
  } else {
    lepton_data.real_efficiency.nominal = h_realeff_mu->GetBinContent(h_realeff_mu->FindBin(lepton.pt()/1000.));
    lepton_data.fake_efficiency.nominal = h_fakeeff_mu->GetBinContent(h_fakeeff_mu->FindBin(lepton.pt()/1000.));
  }
  
}

double comboProb(vector<FakeBkgTools::ParticleData> leptons_data, std::bitset<64> tights, std::bitset<64> reals) {

  double prob = 1.;
  for (unsigned ilep = 0; ilep < leptons_data.size(); ilep++) {
    if (tights[ilep]) {
      if (reals[ilep]) {
    prob *= leptons_data[ilep].real_efficiency.nominal;
      } else {
    prob *= leptons_data[ilep].fake_efficiency.nominal;
      }
    } else {
       if (reals[ilep]) {
     prob *= (1.-leptons_data[ilep].real_efficiency.nominal);
       } else {
     prob *= (1.-leptons_data[ilep].fake_efficiency.nominal);
       }
    }
  }

  return prob;
}

double comboProb_FF(vector<FakeBkgTools::ParticleData> leptons_data, std::bitset<64> tights, std::bitset<64> reals) {

  // like comboProb, but with real efficiencies set to 1.  
  double prob = 1.;
  for (unsigned ilep = 0; ilep < leptons_data.size(); ilep++) {
    if (tights[ilep]) {
      if (reals[ilep]) {
      } else {
	prob *= leptons_data[ilep].fake_efficiency.nominal;
      }
    } else {
      if (reals[ilep]) {
	prob *= 0; 
      } else {
	prob *= (1.-leptons_data[ilep].fake_efficiency.nominal);
      }
    }
  }
  for (unsigned ilep = 0; ilep < leptons_data.size(); ilep++) {
    if (reals[ilep]) {
      float F = leptons_data[ilep].fake_efficiency.nominal/(1-leptons_data[ilep].fake_efficiency.nominal);
      prob += F;
    }
  }
  return prob;
}

void setupSystBranches(const char* baseName, 
		       CP::SystematicVariation sysvar, 
		       float &weight,
		       float &weight_err,
		       std::map<CP::SystematicVariation, float> &syst_map, 
		       std::map<CP::SystematicVariation, float> &syst_err_map,
		       TTree* ntuple) {
  syst_map.emplace(std::make_pair(sysvar, weight));
  std::string systBrName = baseName;
  systBrName = systBrName+"_fakes_"+sysvar.name();
  std::string systBrNameF = systBrName+"/F";
  ntuple->Branch(systBrName.c_str(), &(syst_map.find(sysvar)->second), systBrNameF.c_str());
  syst_err_map.emplace(std::make_pair(sysvar, weight_err));
  systBrName = baseName;
  systBrName = systBrName+"_err_"+sysvar.name();
  systBrNameF = systBrName+"/F";
  ntuple->Branch(systBrName.c_str(), &(syst_err_map.find(sysvar)->second), systBrNameF.c_str());
}
  
void setupSystBranchesAsym(const char* baseName, 
			   CP::SystematicVariation sysvar, 
			   float &weight,
			   float &weight_poserr,
			   float &weight_negerr,
			   std::map<CP::SystematicVariation, float> &syst_map, 
			   std::map<CP::SystematicVariation, float> &syst_poserr_map,
			   std::map<CP::SystematicVariation, float> &syst_negerr_map,
			   TTree* ntuple) {
  syst_map.emplace(std::make_pair(sysvar, weight));
  std::string systBrName = baseName;
  systBrName = systBrName+"_fakes_"+sysvar.name();
  std::string systBrNameF = systBrName+"/F";
  ntuple->Branch(systBrName.c_str(), &(syst_map.find(sysvar)->second), systBrNameF.c_str());
  syst_poserr_map.emplace(std::make_pair(sysvar, weight_poserr));
  systBrName = baseName;
  systBrName = systBrName+"_poserr_"+sysvar.name();
  systBrNameF = systBrName+"/F";
  ntuple->Branch(systBrName.c_str(), &(syst_poserr_map.find(sysvar)->second), systBrNameF.c_str());
  syst_negerr_map.emplace(std::make_pair(sysvar, weight_negerr));
  systBrName = baseName;
  systBrName = systBrName+"_negerr_"+sysvar.name();
  systBrNameF = systBrName+"/F";
  ntuple->Branch(systBrName.c_str(), &(syst_negerr_map.find(sysvar)->second), systBrNameF.c_str());
}

void  doMerge( std::vector<std::string> input, std::string name, fbtTestToyMC_config &config, TH1F* h_lep_pt, float &lep_pt, TH1F* h_lep_eta, float &lep_eta, TH2F* h_lep_pt_eta, float &fakes, float &poserr, float &negerr, int icase) {  

  std::string haddcmd = "hadd -f "+config.mergeFileNameBase+"_"+name+"_"+to_string(icase)+".root "+config.mergeFileNameBase+"_"+name+"_"+to_string(icase)+"_*.root";
      system(haddcmd.c_str());
      
      std::unique_ptr<CP::BaseFakeBkgTool> tool;
      
      if (name == "lhm" || name == "lhf") {
	std::string toolName = "Lhood";
	if (name == "lhm") {
	  toolName += "MM";
	} else {
	  toolName += "FF";
	}
	toolName += to_string(icase)+"_tools_merge";
	tool = std::make_unique<CP::LhoodMM_tools>(toolName);
	if (name == "lhf") {
	  tool->setProperty("DoFakeFactorFit", true);
	}
	tool->setProperty("ProgressFileDirectory", "fakes");
      } else if (name == "asm") {
	tool = std::make_unique<CP::AsymptMatrixTool>("asm_tool_merge");
      } else if (name == "fkf") {
	tool = std::make_unique<CP::ApplyFakeFactor>("fkf_tool_merge");
      }

      std::string mergeFileName =  config.mergeFileNameBase+"_"+name+"_"+to_string(icase)+".root";
      std::cout << mergeFileName << std::endl;
      tool->setProperty("ProgressFileName", mergeFileName);
      initialize(*tool, input, config.selection, config.process, config.verbose);
      if (config.test_histo) {
	tool->register1DHistogram(h_lep_pt, &lep_pt);
	tool->register1DHistogram(h_lep_eta, &lep_eta);	
	tool->register2DHistogram(h_lep_pt_eta, &lep_pt, &lep_eta);
      }

      if(tool->getTotalYield(fakes, poserr, negerr) != StatusCode::SUCCESS) { cout << "ERROR: merged getTotalYield() failed\n"; exit(2); }
      //      cout << "merged lhm nfakes = " << lhoodMM_fakes << " + " << lhoodMM_poserr << " " <<  lhoodMM_negerr << endl;
  
}

void usage() {

  std::cout << std::endl << "fbtTestToyMC provides a toy MC model of a set of pseudoexperiments.  It is intended to explore the statistical properties of the fake lepton background estimate for a given set of experiemental conditions (number of events, number of leptons required, typical values of the real and fake efficiencies, etc.)" << std::endl << std::endl;
  std::cout << "Options: " << std::endl;
  std::cout << "   --ncases, -c:  number of pseudoexperiments to run (default: 100)" << std::endl;
  std::cout << "   --nevents, -e:  number of events in each pseudoexperiment (default: 100)" << std::endl;
  std::cout << "   --poisson, -P:  use Poisson-distributed number of events in each pseudoexperiment, rather than a fixed number (default: false)" << std::endl;
  std::cout << "   --minnb, -m:  minimum number of baseline leptons per event (default: 1)" << std::endl;
  std::cout << "   --maxnb, -m:  maximum number of baseline leptons per event (default: 4)" << std::endl;
  std::cout << "   --rmean, -r: average real lepton efficiency (default: 0.9)" << std::endl;
  std::cout << "   --fmean, -f: average fake lepton efficiency (default: 0.2)" << std::endl;
  std::cout << "   --effsigma, -s: standard deviation for lepton-to-lepton variation in real and fake efficiencies (default: 0.10)" << std::endl;
  std::cout << "   --effdeltawithpt, -d: rate at which the real (fake) efficiency increases (decreases) with simulated pt.  Note that the simulated pt range is 0 to 100" << std::endl;
  std::cout << "   --sel, -l: selection string to be used by the tools (default \" >= 1T\") " << std::endl;
  std::cout << "   --proc, -p: process string to be used by the tools (default \" >= 1F[T]\") " << std::endl;
  std::cout << "   --test_save, -S: save output from subjobs in each pseudoexperiement.  If set, requires an argument to specify the base name of the root files where the output will be saved. (default: false) " << std::endl;
  std::cout << "   --test_merge, -M: merge output from subjobs in each pseudoexperiement.  If set, requires an argument to specify the base name of the root files to be merged.  This should match the base name used in a previous run with the --test_save option (default: false) " << std::endl;
  std::cout << "   --test_histo, -H: test the filling of 1- and 2-dimensional histograms (default: false) " << std::endl;
  std::cout << "   --test_systematics, -E: test the handling of systematic unceratinties (default: false) " << std::endl;
  std::cout << "   --verbose, -v: enable verbose output (default: false) " << std::endl;
  std::cout << "   --help, -h: print this help message" << std::endl;
}
