/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// tope level executable

#include "TApplication.h"
#include "TROOT.h"
#include "TFile.h"
#include "TStyle.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <getopt.h>

#include "TH2F.h"
#include "TString.h"
//#include "TChain.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TH1.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TMath.h"

#include "RealTrackTree.h"

void plotResolution(const TString& coordinate, const TString& versus);
void plotEfficiency();
void plotKinematic();
int calculateEfficiency(Double_t &eff, Double_t &effErr, const Int_t num, const Int_t den);
int calculateEfficiencyAsymErrors(Double_t &eff, Double_t &effErr_p, Double_t &effErr_m, const Int_t num, const Int_t den);
std::vector<float> stableGaussianFit(TH1*);
double error_func(float, Double_t*);
double scaleFactorFitFcn(double *x, double *par);
void SetAtlasStyle ();
TStyle* AtlasStyle();
void PrintH(TH1 *h, TString fileName="", TString drawOptions="");
void PrintH(TGraph *h, TString fileName="", TString drawOptions="");

TFile *outROOTFile;

// Fit method for k-factor for resolution
Int_t fitResKFactorMethod = 2;

int main(int argc, char **argv)
{
  TApplication theApp("App", &argc, argv);

// gStyle->SetPalette(1);
SetAtlasStyle();

//TString inputtype("D3PD");
TString inputtype("histos");
// TString triggerFilter("EF_mbMbts_1_eff");
// TString triggerFilter("L1_MBTS_1_1");
TString triggerFilter("");

// must be created here otherwise it gets closed before plots are done
TFile* f(0);

// --- Scan for custom command-line parameters (ROOT reserves: -l,-b,-n,-q)
 int c;
 extern int optind;
 extern char* optarg;
 
//making the MC availability a separate boolean variable with default false value. 
 Bool_t mcAvailable = false;
 Long64_t maxEvents(-1);

 vector<string> inputRootFilesTxt; //input Root Files from a text file
 while (1) 
 {
   int option_index = 0;
   static struct option long_options[] = {{0, 0, 0, 0}};

   c = getopt_long (argc, argv, "hDMHi:t:o:k:e:",
		    long_options, &option_index);
   if (c == -1)
     break;

   switch (c) 
   {
   case 'D':
     cout << "Using D3PD format as input." << endl;
     inputtype = "D3PD";
     break;
     
   case 'M':  
     cout << "MC info should be available." << endl;
     mcAvailable = true;
     break;
     
   case 'H':
     cout << "Using Athena monitoring histograms as input" << endl;
     inputtype = "histos";
     break;
   case 'i':
     {
       string inputFileName = optarg;
       if (inputFileName[0] == '#') break; //input file commented out
       ifstream inputFileStream(inputFileName.c_str());
       if (inputFileStream.fail()) {
	 cerr << "ERROR: Error opening input file " << inputFileName << endl;
	 return 0;
       }
       cout << "Acquiring input files from " << inputFileName << endl;
       try {
	 while (!inputFileStream.eof()) {
	   string currentRootInFile;
	   inputFileStream >> currentRootInFile;
	   if (currentRootInFile[0] == '#')
	     continue; //skip this line
	   if (!currentRootInFile.empty()) {
             std::cout << currentRootInFile << std::endl;
	     inputRootFilesTxt.push_back(currentRootInFile);
	   }
	 }
       }
       catch ( char* errstr) {
	 cerr << "ERROR: I/O Error while retrieving ROOT file list from " << inputFileName << endl;
	 return 1;
       }
       break;
     }
   case 't':
     triggerFilter = optarg;
     cout << "Activated trigger filter: " << triggerFilter << endl;
     break;
   case 'o':
     {
       TString outROOTFileName = optarg;
       outROOTFile = TFile::Open(outROOTFileName, "RECREATE");
       cout << "Saving output to: " << outROOTFileName << endl;
       break;
     }
   case 'k':
     {
       TString strFitKFactor = optarg;
       fitResKFactorMethod = strFitKFactor.Atoi();
       cout << "Resolution scale factor fit method: " << fitResKFactorMethod << endl;
       break;
     }
   case 'e':
     {
       TString strMaxEvents = optarg;
       maxEvents = strMaxEvents.Atoi();
       cout << "Max number of events: " << maxEvents << endl;
       break;
     }
   case 'h':
     cout << "Usage: " << argv[0] << " [-h] [-D | -H] [-i file_list] [-M] [-k fitMethod] [-o outputROOTFile] [-e maxEvents]" << endl;
     cout << "\t h: show this help." << endl;
     cout << "\t D: Take as input format D3PD files" << endl;
     cout << "\t H: Take as input format Histograms from Athena monitoring" << endl;
     cout << "\t M: Read MC truth information" << endl;
     cout << "\t k: Choose fitting method for resolution scale factors" << endl;
     cout << "\t o: Save output to ROOT file (and print figures)" << endl;
     cout << "\t i: use file_list content to locate input files" << endl;
     cout << "\t e: Upper limit in number of processed events" << endl;
     return 0;
   default:
     cout << "Unknown argument: -" << optind << " " << optarg << endl;
     return 1;
   }//end of switch for the 0th order option argument 
 }//end of while(1) loop

// with D3PD as input
if (inputtype == "D3PD")
{
  TChain* chain = new TChain("InDetTrackTree");
  TChain* trigMeta(0);
  if (triggerFilter!="") trigMeta = new TChain("InDetTrackTreeMeta/TrigConfTree");
  if (inputRootFilesTxt.empty())

  {
    string myDir = "root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/perf-idtracking/trackptstudies/group.perf-idtracking.mc10_7TeV.105001.pythia_minbias.rerecoPT500.VTX_NTUPMON.e574_s932_s946_r1838_tid213413_00.100k.V3/";
    inputRootFilesTxt.push_back(myDir+"group.perf-idtracking.09015_000197.D3PD._00011.root");
    inputRootFilesTxt.push_back(myDir+"group.perf-idtracking.09015_000197.D3PD._00012.root");
    inputRootFilesTxt.push_back(myDir+"group.perf-idtracking.09015_000197.D3PD._00013.root");
    inputRootFilesTxt.push_back(myDir+"group.perf-idtracking.09015_000197.D3PD._00014.root");
    //     inputRootFilesTxt.push_back(myDir+"group.perf-idtracking.09015_000197.D3PD._00015.root");
    //     inputRootFilesTxt.push_back(myDir+"group.perf-idtracking.09015_000197.D3PD._00016.root");
    //     inputRootFilesTxt.push_back(myDir+"group.perf-idtracking.09015_000197.D3PD._00017.root");
    //     inputRootFilesTxt.push_back(myDir+"group.perf-idtracking.09015_000197.D3PD._00018.root");
    //     inputRootFilesTxt.push_back(myDir+"group.perf-idtracking.09015_000197.D3PD._00019.root");
    //     inputRootFilesTxt.push_back(myDir+"group.perf-idtracking.09015_000197.D3PD._00020.root");
  }

  for (vector<string>::iterator iti=inputRootFilesTxt.begin(); iti != inputRootFilesTxt.end(); ++iti) {
    chain->Add(iti->c_str());
    if (trigMeta) trigMeta->Add(iti->c_str());
  }
  RealTrackTree t(chain, trigMeta);
  t.triggerName = triggerFilter;
  t.m_mcAvailable = mcAvailable;
  if (maxEvents >=0)
    t.m_maxEvents = maxEvents;
  t.Loop();
}
// with athena monitoring histo file as input
else if (inputtype == "histos")
{
  if (inputRootFilesTxt.empty())
    inputRootFilesTxt.push_back("/afs/cern.ch/user/w/wildauer/public/VertexStudies/user.AndreasWildauer.data11_7TeV.00180164.express_express.VTXMON_D3PD.f368_m806.V1.root");
  if (inputRootFilesTxt.size() > 1)
    cout << "WARNING: This macro is only working with 1 input file for HISTO input format. Please use hadd to merge several files together first. The macro will now run on the first file only." << endl;
  f = TFile::Open(inputRootFilesTxt[0].c_str());
  f->cd("InDetGlobal/PrimaryVertex");
}
else
{
  std::cout << "Wrong input type: should be either D3PD or histos, you gave: " << inputtype << std::endl;
  return 0;
}

if (inputRootFilesTxt.size() > 1)
  std::cout << "Number of input files: " << inputRootFilesTxt.size() << std::endl;
else
  std::cout << "Input file: " << inputRootFilesTxt[0] << std::endl;

///////////////////////////////

plotResolution("X", "Ntrk");
plotResolution("Y", "Ntrk");
plotResolution("Z", "Ntrk");

plotResolution("X", "SumPt2");
plotResolution("Y", "SumPt2");
plotResolution("Z", "SumPt2");

if (inputtype == "D3PD")
  plotKinematic(); 

plotEfficiency();

TH1F *h_Vrt_BCNumberOfVertices(0); h_Vrt_BCNumberOfVertices  = (TH1F*)gDirectory->Get("Vrt_BCNumberOfVertices");
TH1F *h_Vrt_nBCNumberOfVertices(0); h_Vrt_nBCNumberOfVertices  = (TH1F*)gDirectory->Get("Vrt_nBCNumberOfVertices");
TH1F *h_Vrt_BCNumberOfVertices_4trk(0); h_Vrt_BCNumberOfVertices_4trk   = (TH1F*)gDirectory->Get("Vrt_BCNumberOfVertices_4trk");
TH1F *h_Vrt_nBCNumberOfVertices_4trk(0); h_Vrt_nBCNumberOfVertices_4trk  = (TH1F*)gDirectory->Get("Vrt_nBCNumberOfVertices_4trk");
TH1F *h_Vrt_BC_truth_Ratio_Nvtx(0); h_Vrt_BC_truth_Ratio_Nvtx = (TH1F*)gDirectory->Get("h_Vrt_BC_truth_Ratio_Nvtx");
TH1F *h_Vrt_BC_truth_Nvtx(0); h_Vrt_BC_truth_Nvtx  = (TH1F*)gDirectory->Get("h_Vrt_BC_truth_Nvtx");
TH1F *h_Vrt_BC_truth_Delta_Nvtx(0);  h_Vrt_BC_truth_Delta_Nvtx = (TH1F*)gDirectory->Get("h_Vrt_BC_truth_Delta_Nvtx");
TH1F *h_Vrt_BC_truth_Sigma_Delta_Nvtx(0); h_Vrt_BC_truth_Sigma_Delta_Nvtx = (TH1F*)gDirectory->Get("h_Vrt_BC_truth_Sigma_Delta_Nvtx");
TH1F *h_Vrt_truth_delta_z(0); h_Vrt_truth_delta_z = (TH1F*)gDirectory->Get("h_Vrt_truth_delta_z");
TH1F *h_MCVrt_Num_SimuVtxGood(0); h_MCVrt_Num_SimuVtxGood = (TH1F*)gDirectory->Get("h_MCVrt_Num_SimuVtxGood");
TH1F *h_MCVrt_Num_RecoVtxGood(0); h_MCVrt_Num_RecoVtxGood = (TH1F*)gDirectory->Get("h_MCVrt_Num_RecoVtxGood");
TH1F *h_MCVrt_RecoSimu_VtxM_Match(0); h_MCVrt_RecoSimu_VtxM_Match = (TH1F*)gDirectory->Get("h_MCVrt_RecoSimu_VtxM_Match");
TH1F *h_MCVrt_RecoSimu_VtxM_Split(0); h_MCVrt_RecoSimu_VtxM_Split = (TH1F*)gDirectory->Get("h_MCVrt_RecoSimu_VtxM_Split");
TH1F *h_MCVrt_RecoSimu_VtxM_Merge(0); h_MCVrt_RecoSimu_VtxM_Merge = (TH1F*)gDirectory->Get("h_MCVrt_RecoSimu_VtxM_Merge");
TH1F *h_MCVrt_RecoSimu_VtxM_Ineff(0); h_MCVrt_RecoSimu_VtxM_Ineff = (TH1F*)gDirectory->Get("h_MCVrt_RecoSimu_VtxM_Ineff");
TH1F *h_MCVrt_RecoSimu_VtxM_Fake(0); h_MCVrt_RecoSimu_VtxM_Fake = (TH1F*)gDirectory->Get("h_MCVrt_RecoSimu_VtxM_Fake");
TH1F *h_MCVrt_RecoSimu_VtxM_Match_2(0); h_MCVrt_RecoSimu_VtxM_Match_2 = (TH1F*)gDirectory->Get("h_MCVrt_RecoSimu_VtxM_Match_2");
TH1F *h_MCVrt_RecoSimu_VtxM_Others(0); h_MCVrt_RecoSimu_VtxM_Others = (TH1F*)gDirectory->Get("h_MCVrt_RecoSimu_VtxM_Others");
TH1F *h_MCVrt_RecoSimu_VtxM_Dist_Match(0); h_MCVrt_RecoSimu_VtxM_Dist_Match = (TH1F*)gDirectory->Get("h_MCVrt_RecoSimu_VtxM_Dist_Match");
TH1F *h_MCVrt_RecoSimu_VtxM_Dist_Split_gen_1(0); h_MCVrt_RecoSimu_VtxM_Dist_Split_gen_1 = (TH1F*)gDirectory->Get("h_MCVrt_RecoSimu_VtxM_Dist_Split_gen_1");
TH1F *h_MCVrt_RecoSimu_VtxM_Dist_Split_gen_2(0); h_MCVrt_RecoSimu_VtxM_Dist_Split_gen_2 = (TH1F*)gDirectory->Get("h_MCVrt_RecoSimu_VtxM_Dist_Split_gen_2");
TH1F *h_MCVrt_RecoSimu_VtxM_Dist_Split_rec(0); h_MCVrt_RecoSimu_VtxM_Dist_Split_rec = (TH1F*)gDirectory->Get("h_MCVrt_RecoSimu_VtxM_Dist_Split_rec");
TH1F *h_MCVrt_RecoSimu_VtxM_Dist_Merge_firstGen(0); h_MCVrt_RecoSimu_VtxM_Dist_Merge_firstGen = (TH1F*)gDirectory->Get("h_MCVrt_RecoSimu_VtxM_Dist_Merge_firstGen");
TH1F *h_MCVrt_RecoSimu_VtxM_Dist_Merge_secondGen(0); h_MCVrt_RecoSimu_VtxM_Dist_Merge_secondGen = (TH1F*)gDirectory->Get("h_MCVrt_RecoSimu_VtxM_Dist_Merge_secondGen");
TH1F *h_MCVrt_RecoSimu_VtxM_Dist_Fake(0); h_MCVrt_RecoSimu_VtxM_Dist_Fake = (TH1F*)gDirectory->Get("h_MCVrt_RecoSimu_VtxM_Dist_Fake");
TH1F *h_MCVrt_RecoSimu_VtxM_Dist_Ineff(0); h_MCVrt_RecoSimu_VtxM_Dist_Ineff = (TH1F*)gDirectory->Get("h_MCVrt_RecoSimu_VtxM_Dist_Ineff");

// plot true number of vertices
if(mcAvailable)
{
 TH1F *h_Vrt_trueNumberOfVertices = (TH1F*)gDirectory->Get("Vrt_trueNumberOfVertices");
 
 
 TCanvas* ctrue = new TCanvas("Truth Canvas", "Truth Canvas", 600,600);
 h_Vrt_trueNumberOfVertices->Draw();
 
 h_Vrt_BCNumberOfVertices->Draw("SAME");
 h_Vrt_BCNumberOfVertices->SetLineColor(2);
 h_Vrt_nBCNumberOfVertices->SetLineColor(3); 
 h_Vrt_nBCNumberOfVertices->Draw("SAME");

 if (outROOTFile) {
   cout << "Saving MC plots" << endl;
   h_Vrt_trueNumberOfVertices->Write(); PrintH(h_Vrt_trueNumberOfVertices);
   ctrue->Write();
   h_Vrt_BC_truth_Ratio_Nvtx->Write(); PrintH(h_Vrt_BC_truth_Ratio_Nvtx);
   h_Vrt_BC_truth_Nvtx->Write(); PrintH(h_Vrt_BC_truth_Nvtx);
   h_Vrt_BC_truth_Delta_Nvtx->Write(); PrintH(h_Vrt_BC_truth_Delta_Nvtx);
   h_Vrt_BC_truth_Sigma_Delta_Nvtx->Write(); PrintH(h_Vrt_BC_truth_Sigma_Delta_Nvtx);
   h_Vrt_truth_delta_z->Write(); PrintH(h_Vrt_truth_delta_z);
   h_MCVrt_Num_SimuVtxGood->Write(); PrintH(h_MCVrt_Num_SimuVtxGood);
   h_MCVrt_Num_RecoVtxGood->Write(); PrintH(h_MCVrt_Num_RecoVtxGood);
   h_MCVrt_RecoSimu_VtxM_Match->Write(); PrintH(h_MCVrt_RecoSimu_VtxM_Match); 
   h_MCVrt_RecoSimu_VtxM_Split->Write(); PrintH(h_MCVrt_RecoSimu_VtxM_Split); 
   h_MCVrt_RecoSimu_VtxM_Merge->Write(); PrintH(h_MCVrt_RecoSimu_VtxM_Merge); 
   h_MCVrt_RecoSimu_VtxM_Ineff->Write(); PrintH(h_MCVrt_RecoSimu_VtxM_Ineff); 
   h_MCVrt_RecoSimu_VtxM_Fake->Write(); PrintH(h_MCVrt_RecoSimu_VtxM_Fake); 
   h_MCVrt_RecoSimu_VtxM_Match_2->Write(); PrintH(h_MCVrt_RecoSimu_VtxM_Match_2);
   h_MCVrt_RecoSimu_VtxM_Others->Write(); PrintH(h_MCVrt_RecoSimu_VtxM_Others);
   h_MCVrt_RecoSimu_VtxM_Dist_Match->Write(); PrintH(h_MCVrt_RecoSimu_VtxM_Dist_Match);
   h_MCVrt_RecoSimu_VtxM_Dist_Split_gen_1->Write(); PrintH(h_MCVrt_RecoSimu_VtxM_Dist_Split_gen_1);
   h_MCVrt_RecoSimu_VtxM_Dist_Split_gen_2->Write(); PrintH(h_MCVrt_RecoSimu_VtxM_Dist_Split_gen_2);
   h_MCVrt_RecoSimu_VtxM_Dist_Split_rec->Write(); PrintH(h_MCVrt_RecoSimu_VtxM_Dist_Split_rec);
   h_MCVrt_RecoSimu_VtxM_Dist_Merge_firstGen->Write(); PrintH(h_MCVrt_RecoSimu_VtxM_Dist_Merge_firstGen);
   h_MCVrt_RecoSimu_VtxM_Dist_Merge_secondGen->Write(); PrintH(h_MCVrt_RecoSimu_VtxM_Dist_Merge_secondGen);
   h_MCVrt_RecoSimu_VtxM_Dist_Fake->Write(); PrintH(h_MCVrt_RecoSimu_VtxM_Dist_Fake);
   h_MCVrt_RecoSimu_VtxM_Dist_Ineff->Write(); PrintH(h_MCVrt_RecoSimu_VtxM_Dist_Ineff);
 }

 } // mc available

if (outROOTFile) {
  if (h_Vrt_BCNumberOfVertices)
    h_Vrt_BCNumberOfVertices->Write(); PrintH(h_Vrt_BCNumberOfVertices);
  if (h_Vrt_nBCNumberOfVertices)
    h_Vrt_nBCNumberOfVertices->Write(); PrintH(h_Vrt_nBCNumberOfVertices);
  if (inputtype == "D3PD") {
    h_Vrt_BCNumberOfVertices_4trk->Write(); PrintH(h_Vrt_BCNumberOfVertices_4trk);
    h_Vrt_nBCNumberOfVertices_4trk->Write(); PrintH(h_Vrt_nBCNumberOfVertices_4trk);
  }
 }

 if (outROOTFile)
   outROOTFile->Close();

theApp.Run();

if (inputtype == "histos") { f->Close(); }

return 0;
}

