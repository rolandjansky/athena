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
  AthAlgorithm( name,    pSvcLocator ),
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
  ATH_MSG_DEBUG ( "Calling destructor" ) ;
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode CpuCruncherAlg::initialize()
{
  ATH_MSG_INFO ( "Initializing " << name() << "..." ) ;
  
  // retrieve random number svc
  ATH_CHECK( m_rndmSvc.retrieve() );

  ATH_MSG_INFO ( "CPU usage configuration: <" 
                 << m_meanCpuTime << "> +/- "
                 << m_rmsCpuTime << " seconds" ) ;

  return StatusCode::SUCCESS;
}

StatusCode CpuCruncherAlg::finalize()
{
  ATH_MSG_INFO ( "Finalizing " << name() << "..." ) ;
  return StatusCode::SUCCESS;
}

StatusCode CpuCruncherAlg::execute()
{  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." ) ;
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

