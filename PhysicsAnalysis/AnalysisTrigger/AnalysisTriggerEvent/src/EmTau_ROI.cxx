/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


// Local include(s):
#include "AnalysisTriggerEvent/EmTau_ROI.h"

#include <algorithm>

/**
 * This is the constructor used by the RoIBResultToAOD algorithm. It shouldn't
 * be too interesting to the users.
 */
EmTau_ROI::EmTau_ROI( uint32_t roiWord, float eta, float phi, uint32_t thrPattern )
   : m_roiWord( roiWord ), m_eta( eta ), m_phi( phi ), m_thrPattern( thrPattern ),
     m_thresholdNames( 0 ), m_thresholdValues( 0 ),
     m_Core(0),
     m_EMClus(0),
     m_TauClus(0),
     m_EMIsol(0),
     m_HadIsol(0),
     m_HadCore(0) {

}

/**
 * This constructor just initialises every member to a reasonable starting value.
 */
EmTau_ROI::EmTau_ROI()
   : m_roiWord( 0 ), m_eta( 0. ), m_phi( 0. ), m_thrPattern( 0 ),
     m_thresholdNames( 0 ), m_thresholdValues( 0 ),
     m_Core(0),
     m_EMClus(0),
     m_TauClus(0),
     m_EMIsol(0),
     m_HadIsol(0),
     m_HadCore(0) {

}

/**
 * The destructor is virtual, since the base classes have virtual
 * functions...
 */
EmTau_ROI::~EmTau_ROI() {

}

/**
 * The p<sub>T</sub> of the EM/Tau RoI is calculated as the highest threshold
 * value that this RoI passed. It's not too useful like this, but that's on
 * purpose. Any analysis has to use the detailed properties of the RoI, not
 * just a single value describing its transverse momentum.
 *
 * @return The value of the highest threshold this RoI passed, in MeV/c
 */
double EmTau_ROI::pt() const {
  auto itr = std::max_element(m_thresholdValues.begin(), m_thresholdValues.end());
  return (itr != m_thresholdValues.end() ? *itr : -1);
}
