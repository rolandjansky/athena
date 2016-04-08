/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigHisto2D.h"

using namespace TrigHistoCutType;

//---------------------------------------------------------------

TrigHisto2D::TrigHisto2D(): TrigHisto(),
			    m_nbins_y(0),
			    m_underflowBin_y(0),
			    m_overflowBin_y(0),
			    m_min_y(0.),
			    m_max_y(0.),
			    m_binSize_y(0.) { 
}

//---------------------------------------------------------------

TrigHisto2D::TrigHisto2D(unsigned int nbins_x, 
			 float min_x, float max_x,
			 unsigned int nbins_y, 
			 float min_y, float max_y,
                         const std::vector<float>& contents)
                                                  : TrigHisto(),
						    m_nbins_y(nbins_y),
						    m_min_y(min_y),
						    m_max_y(max_y) {
  m_nbins_x = nbins_x;
  m_min_x = min_x;
  m_max_x = max_x;
  
  unsigned int nbins;
  
  nbins = (nbins_x+2)*(nbins_y+2); // Two additional bins for under and overflow per 1d profile.

  m_contents = contents;
  m_contents.resize(nbins, 0.); // Two additional bins for under and overflow per 1d profile.
 
  if(m_nbins_x != 0) {
    m_binSize_x = (m_max_x - m_min_x)/((float)m_nbins_x); // Calculate bin size.
  }
  else {
    m_binSize_x = 0.;
  }
  if(m_nbins_y != 0) {
    m_binSize_y = (m_max_y - m_min_y)/((float)m_nbins_y); // Calculate bin size.
  }
  else {
    m_binSize_y = 0.;
  }
  
  m_underflowBin_x = 0; // Cache this to make the code more readable.
  m_overflowBin_x = m_nbins_x+1; // Cache this to make the code more readable and faster.
  m_underflowBin_y = 0; // Cache this to make the code more readable.
  m_overflowBin_y = m_nbins_y+1; // Cache this to make the code more readable and faster.
} 

TrigHisto2D::TrigHisto2D(unsigned int nbins_x, 
			 float min_x, float max_x,
			 unsigned int nbins_y, 
			 float min_y, float max_y)
  : TrigHisto2D (nbins_x, min_x, max_x, nbins_y, min_y, max_y,
                 std::vector<float>())
{
}


//---------------------------------------------------------------

TrigHisto2D::~TrigHisto2D() {
}

//---------------------------------------------------------------

TrigHisto2D::TrigHisto2D(const TrigHisto2D& trigHisto): TrigHisto() {  
  m_nbins_x = trigHisto.m_nbins_x;
  m_min_x = trigHisto.m_min_x;
  m_max_x = trigHisto.m_max_x;
  
  m_nbins_y = trigHisto.m_nbins_y;
  m_min_y = trigHisto.m_min_y;
  m_max_y = trigHisto.m_max_y;
  
  m_contents = trigHisto.m_contents;

  m_binSize_x = trigHisto.m_binSize_x;
  m_binSize_y = trigHisto.m_binSize_y;

  m_underflowBin_x = trigHisto.m_underflowBin_x;
  m_overflowBin_x = trigHisto.m_overflowBin_x; 
  m_underflowBin_y = trigHisto.m_underflowBin_y;
  m_overflowBin_y = trigHisto.m_overflowBin_y;
}

//---------------------------------------------------------------

TrigHisto2D::TrigHisto2D(TrigHisto2D&& trigHisto): TrigHisto() {  
  m_nbins_x = trigHisto.m_nbins_x;
  m_min_x = trigHisto.m_min_x;
  m_max_x = trigHisto.m_max_x;
  
  m_nbins_y = trigHisto.m_nbins_y;
  m_min_y = trigHisto.m_min_y;
  m_max_y = trigHisto.m_max_y;
  
  m_contents = std::move(trigHisto.m_contents);

  m_binSize_x = trigHisto.m_binSize_x;
  m_binSize_y = trigHisto.m_binSize_y;

  m_underflowBin_x = trigHisto.m_underflowBin_x;
  m_overflowBin_x = trigHisto.m_overflowBin_x; 
  m_underflowBin_y = trigHisto.m_underflowBin_y;
  m_overflowBin_y = trigHisto.m_overflowBin_y;
}

//---------------------------------------------------------------

TrigHisto2D& TrigHisto2D::operator=(const TrigHisto2D& trigHisto) {
  if (this != &trigHisto) {
    m_nbins_x = trigHisto.m_nbins_x;
    m_min_x = trigHisto.m_min_x;
    m_max_x = trigHisto.m_max_x;

    m_nbins_y = trigHisto.m_nbins_y;
    m_min_y = trigHisto.m_min_y;
    m_max_y = trigHisto.m_max_y;
  
    m_contents = trigHisto.m_contents;

    m_binSize_x = trigHisto.m_binSize_x;
    m_binSize_y = trigHisto.m_binSize_y;

    m_underflowBin_x = trigHisto.m_underflowBin_x;
    m_overflowBin_x = trigHisto.m_overflowBin_x; 
    m_underflowBin_y = trigHisto.m_underflowBin_y;
    m_overflowBin_y = trigHisto.m_overflowBin_y;
  }
  return *this;
}

//---------------------------------------------------------------

