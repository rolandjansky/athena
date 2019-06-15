/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//----------------------------------------------------------------------
//
//  This program provides a toy MC model of a set of pseudoexperiments.  It
//  is intended to explore the statistical properties of the fake lepton
//  background estimate for a given set of experiemental conditions (number
//  of events, number of leptons required, typical values of the real and
//  fake efficiencies, etc.
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
#include <chrono>

using namespace FakeBkgTools;
using namespace std;

TH1F  *h_realeff_e, *h_fakeeff_e,  *h_realeff_mu, *h_fakeeff_mu;
TFile *rootEffFile;

void initialize(CP::BaseFakeBkgTool& tool, const std::vector<std::string>& input, const std::string& selection, const std::string& process, bool verbose);
void writeXML(const string& name, int type);
void writeROOT(const string& name, int type, float realeff_mean, float fakeeff_mean, float eff_spread);
void setupEfficiencies();
void lookupEfficiencies(xAOD::IParticle& lepton, ParticleData& lepton_data);

void Loop(unsigned nevents, unsigned ncases, unsigned minnbaseline, unsigned maxnbaseline, float realeff_mean, float fakeeff_mean, float eff_spread, std::string selection, std::string process, bool test_save, bool test_merge, string saveFaileName, string mergeFileName, bool verbose, bool test_histo, bool test_systematics);

double comboProb(vector<FakeBkgTools::ParticleData> leptons_data, std::bitset<64> tights, std::bitset<64> reals);

double comboProb_FF(vector<FakeBkgTools::ParticleData> leptons_data, std::bitset<64> tights, std::bitset<64> reals);

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

int main(int argc, char *argv[]){

  unsigned nevents, ncases, minnbaseline, maxnbaseline;
  float realeff_mean, fakeeff_mean, eff_spread;
  bool test_save, test_merge, test_histo, test_systematics, verbose;
  std::string saveFileNameBase, mergeFileNameBase;

  // defaults
  nevents = 100;
  ncases = 100;
  realeff_mean = 0.90;
  fakeeff_mean = 0.20;
  eff_spread = 0.10;
  minnbaseline = 1;
  maxnbaseline = 4;
  test_save = false;
  saveFileNameBase = "saveProgress";
  mergeFileNameBase = "saveProgress";
  test_merge = false;
  test_histo = false;
  test_systematics = false;
  verbose = false;
  
  if (verbose) cout << "maxnbaseline = " << maxnbaseline << endl;

  string selection(">=1T"), process(">=1F[T]");

  static struct option long_options[] = 
    {
      {"ncases",  required_argument, 0, 'c'},
      {"nevents", required_argument, 0, 'e'},
      {"minnb", required_argument, 0, 'm'},
      {"maxnb", required_argument, 0, 'n'},
      {"rmean", required_argument, 0, 'r'},
      {"fmean", required_argument, 0, 'f'},
      {"effsigma", required_argument, 0, 's'},
      {"sel", required_argument, 0, 'l'},
      {"proc", required_argument, 0, 'p'},
      {"test_save", no_argument, 0, 'S'},
      {"test_merge", required_argument, 0, 'M'},
      {"test_histo", required_argument, 0, 'H'},
      {"test_systematics", required_argument, 0, 'E'},
      {"verbose", no_argument, 0, 'v'}
    };

  int c;
  int option_index = 0;

  while ((c = getopt_long(argc, argv, "c:e:m:n:r:f:s:l:p:S:M:HEv", long_options, &option_index)) != -1) {
    switch(c) {
    case 'c':
      ncases = atoi(optarg);
      break;
    case 'e':
      nevents = atoi(optarg);
      break;
    case 'r':
      realeff_mean = atof(optarg);
      break;
    case 'f':
      fakeeff_mean = atof(optarg);
      break;
    case 's':
      eff_spread = atof(optarg);
      break;
    case 'l':
      selection = optarg;
       break;
    case 'p':
      process = optarg;
      break;
    case 'm':
      minnbaseline = atoi(optarg);
      break;
    case 'n':
      cout << optarg << endl;
      maxnbaseline = atoi(optarg);
      break;
    case 'S':
      test_save = true;
      saveFileNameBase = optarg;
      break;
    case 'M':
      test_merge = true;
      mergeFileNameBase = optarg;
      break;
    case 'H':
      test_histo = true;
      break;
    case 'E':
      test_systematics = true;
      break;
    case 'v':
      verbose = true;
      break;
    case '?':
      std::cout << "Unknown command-line option " << c << std::endl;
      abort();
    default:
      abort();
    }
  }
  
  // remove any \ from selection and process strings 
  size_t pos;
  pos = selection.find("\"");
  while (pos != string::npos) {
    selection.replace(pos, 1, "");
    pos = selection.find("\"");
  }
  pos = process.find("\"");
  while (pos != string::npos) {
    process.replace(pos, 1, "");
    pos = process.find("\"");
  }

  cout << "maxnbaseline = " << maxnbaseline << endl;

  Loop(nevents,ncases, minnbaseline, maxnbaseline, realeff_mean, fakeeff_mean, eff_spread, selection, process, test_save, test_merge, saveFileNameBase, mergeFileNameBase, verbose, test_histo, test_systematics);

}

