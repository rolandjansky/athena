/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////// 
// MagnitudeEvolution
///////////////////////////
//
#ifndef MagnitudeEvolution_H
#define MagnitudeEvolution_H

//// includes ////
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

//// ROOT includes ////
#include "TArrow.h"
#include "TCanvas.h"
#include "TColor.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TLine.h"
#include "TPad.h"
#include "TString.h"
#include "TStyle.h"
#include "TText.h"

//// parameters ////
#define NIBLSTAVES 0
#define MINENTRIES 36
#define MINENTRIESINBIN 12
#define MINGOODPOINTS 8
#define FITTYPE 3
#define RUNNUMBEREXTRACTIONMODE 0
#define RMSFACTOR 1.5  // factor used in the residuals Gaussian fit to limit the range (in units of RMS)

const bool FULLOUTPUT= false;
const bool USEPLANARONLY = true;
bool LUMIBLOCKANALYSIS = true;
const int REBINFACTOR = 100; // 100; // LumiBlock rebinning factor for the residuals vs LumiBlock histograms
const bool WRITETEXTOUTPUT = true;
//const TString me_trackCollection("AlignTracks_all");
//const TString me_trackCollection("InDetTrackParticles_AlignSel");
//const TString me_trackCollection("ExtendedTracks_alignSelection");
const TString me_trackCollection("ExtendedTracks_all");

const double z_fix = 366.5; // Stave fixing screw position [mm]
float xmax = 0.3;
float xmin = -xmax;

const double me_bowingRangeInPlots = 36.; //in microns

const float me_systUncertaintyBowing = 0.255; // systematic uncertainty on the bowing measurements (in microns)

const int me_colorForIBL = kAzure-1;
const int me_colorForStaves = kOrange+5;

//// variables ////
std::ofstream me_outputTextFile;
std::vector<TString> me_monitoringFileList;
std::vector<int> me_runNumber;

std::vector<int>   me_bowingMagnitudeStave; // per run, and keep the stave result
std::vector<float> me_bowingMagnitudeValue;
std::vector<float> me_bowingMagnitudeError;
std::vector<float> me_bowingBaseLineValue;
std::vector<float> me_bowingBaseLineError;

TFile* me_currentMonitoringFile;
TH1F*  me_h_IBLmeanResPerRing[NIBLSTAVES+1]; // one per stave plus another one for all
TH1F*  me_h_bowingMagnitudeEvolution[NIBLSTAVES+1];
std::vector<TH1F*> me_h_bowingMagnitudePerLB[NIBLSTAVES+1];
TH2F*  me_h_bowingMagnitudePerStaveAndLB;
TH1F*  me_h_bowingBaseLineEvolution[NIBLSTAVES+1];

TH1F* me_h_deltaBxPerStave;

std::vector<TH1F*>  me_h_bowingMagnitudePerStave; 
std::vector<TH1F*>  me_h_bowingBaseLinePerStave; 

//// prototypes ////
void          ME_clear (); 
void          ME_computeDeltaBx (int);
void          ME_conditioningStaveSummaryHisto ();
int           ME_extractRunNumber (int);
void          ME_finalize ();
bool          ME_fitBowingMagnitude (TH1F*, int, int, int lumiblockid = -1);
bool          ME_fitResidualsDistribution (TH1F*, int);
TH3F*         ME_get3DHistoResidualsVsEtaAndLB (int, int);
TH1F*         ME_getHistoResidualsVsEtaAtLB (TH3F*, int, int, int, int);
TH1F*         ME_getIBLMeanResPerRing (TH3F*, int, int);
void          ME_getMonitoringFileList ();
TH3F*         ME_getIBLResiduals3DHisto (int); 
void          ME_goodbye ();
void          ME_init ();
void          ME_loop ();
void          ME_performLumiBlockAnalysis (int);
void          ME_plotDeltaBx ();
void          ME_plotEvolutionHistos ();
void          ME_plotHistosPerStave ();
void          ME_prepareEvolutionHistos ();
void          ME_prepareHistosPerStave ();
void          ME_prepareMagVsLBhisto (TH3F*, int, int);
void          ME_prepareLoop ();
void          ME_prepareOutputTextFile ();
void          ME_preparePalette ();
void          ME_prepareStaveSummaryHisto (TH3F*, int);
void          ME_processFile (int);
void          ME_welcomeLogo ();
void          ME_writeOutputTextFile();

//// imported from BowingFit.cxx by PFButti ////
TGraphErrors* ConvertHistoInGraph(const TH1F* histo);
//TGraphErrors* MakeStaveFit(TFile* OutFile,TH1D* histo,string name,string title,string path,int type,bool renormalised)
bool MakeStaveFit (TGraphErrors*, int);

#endif
