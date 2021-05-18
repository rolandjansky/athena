/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ELECTRONPEARSHAPEALIGNMENTCORRECTION_H
#define ELECTRONPEARSHAPEALIGNMENTCORRECTION_H

#include "TH2.h"
/**
  @class electronPearShapeAlignmentCorrection
  @brief return a correction for deltaEta1 to account for the pear shape
  distortion of the calorimeter
  @author Philip Sommer
  @author Jean-Baptiste De Vivie De Regie
*/

class electronPearShapeAlignmentCorrection
{

public:
  /** @brief constructor (initialization reading a root file)
   */
  electronPearShapeAlignmentCorrection(
    const std::string& infile = "rel22_210510/pearshapedistortion.root");
  ~electronPearShapeAlignmentCorrection() = default;

  /** @brief get the correction to deltaEta
   */
  double getDeltaEtaDistortion(const float eta, const float phi) const;
  double getDeltaEtaCorrection(const float eta, const float phi) const;

private:
  // histograms to store correction
  std::unique_ptr<TH2> m_sdeta;
};

#endif