void plotResolution(const TString& coordinate="Z", const TString& versus="Ntrk")
{
  cout << "Plotting resolution " << coordinate << " " << versus << endl;

  TH2F* h_Vrt_pullVsSomething_split(0);
  TH2F* h_Vrt_err_vs_Something(0);
  TH2F* h_Vrt_Tag_err_vs_Something(0);
  TString xAxisLabel("");
  if (versus == "Ntrk")
  {
    h_Vrt_pullVsSomething_split = (TH2F*)gDirectory->Get("Vrt_"+coordinate+"pullVsNtrkAverage_split");
    h_Vrt_err_vs_Something      = (TH2F*)gDirectory->Get("Vrt_"+coordinate+"err_vs_ntrk");
    h_Vrt_Tag_err_vs_Something      = (TH2F*)gDirectory->Get("Vrt_Tag_"+coordinate+"err_vs_ntrk");
    xAxisLabel = "Number of fitted tracks";
  } else if (versus == "SumPt2")
  {
    h_Vrt_pullVsSomething_split = (TH2F*)gDirectory->Get("Vrt_"+coordinate+"pullVsPt2Average_split");
    h_Vrt_err_vs_Something      = (TH2F*)gDirectory->Get("Vrt_"+coordinate+"err_vs_pt2");
    h_Vrt_Tag_err_vs_Something      = (TH2F*)gDirectory->Get("Vrt_Tag_"+coordinate+"err_vs_pt2");
    xAxisLabel = "#sqrt{#sum p_{T}^{2}} [GeV]";
  } else return;

  if (h_Vrt_pullVsSomething_split == 0) std::cout << "h_Vrt_pullVsSomething_split has zero pointer!" << std::endl;
  if (h_Vrt_err_vs_Something == 0) std::cout << "h_Vrt_err_vs_Something has zero pointer!" << std::endl;
  if (h_Vrt_pullVsSomething_split == 0 or h_Vrt_err_vs_Something == 0) return;

  int n_bins   = h_Vrt_pullVsSomething_split->GetNbinsX();
  std::vector<float> rms_z;
  std::vector<float> rms_z_er;
  std::vector<float> sigma_z;
  std::vector<float> sigma_z_er;
  std::vector<float> bins_z_nt;
  std::vector<float> bins_z_nt_er;
  
//   TCanvas* c2 = new TCanvas("Validate pulls ("+coordinate+", "+versus+")", "Validate pulls ("+coordinate+", "+versus+")", 1000, 800);
//   c2->Divide(4,4);

  // root starts counting the bins at 1, i.e. bin 1 holds NTrk = 0. or sqrt(sumpt2) = 0. - 0.25. GeV
//   std::cout << "n bins: " << n_bins << "\tTotal entries: " << h_Vrt_pullVsSomething_split->GetEntries() << std::endl;
  TH1D * nTrksPerVertex = h_Vrt_pullVsSomething_split->ProjectionX("projectionNTrks_"+coordinate+"_"+versus);

  TH1D * profileZFull = h_Vrt_pullVsSomething_split->ProjectionY("projectionPullsFull_"+coordinate+"_"+versus);

  Int_t startBin = 0;
  TH1D *profileZ = 0;
  const Int_t minEntriesForKFactorBin = 1000;
  for(int bin_count = 1; bin_count < n_bins+1; bin_count++)
  {
    //Fixed binning
//     TH1D *profileZ = h_Vrt_pullVsSomething_split->ProjectionY("projectionPulls", bin_count, bin_count,"e");
//     Double_t binCenter = h_Vrt_pullVsSomething_split->GetXaxis()->GetBinCenter(bin_count);
//     Double_t binWidth = h_Vrt_pullVsSomething_split->GetXaxis()->GetBinWidth(bin_count)/2.;

    //Variable binning
    TH1D *profileZTmp = h_Vrt_pullVsSomething_split->ProjectionY("projectionPulls", bin_count, bin_count,"e");
    //cout << "Bin: " << bin_count << ", Entries: " << profileZTmp->GetEntries() << endl;
    if (profileZ == 0) {
      startBin = bin_count;
      profileZ = (TH1D*) profileZTmp->Clone("projectionPulls_Integrated");
      //cout << "StartBin = " << startBin << endl;
    } else {
      profileZ->Add(profileZTmp);
    }
    delete profileZTmp;
    profileZTmp = 0;
    if ((profileZ->GetEntries() < minEntriesForKFactorBin) && (bin_count < n_bins)) //not enough entries, not last bin
      continue;

    Double_t lowEdge = h_Vrt_pullVsSomething_split->GetXaxis()->GetBinLowEdge(startBin);
    Double_t highEdge = h_Vrt_pullVsSomething_split->GetXaxis()->GetBinLowEdge(bin_count) + h_Vrt_pullVsSomething_split->GetXaxis()->GetBinWidth(bin_count);
    Double_t binCenter = (lowEdge + highEdge) / 2;
    Double_t binWidth = (highEdge - lowEdge) / 2; //half of the bin width
    //cout << "Bin done: " << binCenter << " +/- " << binWidth << ", Entries: " << profileZ->GetEntries() << endl;
    // variable binning end

    bins_z_nt.push_back(binCenter);
    bins_z_nt_er.push_back(binWidth); // dummy error of binwidth for now

    rms_z.push_back(profileZ->GetRMS());
    rms_z_er.push_back(profileZ->GetRMSError());

    //making a gaussian fit if there is anough entries
    if(profileZ->GetEntries() > 100.)
    {
      std::vector<float> fit_res = stableGaussianFit(profileZ);
      sigma_z.push_back(fit_res[0]);
      sigma_z_er.push_back(fit_res[1]);
    }else{
      sigma_z.push_back(0.);
      sigma_z_er.push_back(0.);
    }//end of good number of bins selection

//    c2->cd(1);
//    profileZFull->Draw();
//    unsigned int divide(10);
//    if (bin_count%divide == 0 and bin_count/divide < 16)
//    {
//      std::stringstream out; out << bin_count; TString binstring = TString(out.str()); // just to convert bin_count into a TString ...
//      c2->cd((int)(bin_count/divide)+1);
//      // one needs to copy the profile with different name because it always has the same name in the loop
//      TH1D * profileZNew = (TH1D*)profileZ->Clone(coordinate+versus+binstring);
//      profileZNew->Draw();
//      std::cout << "Bin: " << bin_count << "\tFrom: " << h_Vrt_pullVsSomething_split->GetXaxis()->GetBinLowEdge(bin_count)
//          << "\tTo: " << h_Vrt_pullVsSomething_split->GetXaxis()->GetBinUpEdge(bin_count)
//          << "\tCenter: " << h_Vrt_pullVsSomething_split->GetXaxis()->GetBinCenter(bin_count)
//          << "\tEntries in that bin: " << profileZ->GetEntries()
//          << "\tSigma gaus fit: " << sigma_z.at(bin_count-1)
//          << "\tErr on Sigma gaus fit: " << sigma_z_er.at(bin_count-1)
//          << std::endl;
//    }

    delete profileZ; // must keep this to delete the projection from memory (next one has same name!)
    profileZ = 0;
  }//end of loop over all the ntrk bins

  TGraphErrors * krms_z_vs_ntrk = new TGraphErrors(bins_z_nt.size(), &(bins_z_nt[0]),&(rms_z[0]),&(bins_z_nt_er[0]), &(rms_z_er[0]) );
  krms_z_vs_ntrk->GetYaxis()->SetTitle(coordinate+" scale factor from RMS");
  krms_z_vs_ntrk->GetXaxis()->SetTitle(xAxisLabel);
  krms_z_vs_ntrk->SetTitle("scaleFactor"+coordinate+"_RMS");
  krms_z_vs_ntrk->SetName("scaleFactor"+coordinate+"_"+versus+"_RMS");
//   std::cout << "krms_z_vs_ntrk has " << krms_z_vs_ntrk->GetN() << " bins." << std::endl;

  TGraphErrors * kgs_z_vs_ntrk = new TGraphErrors(bins_z_nt.size(), &(bins_z_nt[0]),&(sigma_z[0]),&(bins_z_nt_er[0]), &(sigma_z_er[0]) );
  kgs_z_vs_ntrk->GetYaxis()->SetTitle(coordinate+" scale factor from gauss fit");
  kgs_z_vs_ntrk->GetXaxis()->SetTitle(xAxisLabel);
  kgs_z_vs_ntrk->SetTitle("scaleFactor"+coordinate+"_Fit");
  kgs_z_vs_ntrk->SetName("scaleFactor_"+coordinate+"_"+versus+"_Fit");
//   std::cout << "kgs_z_vs_ntrk has " << kgs_z_vs_ntrk->GetN() << " bins." << std::endl;

// approximating the graph with 2nd order polynomial.
  float maxFitRange(100.);
  float minFitRange(2.);
  if (versus == "SumPt2") {
    minFitRange = 0.5;
    maxFitRange = 20.;
  }
  TF1 *kgs_z_ntrk_fit;
  Double_t *kgs_z_ntrk_fit_er;
  if (fitResKFactorMethod == 1) {
    //Fit with a pol2
    kgs_z_vs_ntrk->Fit("pol2","Q","",minFitRange,maxFitRange);
    kgs_z_vs_ntrk->GetFunction("pol2")->SetLineColor(kRed);
    kgs_z_ntrk_fit = kgs_z_vs_ntrk->GetFunction("pol2");
    kgs_z_ntrk_fit_er = kgs_z_ntrk_fit->GetParErrors();
  } else if (fitResKFactorMethod == 2) {
    //Fit with a pol1
    kgs_z_vs_ntrk->Fit("pol1","Q","",minFitRange,maxFitRange);
    kgs_z_vs_ntrk->GetFunction("pol1")->SetLineColor(kRed);
    kgs_z_ntrk_fit = kgs_z_vs_ntrk->GetFunction("pol1");
    kgs_z_ntrk_fit_er = kgs_z_ntrk_fit->GetParErrors();
  } else if (fitResKFactorMethod == 3) {
    TF1 *kgsFitFcn = new TF1("kgsFitFcn", scaleFactorFitFcn, minFitRange, maxFitRange,3);
    kgsFitFcn->SetParameter(0, minFitRange);
    kgsFitFcn->SetParameter(1, 1.0);
    kgsFitFcn->SetParameter(2, 1.0);
    for (int ifit=0; ifit < 1; ifit++) //initial estimation of best parameters
      kgs_z_vs_ntrk->Fit(kgsFitFcn, "Q"); 
    kgs_z_vs_ntrk->Fit(kgsFitFcn, "Q"); //perform actual fit
    kgs_z_ntrk_fit = kgsFitFcn;
    kgs_z_ntrk_fit_er = kgsFitFcn->GetParErrors();
    cout << "ScaleFactor fit for " << coordinate << " vs " << versus << " (method " << fitResKFactorMethod << ")= " 
	 << kgsFitFcn->GetParameter(0) << " +/- " << kgsFitFcn->GetParError(0) << " "
	 << kgsFitFcn->GetParameter(1) << " +/- " << kgsFitFcn->GetParError(1) << " "
	 << kgsFitFcn->GetParameter(2) << " +/- " << kgsFitFcn->GetParError(2) << endl;
  } else if (fitResKFactorMethod == 4) {
    //constant fit
    kgs_z_vs_ntrk->Fit("pol0","Q","",minFitRange,maxFitRange);
    kgs_z_vs_ntrk->GetFunction("pol0")->SetLineColor(kRed);
    kgs_z_ntrk_fit = kgs_z_vs_ntrk->GetFunction("pol0");
    kgs_z_ntrk_fit_er = kgs_z_ntrk_fit->GetParErrors();
    cout << "ScaleFactor fit for " << coordinate << " vs " << versus << " (method " << fitResKFactorMethod << ")= " 
	 << kgs_z_ntrk_fit->GetParameter(0) << " +/- " << kgs_z_ntrk_fit->GetParError(0) << endl;
  }

// plotting the fit error of the unconstrained primary vertex and correcting them
  int nbins_z_err_ntrk = h_Vrt_err_vs_Something->GetNbinsX();

  std::vector<float> av_err_z;
  std::vector<float> av_err_z_er;
  std::vector<float> av_err_tag_z;
  std::vector<float> av_err_tag_z_er;
  std::vector<float> err_bins_z_nt;
  std::vector<float> err_bins_z_nt_er;
  std::vector<float> res_z;
  std::vector<float> res_z_er;
  std::vector<float> res_tag_z;
  std::vector<float> res_tag_z_er;

  for(int bin_count = 1; bin_count <= nbins_z_err_ntrk; ++bin_count)
  {
    err_bins_z_nt.push_back(h_Vrt_err_vs_Something->GetXaxis()->GetBinCenter(bin_count));
    err_bins_z_nt_er.push_back(h_Vrt_err_vs_Something->GetXaxis()->GetBinWidth(bin_count)/2.);

    TH1D * profileY = h_Vrt_err_vs_Something->ProjectionY("projectionErrors",bin_count,bin_count,"e");
    TH1D * profileYTag(0);
    if (h_Vrt_Tag_err_vs_Something)
       profileYTag = h_Vrt_Tag_err_vs_Something->ProjectionY("projectionErrorsTag",bin_count,bin_count,"e");

    float mean       = profileY->GetMean();
    float mean_error = profileY->GetMeanError();
    float meanTag(0); 
    float mean_errorTag(0);
    if (profileYTag) {
      meanTag = profileYTag->GetMean();
      mean_errorTag = profileYTag->GetMeanError();
    }
    delete profileY;
    delete profileYTag;

    av_err_z.push_back(mean);
    av_err_z_er.push_back(mean_error);
    av_err_tag_z.push_back(meanTag);
    av_err_tag_z_er.push_back(mean_errorTag);

  //estimating the approximate k-factor and the error value
    double pr_er  =  0.0;
    if (fitResKFactorMethod == 1) {
      pr_er = error_func( bin_count, kgs_z_ntrk_fit_er );
    } else if (fitResKFactorMethod == 2) {
      float val = h_Vrt_err_vs_Something->GetXaxis()->GetBinCenter(bin_count);
      pr_er = TMath::Power(kgs_z_ntrk_fit_er[1]*val, 2) + TMath::Power(kgs_z_ntrk_fit_er[0], 2);
      pr_er = TMath::Sqrt(pr_er); 
      //cout << "val = " << val << ", pr_er = " << pr_er << ", p0er = " << kgs_z_ntrk_fit_er[0] << ", p1er = "<< kgs_z_ntrk_fit_er[1] << endl;
    } else if (fitResKFactorMethod == 3) {
      float val = h_Vrt_err_vs_Something->GetXaxis()->GetBinCenter(bin_count);
      //approximately the error on the plateau
      pr_er = kgs_z_ntrk_fit_er[2];
    } else if (fitResKFactorMethod == 4) {
      pr_er = kgs_z_ntrk_fit_er[0];
    }
//     std::cout << bin_count << "\t" << h_Vrt_err_vs_Something->GetXaxis()->GetBinCenter(bin_count) << "\t" << kgs_z_ntrk_fit->Eval(h_Vrt_err_vs_Something->GetXaxis()->GetBinCenter(bin_count)) << std::endl;
    res_z.push_back(mean * kgs_z_ntrk_fit->Eval(h_Vrt_err_vs_Something->GetXaxis()->GetBinCenter(bin_count)));
    res_z_er.push_back(TMath::Sqrt(TMath::Power(mean_error * kgs_z_ntrk_fit->Eval(h_Vrt_err_vs_Something->GetXaxis()->GetBinCenter(bin_count)),2) + TMath::Power( pr_er * mean ,2)));
    res_tag_z.push_back(meanTag * kgs_z_ntrk_fit->Eval(h_Vrt_err_vs_Something->GetXaxis()->GetBinCenter(bin_count)));
    res_tag_z_er.push_back(TMath::Sqrt(TMath::Power(mean_errorTag * kgs_z_ntrk_fit->Eval(h_Vrt_err_vs_Something->GetXaxis()->GetBinCenter(bin_count)),2) + TMath::Power( pr_er * mean ,2)));
//     res_z_er.push_back(0.);
  }
  TGraphErrors * res_z_vs_ntrk = new TGraphErrors(err_bins_z_nt.size(), &(err_bins_z_nt[0]),&(res_z[0]),&(err_bins_z_nt_er[0]), &(res_z_er[0]) );
  res_z_vs_ntrk->GetYaxis()->SetTitle(coordinate+" Vertex Resolution [mm]");
  res_z_vs_ntrk->GetXaxis()->SetTitle(xAxisLabel);
  res_z_vs_ntrk->SetTitle(coordinate+" Vertex Resolution");
  res_z_vs_ntrk->SetName("resolution_"+coordinate+"_"+versus);

  TGraphErrors * res_tag_z_vs_ntrk = new TGraphErrors(err_bins_z_nt.size(), &(err_bins_z_nt[0]),&(res_tag_z[0]),&(err_bins_z_nt_er[0]), &(res_tag_z_er[0]) );
  res_tag_z_vs_ntrk->GetYaxis()->SetTitle(coordinate+" Tagged Vertex Resolution [mm]");
  res_tag_z_vs_ntrk->GetXaxis()->SetTitle(xAxisLabel);
  res_tag_z_vs_ntrk->SetTitle(coordinate+" Tagged Vertex Resolution");
  res_tag_z_vs_ntrk->SetName("resolution_tag_"+coordinate+"_"+versus);

  TGraphErrors * mean_err_z_vs_ntrk = new TGraphErrors(err_bins_z_nt.size(), &(err_bins_z_nt[0]),&(av_err_z[0]),&(err_bins_z_nt_er[0]), &(av_err_z_er[0]) );
  mean_err_z_vs_ntrk->GetYaxis()->SetTitle(coordinate+" mean vertex error [mm]");
  mean_err_z_vs_ntrk->GetXaxis()->SetTitle(xAxisLabel);
  mean_err_z_vs_ntrk->SetTitle(coordinate+" Mean Vertex Error");
  mean_err_z_vs_ntrk->SetName("resolution_"+coordinate+"_"+versus+"_Uncorrected"); //not corrected with k-factor

  ///////////////
  // DRAWING
  ///////////////
  TCanvas* c1 = new TCanvas(coordinate+"_resolution_vs_"+versus, coordinate+" resolution vs "+versus, 1200,600);
  c1->Divide(3,2);
  c1->cd(1);
  h_Vrt_pullVsSomething_split->Draw("COLZ");
  c1->cd(2);
  c1->cd(2)->SetLogy();
  h_Vrt_err_vs_Something->Draw("COLZ");
  c1->cd(3);
  c1->cd(3)->SetLogy();
  c1->cd(3)->SetGrid();
  if (versus == "Ntrk") res_z_vs_ntrk->GetXaxis()->SetRangeUser(0.,100.);
  else res_z_vs_ntrk->GetXaxis()->SetRangeUser(0.,20.);
  res_z_vs_ntrk->GetYaxis()->SetRangeUser(0.0025,1.);
  res_z_vs_ntrk->Draw("AP");
  mean_err_z_vs_ntrk->SetLineColor(2);
  mean_err_z_vs_ntrk->Draw("SAMEP");
  c1->cd(4);
  if (versus == "Ntrk") krms_z_vs_ntrk->GetXaxis()->SetRangeUser(0.,100.);
  else krms_z_vs_ntrk->GetXaxis()->SetRangeUser(0.,20.);
  krms_z_vs_ntrk->GetYaxis()->SetRangeUser(0.5,1.3);
  krms_z_vs_ntrk->Draw("AP");
  c1->cd(5);
  if (versus == "Ntrk") kgs_z_vs_ntrk->GetXaxis()->SetRangeUser(0.,100.);
  else kgs_z_vs_ntrk->GetXaxis()->SetRangeUser(0.,20.);
  kgs_z_vs_ntrk->GetYaxis()->SetRangeUser(0.5,1.3);
  kgs_z_vs_ntrk->Draw("AP");
  c1->cd(6);
  nTrksPerVertex->GetYaxis()->SetTitle("Entries");
  nTrksPerVertex->Draw();

  if (outROOTFile) {
    //save and print results
    TString outName=(TString("Resolution_")+coordinate+TString("_")+versus+TString(".gif"));
    cout << "Saving " << outName.Data() << endl;
    h_Vrt_pullVsSomething_split->Write(); PrintH(h_Vrt_pullVsSomething_split);
    h_Vrt_err_vs_Something->Write(); PrintH(h_Vrt_err_vs_Something);
    if (h_Vrt_Tag_err_vs_Something)
      h_Vrt_Tag_err_vs_Something->Write(); PrintH(h_Vrt_Tag_err_vs_Something);
    nTrksPerVertex->Write(); PrintH(nTrksPerVertex);
    krms_z_vs_ntrk->Write(); PrintH(krms_z_vs_ntrk);
    kgs_z_vs_ntrk->Write(); PrintH(kgs_z_vs_ntrk);
    res_z_vs_ntrk->Write(); PrintH(res_z_vs_ntrk);
    mean_err_z_vs_ntrk->Write(); PrintH(mean_err_z_vs_ntrk);
    c1->Write();
    c1->Print(outName.Data());
    res_tag_z_vs_ntrk->Write(); PrintH(res_tag_z_vs_ntrk);
  }

  return;
}

