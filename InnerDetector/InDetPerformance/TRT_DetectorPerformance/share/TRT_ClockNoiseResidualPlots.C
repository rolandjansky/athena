#define TRT_ClockNoiseResidualPlots_cxx
#include "TRT_ClockNoiseResidualPlots.h"

// ------------------------------------------------------------------------------------------------------------------
// ------------------------------- TRT_ClockNoiseResidualPlots.C ----------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------
//  - this is a macro for making plots of clock noise effect on residual distributions
//    - it takes a list of clock noisy straws, and groups them based on the phase of the noise
//    - then it makes the residual distributions for all hits on these straws, binned by Leading Edge bin
//    - it compares the mean and sigma of these to the average overall residual distribution mean and sigma
//
//  - the inputs to the macro are:
//    - an ntuple with hit timing information for hits on track, output from the athena alg TRT_DetectorTiming
//    - a text file with noisy straw info (one of two formats)
//      - the simpler format is the output of the athena alg TRT_ClockNoise and contains a list of noisy straws
//      - the more advanced version contains several noise parameters for all straws, and is the output of private code
//  - these can be specified by hardcoding them into the .h file (s_noisetextfile and s_default), or when running in root
//
//  - Usage: Compile in a ROOT session and run:
//      Root > .L TRT_ClockNoiseResidualPlots.C+
//      Root > TRT_ClockNoiseResidualPlots t("ntuple.root","noisystrawlist.dat")
//      Root > t.Loop();       // Loop on all entries
//
//  - based on a ROOT MakeClass() macro
//  - author: Dominick Olivito <olivito@cern.ch>
// ------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------


// ********************************************************************
TRT_ClockNoiseResidualPlots::TRT_ClockNoiseResidualPlots(TString s_file,TString s_textfile)
{
   s_noisetextfile = s_textfile;
  
   TTree* tree = 0;
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(s_file);
      if (!f) {
         f = new TFile(s_file);
      }
      tree = (TTree*)gDirectory->Get("ntuple");

   }
   Init(tree);
}

// ********************************************************************
TRT_ClockNoiseResidualPlots::~TRT_ClockNoiseResidualPlots()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t TRT_ClockNoiseResidualPlots::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}

// ********************************************************************
Long64_t TRT_ClockNoiseResidualPlots::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (!fChain->InheritsFrom(TChain::Class()))  return centry;
   TChain *chain = (TChain*)fChain;
   if (chain->GetTreeNumber() != fCurrent) {
      fCurrent = chain->GetTreeNumber();
      Notify();
   }
   return centry;
}

// ********************************************************************
void TRT_ClockNoiseResidualPlots::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("corLE", &corLE, &b_corLE);
   fChain->SetBranchAddress("EP", &EP, &b_EP);
   fChain->SetBranchAddress("TOT", &TOT, &b_TOT);
   fChain->SetBranchAddress("TEbin", &TEbin, &b_TEbin);
   fChain->SetBranchAddress("LEbin", &LEbin, &b_LEbin);
   fChain->SetBranchAddress("wrongSide", &wrongSide, &b_wrongSide);
   fChain->SetBranchAddress("board", &board, &b_board);
   fChain->SetBranchAddress("locR", &locR, &b_locR);
   fChain->SetBranchAddress("timeCorChip", &timeCorChip, &b_timeCorChip);
   fChain->SetBranchAddress("chip", &chip, &b_chip);
   fChain->SetBranchAddress("side", &side, &b_side);
   fChain->SetBranchAddress("t0", &t0, &b_t0);
   fChain->SetBranchAddress("phi", &phi, &b_phi);
   fChain->SetBranchAddress("straw", &straw, &b_straw);
   fChain->SetBranchAddress("strawLayer", &strawLayer, &b_strawLayer);
   fChain->SetBranchAddress("layer", &layer, &b_layer);
   fChain->SetBranchAddress("nBarHits", &nBarHits, &b_nBarHits);
   fChain->SetBranchAddress("nEcHits", &nEcHits, &b_nEcHits);
   fChain->SetBranchAddress("nTRThits_barrelA", &nTRThits_barrelA, &b_nTRThits_barrelA);
   fChain->SetBranchAddress("nTRThits_barrelC", &nTRThits_barrelC, &b_nTRThits_barrelC);
   fChain->SetBranchAddress("nTRThits_ecA", &nTRThits_ecA, &b_nTRThits_ecA);
   fChain->SetBranchAddress("nTRThits_ecC", &nTRThits_ecC, &b_nTRThits_ecC);
   fChain->SetBranchAddress("nSCThits", &nSCThits, &b_nSCThits);
   fChain->SetBranchAddress("nPixHits", &nPixHits, &b_nPixHits);
   fChain->SetBranchAddress("theta", &theta, &b_theta);
   fChain->SetBranchAddress("eta", &eta, &b_eta);
   fChain->SetBranchAddress("chi2ndf", &chi2ndf, &b_chi2ndf);
   fChain->SetBranchAddress("phi0", &phi0, &b_phi0);
   fChain->SetBranchAddress("z0", &z0, &b_z0);
   fChain->SetBranchAddress("d0", &d0, &b_d0);
   fChain->SetBranchAddress("tubeHit", &tubeHit, &b_tubeHit);
   fChain->SetBranchAddress("locRpat", &locRpat, &b_locRpat);
   Notify();
}

