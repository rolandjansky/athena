/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigHisto.h"

//---------------------------------------------------------------

TrigHisto::TrigHisto(void): m_nbins_x(0),
			    m_underflowBin_x(0),
			    m_overflowBin_x(0),
			    m_min_x(0.),
			    m_max_x(0.),
			    m_binSize_x(0.) {
}

//---------------------------------------------------------------

TrigHisto::~TrigHisto(void) {
}

//---------------------------------------------------------------

void TrigHisto::clear(void) {
  m_itr = m_contents.begin();
  m_itr_end = m_contents.end();
  for(; m_itr != m_itr_end; ++m_itr) (*m_itr) = 0.; 
}

//---------------------------------------------------------------

// Require histogram sizes such that it might be called by TrigHisto2D too.
unsigned int TrigHisto::findBin(unsigned int nbins, 
				float h_min, 
				float h_max, 
				float binSize, 
				float value) const {
  unsigned int ibin = 0;
  
  if(value < h_min) { // Underflow
    ibin = 0;
  }
  else if( !(value < h_max)) { // Overflow (catches NaN)
    ibin = nbins+1;
  }
  else {
    while(value > (ibin*binSize+h_min) && ibin <= nbins) { // None under/overflow from 1 to nbins
      ibin++;
    }
  }
  
  return ibin;
}

//---------------------------------------------------------------
