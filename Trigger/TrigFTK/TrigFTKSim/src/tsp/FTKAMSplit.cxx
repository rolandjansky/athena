/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//File containing splitting functions
#include "TrigFTKSim/tsp/FTKAMSplit.h"
// #include <algorithm>

FTKAMSplit::FTKAMSplit() :
    m_DCSplit(0), m_DCThreshold(0),
    m_AMPatternThreshold(4194304), m_deltaThresholdBin(-1),
    m_totalDeltaPatterns(0), m_BankVolume(0),
    m_volumeReduction(0)
{}

FTKAMSplit::FTKAMSplit(int AMSplit) :
    m_DCSplit (AMSplit), m_DCThreshold(AMSplit),
    m_AMPatternThreshold(4194304), m_deltaThresholdBin(-1),
    m_totalDeltaPatterns(0), m_BankVolume(0),
    m_volumeReduction(0)
{}

void FTKAMSplit::mergeClusters(pair<int, int> /*merge*/)
{

}

list<AMSelection> FTKAMSplit::splitAMPattern(AMSelection /*AMPattern*/, vector< pair<int, int> > /*dendrogram*/, double /*patternVolume*/, int /*size*/ )
{
	return list<AMSelection>();
}

void FTKAMSplit::splitDCThreshold(AMSelection /*AMPattern*/, int /*numberDCBits*/)
{

}

int FTKAMSplit::calculateClusterDistance(int /*i*/, int /*j*/, int /*setDCMask*/)
{
	return 0;
}

bool FTKAMSplit::sortPairs(const pair<int, int> &/*iPair*/, const pair<int, int> &/*jPair*/)
{
	return false;
}

void FTKAMSplit::determineClusterSplit(AMSelection /*AMPattern*/, vector< pair<int, int> > /*dendrogram*/)
{
}

void FTKAMSplit::clusterAMSplit(AMSelection /*AMPattern*/)
{
}

void FTKAMSplit::bankVolumeCheck(list<AMSelection> /*AMPatternList*/)
{

}

list<AMSelection> FTKAMSplit::splittingAlgorithms(list<AMSelection> AMPatternList)
{
   return AMPatternList;
}
