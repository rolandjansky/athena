//////////////////////////////////////////////////////////
// This class has been automtically generated on
// Wed Feb  6 10:04:15 2008 by ROOT version 5.17/02
// from TTree ntuple/data from ascii file
// found on file: ../root/rt.chafik_m4_huge.root
// 
// Author: Brett Jackson <brett.david.jackson@cern.ch>
// Author: Dominick Olivito <dominick.john.olivito@cern.ch>
//////////////////////////////////////////////////////////

#ifndef TRT_calculateTimingConstants_h
#define TRT_calculateTimingConstants_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

#include "Riostream.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>

#include <TH1.h>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <vector>
#include <TProfile.h>
#include <fstream>
#include <cmath>
#include "Riostream.h"
#include <iostream>
#include <TGraphErrors.h>
#include <TF1.h>
#include <THStack.h>
#include <TLegend.h>
#include <TMultiGraph.h>
#include <TNtuple.h>
#include <TPaveStats.h>

///////////////////////////////////
/// Parameters to describe the 
/// binning in radius histograms
///////////////////////////////////
const int rbins   = 40;
const float rmin  = 0.;
const float rmax  = 2.;
const int rbinsBd = 20;

/////////////////////////////////
/// Parameters to describe the 
/// binning in time histograms
/////////////////////////////////
//const int tbins  = 48;
//const float tmin = -12.5;
//const float tmax = 37.5;
//const int tbins  = 60;
//const float tmin = -12.5;
//const float tmax = 50.0;
const int tbins  = 68;
const float tmin = -12.5;
const float tmax = 75.0;

//////////////////////////////////////////
/// Parameters to describe the 
/// binning in time residual histograms
//////////////////////////////////////////
//const int trbins = 16;
const int trbins    = 75;
const float trmin   = -25.;
const float trmax   = 50.;
const float trwidth = 8.;

//////////////////////////////////
/// Values used in the cut when
/// determining residuals 
//////////////////////////////////
const float fitmin = -5.;
const float fitmax = 30.;

/////////////////////////////
/// number of short layers
/////////////////////////////
const int shortLayers = 9;

///////////////////////////////////////////////////////////////
/// Number of chips, phi modules, "sides", boards and straws
///////////////////////////////////////////////////////////////
//const int nChip  = 120;
//const int nPhi   = 32;
//const int nSide  = 5;
//const int nBoard = 9;
//const int nStraw = 1642;
//
const int nChip  = 224;
const int nPhi   = 32;
const int nSide  = 5;
const int nSide_new = 4;
const int nBoard = 29;
const int nStraw = 5482;

//////////////////////////////////////////////////////////
/// Number of chips, boards and straws in barrel region
//////////////////////////////////////////////////////////
//const int nChip_barrel  = 104;
//const int nBoard_barrel = 9;
//const int nStraw_barrel = 1642;
const int nChip_barrel  = nChip;
const int nBoard_barrel = nBoard;
const int nStraw_barrel = nStraw;

//////////////////////////////////////////////////////////
/// Number of chips, boards and straws in endcap region
//////////////////////////////////////////////////////////
//const int nChip_ec  = 120; 
//const int nBoard_ec = 20;
//const int nStraw_ec = 3840;
const int nChip_ec  = nChip; 
const int nBoard_ec = nBoard;
const int nStraw_ec = nStraw;

//////////////////////////////////////////////////////
// barrel/endcap designations for the side variable
//////////////////////////////////////////////////////
const int ecC  = -2;
const int barC = -1;
const int barA = 1;
const int ecA  = 2;

////////////////////////////////////////
/// used to distinguish between upper 
/// and lower stacks - This is only 
/// used as an array index
////////////////////////////////////////
const int UPPER = 1;
const int LOWER = 0;

/////////////////////////////////////////////
/// Minimum number of entries required to
/// compute tr rt relation or the residual
/////////////////////////////////////////////
const int minEntriesTR  = 500;
const int minEntriesRes = 100;
//const int minEntriesRes = 50;