// ********************************************************************
Bool_t TRT_ClockNoiseResidualPlots::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

// ********************************************************************
void TRT_ClockNoiseResidualPlots::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}

// ********************************************************************
int TRT_ClockNoiseResidualPlots::initialize() {
  fout = new TFile(s_outfile,"RECREATE");

  fill_noise_map();
  book_histograms();
  
  cout << "-- initialize(): complete" << endl;
  
  return 0;
}

// ********************************************************************
int TRT_ClockNoiseResidualPlots::book_histograms() {
  
  ctemp = new TCanvas("c_temp","temp");
   
  //style settings for all histograms
  gStyle->SetOptStat(1110); // no name
  gStyle->SetOptFit(111); // only display fit param values and chi2
  
  // -------------------------------------------------------
  // plots for all hits on track
  
  hres = new TH1F("hres","residuals for all hits on track",rbins,-rmax,rmax);
  hres->GetXaxis()->SetTitle("residual [mm]");
  
  for (int ibin=0; ibin<24; ibin++) {
    hresLEbin[ibin] = new TH1F(Form("hresLEbin[%d]",ibin),Form("residuals for all hits on track with LEbin == %d",ibin),rbins,-rmax,rmax);
    hresLEbin[ibin]->GetXaxis()->SetTitle("residual [mm]");    
  }
  
  gresMean = new TGraphErrors(24);
  gresMean->SetTitle("residuals mean by time bin, all hits on track");
  gresMean->SetName("gresMean");
  
  gresSigma = new TGraphErrors(24);
  gresSigma->SetTitle("residuals sigma by time bin, all hits on track");
  gresSigma->SetName("gresSigma");  
  
  // -------------------------------------------------------
  // plots for hits on straws with 40MHz noise
  
  for (int iphase=0;iphase<PERIOD_40MHZ;iphase++) {
    
    hres40mhz[iphase] = new TH1F(Form("hres40mhz[%d]",iphase),Form("residuals for hits on straws with 40MHz noise, phase == %d",iphase),rbins,-rmax,rmax);
    hres40mhz[iphase]->GetXaxis()->SetTitle("residual [mm]");
  
    for (int ibin=0; ibin<24; ibin++) {
      hresLEbin40mhz[iphase][ibin] = new TH1F(Form("hresLEbin40mhz[%d][%d]",iphase,ibin),Form("residuals for hits on straws with 40MHz noise, phase == %d, with LEbin == %d",iphase,ibin),rbins,-rmax,rmax);
      hresLEbin40mhz[iphase][ibin]->GetXaxis()->SetTitle("residual [mm]");    
    }
  
    gresMean40mhz[iphase] = new TGraphErrors(24);
    gresMean40mhz[iphase]->SetTitle(Form("residuals mean by time bin, hits on straws with 40MHz noise, phase == %d",iphase));
    gresMean40mhz[iphase]->SetName(Form("gresMean40mhz[%d]",iphase));
  
    gresSigma40mhz[iphase] = new TGraphErrors(24);
    gresSigma40mhz[iphase]->SetTitle(Form("residuals sigma by time bin, hits on straws with 40MHz noise, phase == %d",iphase));
    gresSigma40mhz[iphase]->SetName(Form("gresSigma40mhz[%d]",iphase));  
        
    // these diff graphs get replaced later, so we need to title them in finalize()
    
    gdiffMean40mhz[iphase] = new TGraphErrors(24);
    gdiffSigma40mhz[iphase] = new TGraphErrors(24);

    gdiffMeanColor40mhz[iphase] = new TMultiGraph();
    gdiffSigmaColor40mhz[iphase] = new TMultiGraph();
        
    
  } // end 40mhz phase loop
  
  return 0;
}

