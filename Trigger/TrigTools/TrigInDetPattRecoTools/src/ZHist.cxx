/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetPattRecoTools/ZHist.h"
#include <algorithm>
#include <iterator>

ZHist::ZHist(const std::vector<float>& binEdges) {
	if (!std::is_sorted(binEdges.begin(), binEdges.end())) {
		m_valid = false;
	}
	else if (binEdges.size()<2) {
		m_valid = false;
	}
	else {
		m_binEdges = binEdges;
		m_entries = std::vector<unsigned int>(binEdges.size()-1, 0);
	}
	//std::cout << "binEdges assigned, size " << m_binEdges.size() << std::endl;
	//std::cout << "entries assigned, size " << m_entries.size() << std::endl;
}

ZHist::ZHist() {
}

void ZHist::fill(float x) {
	auto upperBinEdge = std::upper_bound(m_binEdges.begin(), m_binEdges.end(), x);
	unsigned int upperBinIndex = std::distance(m_binEdges.begin(), upperBinEdge);
	unsigned int end = std::distance(m_binEdges.end(), upperBinEdge);
	if (upperBinIndex == 0) {
		m_underflow++;
	}
	else if (end == 0) {
		m_overflow++;
	}
	else {
		unsigned int binIndex = upperBinIndex-1;
		m_entries[binIndex] += 1;
	}
  m_sum++;
}

float ZHist::getMax() const{
	auto maxIter = std::max_element(m_entries.begin(), m_entries.end());
	//std::cout << "*maxIter " << *maxIter << std::endl;
	unsigned int maxIndex = std::distance(m_entries.begin(), maxIter);
	//std::cout << "maxIndex " << maxIndex << std::endl;
	return getMidpoint(maxIndex);
}

float ZHist::getMaxSlidingWindow(const std::set<unsigned int> excludedBins) const{
  std::pair<unsigned int, float> maxSlidingWindowPair;
  getMaxSlidingWindowPair(excludedBins, maxSlidingWindowPair);
  return maxSlidingWindowPair.second;
}

//Assume window size of 3 for now
//Return true if good sliding window pair is found
bool ZHist::getMaxSlidingWindowPair(const std::set<unsigned int>& excludedBins,
                                    std::pair<unsigned int, float>& maxSlidingWindowPair) const{
  if (m_sum==0) {
    return false;
  }
	unsigned int maxSum = 0;
	unsigned int maxIndex = 0;
	for (unsigned int ii = 1; ii < m_entries.size() - 1; ii++) {
    std::set<unsigned int> bins = {ii-1, ii, ii+1};
    std::set<unsigned int> intersect;
    std::set_intersection(bins.begin(), bins.end(), excludedBins.begin(), excludedBins.end(), 
                          std::inserter(intersect, intersect.begin()));
    if (intersect.size() > 0) {
      continue;
    }
    else {
      unsigned int sum = m_entries[ii-1] + m_entries[ii] + m_entries[ii+1];
      if (sum > maxSum) {
        maxSum = sum;
        maxIndex = ii;
      }
    }
    if (maxSum > 0) {
      maxSlidingWindowPair = std::make_pair(maxIndex, getMidpoint(maxIndex));
    }
	}
  if (maxSum == 0) {
    return false;
  }
	return true;
}

//Progressively remove bins from consideration as maxima are found
//return vector of vertices
const std::vector<float>& ZHist::getAllMaxSlidingWindow(const unsigned int nVertices) {
  m_vertices.clear();
  std::set<unsigned int> excludedBins;
  for (unsigned int ii = 0; ii < nVertices; ii++) {
    std::pair<unsigned int, float> maxSlidingWindowPair;
    bool hasGoodVertex = getMaxSlidingWindowPair(excludedBins, maxSlidingWindowPair);
    if (hasGoodVertex) {
      m_vertices.push_back(maxSlidingWindowPair.second);
      unsigned int maxIndex = maxSlidingWindowPair.first;
      std::set<unsigned int> newExcludedBins = {maxIndex-1,maxIndex,maxIndex+1};
      excludedBins.insert(newExcludedBins.begin(), newExcludedBins.end());
    }
  }
  return m_vertices;
}


//Returns (lowerBinEdge+upperBinEdge)/2
float ZHist::getMidpoint(unsigned int index) const{
	//std::cout << "binEdges, size " << m_binEdges.size() << std::endl;
	//std::cout << "entries, size " << m_entries.size() << std::endl;
	if (index + 1 < m_binEdges.size()) {
			return 0.5*(m_binEdges[index+1]+m_binEdges[index]);
	}
	else {
		return m_binEdges[index];
	}
}

void ZHist::clear() {
	m_entries.clear();
	m_binEdges.clear();
	m_vertices.clear();
}
	
ZHist::~ZHist(){
}
