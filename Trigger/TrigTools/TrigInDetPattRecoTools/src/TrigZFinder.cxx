/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetPattRecoTools/TrigZFinder.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
//#include "TrigInDetPattRecoEvent/TrigInDetTracklet.h"
//#include <iostream>
#include <cmath>

TrigZFinder::TrigZFinder() {}
TrigZFinder::~TrigZFinder() {}

void TrigZFinder::initialize(const TrigRoiDescriptor* roi) {
	setBinEdges(roi);
	//setBinEdges();
}

void TrigZFinder::finalize() {
  m_zHist.clear();
}

void TrigZFinder::setBinEdges() {
	std::vector<float> binEdges;	
  for (unsigned int ii = 0; ii < 41; ii++) {
    float binEdge = -168+168*ii/20.0;
    binEdges.push_back(binEdge);
	}
	m_zHist = ZHist(binEdges);
}

void TrigZFinder::setBinEdges(const TrigRoiDescriptor* roi) {
	double eta = 0;
	if (roi) {
		eta = roi->eta();
	}
  double invZBinSize = 1./(m_minZBinSize + m_zBinSizeEtaCoeff*fabs(eta));
  long HalfZhistoBins = long ( std::ceil( m_ZFinder_MaxZ*invZBinSize ) );
  long NumZhistoBins = 2*HalfZhistoBins;
	std::vector<float> binEdges;	
	for (unsigned int ii = 0; ii < NumZhistoBins; ii++) {
		float binEdge = m_ZFinder_MaxZ*(-1.0 + float(ii)/HalfZhistoBins);
		binEdges.push_back(binEdge);
	}
	m_zHist = ZHist(binEdges);
	//std::cout << "binEdges assigned, size " << m_zHist.binEdges().size() << std::endl;
	//std::cout << "entries assigned, size " << m_zHist.entries().size() << std::endl;
}


float TrigZFinder::findZ() const {
  if (m_zHist.getSum()==0) {
    return -99999;
  }
	return m_zHist.getMaxSlidingWindow();
}

const std::vector<float>& TrigZFinder::findMultipleZ(const unsigned int nVertices) {
  if (m_zHist.getSum()==0) {
    return m_emptyVector;
  }
  return m_zHist.getAllMaxSlidingWindow(nVertices);
}

