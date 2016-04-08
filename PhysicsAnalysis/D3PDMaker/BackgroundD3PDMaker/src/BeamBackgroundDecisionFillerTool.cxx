/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BeamBackgroundDecisionFillerTool.cxx 502891 2012-05-29 11:04:54Z salekd $
#include "BeamBackgroundDecisionFillerTool.h"


namespace D3PD {


BeamBackgroundDecisionFillerTool::BeamBackgroundDecisionFillerTool (const std::string& type,
    const std::string& name,
    const IInterface* parent)
  : BlockFillerTool<BeamBackgroundData> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}

StatusCode BeamBackgroundDecisionFillerTool::initialize()
{
  CHECK( service( "StoreGateSvc", m_storeGate) );

  return StatusCode::SUCCESS;
}

StatusCode BeamBackgroundDecisionFillerTool::book()
{
  CHECK( addVariable ("decision", m_decision) );
  CHECK( addVariable ("numSegment", m_numSegment) );
  CHECK( addVariable ("numSegmentEarly", m_numSegmentEarly) );
  CHECK( addVariable ("numSegmentACNoTime", m_numSegmentACNoTime) );
  CHECK( addVariable ("numSegmentAC", m_numSegmentAC) );
  CHECK( addVariable ("numMatched", m_numMatched) );
  CHECK( addVariable ("numNoTimeLoose", m_numNoTimeLoose) );
  CHECK( addVariable ("numNoTimeMedium", m_numNoTimeMedium) );
  CHECK( addVariable ("numNoTimeTight", m_numNoTimeTight) );
  CHECK( addVariable ("numOneSidedLoose", m_numOneSidedLoose) );
  CHECK( addVariable ("numOneSidedMedium", m_numOneSidedMedium) );
  CHECK( addVariable ("numOneSidedTight", m_numOneSidedTight) );
  CHECK( addVariable ("numTwoSidedNoTime", m_numTwoSidedNoTime) );
  CHECK( addVariable ("numTwoSided", m_numTwoSided) );
  CHECK( addVariable ("numClusterShape", m_numClusterShape) );
  CHECK( addVariable ("direction", m_direction) );

  return StatusCode::SUCCESS;
}


StatusCode BeamBackgroundDecisionFillerTool::fill(const BeamBackgroundData& beamBackgroundData)
{
  *m_decision = beamBackgroundData.GetDecision();
  *m_numSegment = beamBackgroundData.GetNumSegment();
  *m_numSegmentEarly = beamBackgroundData.GetNumSegmentEarly();
  *m_numSegmentACNoTime = beamBackgroundData.GetNumSegmentACNoTime();
  *m_numSegmentAC = beamBackgroundData.GetNumSegmentAC();
  *m_numMatched = beamBackgroundData.GetNumMatched();
  *m_numNoTimeLoose = beamBackgroundData.GetNumNoTimeLoose();
  *m_numNoTimeMedium = beamBackgroundData.GetNumNoTimeMedium();
  *m_numNoTimeTight = beamBackgroundData.GetNumNoTimeTight();
  *m_numOneSidedLoose = beamBackgroundData.GetNumOneSidedLoose();
  *m_numOneSidedMedium = beamBackgroundData.GetNumOneSidedMedium();
  *m_numOneSidedTight = beamBackgroundData.GetNumOneSidedTight();
  *m_numTwoSidedNoTime = beamBackgroundData.GetNumTwoSidedNoTime();
  *m_numTwoSided = beamBackgroundData.GetNumTwoSided();
  *m_numClusterShape = beamBackgroundData.GetNumClusterShape();
  *m_direction = beamBackgroundData.GetDirection();

  return StatusCode::SUCCESS;
}


} // namespace D3PD
