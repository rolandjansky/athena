/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  : Algorithm(name, pSvcLocator)
  , m_runNumber(0)
  , m_runType(0)
  , m_fileName("TileCalibNtuple.root")
  , m_tileCalibToolList()
  , m_rootFile(0)
{
  declareProperty("RunNumber", m_runNumber=0);
  declareProperty("RunType", m_runType=0);
  declareProperty("TileCalibTools",m_tileCalibToolList,"List Of Tools");
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
  MsgStream  log(msgSvc(),name());
  log << MSG::DEBUG << "in initialize()" << endreq;
 
  log << MSG::DEBUG << "Run number set to " << m_runNumber << endreq;
  log << MSG::DEBUG << "Run type set to " << m_runType << endreq;
  log << MSG::DEBUG << "Output file set to " << m_fileName << endreq;
  log << MSG::DEBUG <<  "starting to retrive list " << m_tileCalibToolList << endreq;
  StatusCode sc = m_tileCalibToolList.retrieve();
  if( sc.isFailure() ) {
    log << MSG::ERROR << "Unable to retieve " << m_tileCalibToolList << endreq;
    return sc;
  }else{
    log << MSG::DEBUG <<  m_tileCalibToolList << "retrieved"<< endreq;
  }
  
  // Create output root file, one file for all tools
  m_rootFile = new TFile(m_fileName.c_str(), "recreate");

  ToolHandleArray< ITileCalibTool >::const_iterator itTool = m_tileCalibToolList.begin();
  ToolHandleArray< ITileCalibTool >::const_iterator  itToolEnd = m_tileCalibToolList.end();

  for ( ; itTool != itToolEnd; ++itTool ) {
    sc = (*itTool)->initNtuple(m_runNumber,m_runType,m_rootFile);
    if( sc.isFailure() ) {
      return sc;
    }
  }

  log << MSG::INFO << "initialization completed successfully" << endreq;
  return StatusCode::SUCCESS;
}

/**
 * Execute
 */
StatusCode TileTopCalibAlg::execute()
{
  MsgStream  log(msgSvc(),name());
  log << MSG::DEBUG << "in execute()" << endreq;

  ToolHandleArray< ITileCalibTool >::const_iterator itTool = m_tileCalibToolList.begin();
  ToolHandleArray< ITileCalibTool >::const_iterator itToolEnd = m_tileCalibToolList.end();

  for ( ; itTool != itToolEnd; ++itTool ) {
    StatusCode sc = (*itTool)->execute();
    if( sc.isFailure() ) {
      return sc;
    }
  }
  
  log << MSG::INFO << "execute completed successfully" << endreq;
  return StatusCode::SUCCESS;
}

/**
 *  Finalize
 */
StatusCode 
TileTopCalibAlg::finalize()
{
  MsgStream  log(msgSvc(),name());    
  log << MSG::INFO << "in finalize()" << endreq;

  ToolHandleArray< ITileCalibTool >::const_iterator itTool = m_tileCalibToolList.begin();
  ToolHandleArray< ITileCalibTool >::const_iterator  itToolEnd = m_tileCalibToolList.end();

  for ( ; itTool != itToolEnd; ++itTool ) {
    StatusCode sc = (*itTool)->finalizeCalculations();
    if( sc.isFailure() ) {
      return sc;
    }

    sc = (*itTool)->writeNtuple(m_runNumber,m_runType,m_rootFile);
    if( sc.isFailure() ) {
      return sc;
    }
  }

  m_rootFile->Close();

  log << MSG::INFO << "finalize completed successfully" << endreq;
  return StatusCode::SUCCESS;
}