std::vector<float> stableGaussianFit(TH1 * histo)
{
  std::vector<float> results;
  if(histo)
  {
    double sigmas  = 2.;

    histo->Fit("gaus","Q0","",-2,2);
    TF1* func = histo->GetFunction("gaus");
    double actualSigma=func->GetParameter(2);
    double actualSigmaErr=func->GetParError(2);

    for (int u=0;u<10;u++)
    {
      histo->Fit("gaus","Q0","",-sigmas*actualSigma,sigmas*actualSigma);
      func = histo->GetFunction("gaus");
      actualSigma    = func->GetParameter(2);
      actualSigmaErr = func->GetParError(2);
    }//end of the fitting loop

    results.push_back(actualSigma);
    results.push_back(actualSigmaErr);
  }else{
    results.push_back(0.);
    results.push_back(0.);
  }//end of protection against an empty histogram

  return results;
}//end of stableGaussian Fit function

double error_func( float  x, Double_t *par)
{
//calculating the square of the propagated error on the fit values
  return  ( TMath::Power(par[0],2) +  x * TMath::Power(par[1],2) +   TMath::Power(x * par[2],2)) ;
}

double scaleFactorFitFcn(double *x, double *par)
{
  // Truncated gaus-smeared step function
  // par 0: mean of turn-on (and truncation point)
  // par 1: slope of turn-on
  // par 2: plateau
  return (x[0]>=par[0])*TMath::Erf(par[1]*x[0] - par[0])*par[2];
}

