/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "../CalibratedMuonsProvider.h"
#include "../CalibratedTracksProvider.h"
#include "../TestMCASTTool.h"
#include "MuonMomentumCorrections/MuonCalibrationAndSmearingTool.h"
#include "MuonMomentumCorrections/MuonCalibrationPeriodTool.h"
#include "MuonMomentumCorrections/MuonCalibTool.h"
#include "MuonMomentumCorrections/MuonCalibIntSagittaTool.h"
#include "MuonMomentumCorrections/MuonCalibIntHighpTSmearTool.h"
#include "MuonMomentumCorrections/MuonCalibIntScaleSmearTool.h"
#include "MuonMomentumCorrections/MuonCalibrationPeriodTool.h"

DECLARE_COMPONENT(CP::MuonCalibTool)
DECLARE_COMPONENT(CP::MuonCalibIntSagittaTool)
DECLARE_COMPONENT(CP::MuonCalibIntHighpTSmearTool)
DECLARE_COMPONENT(CP::MuonCalibIntScaleSmearTool)
DECLARE_COMPONENT(CP::MuonCalibrationAndSmearingTool)
DECLARE_COMPONENT(CP::MuonCalibrationPeriodTool)
DECLARE_COMPONENT(CP::TestMCASTTool)
DECLARE_COMPONENT(CP::CalibratedMuonsProvider)
DECLARE_COMPONENT(CP::CalibratedTracksProvider)
