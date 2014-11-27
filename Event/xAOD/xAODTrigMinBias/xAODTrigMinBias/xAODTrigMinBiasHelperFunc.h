/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef xAODTrigMinBiasHelperFunc_H
#define xAODTrigMinBiasHelperFunc_H

#include <vector>

namespace xAOD {
  namespace TrigHistoCutType {
    enum CutTypeEnum {
      BELOW_X,
      ABOVE_X,
      BELOW_X_BELOW_Y,
      ABOVE_X_BELOW_Y,
      BELOW_X_ABOVE_Y,
      ABOVE_X_ABOVE_Y};
  }
   double sumEntries(const std::vector<float> &cont, 
		    unsigned int nbins_x, 
		    float min_x, 
		    float max_x,
		    unsigned int nbins_y, 
		    float min_y, 
		    float max_y, 
		    float value_x, 
		    float value_y, 
		    int cutType);
}
#endif