// Reconstruction efficiency
void plotEfficiency()
{
  cout << "Plotting efficiency" << endl;

  TH1F *h_Vrt_split_tag_ntrk(0);
  TH1F *h_Vrt_split_probe_ntrk(0);
  TH1F *h_Vrt_split_matched_tag_ntrk(0);
  TH1F *h_Vrt_split_matched_probe_ntrk(0);
  TH1F *h_Vrt_split_dist_tag(0);
  TH1F *h_Vrt_split_dist_probe(0);

  h_Vrt_split_tag_ntrk = (TH1F*)gDirectory->Get("Vrt_split_tag_ntrk");
  h_Vrt_split_probe_ntrk = (TH1F*)gDirectory->Get("Vrt_split_probe_ntrk");
  h_Vrt_split_matched_tag_ntrk = (TH1F*)gDirectory->Get("Vrt_split_matched_tag_ntrk");
  h_Vrt_split_matched_probe_ntrk = (TH1F*)gDirectory->Get("Vrt_split_matched_probe_ntrk");
  h_Vrt_split_dist_tag = (TH1F*)gDirectory->Get("Vrt_split_dist_tag");
  h_Vrt_split_dist_probe = (TH1F*)gDirectory->Get("Vrt_split_dist_probe");

  /*
  std::vector<float> recEff_x;
  std::vector<float> recEff_xerr;
  std::vector<float> recEff_y;
  std::vector<float> recEff_yerr_up;
  std::vector<float> recEff_yerr_down;
  std::vector<float> selEff_y;
  std::vector<float> selEff_yerr_up;
  std::vector<float> selEff_yerr_down;

  //Method 1: efficiency = N(tag&probe) / N(tag)
  for (int nb=1; nb <= h_Vrt_split_tag_ntrk->GetNbinsX(); nb++) {  
    Int_t nTrk = nb - 1; //first bin is 0 tracks. 1 bin is 1 in track multiplicity
    recEff_x.push_back(nTrk);
    recEff_xerr.push_back(0.5);
    //calculate reconstruction efficiency
    float N1 = h_Vrt_split_tag_ntrk->GetBinContent(nb);
    float N2 = h_Vrt_split_probe_ntrk->GetBinContent(nb);
    double eff, effErrUp, effErrDown;
    eff = effErrUp = effErrDown = 0.0;
    calculateEfficiencyAsymErrors(eff, effErrUp, effErrDown, N2, N1);
    recEff_y.push_back(eff);
    recEff_yerr_up.push_back(effErrUp);      
    recEff_yerr_down.push_back(effErrDown);      
    // calculate selection efficiency
    N1 = h_Vrt_split_matched_tag_ntrk->GetBinContent(nb);
    N2 = h_Vrt_split_matched_probe_ntrk->GetBinContent(nb);
    eff = effErrUp = effErrDown = 0.0;
    calculateEfficiencyAsymErrors(eff, effErrUp, effErrDown, N2, N1);
    selEff_y.push_back(eff);
    selEff_yerr_up.push_back(effErrUp);      
    selEff_yerr_down.push_back(effErrDown);      
  }

  //create TGraphs for efficiency drawing
  TGraphAsymmErrors *g_Vrt_rec_eff_m1_split_vs_ntrk = new TGraphAsymmErrors(h_Vrt_split_tag_ntrk->GetNbinsX(), 
									    &(recEff_x[0]), &(recEff_y[0]), 
									    &(recEff_xerr[0]), &(recEff_xerr[0]),
									    &(recEff_yerr_down[0]), &(recEff_yerr_up[0]));   
  g_Vrt_rec_eff_m1_split_vs_ntrk->SetName("g_RecEff_M1");
  TGraphAsymmErrors *g_Vrt_sel_eff_m1_split_vs_ntrk = new TGraphAsymmErrors(h_Vrt_split_tag_ntrk->GetNbinsX(), 
									    &(recEff_x[0]), &(selEff_y[0]), 
									    &(recEff_xerr[0]), &(recEff_xerr[0]),
									    &(selEff_yerr_down[0]), &(selEff_yerr_up[0]));   
  g_Vrt_sel_eff_m1_split_vs_ntrk->SetName("g_SelEff_M1");
  */
  // Use BayesDivide routing of TGraphAsymmErrors
  TGraphAsymmErrors *g_Vrt_rec_eff_m1_split_vs_ntrk = new TGraphAsymmErrors();
  g_Vrt_rec_eff_m1_split_vs_ntrk->BayesDivide(h_Vrt_split_probe_ntrk, h_Vrt_split_tag_ntrk);
  g_Vrt_rec_eff_m1_split_vs_ntrk->SetName("g_RecEff_M1");

  TGraphAsymmErrors *g_Vrt_sel_eff_m1_split_vs_ntrk = new TGraphAsymmErrors();  
  g_Vrt_sel_eff_m1_split_vs_ntrk->BayesDivide(h_Vrt_split_matched_probe_ntrk, h_Vrt_split_matched_tag_ntrk);
  g_Vrt_sel_eff_m1_split_vs_ntrk->SetName("g_SelEff_M1");
  
  // Drawing
  TCanvas *c_recEff_M1 = new TCanvas("c_recEff_M1", "Reconstruction Efficiency", 800, 600);
  g_Vrt_rec_eff_m1_split_vs_ntrk->GetHistogram()->GetXaxis()->SetTitle("Number of tracks");
  g_Vrt_rec_eff_m1_split_vs_ntrk->GetHistogram()->GetYaxis()->SetTitle("Reconstruction efficiency");
  g_Vrt_rec_eff_m1_split_vs_ntrk->SetMarkerStyle(20);
  g_Vrt_rec_eff_m1_split_vs_ntrk->Draw("AP");
  c_recEff_M1->Update();

  TCanvas *c_selEff_M1 = new TCanvas("c_selEff_M1", "Selection Efficiency", 800, 600);
  g_Vrt_sel_eff_m1_split_vs_ntrk->GetHistogram()->GetXaxis()->SetTitle("Number of tracks");
  g_Vrt_sel_eff_m1_split_vs_ntrk->GetHistogram()->GetYaxis()->SetTitle("Selection Efficiency");
  g_Vrt_sel_eff_m1_split_vs_ntrk->SetMarkerStyle(20);
  g_Vrt_sel_eff_m1_split_vs_ntrk->Draw("AP");
  c_selEff_M1->Update();

  TCanvas *c_recEffDebug = new TCanvas("c_recEffDebug", "Reconstruction Efficiency - Debug", 800, 600);
  c_recEffDebug->Divide(2,1);
  c_recEffDebug->cd(1);
  h_Vrt_split_tag_ntrk->Draw();
  c_recEffDebug->cd(2);
  h_Vrt_split_probe_ntrk->Draw();
  c_recEffDebug->Update();

  TCanvas *c_selEffDebug = new TCanvas("c_selEffDebug", "Selection Efficiency - Debug", 800, 600);
  c_selEffDebug->Divide(2,2);
  c_selEffDebug->cd(1);
  h_Vrt_split_matched_tag_ntrk->Draw();
  c_selEffDebug->cd(2);
  h_Vrt_split_matched_probe_ntrk->Draw();
  c_selEffDebug->cd(3);
  h_Vrt_split_dist_tag->Draw();
  c_selEffDebug->cd(4);
  h_Vrt_split_dist_probe->Draw();
  c_selEffDebug->Update();

  //save them
  if (outROOTFile) {
    cout << "Saving Efficiency plots" << endl;
    g_Vrt_rec_eff_m1_split_vs_ntrk->Write(); PrintH(g_Vrt_rec_eff_m1_split_vs_ntrk);
    c_recEff_M1->Write();
    g_Vrt_sel_eff_m1_split_vs_ntrk->Write(); PrintH(g_Vrt_sel_eff_m1_split_vs_ntrk);
    c_selEff_M1->Write();
    h_Vrt_split_tag_ntrk->Write(); PrintH(h_Vrt_split_tag_ntrk);
    h_Vrt_split_probe_ntrk->Write(); PrintH(h_Vrt_split_probe_ntrk);
    c_recEffDebug->Write();
    h_Vrt_split_matched_tag_ntrk->Write(); PrintH(h_Vrt_split_matched_tag_ntrk);
    h_Vrt_split_matched_probe_ntrk->Write(); PrintH(h_Vrt_split_matched_probe_ntrk);
    h_Vrt_split_dist_tag->Write(); PrintH(h_Vrt_split_dist_tag);
    h_Vrt_split_dist_probe->Write(); PrintH(h_Vrt_split_dist_probe);
    c_selEffDebug->Write();
    c_recEff_M1->Print("c_recEff_M1.gif");
    c_selEff_M1->Print("c_selEff_M1.gif");
    c_recEffDebug->Print("c_recEffDebug.gif");
    c_selEffDebug->Print("c_selEffDebug.gif");
  }
  return;
}

