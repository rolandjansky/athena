///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PerfMonTestCpuCruncherAlg.cxx 
// Implementation file for class PerfMonTest::CpuCruncherAlg
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <chrono>
#include <cmath>

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
		   m_meanCpuTime = 100.,
		   "Mean (in ms) of CPU time to consume." );
    
  declareProperty( "RmsCpu",
		   m_rmsCpuTime = 5.,
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
  // configure our MsgStream
  msg().setLevel( msgLevel() );

  ATH_MSG_INFO ( "Initializing " << name() << "..." ) ;
  
  // retrieve random number svc
  ATH_CHECK( m_rndmSvc.retrieve() );

  // Get the engine
  m_rndmEngine = m_rndmSvc->GetEngine("PerfMonTestCpuCruncher");

  if(!m_rndmSvc) {
    ATH_MSG_FATAL( "Failed to retrieve random number engine PerfMonTestCpuCruncher");
    return StatusCode::FAILURE;
  }
  
  ATH_MSG_INFO ( "CPU usage configuration: <" 
                 << m_meanCpuTime << "> +/- "
                 << m_rmsCpuTime << " ms" ) ;
  

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

  // Volatile to avoid optimization
  volatile double test_result = 0.0;

  // Sample randomly - use w/ care
  double ms_interval = CLHEP::RandGauss::shoot(m_rndmEngine, m_meanCpuTime, m_rmsCpuTime);

  ATH_MSG_DEBUG ( "Will burn CPU for " << ms_interval << " milliseconds ..." );

  std::chrono::duration<float, std::milli> chrono_interval(ms_interval);

  auto start = std::chrono::system_clock::now();

  while (std::chrono::system_clock::now() - start < chrono_interval)
    test_result += burn(10000);

  ATH_MSG_DEBUG ( "Test result sum is " << test_result );

  return StatusCode::SUCCESS;
}

double CpuCruncherAlg::burn(unsigned long nIterations = 10000000lu) {

  // Volatile to avoid optimization
  volatile double sum = 0.0;

  double val;

  for(auto idx = 0lu; idx < nIterations; ++idx) {
    val = (double) (idx + 1) / nIterations * 0.7854;
    sum += std::tan(std::log(val));
  }

  return sum;
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

