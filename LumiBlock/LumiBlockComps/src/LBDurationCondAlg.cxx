/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file LumiBlockComps/src/LBDurationCondAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Produce LBDurationCondData from /TRIGGER/LUMI/LBLB.
 */


#include "LBDurationCondAlg.h"
#include "CoolKernel/IObject.h"
#include "GaudiKernel/SystemOfUnits.h"


using Gaudi::Units::second;
using Gaudi::Units::nanosecond;


/**
 * @brief Gaudi initialize method.
 */
StatusCode
LBDurationCondAlg::initialize()
{
  ATH_CHECK( m_lblbFolderInputKey.initialize() );
  ATH_CHECK( m_lbDurationOutputKey.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm execute method.
 * @param ctx Event Context.
 */
StatusCode
LBDurationCondAlg::execute (const EventContext& ctx) const
{

  SG::WriteCondHandle<LBDurationCondData> lbDurationCondData
    (m_lbDurationOutputKey, ctx);

  SG::ReadCondHandle<AthenaAttributeList> lblbFolder
    (m_lblbFolderInputKey, ctx);
  lbDurationCondData.addDependency(lblbFolder);

  if ((**lblbFolder)["StartTime"].isNull()) {
    ATH_MSG_ERROR( "StartTime is NULL in " << m_lblbFolderInputKey.key() << "!" );
    return StatusCode::FAILURE;
  }

  if ((**lblbFolder)["EndTime"].isNull()) {
    ATH_MSG_ERROR( "EndTime is NULL in " << m_lblbFolderInputKey.key() << "!" );
    return StatusCode::FAILURE;
  }

  // Times in nanoseconds.
  cool::UInt63 startTime = (**lblbFolder)["StartTime"].data<cool::UInt63>();
  cool::UInt63 endTime = (**lblbFolder)["EndTime"].data<cool::UInt63>();

  // Convert to seconds.
  double duration = (endTime-startTime) * (nanosecond / second);

  auto lbdur = std::make_unique<LBDurationCondData> (duration);

 
  ATH_CHECK( lbDurationCondData.record (std::move (lbdur)) );
  return StatusCode::SUCCESS;
}


