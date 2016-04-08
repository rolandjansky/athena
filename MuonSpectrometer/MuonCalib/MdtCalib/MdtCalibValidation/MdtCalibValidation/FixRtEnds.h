/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_FixRtEnds_h
#define MuonCalib_FixRtEnds_h

//std
#include "vector"

namespace MuonCalib {

//MuonCalibMath
class SamplePoint;

class FixRtEnds {
 public:
//=============================constructor - destructor=========================
  inline FixRtEnds() {}
  inline ~FixRtEnds() {}
//=============================public member functions =========================
  //fix the rt ends for monoton
  int FixEnds(std::vector<SamplePoint> & points);
//===========================static const ======================================
  //return values for FixEnds
  static const int NO_FIX_NEEDED = 0,
    FIX_APPLIED = 1,
    FIX_FAILED = 2;
 private:
//==========================private functions===================================
  bool checkMono(const std::vector<SamplePoint> & points, bool & fix_begin, bool & fix_end) const;
  inline bool fixBegin(std::vector<SamplePoint> & points);
  inline bool fixEnd(std::vector<SamplePoint> & points);
};

} //namespace MuonCalib
#endif
