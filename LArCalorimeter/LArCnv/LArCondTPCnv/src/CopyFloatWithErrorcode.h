/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef COPYFLOATWITHERRORCODE_H
#define COPYFLOATWITHERRORCODE_H

/**
 * @file CopyFloatWithErrorcode.h
 * @brief Copies a single float from a persistent LAr calibration object, protected against roundoff errors
 * @author Walter Lampl <walter.lampl@cern.ch>
 * $Id: CopyFloatWithErrorcode.h,v 1.1 2009-04-27 15:22:44 gunal Exp $
 */

#include "LArElecCalib/LArCalibErrorCode.h"
#include <math.h>
 
//A copy rountine to get floats from persistency,
// protected against roundoff problems and nans
inline float copyFloatPT(const float& pers) {
  //Protect against rounding effects and nans.
  if (std::isnan(pers) || pers<=1.0+LArElecCalib::ERRORCODE)
    return (float)LArElecCalib::ERRORCODE;
  else
    return pers;
}


#endif