////////////////////////////////////////
/// Other parameters to determine the
/// quality of the fit
////////////////////////////////////////
//const float deltat0Thresh = 7.;
const float deltat0Thresh = 15.;
const float chi2Thresh    = 10.;
const float sigmaMin      = 2.; // we expect a sigma around 3ns for the time residual
//const float sigmaMax      = 4.;
//const float sigmaMin      = 1.5; // we expect a sigma around 3ns for the time residual
const float sigmaMax      = 5;

////////////////////////////////////////////////////
/// RMS of pull distribution for boards and chips
/// use to scale up errors
////////////////////////////////////////////////////
const float boardFactor = 1.6;
const float chipFactor  = 1.5;

// std::cout << "<<<<<<<<<<<<< rt.C usage: >>>>>>>>>>>>>" << std::endl
//           << "  > TRT_calculateTimingConstants t" << std::endl
//           << "  > t.Loop(boards = {false,true}, chips = {false,true}, layers = {ALL,INNER,OUTER}, absRes = {false,true}, rtCuts = {true,false})" << std::endl
//           << "[defaults are listed first]" << std::endl << std::endl;


Double_t  p[4]   = {0,0.1,-0.00087,0}; // chafik's values
Double_t  ptr[4] = {0,7.5,3.5,-0.5}; // from previous fit
Int_t     boardChips[nBoard_barrel]     = {10, 21, 36, 45, 54, 65, 77, 90, 104};
Int_t     boardChipCount[nBoard_barrel] = {10, 11, 15, 9, 9, 11, 12, 13, 14};

enum strawlayers { ALL, SHORT, LONG0, LONGALL, BAR, EC };

// std::cout << "<<<<<<<<<<<<< rt.C usage: >>>>>>>>>>>>>" << std::endl
//           << "  > rt t" << std::endl
//           << "  > t.Loop(boards = {false,true}, chips = {false,true}, layers = {ALL,INNER,OUTER}, absRes = {false,true}, rtCuts = {true,false})" << std::endl
//           << "[defaults are listed first]" << std::endl << std::endl;

struct t0Data {
  double t0;
  double sigma;
  double entries;
}; 

class TRT_calculateTimingConstants {
  public :
    
    // constructor and destructor
    TRT_calculateTimingConstants(TString inputdir, TString runnum, TString outputdir = "./");
  virtual ~TRT_calculateTimingConstants();
  
  ///////////////// Loop: this is the only function one needs to call from the root prompt///////////////////////
  //
  //   - loops over all events in the specified ntuple and performs all other calculations
  //   - this has lots of options that can be passed to change what is input and output
  //
  virtual void     Loop(bool bds = false, bool chps = false, bool drawErr = false, bool internets = false, bool tres = true, strawlayers lyrs = ALL, bool absRes = false, bool rtcut = true);
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  // private:
  TTree          *fChain;   //!pointer to the analyzed TTree or TChain
  Int_t           fCurrent; //!current Tree number in a TChain
  
  ////////////////////////////////
  // Declaration of leave types
  ////////////////////////////////
  Double_t   corLE;
  Double_t   EP;
  Double_t   TOT;
  Int_t      TEbin;
  Int_t      LEbin;
  Int_t      wrongSide;
  Int_t      board;
  Double_t   locR;
  Double_t   timeCorChip;
  Int_t      chip;
  Int_t      side;
  Double_t   t0;
  Int_t      phi;
  Int_t      straw;
  Int_t      strawLayer;
  Int_t      layer;
  Int_t      nBarHits;
  Int_t      nEcHits;
  Int_t      nSCThits;
  Int_t      nPixHits;
  Double_t   theta;
  Double_t   eta;
  Double_t   chi2ndf;
  Double_t   phi0;
  Double_t   z0;
  Double_t   d0;

  //////////////////////
  // List of branches
  //////////////////////
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
  TBranch        *b_nSCThits;   //!
  TBranch        *b_nPixHits;   //!
  TBranch        *b_theta;   //!
  TBranch        *b_eta;   //!
  TBranch        *b_chi2ndf;   //!
  TBranch        *b_phi0;   //!
  TBranch        *b_z0;   //!
  TBranch        *b_d0;   //!

