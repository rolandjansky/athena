/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file CoolLumiUtilities/src/OnlineLumiCalibrationCondAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2018
 * @brief 
 */


#include "OnlineLumiCalibrationCondAlg.h"
#include "CoolLumiUtilities/OnlineLumiCalibrator.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteCondHandle.h"


OnlineLumiCalibrationCondAlg::OnlineLumiCalibrationCondAlg
  (const std::string& name,
   ISvcLocator* svcloc)
    : AthReentrantAlgorithm (name, svcloc)
{
}



StatusCode
OnlineLumiCalibrationCondAlg::initialize()
{
  ATH_CHECK( m_calibrationFolderName.initialize() );
  ATH_CHECK( m_condDataName.initialize() );
  return StatusCode::SUCCESS;
}


StatusCode
OnlineLumiCalibrationCondAlg::execute (const EventContext& ctx) const
{
  SG::ReadCondHandle<CondAttrListCollection> calibrationFolder
    (m_calibrationFolderName, ctx);
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

  SG::WriteCondHandle<OnlineLumiCalibrationCondData> condData
    (m_condDataName, ctx);
  ATH_CHECK( condData.record (range, std::move (cali)) );
  return StatusCode::SUCCESS;
}
