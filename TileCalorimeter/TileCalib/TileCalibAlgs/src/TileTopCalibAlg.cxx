/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TileCalibAlgs/TileTopCalibAlg.h"

// ROOT includes
#include "TFile.h"

/**
 * Standard constructor
 * @param name Name of algorithm
 * @param pSvcLocator Service locator
 */

TileTopCalibAlg::TileTopCalibAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_runNumber(0)
  , m_runType(0)
  , m_fileName("TileCalibNtuple.root")
  , m_rootFile(0)
{
  declareProperty("RunNumber", m_runNumber=0);
  declareProperty("RunType", m_runType=0);
  declareProperty("FileName",m_fileName="TileCalibNtuple.root");
}

/**
 * Destructor
 */
TileTopCalibAlg::~TileTopCalibAlg()
{
}

/**
 * Initialize algorithm
 */
StatusCode TileTopCalibAlg::initialize()
{
  ATH_MSG_DEBUG ( "in initialize()" );
 
  ATH_MSG_DEBUG ( "Run number set to " << m_runNumber );
  ATH_MSG_DEBUG ( "Run type set to " << m_runType );
  ATH_MSG_DEBUG ( "Output file set to " << m_fileName );
  ATH_MSG_DEBUG (  "starting to retrive list " << m_tileCalibToolList );
  ATH_CHECK( m_tileCalibToolList.retrieve() );
  ATH_MSG_DEBUG (  m_tileCalibToolList << "retrieved");
  
  // Create output root file, one file for all tools
  m_rootFile = new TFile(m_fileName.c_str(), "recreate");

  ToolHandleArray< ITileCalibTool >::iterator itTool = m_tileCalibToolList.begin();
  ToolHandleArray< ITileCalibTool >::iterator  itToolEnd = m_tileCalibToolList.end();

  for ( ; itTool != itToolEnd; ++itTool ) {
    ATH_CHECK( (*itTool)->initNtuple(m_runNumber,m_runType,m_rootFile) );
  }

  ATH_MSG_INFO ( "initialization completed successfully" );
  return StatusCode::SUCCESS;
}

/**
 * Execute
 */
StatusCode TileTopCalibAlg::execute()
{
  ATH_MSG_DEBUG ( "in execute()" );

  ToolHandleArray< ITileCalibTool >::iterator itTool = m_tileCalibToolList.begin();
  ToolHandleArray< ITileCalibTool >::iterator itToolEnd = m_tileCalibToolList.end();

  for ( ; itTool != itToolEnd; ++itTool ) {
    ATH_CHECK( (*itTool)->execute() );
  }
  
  ATH_MSG_INFO ( "execute completed successfully" );
  return StatusCode::SUCCESS;
}

/**
 *  Finalize
 */
StatusCode 
TileTopCalibAlg::finalize()
{
  ATH_MSG_INFO ( "in finalize()" );

  ToolHandleArray< ITileCalibTool >::iterator itTool = m_tileCalibToolList.begin();
  ToolHandleArray< ITileCalibTool >::iterator  itToolEnd = m_tileCalibToolList.end();

  for ( ; itTool != itToolEnd; ++itTool ) {
    ATH_CHECK( (*itTool)->finalizeCalculations() );
    ATH_CHECK((*itTool)->writeNtuple(m_runNumber,m_runType,m_rootFile) );
  }

  m_rootFile->Close();

  ATH_MSG_INFO ( "finalize completed successfully" );
  return StatusCode::SUCCESS;
}
