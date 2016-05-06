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
    std::list<AMSelection> SplitAMPatternList;


    std::vector< std::pair<UInt_t, UInt_t> > cluster;
    std::vector< std::vector<int> > sim;

    std::vector<UInt_t> dcMask;
    std::vector<double> splitVolume;
    std::vector<double> deltaVolumeDeltaPattern;
    std::vector<double> deltaVolumeCluster;
    std::vector<int> deltaPatternCluster;
    std::vector< std::vector<UInt_t> > amPatterns;
    std::vector< std::pair<int, int> > mergelist;

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


    int countSetBits( UInt_t dc_mask );
    int clusterDistanceZeroDCmask( std::pair<UInt_t, UInt_t> &iPair, //
                                   std::pair<UInt_t, UInt_t> &jPair, //
                                   UInt_t &dc_mask);
    int calculateVolume( UInt_t &dc_mask ); 
    double calculateDeltaVolumeOverDeltaPatternCumulative( TH1D *histo );
    int calculateThresholdBins( TH1D* histo );
    void initHistograms();
    void prepare_similarityMatrix();
    double calculateAMCoverage( std::list<AMSelection>::iterator iAMDCbits );
    void selectSplitMethod( std::list<AMSelection>::iterator iAMDCbits, int countDC ); 
    TH1D* selectHistogram();
    void printPreliminaryNumbers( int nPatWithDCbits, int nPattWithoutDCbits );
    void treat_SinglePatternCase( std::list<AMSelection>::iterator iAMDCbits, int &no );
    UInt_t calculateDCfromHalfPlaneMask( std::list<AMSelection>::iterator iAMDCbits );


public:
    FTKAMSplit();
    FTKAMSplit(int AMSplit, int maxAMAfterSplit, int minDVolOverDNPatt);

    void setDCSplit(int val){ m_DCSplit = val; }
    const int &getDCSplit(){ return m_DCSplit; }

    void setDCThreshold(int val){ m_DCThreshold = val; }
    const int &getDCThreshold(){ return m_DCThreshold; }    

    void setdeltaThresholdBin(int val){ m_deltaThresholdBin = val; }
    const int &getdeltaThresholdBin(){ return m_deltaThresholdBin; }

    void settotalDeltaPatterns(int val){ m_totalDeltaPatterns = val; }
    const int &gettotalDeltaPatterns(){ return m_totalDeltaPatterns; }
    // void addtotalDeltaPatterns(int val){ m_totalDeltaPatterns += val; }

    void mergeClusters( std::pair<int, int> );

    std::list<AMSelection> splitAMPattern(AMSelection , const std::vector< std::pair<int, int> >& , double , int size = -1);

    void splitDCThreshold(AMSelection , int );

    int calculateClusterDistance(int i, int j, int setDCMask = 0);

    static bool sortPairs(const std::pair<int, int> &, const std::pair<int, int> &);

    void determineClusterSplit(AMSelection , const std::vector< std::pair<int, int> >& );

    void clusterAMSplit(AMSelection );

    void bankVolumeCheck(const std::list<AMSelection>& AMPatternList);

    std::list<AMSelection> splittingAlgorithms(const std::list<AMSelection>& AMPatternList);






};



#endif