// ********************************************************************
// do gaussian fits, etc for all relevant histograms here
int TRT_ClockNoiseResidualPlots::fit_all_histos() {
  
  gaus_fit(hres);
  gaus_fit(hresLEbin);
  for (int iphase=0;iphase<PERIOD_40MHZ;iphase++) {
    gaus_fit(hres40mhz[iphase]);
    gaus_fit(hresLEbin40mhz[iphase]);
  }
  
  return 0;
}

// ********************************************************************
// be sure to write graphs to the TFile here!
// also need to set labels for graphs here (after they have points in them), for some asinine reason...
int TRT_ClockNoiseResidualPlots::finalize() {
  
  // -------------------------------------------------------
  // plots for all hits on track
  
  gresMean->GetXaxis()->SetTitle("LEbin");
  gresMean->GetYaxis()->SetTitle("residual mean [mm]");
  gresMean->Write();  
  
  gresSigma->GetXaxis()->SetTitle("LEbin");
  gresSigma->GetYaxis()->SetTitle("residual sigma [mm]");
  gresSigma->Write();
  
  // -------------------------------------------------------
  // plots for hits on straws with 40MHz noise
  
  for (int iphase=0;iphase<PERIOD_40MHZ;iphase++) {
    gresMean40mhz[iphase]->GetXaxis()->SetTitle("LEbin");
    gresMean40mhz[iphase]->GetYaxis()->SetTitle("residual mean [mm]");
    gresMean40mhz[iphase]->Write();
          
    gresSigma40mhz[iphase]->GetXaxis()->SetTitle("LEbin");
    gresSigma40mhz[iphase]->GetYaxis()->SetTitle("residual sigma [mm]");
    gresSigma40mhz[iphase]->Write();
          
    // these diff plots were created outside of book_histograms(), 
    //   so give them titles and names here as well
    
    gdiffMean40mhz[iphase]->SetTitle(Form("residuals mean diff by time bin, hits on straws with 40MHz noise, phase == %d",iphase));
    gdiffMean40mhz[iphase]->SetName(Form("gdiffMean40mhz[%d]",iphase));
    gdiffMean40mhz[iphase]->GetXaxis()->SetTitle("LEbin");
    gdiffMean40mhz[iphase]->GetYaxis()->SetTitle("diff in residual mean [mm]");
    gdiffMean40mhz[iphase]->Write();
          
    gdiffSigma40mhz[iphase]->SetTitle(Form("residuals sigma diff by time bin, hits on straws with 40MHz noise, phase == %d",iphase));
    gdiffSigma40mhz[iphase]->SetName(Form("gdiffSigma40mhz[%d]",iphase));
    gdiffSigma40mhz[iphase]->GetXaxis()->SetTitle("LEbin");
    gdiffSigma40mhz[iphase]->GetYaxis()->SetTitle("diff in residual sigma [mm]");
    gdiffSigma40mhz[iphase]->Write();
          
    
    // for the TMultiGraphs, record the canvas to file instead of the graphs themselves
    //  not sure if this is necessary or optimal, but it might be.
    
    cdiffMeanColor40mhz[iphase] = new TCanvas(Form("cdiffMeanColor40mhz[%d]",iphase),Form("colored diff mean for 40MHz, phase %d",iphase));
    cdiffMeanColor40mhz[iphase]->cd();
    gdiffMeanColor40mhz[iphase]->SetTitle(Form("residuals mean diff by time bin, hits on straws with 40MHz noise, phase == %d",iphase));
    gdiffMeanColor40mhz[iphase]->SetName(Form("gdiffMeanColor40mhz[%d]",iphase));
    // need to draw before we can access the axes on a TMultiGraph -- otherwise we segfault!!
    gdiffMeanColor40mhz[iphase]->Draw("AP"); 
    gdiffMeanColor40mhz[iphase]->GetXaxis()->SetTitle("LEbin");
    gdiffMeanColor40mhz[iphase]->GetYaxis()->SetTitle("diff in residual mean [mm]");
    gdiffMeanColor40mhz[iphase]->Draw("AP"); 
    cdiffMeanColor40mhz[iphase]->Write();
    cdiffMeanColor40mhz[iphase]->Close();
          
    cdiffSigmaColor40mhz[iphase] = new TCanvas(Form("cdiffSigmaColor40mhz[%d]",iphase),Form("colored diff sigma for 40MHz, phase %d",iphase));
    cdiffSigmaColor40mhz[iphase]->cd();
    gdiffSigmaColor40mhz[iphase]->SetTitle(Form("residuals sigma diff by time bin, hits on straws with 40MHz noise, phase == %d",iphase));
    gdiffSigmaColor40mhz[iphase]->SetName(Form("gdiffSigmaColor40mhz[%d]",iphase));
    // need to draw before we can access the axes on a TMultiGraph -- otherwise we segfault!!
    gdiffSigmaColor40mhz[iphase]->Draw("AP");
    gdiffSigmaColor40mhz[iphase]->GetXaxis()->SetTitle("LEbin");
    gdiffSigmaColor40mhz[iphase]->GetYaxis()->SetTitle("diff in residual sigma [mm]");
    gdiffSigmaColor40mhz[iphase]->Draw("AP");
    cdiffSigmaColor40mhz[iphase]->Write();
    cdiffSigmaColor40mhz[iphase]->Close();
          
  } // end 40mhz phase loop

  
  // -------------------------------------------------------
  // write everything else to the TFile
  fout->Write();
  
  ctemp->Close();
  
  // if we close the TFile here, all the histograms disappear from memory
//  fout->Close();
  
  cout << "-- finalize(): complete" << endl
       << "   histograms written to file " << s_outfile << endl << endl;
  
  return 0;
}

