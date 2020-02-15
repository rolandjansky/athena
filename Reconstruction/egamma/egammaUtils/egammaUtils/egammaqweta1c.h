/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMACALOTOOLS_EGAMMAQWETA1C_H
#define EGAMMACALOTOOLS_EGAMMAQWETA1C_H

/**
  @class egammaqweta1c correction for the eta width modulation in strips
   @author H. Ma
   @author Frederic Derue
*/

namespace egammaqweta1c {

/** @brief returns corrected width at eta*/
float Correct(const float eta, const float etacell, const float width);
/** @brief returns relative position within the cell*/
double RelPosition(const float eta, const float etacell);

} // namespace egammaqweta1c

#endif
