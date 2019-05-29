/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CoolLumiUtilities/src/OnlineLumiCalibrationCondAlg.cxx
 * @author scott snyder <snyder@bnl.gov>,
 *         from existing OnlineLumiCalibrationTool
 * @date Aug, 2018
 * @brief Produce lumi calibration data from COOL.
 */


#include "OnlineLumiCalibrationCondAlg.h"
#include "CoolLumiUtilities/OnlineLumiCalibrator.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteCondHandle.h"


/**
 * @brief Gaudi initialize method.
 */
StatusCode
OnlineLumiCalibrationCondAlg::initialize()
{
  ATH_CHECK( m_calibrationFolderInputKey.initialize() );
  ATH_CHECK( m_lumiCalibOutputKey.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm execute method.
 * @param ctx Event Context.
 */
StatusCode
OnlineLumiCalibrationCondAlg::execute (const EventContext& ctx) const
{
  SG::ReadCondHandle<CondAttrListCollection> calibrationFolder
    (m_calibrationFolderInputKey, ctx);
  EventIDRange range;
  ATH_CHECK( calibrationFolder.range (range) );

  auto cali = std::make_unique<OnlineLumiCalibrationCondData>();
  for (const auto& p : *calibrationFolder.retrieve()) {
    unsigned int channel = p.first;
    const coral::AttributeList& attrList = p.second;
    OnlineLumiCalibrator lc;
    if (!lc.setCalibration(attrList)) {
      ATH_MSG_WARNING( "error processing calibration for channel " << channel );
    }
    else{
      ATH_MSG_DEBUG( "Calibration for channel " << channel << ": " << lc );
      cali->set (channel, std::move (lc));
    }
  }

  SG::WriteCondHandle<OnlineLumiCalibrationCondData> lumiCalib
    (m_lumiCalibOutputKey, ctx);
  ATH_CHECK( lumiCalib.record (range, std::move (cali)) );
  return StatusCode::SUCCESS;
}
