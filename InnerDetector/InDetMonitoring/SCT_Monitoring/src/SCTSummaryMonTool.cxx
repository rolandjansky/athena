/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**    @file SCTSummaryMonTool.cxx
 *
 *    @author Elias Coniavitis based on code from Luca Fiorini,
 *    Shaun Roe, Manuel Diaz, Rob McPherson & Richard Batley
 *    Modified by Yuta
 */
#include "SCT_Monitoring/SCTSummaryMonTool.h"


// ====================================================================================================
/** Constructor, calls base class constructor with parameters
 *
 *  several properties are "declared" here, allowing selection
 *  of the filepath for histograms, the first and second plane
 *  numbers to be used, and the timebin.
 */
// ====================================================================================================
SCTSummaryMonTool::SCTSummaryMonTool(const std::string& type, const std::string& name,
                                     const IInterface* parent) : SCTMotherTrigMonTool(type, name, parent) {
  m_numberOfEvents = 0;
}

// ====================================================================================================
//                       SCTSummaryMonTool :: bookHistograms
// ====================================================================================================
StatusCode
SCTSummaryMonTool::bookHistograms() {
  return StatusCode::SUCCESS;
}

StatusCode
SCTSummaryMonTool::bookHistogramsRecurrent() {
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                        SCTSummaryMonTool :: fillHistograms
/// This is the real workhorse, called for each event. It retrieves the data each time
// ====================================================================================================
StatusCode
SCTSummaryMonTool::fillHistograms() {
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                             SCTSummaryMonTool :: procHistograms
// ====================================================================================================
StatusCode
SCTSummaryMonTool::procHistograms() {
  return StatusCode::SUCCESS;
}
