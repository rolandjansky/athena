/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigHisto1D.h"

using namespace TrigHistoCutType;

//---------------------------------------------------------------

TrigHisto1D::TrigHisto1D(): TrigHisto() {
}

//---------------------------------------------------------------

TrigHisto1D::TrigHisto1D(unsigned int nbins_x, 
			 float min_x, 
			 float max_x): TrigHisto() {
  
  m_nbins_x = nbins_x;
  m_min_x = min_x;
  m_max_x = max_x;
  
  m_contents.clear();
  m_contents.resize(m_nbins_x+2, 0); // Two additional bins for under and overflow.
  
  if(m_nbins_x != 0) {
    m_binSize_x = (m_max_x - m_min_x)/((float)m_nbins_x); // Calculate bin size.
  }
  else {
    m_binSize_x = 0.;
  }
  
  m_underflowBin_x = 0; // Cache this to make the code more readable.
  m_overflowBin_x = m_nbins_x+1; // Cache this to make the code more readable and faster.
}

//---------------------------------------------------------------

TrigHisto1D::TrigHisto1D(unsigned int nbins_x, 
			 float min_x, 
			 float max_x, 
			 const std::vector<float>& contents) {
  m_nbins_x = nbins_x;
  m_min_x = min_x;
  m_max_x = max_x;

  m_contents = contents;
  m_contents.resize(m_nbins_x+2, 0); // Resize if it not the correct size.
  
  m_binSize_x = (m_max_x - m_min_x)/((float)m_nbins_x); // Calculate bin size.
  
  m_underflowBin_x = 0; // Cache this to make the code more readable.
  m_overflowBin_x = m_nbins_x+1; // Cache this to make the code more readable and faster.
}

//---------------------------------------------------------------

TrigHisto1D::~TrigHisto1D() {
}

//---------------------------------------------------------------

TrigHisto1D::TrigHisto1D(const TrigHisto1D& trigHisto): TrigHisto() {
  m_nbins_x = trigHisto.m_nbins_x;
  m_min_x = trigHisto.m_min_x;
  m_max_x = trigHisto.m_max_x;
  m_contents = trigHisto.m_contents;
  m_binSize_x = trigHisto.m_binSize_x;
  m_underflowBin_x = trigHisto.m_underflowBin_x;
  m_overflowBin_x = trigHisto.m_overflowBin_x;
}

//---------------------------------------------------------------

TrigHisto1D::TrigHisto1D(TrigHisto1D&& trigHisto): TrigHisto() {
  m_nbins_x = trigHisto.m_nbins_x;
  m_min_x = trigHisto.m_min_x;
  m_max_x = trigHisto.m_max_x;
  m_contents = std::move(trigHisto.m_contents);
  m_binSize_x = trigHisto.m_binSize_x;
  m_underflowBin_x = trigHisto.m_underflowBin_x;
  m_overflowBin_x = trigHisto.m_overflowBin_x;
}

//---------------------------------------------------------------

TrigHisto1D& TrigHisto1D::operator=(const TrigHisto1D& trigHisto) {
  if (this != &trigHisto) {
    m_nbins_x = trigHisto.m_nbins_x;
    m_min_x = trigHisto.m_min_x;
    m_max_x = trigHisto.m_max_x;
    m_contents = trigHisto.m_contents;
    m_binSize_x = trigHisto.m_binSize_x;
    m_underflowBin_x = trigHisto.m_underflowBin_x;
    m_overflowBin_x = trigHisto.m_overflowBin_x;
  }
  return *this;
}

//---------------------------------------------------------------

TrigHisto1D& TrigHisto1D::operator=(TrigHisto1D&& trigHisto) {
  if (this != &trigHisto) {
    m_nbins_x = trigHisto.m_nbins_x;
    m_min_x = trigHisto.m_min_x;
    m_max_x = trigHisto.m_max_x;
    m_contents = std::move(trigHisto.m_contents);
    m_binSize_x = trigHisto.m_binSize_x;
    m_underflowBin_x = trigHisto.m_underflowBin_x;
    m_overflowBin_x = trigHisto.m_overflowBin_x;
  }
  return *this;
}

//---------------------------------------------------------------

void TrigHisto1D::fill(float value_x, float weight) {
  unsigned int ibin;
  
  ibin = findBin(m_nbins_x, m_min_x, m_max_x, m_binSize_x, value_x);
  
  m_contents[ibin] += weight; // Append the weight to this bin.
}

//---------------------------------------------------------------

double TrigHisto1D::sumEntries(float value_x, int cutType) const {
  unsigned int ibin_x, ibin_x_selected;
  double entries;
  
  // Find which bin contains the value_x.
  ibin_x_selected = findBin(m_nbins_x, m_min_x, m_max_x, m_binSize_x, value_x);
  
  entries = 0.;

  if( m_nbins_x==0 ){
    return 0;
  }
  else{

    if (cutType == BELOW_X) {
      for(ibin_x = m_underflowBin_x; ibin_x <= ibin_x_selected; ibin_x++) {
	entries += m_contents[ibin_x];
      }
    }
    else if(cutType == ABOVE_X) {
      for(ibin_x = ibin_x_selected; ibin_x <= m_overflowBin_x; ibin_x++) {
	entries += m_contents[ibin_x];
      }
    }
    else {
      return 0; //<! Should report error message.
    }
  }

  return entries;
}

//---------------------------------------------------------------