// ********************************************************************
int TRT_ClockNoiseResidualPlots::fill_noise_map() {

   //first initialize the noise map arrays
  for (int i=0;i<nPhi;i++) {
    for (int j=0; j<2; j++) {
      for (int k=0; k<nTotStraw; k++) {
        noisy_straw_phase_map[i][j][k] = -1;
        noise_info_map[i][j][k] = NULL;
      }
    }
  }
  
  ifstream in;
  in.open(s_noisetextfile);
  int lines = 0;
  
  // read in a list of noisy straws to look at
  if (use_noisy_straw_list) {
    
    int phi,side,straw,frequency,phase,totalocc;
    
    while(1) {
    
      in >> phi >> side >> straw >> frequency >> phase >> totalocc;
      if (!in.good() || in.eof()) break;
    
      int AorC = 0;
      if (side > 0) AorC = 1;
      
      if (frequency == 40)
        noisy_straw_phase_map[phi][AorC][straw] = phase;
      
      lines++;
    }
    
    
  } else { // read in file with detailed info
  
    int phi,side,straw,strawLayer,layer,chip,board;
  
    while(1) {
      strawNoiseInfo* s = new strawNoiseInfo();
    
      in >> phi >> side >> straw >> strawLayer >> layer >> chip >> board
         >> s->noiseType >> s->phase80 >> s->asym80 >> s->phase40 >> s->asym40
         >> s->occbins >> s->totalocc;
      if (!in.good() || in.eof()) break;
    
      int AorC = 0;
      if (side > 0) AorC = 1;
    
      noise_info_map[phi][AorC][straw] = s;
      lines++;
    }
  
  }

  // check that the file contained something
  if (lines == 0) {
    cerr << "!! problem with file " << s_noisetextfile << endl;
    assert(lines > 0);
  }
      
  return 0;
}

