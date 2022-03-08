/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_TimeCalibrationTools/StraightLineTOFcorrectionTool.h
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date August, 2021
 *
 * @brief This implementation of the TOF correction tool implements a simple
 *        correction using a straight line hypothesis and assuming particles
 *        to move at the speed of light. This leads to known overcorrections,
 *        and has to be refined in future implementations of this tool.
 *
 *  FIXME:
 *  - impact parameter resolution has yet to be propagated to TOF_error!!
 *  - faster sqrt??
 *    e.g. float sqrt2(const float n) { union {int i; float f;} u; u.i =
 * 0x1FB5AD00 + (*(int*)&n >> 1); u.f = n / u.f + u.f; return n / u.f + u.f *
 * 0.25f;}
 */

#ifndef HGTD_STRAIGHTLINETOFCORRECTIONTOOL_H
#define HGTD_STRAIGHTLINETOFCORRECTIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "HGTD_RecToolInterfaces/IHGTD_TOFcorrectionTool.h"

#include "HGTD_ReadoutGeometry/HGTD_DetectorManager.h"

namespace HGTD {

class StraightLineTOFcorrectionTool : public extends<AthAlgTool, IHGTD_TOFcorrectionTool> {

public:
  StraightLineTOFcorrectionTool(const std::string&, const std::string&,
                                const IInterface*);

  virtual StatusCode initialize() override final;

  virtual std::pair<float, float>
  correctTimeAndResolution(const Trk::Track& track,
                           const HGTD_Cluster& cluster,
                           const float time_of_arrival,
                           const float time_of_arrival_err) const override final;

private:
  const HGTD_DetectorManager* m_hgtd_det_mgr{nullptr};
};

} // namespace HGTD

#endif // HGTD_STRAIGHTLINETOFCORRECTIONTOOL_H