  ///////////////////////////
  /// Histgrams and graphs
  ///////////////////////////
  TH2F*          hrt;
  TH2F*          hrt_side[nSide_new]; //~~~
  TH2F*          hrt_barrel;
  TH2F*          hrt_ec;
  TH2F*          htr;
  TH2F*          htr_side[nSide_new]; //~~~
  TH2F*          htr_barrel;
  TH2F*          htr_ec;
  //TH2F*          htrBoard[nPhi][nSide][nBoard_ec];
  //TH2F*          htrChip[nPhi][nSide][nChip_ec];
  TH2F*          htrBoard[nPhi][nSide_new][nBoard];
  TH2F*          htrChip[nPhi][nSide_new][nChip];

  TH1D*          hProjBin[rbins];
  //TH1D*          hProjBin_side[nSide_new][rbins]; //~~~
  //TH1D*          hProjBin_barrel[rbins];
  //TH1D*          hProjBin_ec[rbins];

  TGraphErrors*  grt;
  TGraphErrors*  grt_side[nSide_new]; //~~~
  TGraphErrors*  grt_barrel;
  TGraphErrors*  grt_ec;
  TGraphErrors*  gtr;
  TGraphErrors*  gtr_side[nSide_new]; //~~~
  TGraphErrors*  gtr_barrel;
  TGraphErrors*  gtr_ec;
  //TGraphErrors*  gtrBoard[nPhi][nSide][nBoard_ec];
  //TGraphErrors*  gtrChip[nPhi][nSide][nChip_ec];
  TGraphErrors*  gtrBoard[nPhi][nSide_new][nBoard];
  TGraphErrors*  gtrChip[nPhi][nSide_new][nChip];
  TGraphErrors*  gBoardt0[nPhi][nSide_new];
  TGraphErrors*  gChipt0[nPhi][nSide_new];
  TH1F*          hBoardt0Diff;
  TH1F*          hChipt0Diff;
  TH1F*          hRes;
  TH1F*          hRes_side[nSide_new]; //~~~
  TH1F*          hRes_barrel;
  TH1F*          hRes_ec;

  TH1F*          hResBin[rbins];
  TH1F*          hResBin_side[nSide_new][rbins]; //~~~
  TH1F*          hResBin_barrel[rbins];
  TH1F*          hResBin_ec[rbins];

  TGraphErrors*  gRes;
  TGraphErrors*  gRes_side[nSide_new]; //~~~
  TGraphErrors*  gRes_barrel;
  TGraphErrors*  gRes_ec;

  TH2F*          hrTE;
  TH2F*          hrTE_side[nSide_new]; //~~~
  TH2F*          hrTE_barrel;
  TH2F*          hrTE_ec;
  TH2F*          hTEr;
  TH2F*          hTEr_side[nSide_new]; //~~~
  TH2F*          hTEr_barrel;
  TH2F*          hTEr_ec;
  TGraphErrors*  gTEr;
  TGraphErrors*  gTEr_side[nSide_new]; //~~~
  TGraphErrors*  gTEr_barrel;
  TGraphErrors*  gTEr_ec;
  TH2F*          ht0Err;
  TH1F*          hChi2Board;
  TH1F*          hChi2Chip;
  //TH1F*          htResBoard[nPhi][nSide][nBoard_ec];
  //TH1F*          htResChip[nPhi][nSide][nChip_ec];
  //TGraphErrors*  gChipt0s[nSide][nBoard_ec];
  //TGraph*        gChipt0sFD[nSide][nBoard_ec];
  //TH1F*          ht0FDdiff[nSide][nBoard_ec];
  //TH1F*          htResChipStacked[nSide][nChip_ec];
  //TLine*         lBoard[nBoard_ec];
  //TH1F*          hChipt0[nPhi][nSide][nBoard_ec];
  TH1F*          htResBoard[nPhi][nSide_new][nBoard];
  TH1F*          htResChip[nPhi][nSide_new][nChip];
  TGraphErrors*  gChipt0s[nSide_new][nBoard];
  TGraph*        gChipt0sFD[nSide_new][nBoard];
  TH1F*          ht0FDdiff[nSide_new][nBoard];
  TH1F*          htResChipStacked[nSide_new][nChip];
  TLine*         lBoard[nBoard];
  TH1F*          hChipt0[nPhi][nSide_new][nBoard];
  TH1F*          hBoardt0Side[nSide_new][2];
  
