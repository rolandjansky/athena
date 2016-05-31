/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODTrigMinBias/xAODTrigMinBiasHelperFunc.h"
#include <iostream>

namespace xAOD{
  double sumEntries(const std::vector<float> &cont, 
		    unsigned int nbins_x, 
		    float min_x, 
		    float max_x,
		    unsigned int nbins_y, 
		    float min_y, 
		    float max_y, 
		    float value_x, 
		    float value_y, 
		    int cutType){
    
    
    const unsigned int nbins_x_widened = nbins_x + 2;
    const unsigned int nbins_y_widened = nbins_y + 2;
    
    if((nbins_x_widened*nbins_y_widened) != cont.size()){
      std::cout<<"ERROR in xAOD::sumEntries: nbins and container size don't match!!"<<std::endl;
      return -1.;
    }
    if(nbins_x*nbins_y == 0 ){
      std::cout<<"ERROR in xAOD::sumEntries: nbins_x * nbins_y = 0"<<std::endl;
    }
    unsigned int ibin(0), ibin_x(0), ibin_y(0), ibin_x_selected(0), ibin_y_selected(0);
    const float binWidth_x = (max_x - min_x)/((float)nbins_x);
    const float binWidth_y = (max_y - min_y)/((float)nbins_y);
    
    double entries(0.);
    
    if(value_x < min_x) { // Underflow
      ibin_x = 0;
    }
    else if( !(value_x < max_x) ) { // Overflow (catches NaN)
      ibin_x = nbins_x+1;
    }
    else {
      while(value_x > (ibin_x*binWidth_x+min_x) && ibin_x <= nbins_x) { // None under/overflow from 1 to nbins
	ibin_x++;
      }
    }
    
    //and for y bind
    if(value_y < min_y) { // Underflow
      ibin_y = 0;
    }
    else if( !(value_y < max_y) ) { // Overflow (catches NaN)
      ibin_y = nbins_y+1;
    }
    else {
      while(value_y > (ibin_y*binWidth_y+min_y) && ibin_y <= nbins_y) { // None under/overflow from 1 to nbins
	ibin_y++;
      }
    }
    
    // Find the x bin index that the cut corresponds to.
    ibin_x_selected = ibin_x;
    
    // Find the y bin index that the cut corresponds to.
    ibin_y_selected = ibin_y;
    
    if(cutType == TrigHistoCutType::BELOW_X_BELOW_Y) {
      for(ibin_x = 0; ibin_x <= ibin_x_selected; ibin_x++) {
	for(ibin_y = 0; ibin_y <= ibin_y_selected; ibin_y++) {
	  ibin = ibin_y*(nbins_x_widened) + ibin_x; // Find position in 1d data array
	  entries += cont.at(ibin);
	}
      }
    }
    else if(cutType == TrigHistoCutType::ABOVE_X_BELOW_Y) {
      for(ibin_x = ibin_x_selected; ibin_x < nbins_x_widened; ibin_x++) {
	for(ibin_y = 0; ibin_y <= ibin_y_selected; ibin_y++) {
	  ibin = ibin_y*(nbins_x_widened) + ibin_x; // Find position in 1d data array
	  entries += cont.at(ibin);
	}
      }
    }
    else if(cutType == TrigHistoCutType::BELOW_X_ABOVE_Y) {
      for(ibin_x = 0; ibin_x <= ibin_x_selected; ibin_x++) {
	for(ibin_y = ibin_y_selected; ibin_y < nbins_y_widened; ibin_y++) {
	  ibin = ibin_y*(nbins_x_widened) + ibin_x; // Find position in 1d data array
	  entries += cont.at(ibin);
	}
      }
    }
    else if(cutType == TrigHistoCutType::ABOVE_X_ABOVE_Y) {   
      for(ibin_x = ibin_x_selected; ibin_x < nbins_x_widened; ibin_x++) {
	for(ibin_y = ibin_y_selected; ibin_y < nbins_y_widened; ibin_y++) {
	  ibin = ibin_y*(nbins_x_widened) + ibin_x; // Find position in 1d data array
	  entries += cont.at(ibin);
	}
      }
    }
    else {
      return 0;
    }
  return entries;
  } //end function
} //xAOD namespace