/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SolenoidTest.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
/*
 * Masahiro Morii, Harvard University
 */
// class include
#include "MagFieldUtils/SolenoidTest.h"

// Framework
#include "GaudiKernel/ITHistSvc.h"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Units/SystemOfUnits.h"

// MagneticField
#include "MagFieldInterfaces/IMagFieldSvc.h"

// floating point precision
#include <limits>

//root
#include "TTree.h"
#include "TFile.h"

#include "TRandom3.h"

// performance test
#include <time.h>
#include <vector>

///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructor
////////////////
MagField::SolenoidTest::SolenoidTest( const std::string& name, ISvcLocator* pSvcLocator ) :
    ::AthAlgorithm( name, pSvcLocator ),
    m_magFieldSvc( "AtlasFieldSvc", name ),
    m_thistSvc( "THistSvc", name ),
    m_tree(0),
    m_event(0)
{
    // histogram service
    declareProperty( "THistService", m_thistSvc, "The HistogramService" );
    declareProperty( "HistogramStreamName", m_histStream = "SolenoidTest",
                     "Name of the THistSvc output stream" );
    // TTree object name
    declareProperty( "ROOTTreeName", m_treeName = "field",
                     "Name of the TTree object in the output file." );
    // boundaries for the magfield validation
    declareProperty( "MinimumR", m_minR = 0., "minimum R" );
    declareProperty( "MaximumR", m_maxR = 1075., "maximum R" );
    declareProperty( "MinimumZ", m_minZ = -2820., "minimum Z" );
    declareProperty( "MaximumZ", m_maxZ = 2820., "maximum Z" );
    // number of steps in each dimension (granularity)
    declareProperty( "StepsR", m_stepsR = 200,
                     "Number of steps along radius (granularity)");
    declareProperty( "StepsZ", m_stepsZ = 200,
                     "Number of steps along z (granularity)");
    declareProperty( "StepsPhi", m_stepsPhi = 200,
                     "Number of steps along phi (granularity)");
    declareProperty( "UseFullField", m_useFullField = true, "compute the full 3D field");
    declareProperty( "UseFastField", m_useFastField = true, "compute the fast 2D field");
    declareProperty( "WriteNtuple", m_writeNtuple = true, "produce the output ntuple");
    declareProperty( "Derivatives", m_derivatives = false, "compute the derivatives");
}

// Destructor
///////////////
MagField::SolenoidTest::~SolenoidTest()
{;}

// Athena hook:
StatusCode MagField::SolenoidTest::initialize()
{
    ATH_MSG_INFO("entering initialize()...");

    if ( m_writeNtuple ) {
        // retrieve the histogram service
        if ( m_thistSvc.retrieve().isSuccess() ) {

            // Create the prefix of histogram names for the THistSvc
            std::string prefix = "/" + m_histStream + "/";

            // the ROOT tree
            m_tree = new TTree( m_treeName.c_str(), "Magnetic field in the solenoid" );
            m_tree->Branch( "pos", &m_xyzt, "x/D:y/D:z/D" );
            if ( m_useFullField ) {
                m_tree->Branch( "fieldFull", &m_field, "Bx/D:By/D:Bz/D" );
                if ( m_derivatives ) {
                    m_tree->Branch( "deriv", &m_deriv,
                    "dBxdx/D:dBxdy/D:dBxdz/D:dBydx/D:dBydy/D:dBydz/D:dBzdx/D:dBzdy/D:dBzdz/D" );
                }
            }
            if ( m_useFastField ) {
                m_tree->Branch( "fieldFast", &m_fieldZR, "Bx2/D:By2/D:Bz2/D" );
                if ( m_derivatives ) {
                    m_tree->Branch( "derivZR", &m_derivZR,
                    "dBxdx2/D:dBxdy2/D:dBxdz2/D:dBydx2/D:dBydy2/D:dBydz2/D:dBzdx2/D:dBzdy2/D:dBzdz2/D" );
                }
            }
            // register this ROOT TTree to the THistSvc
            if ( m_thistSvc->regTree(prefix + m_treeName, m_tree).isFailure() ) {
                ATH_MSG_ERROR("Unable to register TTree to THistSvc");
                return StatusCode::FAILURE;
            }
        } else { // failure in THistSvc retrieve
            ATH_MSG_ERROR("Unable to retrieve HistogramSvc");
            return StatusCode::FAILURE;
        }
    }

    // success
    ATH_MSG_INFO("end of initialize()");
    return StatusCode::SUCCESS;
}

// Athena hook:
StatusCode MagField::SolenoidTest::finalize() {
    ATH_MSG_INFO("entering finalize()...");
    ATH_MSG_INFO("end of finalize()");
    return StatusCode::SUCCESS;
}

// Athena hook:
StatusCode MagField::SolenoidTest::execute() {

    if( m_event==0 ) { // event #0
        // call field service once for initialization
        m_xyzt[0] = m_xyzt[1] = m_xyzt[2] = 0;
        if ( m_useFullField ) m_magFieldSvc->getField( m_xyzt, m_field, 0 );
        if ( m_useFastField ) m_magFieldSvc->getFieldZR( m_xyzt, m_fieldZR, 0 );
        // check the field status
        ATH_MSG_INFO("New service solenoidOn = " << m_magFieldSvc->solenoidOn());
        ATH_MSG_INFO("New service toroidOn   = " << m_magFieldSvc->toroidOn());
    }
    else if ( m_event==1 ) { // event #1

      ATH_MSG_INFO( "solenoid field service test in progress." );

      // initialize performance timer
      time_t seconds;
      seconds = time(NULL);

      for ( int k = 0; k < m_stepsPhi; k++ ) { // loop over phi
          double phi = 2.*M_PI*double(k)/double(m_stepsPhi);
          for ( int j = 0; j < m_stepsZ; j++ ) { // loop over Z
              m_xyzt[2] = (m_maxZ-m_minZ)*double(j)/double(m_stepsZ-1) + m_minZ;
              for ( int i = 0; i < m_stepsR; i++ ) { // loop over R
                  double r = (m_maxR-m_minR)*double(i)/double(m_stepsR-1) + m_minR;
                  m_xyzt[0] = r*cos(phi);
                  m_xyzt[1] = r*sin(phi);

                  // compute the field
                  if ( m_useFullField ) {
                      if ( m_derivatives )
                          m_magFieldSvc->getField( m_xyzt, m_field, m_deriv );
                      else
                          m_magFieldSvc->getField( m_xyzt, m_field, 0 );
                  }
                  if ( m_useFastField ) {
                      if ( m_derivatives ) 
                          m_magFieldSvc->getFieldZR( m_xyzt, m_fieldZR, m_derivZR );
                      else
                          m_magFieldSvc->getFieldZR( m_xyzt, m_fieldZR, 0 );
                  }

                  // fill the ROOT Tree
                  if ( m_writeNtuple ) m_tree->Fill();
              }
          }
      }
      seconds = time(NULL) - seconds; 
      ATH_MSG_INFO( "accessing " << m_stepsR * m_stepsZ * m_stepsPhi
                    << " values in a grid took " << seconds << " seconds." );
    }
    m_event++;
    return StatusCode::SUCCESS;
}