// ********************************************************************
// put quality cuts on hits here
// !!! need some event phase cut
int TRT_ClockNoiseResidualPlots::cut_hit() {
  if ((TOT>7) && (TEbin<24) && (LEbin>0) && (LEbin<24) && (tubeHit<1))
    return 1;
  
  // failed the cut
  return -1;
}


// ********************************************************************
// select the noisy straws here
//  - if we're using a list of noisy straws, just check that this straw is on the list
//  - if we're using detailed info, impose a cut based on occupancy, asymmetry, etc
int TRT_ClockNoiseResidualPlots::cut_noise() {

  int AorC = 0;
  if (side > 0) AorC = 1;
  
  if (use_noisy_straw_list) { // noisy straw list
    if (noisy_straw_phase_map[phi][AorC][straw] > -1)
      return 1;
  } else { // detailed info
  
    strawNoiseInfo* s = noise_info_map[phi][AorC][straw];
  
    if ((s->asym40 > 0.99) && (s->totalocc >= min_occ)) 
      return 1;
  }
  
  // failed the cut
  return -1;
}

// ********************************************************************
// fit with a gaussian from -0.5 to 0.5
// for fitting residual distributions
void TRT_ClockNoiseResidualPlots::gaus_fit(TH1F* h) {
  h->Fit("gaus","Q","",-0.5,0.5);
  return;
}

// ********************************************************************
// overloaded version of gaus_fit for arrays of histograms
void TRT_ClockNoiseResidualPlots::gaus_fit(TH1F* h[24]) {
  for (int ibin=0;ibin<24;ibin++) {
    gaus_fit(h[ibin]);
  }
  return;
}

// ********************************************************************
// this will graph the residuals as a function of LEbin, given an output graph and a set of residual histos
// !! the histograms need to be fit with Gaussians before calling this function
void TRT_ClockNoiseResidualPlots::graph_residuals_vs_LEbin( TH1F* h[24], TGraphErrors* gMean, TGraphErrors* gSigma ) {
  for (int ibin=0; ibin<24; ibin++) {
    double mean,err,sigma,sigmaerr;
    
    // make sure there are at least some entries in the residual histo
    if (h[ibin]->GetEntries() >= min_entries) {
      TF1* myfunc = h[ibin]->GetFunction("gaus");
      mean = myfunc->GetParameter(1);
      sigma = myfunc->GetParameter(2);
      sigmaerr = myfunc->GetParError(2);
      err = sigma / sqrt(h[ibin]->GetEntries());
    } else { // too few entries: set the mean, sigma, errors to 0
      mean = 0.;
      err = 0.;
      sigma = 0.;
      sigmaerr = 0.;
    }
          
    gMean->SetPoint(ibin,ibin,mean);
    gMean->SetPointError(ibin,0.,err);
    
    gSigma->SetPoint(ibin,ibin,sigma);
    gSigma->SetPointError(ibin,0.,sigmaerr);
  }
  return;
}

// ********************************************************************
// subtracts two TGraphErrors: g - gref, and returns the resulting graph
TGraphErrors* TRT_ClockNoiseResidualPlots::subtract_graphs(TGraphErrors* g, TGraphErrors* gref) {
  
  TGraphErrors* gdiff = new TGraphErrors(24);
  
  for (int ibin=0; ibin<24; ibin++) {
    double xdummy = 0.;
    double y = 0.;
    double yref = 0.;
    double yerr = g->GetErrorY(ibin);
    double yreferr = gref->GetErrorY(ibin);
    g->GetPoint(ibin,xdummy,y);
    gref->GetPoint(ibin,xdummy,yref);
    double ynew = y - yref;
    double ynewerr = sqrt(yerr*yerr+yreferr*yreferr);

    // if yerr == 0 or yreferr == 0, the point is bogus
    // set the values to 0 here as well
    if ((yerr == 0.) || (yreferr == 0.)) {
      ynew = 0.;
      ynewerr = 0.;
    } 
    
    gdiff->SetPoint(ibin,ibin,ynew);
    gdiff->SetPointError(ibin,0.,ynewerr);
  }
  
  return gdiff;
}

