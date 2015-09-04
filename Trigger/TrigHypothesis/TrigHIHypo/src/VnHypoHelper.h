/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigHIHypo__VNHYPOHELPER_H__
#define TrigHIHypo__VNHYPOHELPER_H__
#include <vector>
struct VnHypoHelper {
  std::vector<float> centcuts;
  std::vector<float> thresholds;
  std::vector<float> qxshifts;
  std::vector<float> qyshifts;

  int getCentBin( double et ) const;
  float getThreshold( int centBin ) const;
  float getQxShift( int centBin ) const;
  float getQyShift( int centBin ) const;
  bool valid() const;
};



#endif