  TH1F*          htRes;
  TH1F*          htRes_side[nSide_new]; //~~~
  TH1F*          htRes_barrel;
  TH1F*          htRes_ec;
  TH1F*          hTEres;
  TH1F*          hTEres_side[nSide_new]; //~~~
  TH1F*          hTEres_barrel;
  TH1F*          hTEres_ec;
  
  TH1F*          hTOTcut;
  TGraphErrors*  gVdrift;
  TH1F*          ht0Bcomp;
  TH1F*          ht0Ccomp;

  TF1*      rtRel;
  TF1*      rtRel_side[nSide_new]; //~~~
  TF1*      rtRel_barrel;
  TF1*      rtRel_ec;
  TF1*      trRel;
  TF1*      trRel_side[nSide_new]; //~~~
  TF1*      trRel_barrel;
  TF1*      trRel_ec;
  //TF1*      trRelBoard[nPhi][nSide][nBoard_ec];
  //TF1*      trRelChip[nPhi][nSide][nChip_ec];
  TF1*      trRelBoard[nPhi][nSide_new][nBoard];
  TF1*      trRelChip[nPhi][nSide_new][nChip];
  TF1*      rtChafik;
  TF1*      TErRel;
  TF1*      TErRel_side[nSide_new]; //~~~
  TF1*      TErRel_barrel;
  TF1*      TErRel_ec;


  //////////////////////
  /// Other variables
  //////////////////////
  TString   s_inputdir;
  TString   s_runnum;
  TString   s_outputdir;
  TString   s_boardFile;
  TString   s_chipFile;
  TString   s_logFile;
  TString   s_chipSideFile;

  TFile*    fout;
  ofstream  outBoard;
  ofstream  outChip;
  ofstream  log;
  ofstream  outChipSide;
  TString   s_save;
  TString   s_histoSave;


  Double_t  rtmin;
  Double_t  rtmax;

  Double_t  rdelta;
  Double_t  rdeltaBd;
  Double_t  tdelta;

  TCanvas*  c1;

  bool      saveForInternets;
  bool      savePDF;
  bool      t0FromRes;
  bool      drawErrors;
  bool      boards;
  bool      chips;
  bool      showRT;
  bool      showRes;
  bool      showt0;
  //bool      fullB[nPhi][nSide][nBoard_ec];
  bool      fullB[nPhi][nSide_new][nBoard];
  bool      drawTails;

  //t0Data    t0Board[nPhi][nSide][nBoard_ec];
  //t0Data    t0Chip[nPhi][nSide][nChip_ec];
  t0Data    t0Board[nPhi][nSide_new][nBoard];
  t0Data    t0Chip[nPhi][nSide_new][nChip];
  Int_t     histoIndex;
  // !!! chipCor should be ADDED to LE, not subtracted like a t0...
  strawlayers  layers; // int?
  //Double_t  chipCor[nSide][nChip_ec];
  //Int_t     shortStrawChips[nChip_barrel];
  //Float_t   newt0B[nPhi][nSide][nBoard_ec];
  //Float_t   newt0C[nPhi][nSide][nChip_ec];
  //Float_t   dbt0[nPhi][nSide][nStraw_ec];
  //Int_t     strawToChip[nStraw_ec];
  Double_t  chipCor[nSide_new][nChip];
  Int_t     shortStrawChips[nChip];
  Float_t   newt0B[nPhi][nSide_new][nBoard];
  Float_t   newt0C[nPhi][nSide_new][nChip];
  Float_t   dbt0[nPhi][nSide_new][nStraw];
//  Int_t     strawToChip[nStraw];
  