//Calculate efficiency with (binomial) errors
int calculateEfficiency(Double_t &eff, Double_t &effErr, const Int_t num, const Int_t den)
{
  if (den == 0) {
    eff = 0.0;
    effErr = 0.0;
    return -1; //return -1, so that the user knows it's not meaningful
  } else {
    //standard binomial errors
    eff = Double_t(num) / Double_t(den);
    effErr = TMath::Sqrt(eff*(1-eff)/Double_t(den));
    return 0;
  }
  return -2; //will never happen
}

//Calculate efficiency with (binomial) errors, handle extreme cases with Poisson
int calculateEfficiencyAsymErrors(Double_t &eff, Double_t &effErr_p, Double_t &effErr_m, const Int_t num, const Int_t den)
{
  if (num == 0) {
    if (den == 0) {
      //numerator and denominator == 0, cannot say anything
      eff = 0.0;
      effErr_m = 0.0;
      effErr_p = 1.0; //total ignorance
      return -1; //return -1, so that the user knows it's not meaningful
    } else {
      //numerator == 0, assume poisson denominator, 1 sigma single-side error
      eff = 0.0;
      effErr_p = 1.149 / Double_t(den); //only positive error. Poisson 68% limit.
      effErr_m = 0.0;
      return 0;
    }
  } else if (num == den) {
    //numerator == denominator != 0
    eff = 1.0;
    effErr_p = 0.0;
    effErr_m = 1.149 / Double_t(den); //only negative error. Poisson 68% limit.
    return 2; //only negative error set
  } else {
    //standard binomial symmetric errors.. we can do better..
    eff = Double_t(num) / Double_t(den);
    effErr_p = effErr_m = TMath::Sqrt(eff*(1-eff)/Double_t(den));
    return 0;
  }
  return -2; //will never happen
}

