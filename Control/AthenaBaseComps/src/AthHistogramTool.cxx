///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthHistogramTool.cxx 
// Implementation file for class AthHistogramTool
// Author: Karsten Koeneke
/////////////////////////////////////////////////////////////////// 

// This class' header includes
#include "AthenaBaseComps/AthHistogramTool.h"

// STL includes
#include <string>

// Framework includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"




/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
AthHistogramTool::AthHistogramTool( const std::string& type, 
                                    const std::string& name,
                                    const IInterface* parent ) :
  ::AthAlgTool( type, name, parent ),
  ::AthHistogramming( name ),
  m_histSvc ( "THistSvc/THistSvc", name )
{
  // Properties go here
  declareProperty("THistSvc",
                  m_histSvc = ServiceHandle<ITHistSvc>("THistSvc/THistSvc", name),
                  "Handle to a THistSvc instance: it will be used to write "
                  "ROOT objects to ROOT files" );

  declareProperty("RootStreamName",    m_prefix  = "/ANALYSIS", "Name of the output ROOT stream (file) that the THistSvc uses");
  declareProperty("RootDirName",       m_rootDir = "",
                  "Name of the ROOT directory inside the ROOT file where the histograms will go");

  declareProperty( "HistNamePrefix",   m_histNamePrefix  = "", "The prefix for the histogram THx name" );
  declareProperty( "HistNamePostfix",  m_histNamePostfix = "", "The postfix for the histogram THx name" );

  declareProperty( "HistTitlePrefix",  m_histTitlePrefix  = "", "The prefix for the histogram THx title" );
  declareProperty( "HistTitlePostfix", m_histTitlePostfix = "", "The postfix for the histogram THx title" );
}




// Destructor
///////////////
AthHistogramTool::~AthHistogramTool()
{
  ATH_MSG_DEBUG ("Calling destructor");
}


/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 


//=============================================================================
// Athena initialize method
//=============================================================================
StatusCode AthHistogramTool::sysInitialize()
{
  /*
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
  */

  // Get the THistSvc
  ATH_CHECK ( histSvc().retrieve() );

  // Configure the underlying AthHistogramming helper
  ATH_CHECK ( AthHistogramming::configAthHistogramming( histSvc(),
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
  return AthAlgTool::sysInitialize();
}
