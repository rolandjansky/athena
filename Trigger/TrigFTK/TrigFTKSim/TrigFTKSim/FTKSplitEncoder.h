/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTK_SPLIT_ENCODER_H
#define FTK_SPLIT_ENCODER_H

/*
  A simple lookup table for split-arch pseudolayer sector bounds along eta
  Why do we need one? Because in the barrel, we split in equal *ctheta* bins
  But in the endcaps, we split in equal *eta* bins.
  Since TrigFTK split implementation uses ctheta as a basic variable, sector bounds
  for the endcaps are not a simple mathematical expression. Thus, we use lookups.
  Note: see additional documentation in .cxx file
*/

class FTKSplitEncoder {
 private:
  static FTKSplitEncoder m_instance; // singleton instance
  const int m_ctheta_nbins;
  double *m_ctheta_lookup;
  int m_ctheta_curindex;

 public:
  FTKSplitEncoder(int nbins);
  ~FTKSplitEncoder();
  int cthetaIndex(double v);
  double cthetaLowerBound() const { return m_ctheta_lookup[m_ctheta_curindex]; }
  double cthetaUpperBound() const { return m_ctheta_lookup[m_ctheta_curindex+1]; }
  
  static FTKSplitEncoder& get() { return m_instance; }
};

#endif