// ********************************************************************
// colors the points on a graph to make periodic effects easier to see by eye
//   points separated by N*period are the same color
TMultiGraph* TRT_ClockNoiseResidualPlots::graph_with_colors(TGraphErrors* g, int period, int phase) {

  TMultiGraph* mg = new TMultiGraph();
  for (int i=0; i<period; i++) {
    int numbins = int(24/period);
    TGraphErrors* gPhase = new TGraphErrors(numbins);
    for (int j=0; j<numbins; j++) {
      int k = j*period + i;
      if (k > 23) std::cout << "!! graph_with_colors: bin index k == " << k << std::endl;
      double x = 0.;
      double y = 0.;
      g->GetPoint(k,x,y);
      double erry = g->GetErrorY(k);
      gPhase->SetPoint(j,x,y);
      gPhase->SetPointError(j,0.,erry);
    }
    gPhase->SetLineColor((period-phase+i)%period+1);
    mg->Add(gPhase,"P");
  }
     
  return mg;
}

// ********************************************************************
void TRT_ClockNoiseResidualPlots::Loop(bool noisy_straw_list)
{

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//

  use_noisy_straw_list = noisy_straw_list;
  
  if (use_noisy_straw_list) {
    cout << "-- will read in noisy straw list from file: " << s_noisetextfile << endl;
  } else {
    cout << "-- will read in detailed noise info from file: " << s_noisetextfile << endl;
  }

  initialize();

  if (fChain == 0) return;

  Long64_t nentries = fChain->GetEntriesFast();

  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
     Long64_t ientry = LoadTree(jentry);
     if (ientry < 0) break;
     nb = fChain->GetEntry(jentry);   nbytes += nb;
     
     // cut on hit/track quality first
     if (cut_hit() < 0) continue;
     
     hres->Fill(locR - locRpat);
     hresLEbin[LEbin]->Fill(locR - locRpat);
     
     // select out only the noise we want
     if (cut_noise() < 0) continue;
     
     int AorC = 0;
     if (side > 0) AorC = 1;
     
     int phase = -1;
     
     if (use_noisy_straw_list) {
       phase = noisy_straw_phase_map[phi][AorC][straw];
     } else {
       phase = noise_info_map[phi][AorC][straw]->phase40;
     }
     
     hres40mhz[phase]->Fill(locR - locRpat);
     hresLEbin40mhz[phase][LEbin]->Fill(locR - locRpat);
  } // loop over ntuple
  
  cout << "-- Loop(): finished loop over ntuple" << endl;
  
  fit_all_histos();
  graph_residuals_vs_LEbin(hresLEbin,gresMean,gresSigma);
  
  for (int iphase = 0;iphase<PERIOD_40MHZ;iphase++) {
    
    graph_residuals_vs_LEbin(hresLEbin40mhz[iphase],gresMean40mhz[iphase],gresSigma40mhz[iphase]);
    gdiffMean40mhz[iphase] = subtract_graphs(gresMean40mhz[iphase],gresMean);
    gdiffSigma40mhz[iphase] = subtract_graphs(gresSigma40mhz[iphase],gresSigma);
    
    gdiffMeanColor40mhz[iphase] = graph_with_colors(gdiffMean40mhz[iphase],PERIOD_40MHZ,iphase);
    gdiffSigmaColor40mhz[iphase] = graph_with_colors(gdiffSigma40mhz[iphase],PERIOD_40MHZ,iphase);
    
  } // end loop over 40mhz phases
    
  cout << "-- Loop(): finished making plots" << endl;  
  
  finalize();
}