TrigHisto2D& TrigHisto2D::operator=(TrigHisto2D&& trigHisto) {
  if (this != &trigHisto) {
    m_nbins_x = trigHisto.m_nbins_x;
    m_min_x = trigHisto.m_min_x;
    m_max_x = trigHisto.m_max_x;
  
    m_nbins_y = trigHisto.m_nbins_y;
    m_min_y = trigHisto.m_min_y;
    m_max_y = trigHisto.m_max_y;
  
    m_contents = std::move(trigHisto.m_contents);

    m_binSize_x = trigHisto.m_binSize_x;
    m_binSize_y = trigHisto.m_binSize_y;

    m_underflowBin_x = trigHisto.m_underflowBin_x;
    m_overflowBin_x = trigHisto.m_overflowBin_x; 
    m_underflowBin_y = trigHisto.m_underflowBin_y;
    m_overflowBin_y = trigHisto.m_overflowBin_y;
  }
  return *this;
}

//---------------------------------------------------------------

void TrigHisto2D::fill(float value_x, float value_y, float weight) {
  unsigned int ibin_x, ibin_y, ibin;
  
  ibin_x = findBin(m_nbins_x, m_min_x, m_max_x, m_binSize_x, value_x);
  ibin_y = findBin(m_nbins_y, m_min_y, m_max_y, m_binSize_y, value_y);
  
  ibin = ibin_y*(m_nbins_x+2) + ibin_x; // Find position in 1d data array

  m_contents[ibin] += weight; // Append the weight to this bin
}

//---------------------------------------------------------------

double TrigHisto2D::sumEntries(float value_x, float value_y, int cutType) const {
  unsigned int ibin, ibin_x, ibin_y, ibin_x_selected, ibin_y_selected;
  double entries;


  
  // Find the x bin index that the cut corresponds to.
  ibin_x_selected = findBin(m_nbins_x, m_min_x, m_max_x, m_binSize_x, value_x);
  
  // Find the y bin index that the cut corresponds to.
  ibin_y_selected = findBin(m_nbins_y, m_min_y, m_max_y, m_binSize_y, value_y);
  
  entries = 0.;
  
  
  if( m_nbins_x==0 || m_nbins_y==0 ){
    return 0;
  }
  else{

    if(cutType == BELOW_X_BELOW_Y) {
      for(ibin_x = m_underflowBin_x; ibin_x <= ibin_x_selected; ibin_x++) {
	for(ibin_y = m_underflowBin_y; ibin_y <= ibin_y_selected; ibin_y++) {
	  ibin = ibin_y*(m_nbins_x+2) + ibin_x; // Find position in 1d data array
	  entries += m_contents[ibin];
	}
      }
    }
    else if(cutType == ABOVE_X_BELOW_Y) {
      for(ibin_x = ibin_x_selected; ibin_x <= m_overflowBin_x; ibin_x++) {
	for(ibin_y = m_underflowBin_y; ibin_y <= ibin_y_selected; ibin_y++) {
	  ibin = ibin_y*(m_nbins_x+2) + ibin_x; // Find position in 1d data array
	  entries += m_contents[ibin];
	}
      }
    }
    else if(cutType == BELOW_X_ABOVE_Y) {
      for(ibin_x = m_underflowBin_x; ibin_x <= ibin_x_selected; ibin_x++) {
	for(ibin_y = ibin_y_selected; ibin_y <= m_overflowBin_y; ibin_y++) {
	  ibin = ibin_y*(m_nbins_x+2) + ibin_x; // Find position in 1d data array
	  entries += m_contents[ibin];
	}
      }
    }
    else if(cutType == ABOVE_X_ABOVE_Y) {   
      for(ibin_x = ibin_x_selected; ibin_x <= m_overflowBin_x; ibin_x++) {
	for(ibin_y = ibin_y_selected; ibin_y <= m_overflowBin_y; ibin_y++) {
	  ibin = ibin_y*(m_nbins_x+2) + ibin_x; // Find position in 1d data array
	  entries += m_contents[ibin];
	}
      }
    }
    else {
      return 0;
    }
  }// else of m_nbins!=0
  
  return entries;
}

//---------------------------------------------------------------

TrigHisto1D TrigHisto2D::profileX(void) const {
  unsigned int ibin, ibin_x, ibin_y;

  // Define size carefully to avoid memory problems in for loop.
  std::vector<float> contents((m_overflowBin_x-m_underflowBin_x+1),0.);

  // Sum all y bins for each x bin.
  for(ibin_x = m_underflowBin_x; ibin_x <= m_overflowBin_x; ibin_x++) {
    for(ibin_y = m_underflowBin_y; ibin_y <= m_overflowBin_y; ibin_y++) {
      ibin = ibin_y*(m_nbins_x+2) + ibin_x; // Find position in 1d data array
      contents[ibin_x] += m_contents[ibin];
    }
  }

  // Return a 1D histogram containing the profile contents.
  return TrigHisto1D(m_nbins_x, m_min_x, m_max_x, contents);
}

//---------------------------------------------------------------

TrigHisto1D TrigHisto2D::profileY(void) const {
  unsigned int ibin, ibin_x, ibin_y;

  // Define size carefully to avoid memory problems in for loop.
  std::vector<float> contents((m_overflowBin_y-m_underflowBin_y+1),0.);

  // Sum all x bins for each y bin.
  for(ibin_y = m_underflowBin_y; ibin_y <= m_overflowBin_y; ibin_y++) {
    for(ibin_x = m_underflowBin_x; ibin_x <= m_overflowBin_x; ibin_x++) {
      ibin = ibin_y*(m_nbins_x+2) + ibin_x; // Find position in 1d data array
      contents[ibin_y] += m_contents[ibin];
    }
  }

  // Return a 1D histogram containing the profile contents.
  return TrigHisto1D(m_nbins_y, m_min_y, m_max_y, contents);
}

//---------------------------------------------------------------
