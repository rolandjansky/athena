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
#include "TDatime.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TLine.h"
#include "TMultiGraph.h"
#include "TPad.h"
#include "TString.h"
#include "TStyle.h"
#include "TText.h"
#include "TSystem.h"

//// ATLAS includes ////
#include "include/AtlasLabels.C"
#include "include/AtlasStyle.C"
#include "include/AtlasUtils.C"

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
const bool LUMIBLOCKANALYSIS = false;
const bool ITER3ANALYSIS = false;
const int  REBINFACTOR = 2; // 100; // LumiBlock rebinning factor for the residuals vs LumiBlock histograms
const bool WRITETEXTOUTPUT = true;
const bool REGENERATEAFTERALIGMENTMONITORINGFILE = false;

//const TString me_trackCollection("AlignTracks_all");
//const TString me_trackCollection("InDetTrackParticles_AlignSel");
//const TString me_trackCollection("ExtendedTracks_alignSelection");
const TString me_trackCollection("ExtendedTracks_all");
TString me_outputFolder;
TString me_afterAlignmentHistosFolder;
TString me_HOSTNAME;

const double z_fix = 366.5; // Stave fixing screw position [mm]
float xmax = 0.3;
float xmin = -xmax;

const double me_bowingRangeInPlots = 12.; //12.; 30. //in microns

const float me_systUncertaintyBowing = 0.255; // systematic uncertainty on the bowing measurements (in microns)

const int me_colorForIBL = kAzure-1;
const int me_colorForStaves = kOrange+5;

//// variables ////
std::ofstream me_outputTextFile;
std::vector<TString> me_iter0MonitoringFileList;
std::vector<TString> me_iter3MonitoringFileList;
std::vector<int> me_runNumber;
std::vector<int> me_iter3runNumber; // as there is one file per LB group, then for each iter3 file, the run number is kept
std::vector<TDatime> me_runDateTime;
std::vector<TString> me_listOfDataProjects;
std::vector<TString> me_listOfDataStreams;

// initial or before alignment (Iter0)
std::vector<int>   me_bowingMagnitudeStave; // per run, and keep the stave result
std::vector<float> me_bowingMagnitudeValue;
std::vector<float> me_bowingMagnitudeError;
std::vector<float> me_bowingBaseLineValue;
std::vector<float> me_bowingBaseLineError;

// final or after alignment (Iter3)
std::vector<float> me_bowingMagnitudeValue_AfterAlignment;

// initial or before alignment (Iter0)
TFile* me_currentMonitoringFile;
TH1F*  me_h_IBLmeanResPerRing[NIBLSTAVES+1]; // one per stave plus another one for all
TH1F*  me_h_bowingMagnitudeEvolution[NIBLSTAVES+1];
std::vector<TH1F*> me_h_bowingMagnitudePerLB[NIBLSTAVES+1];
TH2F*  me_h_bowingMagnitudePerStaveAndLB;
TH1F*  me_h_bowingBaseLineEvolution[NIBLSTAVES+1];

TH1F* me_h_deltaBxPerStave;

std::vector<TH1F*>  me_h_bowingMagnitudePerStave; 
std::vector<TH1F*>  me_h_bowingBaseLinePerStave; 

// final or after alignment (Iter3)
TH1F* me_h_bowingMagnitudeEvolution_AfterAlignment[NIBLSTAVES+1];



//// prototypes ////
TString       ME_chainAfterAlignmentMonitoringFiles(int);
void          ME_clear (); 
void          ME_computeDeltaBx (int);
void          ME_conditioningStaveSummaryHisto ();
void          ME_drawEvolInTimeFormat();
//int           ME_extractRunNumber (int);
void          ME_finalize ();
bool          ME_fitBowingMagnitude (TH1F*, int, int, int lumiblockid = -1, TString theSuffix="");
bool          ME_fitResidualsDistribution (TH1F*, int);
TH3F*         ME_get3DHistoResidualsVsEtaAndLB (int, int);
float         ME_getBowingMagFor2015SpecialRuns (int);
TH1F*         ME_getHistoResidualsVsEtaAtLB (TH3F*, int, int, int, int);
TH1F*         ME_getIBLMeanResPerRing (TH3F*, int, int);
TH3F*         ME_getIBLResiduals3DHisto (int); 
void          ME_getIter3MonitoringFiles (TString, int);
int           ME_getLatestTier0ProcessingID(TString, TString);
void          ME_getMonitoringFileList ();
TDatime       ME_getRunDateTime (TString);
void          ME_goodbye ();
void          ME_init ();
TString       ME_iter3FolderName (TString, TString, TString, TString, int);
void          ME_iter3Analysis_PrepareHistos();
void          ME_loop ();
void          ME_performLumiBlockAnalysis (int);
void          ME_performIter3Analysis (int);
void          ME_plotDeltaBx ();
void          ME_plotEvolutionHistos ();
void          ME_plotHistosPerStave ();
void          ME_prepareAfterAlignmentHistosFolder ();
void          ME_prepareEvolutionHistos ();
void          ME_prepareHistosPerStave ();
void          ME_prepareMagVsLBhisto (TH3F*, int, int);
void          ME_prepareLoop ();
void          ME_prepareOutputFolder ();
void          ME_prepareOutputTextFile ();
void          ME_preparePalette (float, float);
void          ME_prepareStaveSummaryHisto (TH3F*, int);
void          ME_processFile (int);
bool          ME_runIsGood (int);
bool          ME_runIsInSpecialTreatment2015RunList (int);
void          ME_welcomeLogo ();
void          ME_writeOutputTextFile();

//// imported from BowingFit.cxx by PFButti ////
TGraphErrors* ConvertHistoInGraph(const TH1F* histo);
//TGraphErrors* MakeStaveFit(TFile* OutFile,TH1D* histo,string name,string title,string path,int type,bool renormalised)
bool MakeStaveFit (TGraphErrors*, int);

#endif