void Loop(unsigned nevents, unsigned ncases, unsigned minnbaseline, unsigned maxnbaseline, float realeff_mean, float fakeeff_mean, float eff_spread, std::string selection, std::string process, bool test_save, bool test_merge, string saveFileNameBase, string mergeFileNameBase, bool verbose, bool test_histo, bool test_systematics)
{
   //Open an output file
  if (verbose) cout << "maxnbaseline = " << maxnbaseline << endl;

  string outputdirname;
  string rootfilename;
  outputdirname = "FakeBkgTools_toy_MC_nevents_";
  outputdirname += to_string(nevents);
  outputdirname += "_ncases_";
  outputdirname += to_string(ncases);
  outputdirname += "_minnbaseline_";
  outputdirname += to_string(minnbaseline);
  outputdirname += "_maxnbaseline_";
  outputdirname += to_string(maxnbaseline);
  outputdirname += "_realeff_";
  outputdirname += to_string(realeff_mean);
  outputdirname += "_fakeeff_";
  outputdirname += to_string(fakeeff_mean);
  outputdirname += "_effspread_";
  outputdirname += to_string(eff_spread);
  outputdirname += "_selection_";
  outputdirname += selection;
  outputdirname += "_process_";
  outputdirname += process;
  
  //now replace thing like >, < from selection and process strings to get a 
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
  std::unique_ptr<TFile> f_out(TFile::Open(rootfilename.c_str(),"RECREATE"));


  // create output ntuple
  TTree *ntuple = new TTree("FakeBkg", "Variables from toy MC for lhood and standard MM");
  ntuple->Branch("nevents", &nevents, "nevents/I");
  Float_t fake_lep_frac;
  ntuple->Branch("fake_lep_frac", &fake_lep_frac, "fake_lep_frac/F");
  Float_t true_fakes;
  ntuple->Branch("true_fakes", &true_fakes,"true_fakes/F");
  Float_t true_FF;
  ntuple->Branch("true_FF", &true_FF,"true_FF/F");
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

  TRandom3 rand(242868252);
  
  
  Double_t realeff_spread = eff_spread;
  Double_t fakeeff_spread = eff_spread;
  
  std::vector<bool> isTight;
  std::vector<std::string> input = { outputdirname+"efficiencies_full" };
  
  if(false) // switch to test either XML or ROOT input
    {
      // not implemented yet!
      //      input.back().append(".xml");
      //      writeXML(input.back(), 0);
    }
  else
    {
	input.back().append(".root");
	writeROOT(input.back(), 0, realeff_mean, fakeeff_mean, eff_spread);
	rootEffFileName = input.back();
	setupEfficiencies();
    }
  
  for (unsigned icase = 0; icase < ncases; icase++) {
  
    TH1F* h_lep_pt = new TH1F("lep_pt", "lep_pt", 10, 0, 100);
    TH2F* h_lep_pt_eta =  new TH2F("lep_pt_eta", "lep_pt_eta", 10, 0, 100, 10, -5, 5);
    float lep_pt, lep_eta;
      
    if (!test_merge) {

      cout << "Starting case " << icase << endl;
      CP::LhoodMM_tools lhmTool("LhoodMM_tools");
      CP::AsymptMatrixTool asmTool("AsymptMatrixTool");
      float asmYield = 0;
      float asmErr = 0;
      if (test_histo) {
	lhmTool.register1DHistogram(h_lep_pt, &lep_pt);
	lhmTool.register2DHistogram(h_lep_pt_eta, &lep_pt, &lep_eta);
      }

      initialize(asmTool, input, selection, process, verbose);
      initialize(lhmTool, input, selection, process, verbose); 

      if (test_systematics) {
	auto sysvars = lhmTool.affectingSystematics();
	for(auto& sysvar : lhmTool.affectingSystematics())
	  {
	    lhmTool.getSystDescriptor().printUncertaintyDescription(sysvar);
	  }
      }

      Double_t realeff_mean_thiscase = rand.Gaus(realeff_mean, realeff_spread);
      if (realeff_mean_thiscase > 0.99) realeff_mean_thiscase = 0.99;
      Double_t fakeeff_mean_thiscase = rand.Gaus(fakeeff_mean, fakeeff_spread);
      if (fakeeff_mean_thiscase < 0.01) fakeeff_mean_thiscase = 0.01;
      if (fakeeff_mean_thiscase > realeff_mean_thiscase) fakeeff_mean_thiscase = realeff_mean_thiscase-0.01;
      
      fake_lep_frac = 0.95*rand.Uniform();  // fraction of fake leptons in the loose sample
      
      totWeight_std = 0.;
      err_std = 0;
      n_fake_lhood = 0.;
      n_fake_lhood_tot_poserr = 0.;
      n_fake_lhood_tot_negerr = 0.;
      
      std::vector<LhoodMMEvent> mmevts;
      
      true_fakes = 0; 
      true_FF = 0.;
      
      std::vector<double> realeff, fakeeff;
      
      Int_t nReal = 0;
      
      vector<FinalState*> fs;
      FinalState* this_fs;
      for (unsigned ilep = 0; ilep <= maxnbaseline; ilep++) {
	string error;
	this_fs = new FinalState(0, ilep, selection, process, error);
	fs.push_back(this_fs);
      }
      
      for (Long64_t ievt=0; ievt<nevents;ievt++) {
	
	float extraweight = 1.0;
	
	realeff.clear();
	fakeeff.clear();
	
	float nlep_frac = 1./(maxnbaseline-minnbaseline+1);
	
	Int_t nlep_select = minnbaseline+rand.Uniform()/nlep_frac;
	
	xAOD::IParticleContainer leptons(SG::VIEW_ELEMENTS);
	vector<FakeBkgTools::ParticleData> leptons_data;
	
	vector<bool> lep_real;
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

	  lep_pt = 100*rand.Uniform();
	  h_lep_pt->Fill(lep_pt);
	  lep_eta = -5. + 10*rand.Uniform();
	  lepton->setP4( 1000*lep_pt, lep_eta, 0, 0.511);

	  FakeBkgTools::ParticleData lepton_data;
	  lookupEfficiencies(*lepton, lepton_data);
	  // decide if lepton is tight or not
	  if (isReal) {
	    if (rand.Uniform() < lepton_data.real_efficiency.nominal) {
	      lepton->auxdata<char>("Tight") = true;
	    } else {
	      lepton->auxdata<char>("Tight") = false;
	    }
	  } else {
	    if (rand.Uniform() < lepton_data.fake_efficiency.nominal) {
	      lepton->auxdata<char>("Tight") = true;
	    } else {
	      lepton->auxdata<char>("Tight") = false;
	    }
	  }
	  leptons.push_back(static_cast<xAOD::IParticle*>(lepton));
	 
	  leptons_data.push_back(lepton_data);
	}
	
	asmTool.addEvent(leptons, extraweight);
	lhmTool.addEvent(leptons, extraweight);

	// determine the expected number of fake lepton events
	// start by looping over all possible number of tight leptons
	vector<int> lep_indices;
	std::bitset<64> tights, reals, charges;
	for (int ilep = 0; ilep < nlep_select; ilep++) {
	  lep_indices.push_back(ilep);
	  if (lep_real[ilep]) reals.set(ilep);
	}

	for (long comb = 0; comb < (1<<nlep_select); ++comb) {
	  tights = std::bitset<64>(comb);
	  if (fs[nlep_select]->accept_selection(tights, charges)) {
	    if (fs[nlep_select]->accept_process(nlep_select, reals, tights)) {
	      true_fakes += extraweight*comboProb(leptons_data, tights, reals);
	      //true_FF += comboProb_FF(leptons_data, tights, reals); 
	    }
	  }
	}
	
	float wgt;
	if(asmTool.getEventWeight(wgt, selection, process) != StatusCode::SUCCESS) { cout << "ERROR: AsymptMatrixTool::getEventWeight() failed\n"; exit(2); }
	asmYield += wgt;
	asmErr += wgt*wgt;
	
	if (test_save) {
	  if (ievt > 0 &&  (((10*ievt)%nevents ==0) || ievt == nevents -1) ) {
	    string saveFileName = saveFileNameBase;
	    saveFileName+=  "_lhm_"+to_string(ievt)+".root";
	    std::unique_ptr<TDirectory>saveFile = std::make_unique<TDirectory>(saveFileName.c_str(), "RECREATE");
	    cout << "testing save/merge" << endl;
	    lhmTool.saveProgress(saveFile.get());
	    saveFile->Close();
	    saveFileName =  saveFileNameBase+"_asm_"+to_string(ievt)+".root";
	    saveFile = std::make_unique<TDirectory>(saveFileName.c_str(), "RECREATE");
	    asmTool.saveProgress(saveFile.get());
	    saveFile->Close();
	  }
	}

	for (xAOD::IParticleContainer::iterator it = leptons.begin(); it != leptons.end(); it++) {
	  if (*it != nullptr) delete *it;
	}
	leptons.clear();
      }
      
      asm_err = sqrt(asmErr);

      if(lhmTool.getTotalYield(lhoodMM_fakes, lhoodMM_poserr, lhoodMM_negerr) != StatusCode::SUCCESS) { cout << "ERROR: LhoodMM_tools::getTotalYield() failed\n"; exit(2); }

      asm_fakes = asmYield;
      
      if (test_systematics) {
	auto sysvars = asmTool.affectingSystematics();
	
	for(auto& sysvar : sysvars)
	  {
	    float weight, statUp, statDown;
	    asmTool.applySystematicVariation({sysvar});
	    asmTool.getTotalYield(weight, statUp, statDown);
	    asmTool.getSystDescriptor().printUncertaintyDescription(sysvar);
	    cout << "asm weight = " << weight << endl;
	    lhmTool.applySystematicVariation({sysvar});
	    lhmTool.getTotalYield(weight, statUp, statDown);
	    std::cout << "sysvar = " << sysvar << std::endl;
	    cout << "lhm weight = " << weight << endl;
	  }
      }

      asm_fakes = asmYield;
      
      cout << "OUTPUT nfakes = " << lhoodMM_fakes << " +" << lhoodMM_poserr << " -" <<  lhoodMM_negerr <<  endl;
      cout << "OUTPUT true_fakes = " << true_fakes << endl;
      ntuple->Fill();
    } else {
      std::string haddcmd = "hadd -f "+mergeFileNameBase+"_lhm.root "+mergeFileNameBase+"_lhm_*.root";
      system(haddcmd.c_str());
      haddcmd = "hadd -f "+mergeFileNameBase+"_asm.root "+mergeFileNameBase+"_asm_*.root";
      system(haddcmd.c_str());
      CP::LhoodMM_tools lhmTool_merge("LhoodMM_tools_merge");
      std::string mergeFileName =  mergeFileNameBase+"_lhm.root";
      lhmTool_merge.setProperty("ProgressFileName", mergeFileName);
      cout << "verbose = " << verbose << endl;
      initialize(lhmTool_merge, input, selection, process, verbose);
      if (test_histo) {
	lhmTool_merge.register1DHistogram(h_lep_pt, &lep_pt);
	lhmTool_merge.register2DHistogram(h_lep_pt_eta, &lep_pt, &lep_eta);
      }
      if(lhmTool_merge.getTotalYield(lhoodMM_fakes, lhoodMM_poserr, lhoodMM_negerr) != StatusCode::SUCCESS) { cout << "ERROR: LhoodMM_tools::getTotalYield() failed\n"; exit(2); }
      cout << "merged lhm nfakes = " << lhoodMM_fakes << " + " << lhoodMM_poserr << " " <<  lhoodMM_negerr << endl;
      
      CP::AsymptMatrixTool asmTool_merge("asm_tool_merge");
      mergeFileName =  mergeFileNameBase+"_asm.root";
      asmTool_merge.setProperty("ProgressFileName", mergeFileName);
      cout << "verbose = " << verbose << endl;
      initialize(asmTool_merge, input, selection, process, verbose);
      if (test_histo) {
	lhmTool_merge.register1DHistogram(h_lep_pt, &lep_pt);
	lhmTool_merge.register2DHistogram(h_lep_pt_eta, &lep_pt, &lep_eta);
      }
      if(asmTool_merge.getTotalYield(asm_fakes, asm_err, asm_err) != StatusCode::SUCCESS) { cout << "ERROR: AsymptMatrixTool::getTotalYield() failed\n"; exit(2); }
      cout << "merged asm nfakes = " << asm_fakes << " + " << asm_err << " " <<  asm_err << endl;

      if (test_systematics) {
	auto sysvars = lhmTool_merge.affectingSystematics();
	for(auto& sysvar : sysvars)
	  {
	    float weight = 0, statUp, statDown;
	    lhmTool_merge.getSystDescriptor().printUncertaintyDescription(sysvar);
	    std::cout << "sysvar = " << sysvar << std::endl;
	    lhmTool_merge.applySystematicVariation({sysvar});
	    lhmTool_merge.getTotalYield(weight, statUp, statDown);
	    cout << "merged lhm weight = " << weight << endl;
	    
	    asmTool_merge.getSystDescriptor().printUncertaintyDescription(sysvar);
	    asmTool_merge.applySystematicVariation({sysvar});
	    asmTool_merge.getTotalYield(weight, statUp, statDown);
	  cout << "merged asm weight = " << weight << endl;
	}
      }      
    }


    f_out->cd();
    if (test_histo) {
      h_lep_pt->Write();
      h_lep_pt_eta->Write();

      delete h_lep_pt;
      delete h_lep_pt_eta;
    }
  }

  f_out->Write();
  f_out->Close();

}

void writeROOT(const string& name, int type, float realeff_mean, float fakeeff_mean, float eff_spread)
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
	Double_t realeff = TMath::Min(rnd.Gaus(realeff_mean, eff_spread), 0.99);
	Double_t fakeeff = TMath::Max(rnd.Gaus(fakeeff_mean, eff_spread), 0.01);
    
	if (realeff - fakeeff < 0.01) {
	  fakeeff = realeff - 0.01;
	}
	
	hElFake.SetBinContent(ibin, fakeeff);
	hElFake.SetBinError(ibin, eff_spread);
	hMuFake.SetBinContent(ibin, fakeeff);
	hMuFake.SetBinError(ibin, eff_spread);
	hElReal.SetBinContent(ibin, realeff);
	hElReal.SetBinError(ibin, eff_spread);
	hMuReal.SetBinContent(ibin, realeff);
	hMuReal.SetBinError(ibin, eff_spread);


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

  return prob;
}
