/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ISvcLocator.h"

#include "TrigL2MuonSA/PtBarrelLUTSvc.h"
#include "PathResolver/PathResolver.h"

using namespace std;

TrigL2MuonSA::PtBarrelLUTSvc::PtBarrelLUTSvc(const std::string& name,ISvcLocator* sl) :
  Service(name,sl),
  m_msg(0),
  m_ptBarrelLUT(0)
{
  declareProperty( "LUTfile", m_lut_fileName="pt_barrel.lut" );
  declareProperty( "SP_LUTfile", m_lutSP_fileName="pt_barrelSP.lut" );
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::PtBarrelLUTSvc::queryInterface(const InterfaceID& riid, void** ppvIF) 
{ 
  MsgStream log(messageService(), name());
  if (TrigL2MuonSA::PtBarrelLUTSvc::interfaceID().versionMatch(riid)) {
    *ppvIF = (PtBarrelLUTSvc*)this;
    return StatusCode::SUCCESS;
  } else {
    log << MSG::DEBUG << name() << " cannot find the interface!"
      	<< " Query the interface of the base class." << endreq;
    return Service::queryInterface(riid, ppvIF);
  }
} 

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::PtBarrelLUTSvc::initialize()
{
  m_msg = new MsgStream( msgSvc(), name() );
  msg() << MSG::DEBUG << "Initializing " << name() << " - package version " << PACKAGE_VERSION << endreq;
  
  StatusCode sc;
  
  sc = Service::initialize();
  if ( sc.isFailure() ) return sc;
  
  // implement the search of LUT trought the pathresolver Tool.
  std::string lut_fileName = PathResolver::find_file (m_lut_fileName, "DATAPATH");
  msg() << MSG::INFO << lut_fileName << endreq;
  
  if (lut_fileName.empty()) {
    msg() << MSG::ERROR << "Cannot find barrel LUT file " << lut_fileName << endreq;
    return StatusCode::FAILURE;
  }
  
  // implement the search of SP LUT trought the pathresolver Tool.
  std::string lutSP_fileName = PathResolver::find_file (m_lutSP_fileName, "DATAPATH");
  msg() << MSG::INFO << lutSP_fileName << endreq;
  
  if (lutSP_fileName.empty()) {
    msg() << MSG::ERROR << "Cannot find barrel SP LUT file " << lutSP_fileName << endreq;
    return StatusCode::FAILURE;
  }
  
  m_ptBarrelLUT = new PtBarrelLUT(m_msg);
  if( !m_ptBarrelLUT ) {
    msg() << MSG::ERROR << "Barrel LUT are not loaded!"  << endreq;
    return StatusCode::FAILURE;
  }

  // read LUT
  sc = m_ptBarrelLUT->readLUT(lut_fileName, lutSP_fileName);
  if ( sc.isFailure() ) {
    msg() << MSG::ERROR << "Failed to read barrel LUT" << lut_fileName
	  << " and/or " << lutSP_fileName << endreq;
    return sc;
  }
  
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::PtBarrelLUTSvc::finalize() 
{
  if (m_ptBarrelLUT) delete m_ptBarrelLUT;
  if (m_msg) delete m_msg;
  return StatusCode::SUCCESS;
} 

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

const TrigL2MuonSA::PtBarrelLUT* TrigL2MuonSA::PtBarrelLUTSvc::ptBarrelLUT() const
{
    return m_ptBarrelLUT;
} 
