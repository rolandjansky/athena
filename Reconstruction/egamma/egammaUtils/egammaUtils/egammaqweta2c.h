/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMACALOTOOLS_EGAMMAQWETA2C_H
#define EGAMMACALOTOOLS_EGAMMAQWETA2C_H

/**
  @class egammaqweta2c
          correction for the eta width modulation in sampling 2
   @author H. Ma
   @author Frederic Derue

*/
namespace egammaqweta2c {
/** @brief returns corrected width at eta. */
float Correct(float eta, float etacell, float width);
/** @brief returns method within the cell*/
double RelPosition(const float eta, const float etacell);

} // namespace egammaqweta2c

#endif