void plotKinematic()
{
  TH1F* h_ntrk = (TH1F*) gDirectory->Get("Vrt_Ntrk");
  TH1F* h_sumpt2 = (TH1F*) gDirectory->Get("Vrt_SumPt2");
  TH1F* h_tag_ntrk = (TH1F*) gDirectory->Get("Vrt_Tag_Ntrk");
  TH1F* h_tag_sumpt2 = (TH1F*) gDirectory->Get("Vrt_Tag_SumPt2");

  TH1F* h_bc_ntrk = (TH1F*) gDirectory->Get("VrtBC_Ntrk");
  TH1F* h_bc_sumpt2 = (TH1F*) gDirectory->Get("VrtBC_SumPt2");
  TH1F* h_bc_tag_ntrk = (TH1F*) gDirectory->Get("VrtBC_Tag_Ntrk");
  TH1F* h_bc_tag_sumpt2 = (TH1F*) gDirectory->Get("VrtBC_Tag_SumPt2");

  TH1F *h_2vtx_merge_deltaZ = (TH1F*)gDirectory->Get("h_2vtx_merge_deltaZ");
  TH1F *h_2vtx_merge_deltaZsig = (TH1F*)gDirectory->Get("h_2vtx_merge_deltaZsig");

  TCanvas *c_Kin1 = new TCanvas("c_Vrt_Kinematic");
  c_Kin1->Divide(2,2);
  c_Kin1->cd(1);
  h_ntrk->Draw();
  c_Kin1->cd(2);
  h_sumpt2->Draw();
  c_Kin1->cd(3);
  h_tag_ntrk->Draw();
  c_Kin1->cd(4);
  h_tag_sumpt2->Draw();
  c_Kin1->Update();

  TCanvas *c_Kin2 = new TCanvas("c_VrtBC_Kinematic");
  c_Kin2->Divide(2,2);
  c_Kin2->cd(1);
  h_bc_ntrk->Draw();
  c_Kin2->cd(2);
  h_bc_sumpt2->Draw();
  c_Kin2->cd(3);
  h_bc_tag_ntrk->Draw();
  c_Kin2->cd(4);
  h_bc_tag_sumpt2->Draw();
  c_Kin2->Update();

  TCanvas *c_mergeVtx = new TCanvas("c_mergeVtx");
  c_mergeVtx->Divide(2,1);
  c_mergeVtx->cd(1);
  h_2vtx_merge_deltaZ->Draw();
  c_mergeVtx->cd(2);
  h_2vtx_merge_deltaZsig->Draw();
  c_mergeVtx->Update();

  if (outROOTFile) {
    h_ntrk->Write(); PrintH(h_ntrk);
    h_sumpt2->Write(); PrintH(h_sumpt2);
    h_tag_ntrk->Write(); PrintH(h_tag_ntrk);
    h_tag_sumpt2->Write(); PrintH(h_tag_sumpt2);
    c_Kin1->Write();
    c_Kin1->Print("c_Vrt_Kinematic.gif");
    c_Kin2->Write();
    c_Kin2->Print("c_VrtBC_Kinematic.gif");
    h_bc_ntrk->Write(); PrintH(h_bc_ntrk);
    h_bc_sumpt2->Write(); PrintH(h_bc_sumpt2);
    h_bc_tag_ntrk->Write(); PrintH(h_bc_tag_ntrk);
    h_bc_tag_sumpt2->Write(); PrintH(h_bc_tag_sumpt2);
    c_mergeVtx->Write();
    h_2vtx_merge_deltaZ->Write();
    h_2vtx_merge_deltaZsig->Write();
  }
}

