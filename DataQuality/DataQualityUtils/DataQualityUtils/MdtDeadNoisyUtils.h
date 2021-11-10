/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef dqutilsMdtDeadNoisy_h
#define dqutilsMdtDeadNoisy_h

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <stdio.h>

#include <TString.h>
#include <TH1F.h>
#include <TCanvas.h>

namespace dqutils_mdtdeadnoisy {

  struct TubeRange {
    TubeRange(int a, int b) {x1 = a; x2 = b;}
    bool InRange(int x) { if( (x >= x1) && (x <= x2) ) return true; return false; }
    int x1;
    int x2;
  };

  double sqr( double x );
  void displayList( std::vector<int> & v );
  void displayList( std::vector<int> & v, std::ofstream & outputfile );
  void getDeadList( TH1F* h, std::vector<int> & v );
  bool find( std::vector<int> & v, unsigned int x );
  void getSurelyDeadList( TH1F* h, std::vector<int> & v, std::vector<int> & betweenlist );
  void getNotBetweenList( std::vector<int> & v, std::vector<int> & surely_v, std::vector<int> & betweenlist );
  void getNotMasked( std::vector<int> & v, TH1F* h );
  void getNotInDeadML( std::vector<int> & v, std::vector<int> & deadML_v, const TString & hardware_name, int totalTubes);
  void getNotInDeadLayer( std::vector<int> & v, std::vector<int> & deadLayer_v, const TString & hardware_name, int totalTubes);
  void getNotInDeadMezz( std::vector<int> & v, std::vector<int> & deadMezz_v, const TString & hardware_name, int totalTubes);
  void getAllBins( TH1F* h, std::vector<int> & v );
  void getAllBinsInRange( TH1F* h, std::vector<int> & v, int x1, int x2 );
  void getAllBinsInRangeBelowCutoff( TH1F* h, std::vector<int> & v, int crustCutoff, int x1, int x2 );
  void getAllUnmaskedBinsInRangeBelowCrustCutoff( TH1F* h, std::vector<int> & v, int crustCutoff, int x1, int x2 );
  void getBins( TH1F* h, std::vector<int> & indices, std::vector<int> & content );
  void removeZeros( std::vector<int> & v );
  void getAllTubes( TH1F* h, std::vector<int> & v );
  void getNonEmptyBins( TH1F* h, std::vector<int> & v );
  void getNonEmptyTubes( TH1F* h, std::vector<int> & v );
  template <class T>
    double getMean( std::vector<T> & v );
  double getMean_WithCut( std::vector<int> & v, double cut );
  void getNoBetweens( TH1F* h, std::vector<int> & no_dead, std::vector<int> & no_betweens, std::vector<int> & betweenlist, float minBinContent );
  void getNoBetweens_middle( std::vector<int> no_betweens, std::vector<int> & no_betweens_middle );
  void getNoBetweens_lowmiddle( std::vector<int> no_betweens, std::vector<int> & no_betweens_lowmiddle );
  void getNoBetweens_middle_aggressive( std::vector<int> no_betweens, std::vector<int> & no_betweens_middle_aggressive );
  void getNoBetweens_middle_semiaggressive( std::vector<int> no_betweens, std::vector<int> & no_betweens_middle_aggressive );
  void getNoBetweens_lowmiddle_aggressive( std::vector<int> no_betweens, std::vector<int> & no_betweens_lowmiddle_aggressive );
  void getNoBetweens_highmiddle_aggressive( std::vector<int> no_betweens, std::vector<int> & no_betweens_lowmiddle_aggressive );
  double getStandardDevFromMean( std::vector<int> & v, double mean );
  double getStandardDevFromMean_neg( std::vector<int> & v, double mean );
  double getStandardDevFromMean_pos( std::vector<int> & v, double mean );
  void getNoisyList( TH1F* h, std::vector<int> & no_betweens_middle, std::vector<int> & no_betweens_middle_aggressive, double num_std_dev, std::vector<int> & noisylist); 
  void getNoisyList2( TH1F* h, std::vector<int> & no_betweens_middle, std::vector<int> & no_betweens_middle_aggressive, double num_pct, std::vector<int> & noisylist);
  void getIneffList( TH1F* h, std::vector<int> & no_betweens_middle, std::vector<int> & no_betweens_lowmiddle_aggressive, double num_std_dev, std::vector<int> & inefflist);
  void getIneffList( TH1F* h, std::vector<int> & no_betweens_middle, std::vector<int> & no_betweens_lowmiddle_aggressive, double num_std_dev_1, double num_std_dev_2, std::vector<int> & inefflist); 
  void getIneffList2( TH1F* h, std::vector<int> & no_betweens_middle, std::vector<int> & no_betweens_lowmiddle_aggressive, double num_pct, std::vector<int> & inefflist);
  void getIneffList2( TH1F* h, std::vector<int> & no_betweens_middle, std::vector<int> & no_betweens_lowmiddle_aggressive, double num_pct_1, double num_pct_2, std::vector<int> & inefflist);
  void getIneffList2( std::vector<int> & indices, TH1F* h, std::vector<int> & no_betweens_middle, std::vector<int> & no_betweens_lowmiddle_aggressive, double num_pct, std::vector<int> & inefflist);
  bool validityCheckDead( std::vector<int> & no_betweens_middle, std::vector<int> & no_betweens_middle_aggressive );
  bool validityCheckNoise( std::vector<int> & no_betweens_middle, std::vector<int> & no_betweens_middle_aggressive /*, int max*/ ); //max parameter is not currently used
  void getNonEmptyNonNoisyTubes( TH1F* h_tube_fornoise, TH1F* h_tube, std::vector<int> & non_dead_non_noisy);
  void removeNonContiguous( std::vector<int> & betweenlist, unsigned int sz );
  void CatalogNoisyEff( TH1F* h_tube_bkd, TH1F* h_tube_fornoise, TH1F* num, TH1F* den, int nTriggers, TH1F* EffVSNoise_num, TH1F* EffVSNoise_den, TH1F* EffVSNoise_ADCCut_num, TH1F* EffVSNoise_ADCCut_den, TH1F* EffVSBkd_num, TH1F* EffVSBkd_den);
  std::vector<int> GetNoisyTubes( TH1F* h_tube_fornoise, int nTriggers, float frequency);//Simply determine noisy tubes from freq.
  std::vector<int> GetNoisyTubes( TH1F* h_tube_fornoise, TH1F* h_tube, TString chamber, int & validity);
  std::vector<int> GetNoisyTubes_WithoutForNoise( TH1F* h_tube, int & validity, TString chamber );
  //std::vector<int> GetDeadTubes( TH1F* h_tube_fornoise, TH1F* h_tube, TString chamber);
  std::vector<int> GetDeadTubes( TH1F* h_tube, int & validity, std::vector<int> & deadML_v, std::vector<int> & deadLayer_v, std::vector<int> & deadMezz_v );
  //std::vector<int> GetMaskedTubesForDead( TH1F* h_tube_fornoise, TH1F* h_tube, TString chamber);
  std::vector<int> GetMaskedTubesForDead( TH1F* h_tube );
  //std::vector<int> GetMaskedTubesForNoisy( TH1F* h_tube_fornoise, TH1F* h_tube, TString chamber);
  std::vector<int> GetMaskedTubesForNoisy( TH1F* h_tube_fornoise );
  double GetMeanFromHist( TH1F* h_tube );
  double GetStandardDevFromHist( TH1F* h_tube, double mean );
  int getLastTubeInLayer(int & firstTubeInLayer, const TString & hardware_name, int totalTubes);
  std::vector<TubeRange> getLayerRanges(const TString & hardware_name, int totalTubes);
  int getLastTubeInML(int & firstTubeInML, const TString & hardware_name, int totalTubes);
  std::vector<TubeRange> getMLRanges(const TString & hardware_name, int totalTubes);
  std::vector<TubeRange> getMezzRanges(const TString & hardware_name, int totalTubes, int & groupsPerLayer);
  int Get_ML_of_Mezz(int &mezz, const TString & hardware_name, int totalTubes);
  int Get_ML_of_Mezz_degenerate(int &mezz, const TString & hardware_name, int totalTubes);
  std::vector<int> /*void*/ FindDeadMezz( TH1F* h_tube, const TString & hardware_name, std::vector<int> & deadML_v);
  std::vector<int> /*void*/ FindDeadLayer( TH1F* h_tube, const TString & hardware_name, std::vector<int> & deadML_v);
  std::vector<int> /*void*/ FindDeadML( TH1F* h_tube, const TString & hardware_name);
  int GetNumML(const TString & hardware_name);
  int GetNumLayersPerML(const TString & hardware_name);
  int GetNumTubesPerMezz(const TString & hardware_name, int mezz);//For Getting total tube count
  //global layer i.e. 5 instead of ML2 layer 2
  int GetNumTubesPerML(const TString & hardware_name, int layer, int TotalTubes);//For Getting total tube count
  int GetNumTubesPerLayer(const TString & hardware_name, int ML, int TotalTubes);//For Getting total tube count
  int Get_ML_of_Layer(int & layer, const TString & hardware_name);
  int Get_ML_of_Tube(int & tubeID, const TString & hardware_name, int totalTubes);
  int Get_Layer_of_Tube(int & tubeID, const TString & hardware_name, int totalTubes);
  int Get_Mezz_of_Tube(int & tubeID, const TString & hardware_name/*, int totalTubes*/, std::vector<TubeRange> & tubeRanges, int groupsPerLayer);
  int GetNumNonMaskedTubes( TH1F* h_tube );
  int GetNumMezz( TH1F* h_tube );
  bool AinB( int A, std::vector<int> & B );
  bool AinB( int A, const std::vector<int> * B );

  //For dumping pdf
  void FillPDF(const std::string & inFilename, TH1F* hDead, TH1F* hNoise, TCanvas* c, 
	       const std::vector<int>* deadTubes, const std::vector<int>* deadASD, const std::vector<int>* deadMEZZ, const std::vector<int>* deadLayer, const std::vector<int>* deadML,
	       const std::vector<int>* noisyTubes, const std::vector<int>* noisyASD, const std::vector<int>* noisyMEZZ, const std::vector<int>* noisyLayer, const std::vector<int>* noisyML, 
	       int deadTubesChamberValidity, int noisyTubesChamberValidity,
	       bool draw_Mezz_L_ML_guidlines, bool draw_mean, bool draw_masked_tubes, bool separate_dead_noisy_histos, bool draw_validity_message, bool draw_histo_if_nothing_to_report);

} // end namespace

#endif
