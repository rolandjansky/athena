/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* **************************************************** *
 * Authors: Toni Baroncelli                             *
 * **************************************************** */

#ifndef SRC_MDTTUBEANA_H
#define SRC_MDTTUBEANA_H

#include <string>
#include <stdio.h>

using namespace std;
static const int nmmax=100;
static const int ntmax=600;
Double_t polnml3(Double_t * x, Double_t * parf);
double completefunc(double * x, double * parf);
class MDTDeadElementsAnalysis {
 public:

       MDTDeadElementsAnalysis();
       ~MDTDeadElementsAnalysis();

TF1 *polfunc ;
  string chamberName;
  TH1I * deadTubesMap;
  TH1I * geo;
  //  static const int nmmax=20;
  int ndeadmezz;
  double deadmezz[nmmax];
  double deadmezzMLay[nmmax];

  vector<TString> noisyChamber;

  vector<TString> deadChamber;
 
  vector<int> deadLayer;
  vector<TString> deadLayerChamber;

  vector<int> deadMultilayer;
  vector<TString> deadMLChamber;
  vector<double> deadMLCounts;
  vector<double> deadMLLevel;

  vector<int> deadMezz;
  vector<int> deadMezz_red;
  vector<int> deadMezz_csm;
  vector<TString> deadMezzChamber;
  vector<int> deadMezzML;
  vector<double> deadMezzEffi;
  vector<double> deadMezzErrEffi;
  vector<double> deadMezzChi2;

  vector<TString> deadTubeChamber;
  vector<int> deadTube;
  vector<int> deadTubeId;
  vector<double> deadTubeHistoBin;
  vector<int> deadTubeLay;
  vector<int> deadTubeML;
  vector<double> deadTubeChi2;
  vector<double> deadTubeEffi;
  vector<double> deadTubeErrEffi;

  int ndeadtubes;

  int position_spikes[ntmax];
  double deadtube[ntmax];
  double deadtubeseed[ntmax];
  double deadtubeLay[ntmax];
  double deadtubeML[ntmax];
  double deadtubelen[ntmax]; 
  double deadtubechi2[ntmax];
  double deadtubeeffi[ntmax];
  double deadtubeerr[ntmax];
  double deadtubestat[ntmax][3];

  bool verbose;
  bool drawPlots;
  bool writeResultsToAsciFile;

  bool deadMezzDone;
  bool deadTubesDone;
  bool deadMultilayerDone;

  void histogramScanGnam(string rootfile);
  void histogramScanCalibCenters(TFile * rootfile);
  void getBasicGeometry(TH1F* geo, string chambname);
  void getBasicGeometry(TH1F* geo, int nlayers_ml, string chambname);
  void deadElementsAnalysis(TH1F* idh1, TH1F* geo, TH1F* deadTubesMap, int nlayers_ml, string chambname);
  void deadElementsAnalysis(TH1F* idh1, TH1F* geo, TH1F* deadTubesMap, int nlayers_ml, 
			    string chambname, int convert_mezzanine[20]);
  void deadChambers(TH1F* idh1, TH1F* HRef, TH1F* deadTubesMap, string chambname);
  void deadLayers(TH1F* idh1,TH1F* HRef, TH1F* deadTubesMap, string chambname);
  void Clear(int ntubes);
  void ClearList();
  void deadTubes(TH1F *idh1 , TH1F* HRef, TH1F* deadTubesMap, string chambername);
  void getAverageLevelInInterval(TH1F*, int firsttube, int lasttube);
  void getSpikesInInterval(TH1F*, int firsttube, int lasttube, TF1 *polfunc);
  void getHolesInInterval(TH1F*, int firsttube, int lasttube, int ntubes, TF1 *polfunc);
  void getHolesInInterval_strategy5(TH1F*, int firsttube, int lasttube, int ntubes, TF1 *polfunc);
  string validateDeadTubes(double chi2cut, TH1F* HRef, TH1F* deadTubesMap, string chambname);
  void deadMezzanines(TH1F *idh1, TH1F* HRef, TH1F* deadTubesMap, string chambername);
  void deadMezzanines(TH1F *idh1, TH1F* HRef, TH1F* deadTubesMap, 
		      string chambernamee, int convert_mezzanine[20]);
  void deadMultilayers(TH1F *idh1 , TH1F* HRef, TH1F* deadTubesMap, string chambername);
  bool NoisyChamber(TH1F *idh1, string chambername);
  void setVerbose();
  void setNoVerbose();
  void setChamberName(string chambname);
  string getChamberName();
  void printSummary();
  void printSummaryToAsciFile();
  void printCompactSummaryToAsciFile();
  void WriteAsciFile();

  // Basic chamber geometry
  string schamber;
  string chtype;
  int nlayers;
  int nbins_per_layer[2];
  int nlayers_per_ml;
  int ntubes[2];
  double tstep[2];
  int nmezz[2];
  int ntubes_per_mezz;
  int firstbinML[2];
  int lastbinML[2];
  double chi2poly;
  double polyparf[4];
  double average_entry;
  double n_entry;
  int holes[100];
  int nholes;
  double eff_hole[100];
  int position_holes[1000];
}; //MDTDeadElementsAnalysis

#endif
