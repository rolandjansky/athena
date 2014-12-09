/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Header file for split functions
//File containing splitting functions
#ifndef FTKAMSPLIT_H
#define FTKAMSPLIT_H
#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/tsp/FTKTSPBank.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TF1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TMath.h"
#include <iostream>
#include <list>
#include <map>
#include <vector>
#include <algorithm>
#include <math.h>
#include <iomanip>
#include <assert.h>
#include <limits.h>

using namespace std;
class FTKAMSplit
{
private:
    int m_DCSplit;
    int m_DCThreshold;
    int m_AMPatternThreshold;
    int m_deltaThresholdBin;
    int m_totalDeltaPatterns;

    double m_BankVolume;
    double m_volumeReduction;
    list<AMSelection> SplitAMPatternList;


    vector< pair<UInt_t, UInt_t> > cluster;
    vector< vector<int> > sim;

    vector<UInt_t> dcMask;
    vector<double> splitVolume;
    vector<double> deltaVolumeDeltaPattern;
    vector<double> deltaVolumeCluster;
    vector<int> deltaPatternCluster;
    vector< vector<UInt_t> > amPatterns;
    vector< pair<int, int> > mergelist;

    TH1D *DCBitsHist;
    TH1D *deltaVolumeOverDeltaPatternsBin1;
    TH1D *deltaVolumeOverDeltaPatternsClusterBin1;

    TH1D *deltaVolume;
    TH1D *deltaVolumeClusterH;
    TH1D *deltaPatterns;
    TH1D *deltaPatternsClusterH;

    TH1D *WeightedDeltaVolumeOverDeltaPatternsBin1;
    TH1D *WeightedDeltaVolumeOverDeltaPatternsClusterBin1;

    TH1D *DeltaVolumeOverDeltaPatternCumulativeH;


public:
    FTKAMSplit();
    FTKAMSplit(int);

    void setDCSplit(int val){ m_DCSplit = val; }
    const int &getDCSplit(){ return m_DCSplit; }

    void setDCThreshold(int val){ m_DCThreshold = val; }
    const int &getDCThreshold(){ return m_DCThreshold; }    

    void setdeltaThresholdBin(int val){ m_deltaThresholdBin = val; }
    const int &getdeltaThresholdBin(){ return m_deltaThresholdBin; }

    void settotalDeltaPatterns(int val){ m_totalDeltaPatterns = val; }
    const int &gettotalDeltaPatterns(){ return m_totalDeltaPatterns; }
    // void addtotalDeltaPatterns(int val){ m_totalDeltaPatterns += val; }

    void mergeClusters( pair<int, int> );

    list<AMSelection> splitAMPattern(AMSelection , vector< pair<int, int> > , double , int size = -1);

    void splitDCThreshold(AMSelection , int );

    int calculateClusterDistance(int i, int j, int setDCMask = 0);

    static bool sortPairs(const pair<int, int> &, const pair<int, int> &);

    void determineClusterSplit(AMSelection , vector< pair<int, int> > );

    void clusterAMSplit(AMSelection );

    void bankVolumeCheck(list<AMSelection> AMPatternList);

    list<AMSelection> splittingAlgorithms(list<AMSelection> AMPatternList);






};



#endif
