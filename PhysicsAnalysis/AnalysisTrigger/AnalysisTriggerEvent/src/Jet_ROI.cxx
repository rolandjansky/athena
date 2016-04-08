/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Jet_ROI.cxx,v 1.5 2008-05-08 15:00:11 krasznaa Exp $

// Local include(s):
#include "AnalysisTriggerEvent/Jet_ROI.h"

/**
 * This is the constructor used by the RoIBResultToAOD algorithm. It shouldn't
 * be too interesting to the users.
 */
Jet_ROI::Jet_ROI( uint32_t roiWord, float eta, float phi, uint32_t thrPattern )
   : m_roiWord( roiWord ), m_eta( eta ), m_phi( phi ), m_thrPattern( thrPattern ),
     m_thresholdNames( 0 ), m_thresholdValues( 0 ),
     m_ET4x4(0),
     m_ET6x6(0),
     m_ET8x8(0),
     m_highestThreshold( -1. ) {

}

/**
 * This constructor just initialises every member to a reasonable starting value.
 */
Jet_ROI::Jet_ROI()
   : m_roiWord( 0 ), m_eta( 0. ), m_phi( 0. ), m_thrPattern( 0 ),
     m_thresholdNames( 0 ), m_thresholdValues( 0 ),
     m_ET4x4(0),
     m_ET6x6(0),
     m_ET8x8(0),
     m_highestThreshold( -1. ) {

}

/**
 * The destructor is virtual, since the base classes have virtual
 * functions...
 */
Jet_ROI::~Jet_ROI() {

}

/**
 * The p<sub>T</sub> of the Jet RoI is calculated as the highest threshold
 * value that this RoI passed. It's not too useful like this, but that's on
 * purpose. Any analysis has to use the detailed properties of the RoI, not
 * just a single value describing its transverse momentum.
 *
 * @return The value of the highest threshold this RoI passed, in MeV/c
 */
double Jet_ROI::pt() const {

   //
   // Calculate the pT if it hasn't been done yet:
   //
   if( m_highestThreshold < 0 ) {
      thr_value_type::const_iterator it = m_thresholdValues.begin();
      for( ; it != m_thresholdValues.end(); ++it ) {
         if( ( *it ) > m_highestThreshold ) m_highestThreshold = ( *it );
      }
   }

   return m_highestThreshold;
}
