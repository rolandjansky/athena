/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ISvcLocator.h"

#include "TrigL2MuonSA/PtEndcapLUTSvc.h"
#include "PathResolver/PathResolver.h"

TrigL2MuonSA::PtEndcapLUTSvc::PtEndcapLUTSvc(const std::string& name, ISvcLocator* sl) :
  AthService(name,sl),
  m_msg(0),
  m_ptEndcapLUT(0)
{
  declareProperty("FileName3", m_lut_fileName="pt_endcap_small_large.lut");
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::PtEndcapLUTSvc::queryInterface(const InterfaceID& riid, void** ppvIF)
{
  MsgStream log(messageService(), name());
  if (TrigL2MuonSA::PtEndcapLUTSvc::interfaceID().versionMatch(riid)) {
    *ppvIF = (PtEndcapLUTSvc*)this;
    return StatusCode::SUCCESS;
  } else {
    log << MSG::DEBUG << name() << " cannot find the interface!"
	<< " Query the interface of the base class." << endreq;
    return AthService::queryInterface(riid, ppvIF);
  }
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::PtEndcapLUTSvc::initialize()
{   
  m_msg = new MsgStream( msgSvc(), name());
  msg() << MSG::DEBUG << "Initializing " << name() << " - package version " << PACKAGE_VERSION << endreq;  

  StatusCode sc;
  
  sc = AthService::initialize();
  if ( sc.isFailure() ) return sc;
 
  // implement the search of LUT trought the pathresolver Tool.
  std::string lut_fileName = PathResolver::find_file(m_lut_fileName, "DATAPATH");
  msg() << MSG::INFO << lut_fileName << endreq;
  
  if (lut_fileName.empty()) {
    msg() << MSG::ERROR << "Cannot find EndcapLUT file " << m_lut_fileName << endreq;
    return StatusCode::FAILURE;
  }
  
  m_ptEndcapLUT = new PtEndcapLUT(m_msg);
  if( !m_ptEndcapLUT ) {
    msg() << MSG::ERROR <<   "Endcap LUT are not loaded!"  << endreq;
    return StatusCode::FAILURE;
  }

  // read LUT
  sc = m_ptEndcapLUT->readLUT(lut_fileName);
  if ( sc.isFailure() ) {
    msg() << MSG::ERROR << "Failed to read endcap LUT" << m_lut_fileName << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::PtEndcapLUTSvc::finalize()
{
  if (m_ptEndcapLUT) delete m_ptEndcapLUT;
  if (m_msg) delete m_msg;
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

const TrigL2MuonSA::PtEndcapLUT* TrigL2MuonSA::PtEndcapLUTSvc::ptEndcapLUT() const
{
  return m_ptEndcapLUT;
} 
