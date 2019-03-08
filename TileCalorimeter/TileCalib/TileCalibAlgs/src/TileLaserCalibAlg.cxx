/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi includes
#include "GaudiKernel/ISvcLocator.h"

// Athena includes
#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "AthenaKernel/errorcheck.h"


// Tile includes
#include "TileCalibAlgs/TileLaserCalibAlg.h"

#include "TFile.h"
#include <iostream>

/****************************************************/
/* TileLaserCalibAlg.cxx     March 5th 2008         */
/*                                                  */
/* M.Cascella    michele.cascella@cern.ch           */
/* I.Vivarelli   iacopo.vivarelli@cern.ch           */
/* A.Dotti       andrea.dotti@cern.ch               */
/*                                                  */
/* S. Viret (25/06/2008) :                          */
/* Code cleaning to fit with the new                */
/* conditions stuff                                 */
/****************************************************/

TileLaserCalibAlg::TileLaserCalibAlg(const std::string& name, ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator)
  , m_RobSvc("ROBDataProviderSvc" , name)
  , m_outfile(0)
  , m_fileName("LaserCalib.root")
  , m_runNo(0)
  , m_runType(0)
{

  declareProperty("FileName", m_fileName);
}

TileLaserCalibAlg::~TileLaserCalibAlg() {
}

///////////////////////////////////////////////

/** implementing the Gaudi Hooks */

StatusCode TileLaserCalibAlg::initialize() {

  ATH_MSG_INFO( "Initializing TileLaserCalibAlg" );

  // Find necessary services
  CHECK( m_RobSvc.retrieve() );

  m_runNo = 0;

  /** Retrieve the tools using the ToolHandleArray */
  CHECK( m_lasTools.retrieve() );

  /** Initializing all the tools **/
  lastools_t::iterator fTool = m_lasTools.begin();
  for (; fTool != m_lasTools.end(); ++fTool) {
    CHECK( (*fTool)->initialize() );
  }

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////

StatusCode TileLaserCalibAlg::execute() {

  ATH_MSG_DEBUG( "Running on execute! TileLaserCalibAlg" );

  // Get the run number and type once for all 

  if (m_runNo == 0) {
    m_runNo = static_cast<uint32_t>(m_RobSvc->getEvent()->run_no());
    m_runType = static_cast<uint32_t>(m_RobSvc->getEvent()->run_type());
  }

  // Then check that we are dealing with a LASER event
  //
  // If run_type is 0,1 or 2, the correct trigger type (52) should be there
  // If run_type is 15 (Test) trigger type is not available

  int trig_type = static_cast<uint32_t>(m_RobSvc->getEvent()->lvl1_trigger_type());

  if (trig_type != 52 && m_runType != 15) {
    ATH_MSG_DEBUG( "This is not a LASER event : skip it !!" );
    return StatusCode::SUCCESS;
  }

  /** Delegate to the tools the computing part. Execute the tools */

  lastools_t::iterator fTool = m_lasTools.begin();

  for (; fTool != m_lasTools.end(); ++fTool) {
    CHECK( (*fTool)->execute() );
  }

  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////

StatusCode TileLaserCalibAlg::finalize() {

  ATH_MSG_INFO( "Finalizing TileLaserCalibAlg" );

  // Writing the corresponding ROOTfile
  m_outfile = new TFile(m_fileName.c_str(), "recreate");

  /** Take the map of corrections from the tools */

  lastools_t::iterator fTool = m_lasTools.begin();

  for (; fTool != m_lasTools.end(); ++fTool) {
    CHECK( (*fTool)->finalizeCalculations() ); // Perform the calibration
    CHECK( (*fTool)->writeNtuple(m_runNo, 2, m_outfile) ); // Fill the nTuple
  }

  m_outfile->Write();
  m_outfile->Close();

  return StatusCode::SUCCESS;
}
