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

void Loop(unsigned nevents, unsigned ncases, unsigned minnbaseline, unsigned maxnbaseline, float realeff_mean, float fakeeff_mean, float eff_spread, std::string selection, std::string process, bool test_save, bool test_merge, string saveFaileName, string mergeFileName, bool verbose);

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
  bool test_save, test_merge, verbose;
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
      {"test_save", required_argument, 0, 'S'},
      {"test_merge", required_argument, 0, 'M'},
      {"verbose", no_argument, 0, 'v'}
    };

  int c;
  int option_index = 0;

  while ((c = getopt_long(argc, argv, "c:e:m:n:r:f:s:l:p:S:M:v", long_options, &option_index)) != -1) {
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

  Loop(nevents,ncases, minnbaseline, maxnbaseline, realeff_mean, fakeeff_mean, eff_spread, selection, process, test_save, test_merge, saveFileNameBase, mergeFileNameBase, verbose);

}

void Loop(unsigned nevents, unsigned ncases, unsigned minnbaseline, unsigned maxnbaseline, float realeff_mean, float fakeeff_mean, float eff_spread, std::string selection, std::string process, bool test_save, bool test_merge, string saveFileNameBase, string mergeFileNameBase, bool verbose)
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
  TFile *f_out = TFile::Open(rootfilename.c_str(),"RECREATE");


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
      //following is to run likelihood MM in fake-factor mode
      //LhoodMM_tools lhmTool_FF;
      //lhmTool_FF.setFitType("FF");
       CP::ApplyFakeFactor fkfTool("ApplyFakeFactor");
      CP::AsymptMatrixTool asmTool("AsymptMatrixTool");
      float asmYield = 0;
      //suppress FakeFactor tool for now since it will print warning messages
      //when given unsupported selection or process arguments
      //float fkfYield;
      float asmErr = 0;
      //float fkfErr;
      
      lhmTool.register1DHistogram(h_lep_pt, &lep_pt);
      lhmTool.register2DHistogram(h_lep_pt_eta, &lep_pt, &lep_eta);

      initialize(fkfTool, input, selection, process, verbose);
      initialize(asmTool, input, selection, process, verbose);
      initialize(lhmTool, input, selection, process, verbose); 
      //     initialize(lhmTool_FF, input, selection, process); 
      
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
	
	fkfTool.addEvent(leptons);
	asmTool.addEvent(leptons, extraweight);
	lhmTool.addEvent(leptons, extraweight);
	
	//       lhmTool_FF.addEvent(leptons);
	
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
	    //true_FF += comboProb_FF(leptons_data, tights, reals);
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
	
	//if(fkfTool.getEventWeightAndUncertainties(wgt, selection, process) != StatusCode::SUCCESS) { cout << "ERROR: ApplyFakeFactor::getEventWeightAndUncertainties() failed\n"; exit(2); }
	//fkfYield.add(wgt);
	
	if (test_save) {
	  if (ievt > 0 &&  (((10*ievt)%nevents ==0) || ievt == nevents -1) ) {
	    string saveFileName = saveFileNameBase;
	    saveFileName+=  "_"+to_string(ievt)+".root";
	    TFile *saveFile =  new TFile(saveFileName.c_str(), "RECREATE");
	    cout << "testing save/merge" << endl;
	    lhmTool.saveProgress(saveFile);
	    saveFile->Close();
	  }
	}

	for (xAOD::IParticleContainer::iterator it = leptons.begin(); it != leptons.end(); it++) {
	  if (*it != nullptr) delete *it;
	}
	leptons.clear();
      }
      
      asmErr = sqrt(asmErr);

      if(lhmTool.getTotalYield(lhoodMM_fakes, lhoodMM_poserr, lhoodMM_negerr) != StatusCode::SUCCESS) { cout << "ERROR: LhoodMM_tools::getTotalYield() failed\n"; exit(2); }

      CP::SystematicVariation stat{"FAKEBKG_YIELD_STAT", 0};
      
      asm_fakes = asmYield;
      
      //      fkf_fakes = fkfYield;
      
      
      //     lhoodFF_fakes = lhmYield_FF.value;
      //lhoodFF_negerr = lhmYield_FF.uncertainties[Uncertainty::totalStat()].down;
      //     lhoodFF_poserr = lhmYield_FF.uncertainties[Uncertainty::totalStat()].up;
      
      cout << "OUTPUT nfakes = " << lhoodMM_fakes << " +" << lhoodMM_poserr << " -" <<  lhoodMM_negerr <<  endl;
      cout << "OUTPUT true_fakes = " << true_fakes << endl;
      ntuple->Fill();
    } else {
      std::string haddcmd = "hadd -f "+mergeFileNameBase+".root "+mergeFileNameBase+"_*.root";
      system(haddcmd.c_str());
      CP::LhoodMM_tools lhmTool_merge("LhoodMM_tools_merge");
 
      std::string mergeFileName =  mergeFileNameBase+".root";
      TFile* fmerge = new TFile(mergeFileName.c_str());
      TDirectory* myDir = (TDirectory*)fmerge;
      cout << "Really doing this..." << endl;
      lhmTool_merge.setProperty("ProgressFileName", myDir);
      cout << "verbose = " << verbose << endl;
      initialize(lhmTool_merge, input, selection, process, verbose);
      lhmTool_merge.register1DHistogram(h_lep_pt, &lep_pt);
      lhmTool_merge.register2DHistogram(h_lep_pt_eta, &lep_pt, &lep_eta);
      if(lhmTool_merge.getTotalYield(lhoodMM_fakes, lhoodMM_poserr, lhoodMM_negerr) != StatusCode::SUCCESS) { cout << "ERROR: LhoodMM_tools::getTotalYield() failed\n"; exit(2); }
      cout << "merged nfakes = " << lhoodMM_fakes << " + " << lhoodMM_poserr << " " <<  lhoodMM_negerr << endl;
    }


    f_out->cd();
    h_lep_pt->Write();
    h_lep_pt_eta->Write();

    delete h_lep_pt;
    delete h_lep_pt_eta;

  }

  f_out->Write();
  f_out->Close();

}

void writeROOT(const string& name, int type, float realeff_mean, float fakeeff_mean, float eff_spread)
{
  TRandom3 rnd(235789);

  int nbin = 100;

  TFile* file = TFile::Open(name.c_str(), "RECREATE");

  if(type == 0)
    {
      TH1D hElFake("FakeEfficiency_el_pt","FakeEfficiency", nbin, 0., 1.*nbin);
      TH1D hMuFake("FakeEfficiency_mu_pt","FakeEfficiency", nbin, 0., 1.*nbin);
      TH1D hElReal("RealEfficiency_el_pt","RealEfficiency", nbin, 0., 1.*nbin);
      TH1D hMuReal("RealEfficiency_mu_pt","RealEfficiency", nbin, 0., 1.*nbin);

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
      }

      file->cd();
      hElFake.Write();
      hElReal.Write();
      hMuFake.Write();
      hMuReal.Write();
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