  //////////////////////
  /// Basic functions
  //////////////////////
  virtual Int_t    GetEntry(Long64_t entry);
  virtual Long64_t LoadTree(Long64_t entry);
  virtual void     Init(TTree *tree);
  virtual Bool_t   Notify();
  virtual void     Show(Long64_t entry = -1);
  inline unsigned int getSideIndex(int side);
  
  ////////////////////////////////////
  /// Initialization-type functions
  ////////////////////////////////////
   
  // initialize: this defines some basic names and things, may need to be changed occasionally
  //  -- also contains bools for controlling which histograms are displayed/saved
  virtual void     initialize();
   
  // histoprep: this sets up all the histograms and graphs.  new histos should be added here
  virtual void     histoPrep();
   
  // fillChipCor: this fills an array with fine delay chip-to-chip corrections, from an input file
  virtual void     fillChipCor();
   
  // shortStrawInfo: fills an array with # of short straws on each chip, from an input file
  virtual void     shortStrawInfo();
    
  // fillStrawToChipMapping: fills an array with chip numbers for each straw from input file
//  virtual void     fillStrawToChipMapping(); 
  
  // closingTime: closes any open files, writes all graphs and TF1s to the open TFile
  virtual void     closingTime();
      
  /////////////////////////////////////////
  /// Loop cuts:
  ///
  /// These are for use inside of Loop()
  /////////////////////////////////////////
   
  // Cut: this is where cuts on TOT, LEbin, TEbin, and wrongSide are implemented
  //   -- also incorporates layerCut
  virtual Int_t    Cut(Long64_t entry);

  // rtCut: throws out points with t such that r(t) > 2 or r(t) < 0
  //   -- used only for calculating residuals
  virtual Int_t    rtCut(Long64_t entry);

  // layerCut: select short or long straws (or all)
  virtual Int_t    layerCut(Long64_t entry);
   
  /////////////////
  /// Other cuts
  /////////////////
   
  // phi/sideCut: these select which parts of the detector are in the data sample
  virtual Int_t    phiCut(int iPhi);
  virtual Int_t    sideCut(int iSide);

  // skipBoard/Chip: here I list bad boards/chips to be skipped when calculating t0s
  virtual bool     skipBoard(int iPhi, int iSide, int iBoard);
  virtual bool     skipChip(int iPhi, int iSide, int iChip);
   
  
  /////////////////////////
  /// Major calculations
  /////////////////////////
   
  // tResCalc: calculates t0s for boards/chips by forming a time residual distribution and fitting a gaussian to that
  //   -- this is very similar to how chafik determines t0s
  //   -- also, I trust the errors more for this t0 determination, based on pull distributions
  //   -- also calculates chip t0 by summing over all stacks on a given side, to compare to fine delay chip values
  virtual void     tResCalc();
   
  // sliceNfit: this takes a 2d rt or tr plot, slices it in distance bins, and fits gaussians to the slices
  virtual void     sliceNfit(TH2F* h, TGraphErrors* g, bool xaxis = true, bool global = true);
   
  // curvefits: these fit either an rt or tr curve to a graph made from sliceNfit
  virtual void     rtCurveFit(TGraphErrors* g, TF1* rtrel);
  virtual void     trCurveFit(TGraphErrors* g, TF1* rtrel, bool global = true);
   
  // resBins: makes residual plots for bins in locR
  virtual void     resBins(bool absRes = false);
   
  // resCalc: calculates resolution as a function of locR by binning in distance
  virtual void     resCalc();
   
  // boardTR: fits for the constant parameter of a TR relation for each board to find t0
  virtual void     boardTR();
   
  // chipTR: fits for the constant parameter of a TR relation for each chip to find t0
  virtual void     chipTR();
   
