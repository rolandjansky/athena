/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



//File containing splitting functions
#include "TrigFTKSim/tsp/FTKAMSplit.h"
// #include <algorithm>

using namespace std;


FTKAMSplit::FTKAMSplit() :
    m_DCSplit(0), m_DCThreshold(0),
    m_AMPatternThreshold(4194304), m_deltaThresholdBin(-1),
    m_totalDeltaPatterns(0), m_BankVolume(0),
    m_volumeReduction(0)
{}


FTKAMSplit::FTKAMSplit(int AMSplit, int maxAMAfterSplit, int /*minDVolOverDNPatt*/) :
    m_DCSplit (AMSplit), m_DCThreshold(AMSplit),
    m_AMPatternThreshold(maxAMAfterSplit), m_deltaThresholdBin(-1),
    m_totalDeltaPatterns(0), m_BankVolume(0),
    m_volumeReduction(0)
{}

void FTKAMSplit::mergeClusters(pair<int, int> /*merge*/)
{
}

list<AMSelection> FTKAMSplit::splitAMPattern(AMSelection /*AMPattern*/, const vector< pair<int, int> >& /*dendrogram*/, double /*patternVolume*/, int /*size*/ )
{
    return list<AMSelection>();
}

/*One threshold Version*/
void FTKAMSplit::splitDCThreshold(AMSelection /*AMPattern*/, int /*numberDCBits*/)
{
}





int FTKAMSplit::calculateClusterDistance(int /*i*/, int /*j*/, int /*setDCMask*/)
{
  return 0;
}


int FTKAMSplit::clusterDistanceZeroDCmask( pair<UInt_t, UInt_t> &/*iPair*/, //
                                           pair<UInt_t, UInt_t> &/*jPair*/, //
                                           UInt_t &/*dc_mask*/) {

  return 0;
}



int FTKAMSplit::calculateVolume( UInt_t &/*dc_mask*/ ) {

  return 0;
}



int FTKAMSplit::countSetBits( UInt_t /*dc_mask*/ ) {

  return 0;
}




bool FTKAMSplit::sortPairs(const pair<int, int> &/*iPair*/, const pair<int, int> &/*jPair*/)
{
    return false;
}




void FTKAMSplit::determineClusterSplit(AMSelection /*AMPattern*/, const vector< pair<int, int> >& /*dendrogram*/)
{
}

void FTKAMSplit::clusterAMSplit(AMSelection /*AMPattern*/)
{

}




void FTKAMSplit::bankVolumeCheck(const list<AMSelection>& /*AMPatternList*/)
{
}



list<AMSelection> FTKAMSplit::splittingAlgorithms(const list<AMSelection>& AMPatternList) {

  return AMPatternList;
}



double FTKAMSplit::calculateDeltaVolumeOverDeltaPatternCumulative( TH1D */*histo*/ ) {

  return 0;
}



int FTKAMSplit::calculateThresholdBins( TH1D* /*histo*/ ) {

  return 0;
}



void FTKAMSplit::initHistograms() {


}



void FTKAMSplit::prepare_similarityMatrix() {

}


double FTKAMSplit::calculateAMCoverage( list<AMSelection>::iterator /*iAMDCbits*/ ) {
  return 0;
}



void FTKAMSplit::selectSplitMethod( list<AMSelection>::iterator /*iAMDCbits*/, int /*countDC*/ ) {
}



TH1D* FTKAMSplit::selectHistogram() {
  return 0;
}



void FTKAMSplit::printPreliminaryNumbers( int /*nPatWithDCbits*/, int /*nPattWithoutDCbits*/ ) {
}



void FTKAMSplit::treat_SinglePatternCase( list<AMSelection>::iterator /*iAMDCbits*/, int &/*no*/ ) {

}




UInt_t FTKAMSplit::calculateDCfromHalfPlaneMask( list<AMSelection>::iterator /*iAMDCbits*/ ) {

  return 0;
}
 

