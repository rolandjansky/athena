/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class OnlineLumiCalibrator
   @brief Utility class to apply calibrations from /TDAQ/OLC/CALIBRATIONS folder
   @author E.Torrence
**/

#ifndef LUMIBLOCKCOMPS_ONLINELUMICALIBRATOR_H
#define LUMIBLOCKCOMPS_ONLINELUMICALIBRATOR_H

#include "CoralBase/AttributeList.h"
#include "GaudiKernel/MsgStream.h"

#include <vector>
#include <string>

class OnlineLumiCalibrator {

 public:

  OnlineLumiCalibrator();

  // Pass DB object to set parameters, return False on error
  bool setCalibration(const coral::AttributeList& attrList);

  // Return False on error
  bool calibrateMu(float rawLumi, float& calMu) const;
  bool calibrateLumi(float rawLumi, float& calLumi) const;

  // Online calibration - no afterglow corrections made.  Vector can be arbitrary length
  bool calibrateMu(const std::vector<float>& rawLumi, std::vector<float>& calMu) const;
  bool calibrateLumi(const std::vector<float>& rawLumi, std::vector<float>&calLumi) const;

  float getMuToLumi() const;

  // Allow to dump out the calibration values
  MsgStream& dump(MsgStream&) const;

 private:

  unsigned int m_nPar;
  std::string m_fType;
  float m_muToLumi;
  std::vector<float> m_parVec;

  float getMuVis(float rawPerBX, float sigo, float siga) const;
  float getMuVis2(float rawPerBX, float sigo, float siga) const;

};

MsgStream&    operator<< (MsgStream&   ,const OnlineLumiCalibrator&);

inline MsgStream& operator<< (MsgStream& stream, const OnlineLumiCalibrator& lc)
{
  return lc.dump(stream);
}

#endif
