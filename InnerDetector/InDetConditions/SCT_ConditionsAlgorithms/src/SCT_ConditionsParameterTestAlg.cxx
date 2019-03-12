/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file SCT_ConditionsParameterTestAlg.cxx  Implementation file for SCT_ConditionsParameterTestAlg class.
 *  @author Shaun Roe, based on Jorgen Dalmau example
 **/

// Include SCT_ConditionsParameterTestAlg and Tool
#include "SCT_ConditionsParameterTestAlg.h"
#include "SCT_ConditionsData/SCT_ConditionsParameters.h"
#include "SCT_ConditionsData/SCT_CondParameterData.h"

// Local include
#include "SCT_SimpleHisto.h"
#include "SCT_ConditionsUtilities.h"

// Include Athena stuff
#include "Identifier/IdentifierHash.h"

// Include STL stuff
#include <string>
#include <iostream>
#include <fstream>

using namespace SCT_ConditionsAlgorithms;

SCT_ConditionsParameterTestAlg::SCT_ConditionsParameterTestAlg(const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthReentrantAlgorithm(name, pSvcLocator)
{ //nop
}

//----------------------------------------------------------------------
StatusCode SCT_ConditionsParameterTestAlg::initialize() {
  // Get the messaging service, print where you are
  ATH_MSG_INFO("in initialize()");
  //
  ATH_CHECK(m_conditionsParameterTool.retrieve());

  return StatusCode::SUCCESS;
} // SCT_ConditionsParameterTestAlg::execute()

//----------------------------------------------------------------------
StatusCode SCT_ConditionsParameterTestAlg::execute(const EventContext& ctx) const {
  //This method is only used to test the service, and only used within this package,
  // so the INFO level messages have no impact on performance of these services when used by clients
  ATH_MSG_DEBUG("in execute()");
  //
  StatusCode sc{StatusCode::SUCCESS, true};
  
  // Get the current event
  ATH_MSG_DEBUG("Current Run.Event,Time: "
                << "[" << ctx.eventID().run_number()
                << "." << ctx.eventID().event_number()
                << "," << ctx.eventID().time_stamp()
                << "]");
  
  bool paramFilled{false};
  ATH_MSG_INFO(m_conditionsParameterTool);
  
  try {
    paramFilled =(m_conditionsParameterTool->filled(ctx));
  } catch (...) {
    ATH_MSG_FATAL("Exception caught while trying to determine whether the data object was filled");
    return StatusCode::FAILURE;
  }

  try {
    float maxval{m_conditionsParameterTool->max(SCT_CondParameterData::AVG_THRESHOLD, ctx)};
    float minval{m_conditionsParameterTool->min(SCT_CondParameterData::AVG_THRESHOLD, ctx)};
    float avg{m_conditionsParameterTool->avg(SCT_CondParameterData::AVG_THRESHOLD, ctx)};
    float sd{m_conditionsParameterTool->sd(SCT_CondParameterData::AVG_THRESHOLD, ctx)};
    unsigned int n{m_conditionsParameterTool->n(SCT_CondParameterData::AVG_THRESHOLD, ctx)};
    float thresh{m_conditionsParameterTool->value(IdentifierHash{1760}, SCT_CondParameterData::AVG_THRESHOLD, ctx)};
    ATH_MSG_INFO("   value element 1760: " << thresh);
    ATH_MSG_INFO("        max threshold: " << maxval);
    ATH_MSG_INFO("        min threshold: " << minval);
    ATH_MSG_INFO("        avg threshold: " << avg);
    ATH_MSG_INFO("   standard deviation: " << sd);
    ATH_MSG_INFO("          no measured: " << n);
  } catch (...) {
    ATH_MSG_FATAL("Exception caught while trying to access the thresholds");
    return StatusCode::FAILURE;
  }
  //simple histogram
  SCT_ConditionsAlgorithms::S_t histo;
  init(histo, 0.0, 8.0, 100);
  std::vector<float> values;
  m_conditionsParameterTool->getValues(values, SCT_CondParameterData::AVG_THRESHOLD, ctx);
  for (float i: values) {
    fill(histo, i);
  }
  std::string histoString = asXmlString(histo);

  ATH_MSG_INFO(histoString);

  if (not paramFilled) sc = StatusCode::FAILURE;

  return sc;
} // SCT_ConditionsParameterTestAlg::execute()

//----------------------------------------------------------------------
StatusCode SCT_ConditionsParameterTestAlg::finalize() {
  // Get the messaging service, print where you are
  ATH_MSG_INFO("in finalize()");

  //  string xmlPath{CoveritySafe::getenv("XMLPATH")};
  //  size_t found{xmlPath.find(":")};
  //  if (found!=std::string::npos) xmlPath = xmlPath.substr(0, found);
  std::string stylePath{"./histo.xsl"};
  std::string filename{"./thresholds.xml"};
  ATH_MSG_INFO("Filename: " << filename);

  ATH_MSG_INFO("------------");

  std::ofstream opFile{filename.c_str(), std::ios::out};
  opFile << xmlHeader();
  opFile << stylesheet(stylePath);

  return StatusCode::SUCCESS;
} // SCT_ConditionsParameterTestAlg::finalize()
