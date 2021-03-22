/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef SCT_GetPotentialValue_h
#define SCT_GetPotentialValue_h
/**
 * @author shaun.roe@cern.char
 * (refactored from existing code)
 * @date 19 March 2021
 **/

/**
 * @brief A free function to return the SCT electric field potential internal to the silicon
 * on a 81 * 115 array
 *      ix = 0 -  80 : 5um step, 40um - 440 um
 *      iy = 0 - 114 : 2.5um step, 0um - 285 um
 **/ 
double 
getPotentialValue(const int ix, const int iy);

#endif

