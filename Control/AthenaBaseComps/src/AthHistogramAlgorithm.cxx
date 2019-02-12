///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @class AthHistogramAlgorithm.h 
/// Header file for class AthHistogramAlgorithm
/// @author: Karsten Koeneke (karsten.koeneke@cern.ch)
/////////////////////////////////////////////////////////////////// 

// This class' header file
#include "AthenaBaseComps/AthHistogramAlgorithm.h"

// STL includes
#include <string>
#include <map>

// Framework includes
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ITHistSvc.h"



/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
AthHistogramAlgorithm::AthHistogramAlgorithm( const std::string& name, 
                                              ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  ::AthHistogramming( name ),
  m_histSvc ( "THistSvc/THistSvc", name )
{
  //
  // Property declaration
  // 
  // The histogram service
  declareProperty("THistSvc",
                  m_histSvc = ServiceHandle<ITHistSvc>("THistSvc/THistSvc", name),
                  "Handle to a THistSvc instance: it will be used to write "
                  "ROOT objects to ROOT files" );
  //  declareProperty("THistService",      m_histSvc, "The THistSvc" );
  
  declareProperty("RootStreamName",    m_prefix  = "/", "Name of the output ROOT stream (file) that the THistSvc uses");
  declareProperty("RootDirName",       m_rootDir = "",
                  "Name of the ROOT directory inside the ROOT file where the histograms will go");

  declareProperty( "HistNamePrefix",   m_histNamePrefix  = "", "The prefix for the histogram THx name" );
  declareProperty( "HistNamePostfix",  m_histNamePostfix = "",      "The postfix for the histogram THx name" );

  declareProperty( "HistTitlePrefix",  m_histTitlePrefix  = "", "The prefix for the histogram THx title" );
  declareProperty( "HistTitlePostfix", m_histTitlePostfix = "", "The postfix for the histogram THx title" );
}




// Destructor
///////////////
AthHistogramAlgorithm::~AthHistogramAlgorithm()
{
  ATH_MSG_DEBUG ("Calling destructor");
}



/** Initialization method invoked by the framework. This method is responsible
 *  for any bookkeeping of initialization required by the framework itself.
 *  It will in turn invoke the initialize() method of the derived algorithm,
 *  and of any sub-algorithms which it creates.
 */
StatusCode AthHistogramAlgorithm::sysInitialize()
{
  // ---- stolen from GaudiKernel/Algorithm::sysInitialize -------
  // Bypass the initialization if the algorithm
  // has already been initialized.
  if ( Gaudi::StateMachine::INITIALIZED <= FSMState() ) return StatusCode::SUCCESS;

  // Set the Algorithm's properties
  ATH_CHECK(setProperties());

  // Bypass the initialization if the algorithm is disabled.
  // Need to do this after setProperties.
  if ( !isEnabled( ) ) return StatusCode::SUCCESS;
  
  // ---- stolen from GaudiKernel/Algorithm::sysInitialize ------- END ---


  // Get the THistSvc
  ATH_CHECK ( histSvc().retrieve() );

  // Configure the underlying AthHistogramming helper
  ATH_CHECK ( AthHistogramming::configAthHistogramming( m_histSvc,
                                                        m_prefix,          m_rootDir, 
                                                        m_histNamePrefix,  m_histNamePostfix,
                                                        m_histTitlePrefix, m_histTitlePostfix ) );
  
  // Print some setup information into the log file
  ATH_MSG_DEBUG ("Initializing " << name() << "...");
  ATH_MSG_DEBUG (" using THistService     = " << m_histSvc );
  ATH_MSG_DEBUG (" using RootStreamName   = " << m_prefix );
  ATH_MSG_DEBUG (" using RootDirName      = " << m_rootDir );
  ATH_MSG_DEBUG (" using HistNamePrefix   = " << m_histNamePrefix );
  ATH_MSG_DEBUG (" using HistNamePostfix  = " << m_histNamePostfix );
  ATH_MSG_DEBUG (" using HistTitlePrefix  = " << m_histTitlePrefix );
  ATH_MSG_DEBUG (" using HistTitlePostfix = " << m_histTitlePostfix );


  // re-direct to base class...
  return AthAlgorithm::sysInitialize();
}
