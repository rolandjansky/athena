//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Oct 22 10:24:41 2008 by ROOT version 5.18/00d
// from TTree ntuple/ntuple
// found on file: timinginfo.root
//////////////////////////////////////////////////////////

#ifndef TRT_ClockNoiseResidualPlots_h
#define TRT_ClockNoiseResidualPlots_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

#include "Riostream.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <ctime>
#include <vector>
#include <fstream>

#include <TH1.h>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TProfile.h>
#include "Riostream.h"
#include <TGraphErrors.h>
#include <TF1.h>
#include <THStack.h>
#include <TLegend.h>
#include <TMultiGraph.h>
#include <TNtuple.h>
#include <TPaveStats.h>

// detector constants
#define nPhi 32
#define nSide 5

#define nChipB 104
#define nBoardB 9
#define nStrawB 1642

#define nChipE 120
#define nBoardE 20
#define nStrawE 3840

// barrel/endcap designations
#define ecC -2
#define barC -1
#define barA 1
#define ecA 2

// period for each frequency of noise
#define PERIOD_40MHZ 8
#define PERIOD_80MHZ 4

// minimum number of noise hits to consider a straw
#define min_occ 100

// parameters for residual plots
#define rmax 2.0
#define rbins 40

// minimum number of entries to fit a gaussian to a residual plot -- must be >= 1
#define min_entries 100

using namespace std;

TString s_default = "timinginfo.root";
TString s_outfile = "clocknoise.plots.root";
TString s_noisetextfile = "clocknoise.dat";
const int nTotStraw = nStrawB + nStrawE;

class strawNoiseInfo{
public:
    float asym80;
    int phase80;
    float asym40;
    int phase40;
    int occbins;
    int totalocc;
    int noiseType;
};

class TRT_ClockNoiseResidualPlots {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Double_t        corLE;
   Double_t        EP;
   Double_t        TOT;
   Int_t           TEbin;
   Int_t           LEbin;
   Int_t           wrongSide;
   Int_t           board;
   Double_t        locR;
   Double_t        timeCorChip;
   Int_t           chip;
   Int_t           side;
   Double_t        t0;
   Int_t           phi;
   Int_t           straw;
   Int_t           strawLayer;
   Int_t           layer;
   Int_t           nBarHits;
   Int_t           nEcHits;
   Int_t           nTRThits_barrelA;
   Int_t           nTRThits_barrelC;
   Int_t           nTRThits_ecA;
   Int_t           nTRThits_ecC;
   Int_t           nSCThits;
   Int_t           nPixHits;
   Double_t        theta;
   Double_t        eta;
   Double_t        chi2ndf;
   Double_t        phi0;
   Double_t        z0;
   Double_t        d0;
   Int_t           tubeHit;
   Double_t        locRpat;

   // List of branches
   TBranch        *b_corLE;   //!
   TBranch        *b_EP;   //!
   TBranch        *b_TOT;   //!
   TBranch        *b_TEbin;   //!
   TBranch        *b_LEbin;   //!
   TBranch        *b_wrongSide;   //!
   TBranch        *b_board;   //!
   TBranch        *b_locR;   //!
   TBranch        *b_timeCorChip;   //!
   TBranch        *b_chip;   //!
   TBranch        *b_side;   //!
   TBranch        *b_t0;   //!
   TBranch        *b_phi;   //!
   TBranch        *b_straw;   //!
   TBranch        *b_strawLayer;   //!
   TBranch        *b_layer;   //!
   TBranch        *b_nBarHits;   //!
   TBranch        *b_nEcHits;   //!
   TBranch        *b_nTRThits_barrelA;   //!
   TBranch        *b_nTRThits_barrelC;   //!
   TBranch        *b_nTRThits_ecA;   //!
   TBranch        *b_nTRThits_ecC;   //!
   TBranch        *b_nSCThits;   //!
   TBranch        *b_nPixHits;   //!
   TBranch        *b_theta;   //!
   TBranch        *b_eta;   //!
   TBranch        *b_chi2ndf;   //!
   TBranch        *b_phi0;   //!
   TBranch        *b_z0;   //!
   TBranch        *b_d0;   //!
   TBranch        *b_tubeHit;   //!
   TBranch        *b_locRpat;   //!

   // -------- MakeClass methods ------------------------------------------
   TRT_ClockNoiseResidualPlots(TString s_file = s_default, TString s_textfile = s_noisetextfile);
   virtual ~TRT_ClockNoiseResidualPlots();
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop(bool noisy_straw_list = true);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
   
   // ----------- user methods ----------------------------------------------
   virtual int               initialize();
   virtual int               book_histograms();
   virtual int               fit_all_histos();
   virtual int               finalize();
   virtual int               fill_noise_map();
   virtual int               cut_hit();
   virtual int               cut_noise();
   virtual void              gaus_fit(TH1F* h);
   virtual void              gaus_fit(TH1F* h[24]);
   virtual void              graph_residuals_vs_LEbin(TH1F* h[24],TGraphErrors* gMean,TGraphErrors* gSigma);
   virtual TGraphErrors*     subtract_graphs(TGraphErrors* g, TGraphErrors* gref);
   virtual TMultiGraph*      graph_with_colors(TGraphErrors* g, int period, int phase);
   
   // ---------- user variables ---------------------------------------------
   strawNoiseInfo*  noise_info_map[nPhi][2][nTotStraw]; //map to hold all the noise info
   int              noisy_straw_phase_map[nPhi][2][nTotStraw]; // holds phase for noisy straw, -1 if not noisy
   TFile*           fout;  // output file for histograms

   // if this is set to true, then the text file read in contains just noisy straws, not full info
   bool             use_noisy_straw_list;
   
   // ---------- user histos and graphs --------------------------------------
   TCanvas*        ctemp;
   
   // for all hits on track
   TH1F*           hres;
   TH1F*           hresLEbin[24];
   TGraphErrors*   gresMean;
   TGraphErrors*   gresSigma;
   
   // for hits on straws with 40MHz noise
   TH1F*           hres40mhz[PERIOD_40MHZ];
   TH1F*           hresLEbin40mhz[PERIOD_40MHZ][24];
   TGraphErrors*   gresMean40mhz[PERIOD_40MHZ];
   TGraphErrors*   gresSigma40mhz[PERIOD_40MHZ];
   
   TGraphErrors*   gdiffMean40mhz[PERIOD_40MHZ];
   TGraphErrors*   gdiffSigma40mhz[PERIOD_40MHZ];
   
   TMultiGraph*    gdiffMeanColor40mhz[PERIOD_40MHZ];
   TCanvas*        cdiffMeanColor40mhz[PERIOD_40MHZ];
   TMultiGraph*    gdiffSigmaColor40mhz[PERIOD_40MHZ];
   TCanvas*        cdiffSigmaColor40mhz[PERIOD_40MHZ];
   
};

#endif

#ifdef TRT_ClockNoiseResidualPlots_cxx
#endif // #ifdef TRT_ClockNoiseResidualPlots_cxx
