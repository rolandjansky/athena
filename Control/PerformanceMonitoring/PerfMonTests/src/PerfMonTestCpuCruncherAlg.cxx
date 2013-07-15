///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PerfMonTestCpuCruncherAlg.cxx 
// Implementation file for class PerfMonTest::CpuCruncherAlg
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

// CLHEP includes
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Random/RandGauss.h"

// PerfMonTests includes
#include "PerfMonTestCpuCruncherAlg.h"

using namespace PerfMonTest;

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
CpuCruncherAlg::CpuCruncherAlg( const std::string& name, 
				ISvcLocator* pSvcLocator ) : 
  Algorithm   ( name,    pSvcLocator ),
  m_msg       ( msgSvc(),       name ),
  m_rndmSvc   ( "AtRndmGenSvc", name ),
  m_rndmEngine( 0 )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty( "MeanCpu",
		   m_meanCpuTime = 10.*CLHEP::millisecond,
		   "Mean (in ms) of CPU time to consume." );

  declareProperty( "RmsCpu",
		   m_rmsCpuTime =   2.*CLHEP::millisecond,
		   "RMS (in ms) of CPU time to consume." );
}

// Destructor
///////////////
CpuCruncherAlg::~CpuCruncherAlg()
{ 
  m_msg << MSG::DEBUG << "Calling destructor" << endreq;
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode CpuCruncherAlg::initialize()
{
  // configure our MsgStream
  m_msg.setLevel( outputLevel() );

  m_msg << MSG::INFO 
	<< "Initializing " << name() << "..." 
	<< endreq;
  
  // retrieve random number svc
  if ( !m_rndmSvc.retrieve().isSuccess() ) {
    m_msg << MSG::ERROR
	  << "Could not retrieve svc [" << m_rndmSvc.typeAndName() << "] !!"
	  << endreq;
    return StatusCode::FAILURE;
  }
  // retrieve engine
  //m_rndmEngine = m_rndmSvc->GetEngine( "somename" );

  m_msg << MSG::INFO << "CPU usage configuration: <" 
	<< m_meanCpuTime << "> +/- "
	<< m_rmsCpuTime << " seconds"
	<< endreq;

  return StatusCode::SUCCESS;
}

StatusCode CpuCruncherAlg::finalize()
{
  m_msg << MSG::INFO 
	<< "Finalizing " << name() << "..." 
	<< endreq;

  return StatusCode::SUCCESS;
}

StatusCode CpuCruncherAlg::execute()
{  
  m_msg << MSG::DEBUG << "Executing " << name() << "..." 
	<< endreq;

  // spend CPU time here...
  
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