void PrintH(TH1 *h, TString fileName, TString drawOptions)
{
  if (!h)
    return;
  // Print creating a temporary canvas
  TString fn = fileName;
  if (fn == "") {
    fn = h->GetName();
    fn += ".gif";
  }
  TCanvas *c_printh = new TCanvas();
  h->Draw(drawOptions.Data());
  c_printh->Update();
  cout << "Saving " << fn.Data() << endl;
  c_printh->Print(fn.Data());
  delete c_printh;
}

void PrintH(TGraph *h, TString fileName, TString drawOptions)
{
  if (!h)
    return;
  // Print creating a temporary canvas
  TString fn = fileName;
  TString opt = drawOptions;
  if (fn == "") {
    fn = h->GetName();
    fn += ".gif";
  }
  if (opt == "") {
    opt = "AP"; //default options
  }
  TCanvas *c_printh = new TCanvas();
  h->Draw(opt.Data());
  c_printh->Update();
  cout << "Saving " << fn.Data() << endl;
  c_printh->Print(fn.Data());
  delete c_printh;
}

// the rest is taken from the ATLAS style wiki, and adjusted that it actually compiles ...
TStyle* AtlasStyle() 
{
  TStyle *atlasStyle = new TStyle("ATLAS","Atlas style");

  // use plain black on white colors
  Int_t icol = 0; // WHITE
  atlasStyle->SetFrameBorderMode(icol);
  atlasStyle->SetFrameFillColor(icol);
  atlasStyle->SetCanvasBorderMode(icol);
  atlasStyle->SetCanvasColor(icol);
  atlasStyle->SetPadBorderMode(icol);
  atlasStyle->SetPadColor(icol);
  atlasStyle->SetStatColor(icol);
  //atlasStyle->SetFillColor(icol); // don't use: white fill color for *all* objects

  // set the paper & margin sizes
  atlasStyle->SetPaperSize(20,26);

  // set margin sizes
  atlasStyle->SetPadTopMargin(0.05);
  atlasStyle->SetPadRightMargin(0.05);
  atlasStyle->SetPadBottomMargin(0.16);
  atlasStyle->SetPadLeftMargin(0.16);

  // set title offsets (for axis label)
  atlasStyle->SetTitleXOffset(1.4);
  atlasStyle->SetTitleYOffset(1.4);

  // use large fonts
  Int_t font=42; // Helvetica
  Double_t tsize=0.05;
  atlasStyle->SetTextFont(font);

  atlasStyle->SetTextSize(tsize);
  atlasStyle->SetLabelFont(font,"x");
  atlasStyle->SetTitleFont(font,"x");
  atlasStyle->SetLabelFont(font,"y");
  atlasStyle->SetTitleFont(font,"y");
  atlasStyle->SetLabelFont(font,"z");
  atlasStyle->SetTitleFont(font,"z");

  atlasStyle->SetLabelSize(tsize,"x");
  atlasStyle->SetTitleSize(tsize,"x");
  atlasStyle->SetLabelSize(tsize,"y");
  atlasStyle->SetTitleSize(tsize,"y");
  atlasStyle->SetLabelSize(tsize,"z");
  atlasStyle->SetTitleSize(tsize,"z");

  // use bold lines and markers
//   atlasStyle->SetMarkerStyle(20);
//   atlasStyle->SetMarkerSize(1.2);
  atlasStyle->SetHistLineWidth(2);
  atlasStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes

  // get rid of X error bars 
  //atlasStyle->SetErrorX(0.001);
  // get rid of error bar caps
  atlasStyle->SetEndErrorSize(0.);

  // do not display any of the standard histogram decorations
  atlasStyle->SetOptTitle(0);
//   atlasStyle->SetOptStat(111111);
  atlasStyle->SetOptStat(0);
//   atlasStyle->SetOptFit(1111111);
//  atlasStyle->SetOptFit(0);

  // put tick marks on top and RHS of plots
  atlasStyle->SetPadTickX(1);
  atlasStyle->SetPadTickY(1);

  atlasStyle->SetPalette(1);
  atlasStyle->SetPadRightMargin(15);

  return atlasStyle;
}

void SetAtlasStyle ()
{
   std::cout << "\nApplying ATLAS style settings...\n" << std::endl ;
   TStyle* atlasStyle = AtlasStyle();
   gROOT->SetStyle("ATLAS");
   gROOT->ForceStyle();
}

