/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloD3PDMaker/src/MBTSFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2010
 * @brief Block filler tool for combined MBTS time information.
 */


#include "MBTSTimeFillerTool.h"
#include "PrimaryDPDMaker/MBTSTimeFilterTool.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
MBTSTimeFillerTool::MBTSTimeFillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
  : BlockFillerTool<void> (type, name, parent),
    m_timetool ("MBTSTimeFilterTool")
{
  declareProperty ("TimeTool", m_timetool,
                   "Tool to calculate MBTS times.");

  book().ignore(); // Avoid coverity warnings
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode MBTSTimeFillerTool::initialize()
{
  CHECK( m_timetool.retrieve() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Book variables for this block.
 */
StatusCode MBTSTimeFillerTool::book()
{
  CHECK( addVariable ("timeDiff", m_timeDiff,
                      "MBTS A-C time difference.") );
  CHECK( addVariable ("timeA",    m_timeA,
                      "MBTS A-side time.") );
  CHECK( addVariable ("timeC",    m_timeC,
                      "MBTS C-side time.") );
  CHECK( addVariable ("countA",   m_countA,
                      "Number of A-side MBTS counters used for time.") );
  CHECK( addVariable ("countC",   m_countC,
                      "Number of C-side MBTS counters used for time.") );
 
  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 */
StatusCode MBTSTimeFillerTool::fill ()
{
  bool passCut;
  double timeDiff;
  double timeA;
  double timeC;
  CHECK( m_timetool->getTimeDifference (passCut, timeDiff, timeA, timeC,
                                        *m_countA, *m_countC) );
  *m_timeDiff = timeDiff;
  *m_timeA = timeA;
  *m_timeC = timeC;

  return StatusCode::SUCCESS;
}


} // namespace D3PD
