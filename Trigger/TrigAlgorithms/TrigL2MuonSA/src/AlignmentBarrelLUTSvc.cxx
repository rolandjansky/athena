/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ISvcLocator.h"

#include "TrigL2MuonSA/AlignmentBarrelLUTSvc.h"
#include "PathResolver/PathResolver.h"

using namespace std;

TrigL2MuonSA::AlignmentBarrelLUTSvc::AlignmentBarrelLUTSvc(const std::string& name,ISvcLocator* sl) :
  Service(name,sl),
  m_msg(0),
  m_alignmentBarrelLUT(0)
{
  declareProperty( "LUTfile", m_lut_fileName="dZ_barrel.lut" );
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::AlignmentBarrelLUTSvc::queryInterface(const InterfaceID& riid, void** ppvIF) 
{ 
  MsgStream log(messageService(), name());
  if (TrigL2MuonSA::AlignmentBarrelLUTSvc::interfaceID().versionMatch(riid)) {
    *ppvIF = (AlignmentBarrelLUTSvc*)this;
    return StatusCode::SUCCESS;
  } else {
    log << MSG::DEBUG << name() << " cannot find the interface!"
      	<< " Query the interface of the base class." << endreq;
    return Service::queryInterface(riid, ppvIF);
  }
} 

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::AlignmentBarrelLUTSvc::initialize()
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
    msg() << MSG::ERROR << "Cannot find EndcapLUT file " << lut_fileName << endreq;
    return StatusCode::FAILURE;
  }
  
  m_alignmentBarrelLUT = new AlignmentBarrelLUT(m_msg);
  if( !m_alignmentBarrelLUT ) {
    msg() << MSG::ERROR << "Barrel alignment LUT are not loaded!"  << endreq;
    return StatusCode::FAILURE;
  }

  // read LUT
  sc = m_alignmentBarrelLUT->readLUT(lut_fileName);
  if ( sc.isFailure() ) {
    msg() << MSG::ERROR << "Failed to read barrel alignment LUT" << lut_fileName << endreq;
    return sc;
  }
  
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::AlignmentBarrelLUTSvc::finalize() 
{
  if (m_alignmentBarrelLUT) delete m_alignmentBarrelLUT;
  if (m_msg) delete m_msg;
  return StatusCode::SUCCESS;
} 

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

const TrigL2MuonSA::AlignmentBarrelLUT* TrigL2MuonSA::AlignmentBarrelLUTSvc::alignmentBarrelLUT() const
{
    return m_alignmentBarrelLUT;
} 