  // removeOutlier: for fitting TR curves at board/chip level
  //   -- try removing one point to improve chi2 (in case there's an outlier)
  virtual void     removeOutlier(TGraphErrors* g, TF1* f1);
   
  //////////////////////////
  /// simple calculations
  //////////////////////////
  
  // chipToBoardMapping: looks up which board a given chip belongs to
  // --- Currently only works for barrel region. May phase this out 
  //     to be replaced by another entry in the ntuple to store board
  //     number for each hit in both the barrel and endcap
  virtual Int_t    chipToBoardMapping(int iChip);
  
  // chipOnBoard: looks up the relative chip # (i.e. 0-X) for a given chip on a board
  // --- Again only for barrel region
  virtual Int_t    chipOnBoard(int iBoard, int iChip);
  
  // RofT: calculates r(t) using chafik's constants
  virtual Double_t RofT( Double_t t );
  
  // ResAbsCuts: calculates the distance residual as
  //  residual = fabs(locR) - fabs(r(t))
  //   - cuts off r(t) at > 2mm, <0mm
  virtual Double_t ResAbsCuts( Double_t t, Double_t locR );
  
  // ResAbs: calculates the distance residual as
  //  residual = fabs(locR) - fabs(r(t))
  virtual Double_t ResAbs( Double_t t, Double_t locR );
  
  // ResSignCuts: calculates the distance residual as
  //  residual = +- fabs(locR - r(t))
  //   - cuts off r(t) at > 2mm, <0mm
  virtual Double_t ResSignCuts( Double_t t, Double_t locR );
   
  // gausFit: fits a gaussian from -0.5 to 0.5
  virtual void     gausFit(TH1F* h);
   
  // gausFitPeak: fits a gaussian where the peak is not at 0, from peak-trwidth to peak+trwidth
  virtual void     gausFitPeak(TH1F* h);

  // chi2NDF: calculates chi2/ndf for a given function
  virtual Double_t chi2NDF(TF1* f1);
   
  // getNboard: returns the number of boards, which differs endcap to barrel
  virtual Int_t getNboard( int iSide );

  // getNchip: returns the number of chips, which differs endcap to barrel
  virtual Int_t getNchip( int iSide );

  // getNstraw: returns the number of straws, which differs endcap to barrel
  virtual Int_t getNstraw( int iSide );

  // isUpper: returns 1 if the phi stack is in the upper half of the detector, 0 if lower
  virtual Int_t isUpper( int iPhi );

  // strawToBoardMapping: returns the board for a given straw
//  virtual Int_t strawToBoardMapping( int iStraw );
  
  ////////////////////////
  /// Drawing functions
  ////////////////////////
   
  // drawTR/RT: these draw the rt plots to the screen and/or to file
  virtual void     drawRT(bool chafik);
  virtual void     drawTR();
  virtual void     drawTEr();
  virtual void     drawTR(TCanvas* c2, int iPhi, int iSide, int iBoard, int iChip, bool save);
   
  // graphResR: draws resolution as a function of locR to screen/file
  virtual void     graphResR();
      
  // saveCanvas: saves a canvas as pdf with the given name
  virtual void     saveCanvas(TCanvas* c2, TString name);
   
  // saveGIF: saves the canvas as a GIF with a generic name for website use
  virtual void     saveGIF(TCanvas* c2);
   
  // drawFlows: draws a histogram with over/underflows
  virtual void     drawFlows(TH1F* h, TString name);
   
  // graphChipt0s: this graphs chipt0s from the time residual calculation vs fine delay
  virtual void     graphChipt0s(int iSide);
   
  // drawBoardLines: draws lines to seperate chips on different boards, for graphChipt0s
  virtual void     drawBoardLines(double ymin, double ymax);
   
  // graphVdrift: approximates Vdrift at a few points in locR and graphs these
  virtual void     graphVdrift();
   
  // plott0Comp: makes histos comparing new t0s with DB on board/chip level
//  virtual void     plott0Comp();
};

#endif

#ifdef rt_cxx

#endif // #ifdef rt_cxx
