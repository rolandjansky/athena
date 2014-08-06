/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMissingEtEvent/TrigMissingET.h"
#include "ETSumHypo.h"

ETSumHypo::ETSumHypo(const std::string& name, ISvcLocator* pSvcLocator) 
  : HLT::HypoAlgo(name, pSvcLocator) {
  m_componentsProp.push_back("FCalEM");
  m_componentsProp.push_back("FCalHad1");
  m_componentsProp.push_back("FCalHad2");
  m_calibrationProp.assign(m_componentsProp.size(), 1.); // default calibration is ... no calibration

  declareProperty("Components", m_componentsProp, "Component on which the actual cut is performed");
  declareProperty("Calibration" ,m_calibrationProp,  "Flag switching off/on calibration.");
  declareProperty("UpperBound", m_upper = -1., "Upper bound of passing values, negative means +inf.");
  declareProperty("LowerBound" ,m_lower = -1,  "Lower bound of passing values, negative means -inf.");

  declareMonitoredVariable("SumEt", m_sum_Et, 0.);
  declareMonitoredVariable("SumEtPassing", m_sum_Et_passing, -1.);

}

HLT::ErrorCode ETSumHypo::hltInitialize() { 
  if (m_calibrationProp.size() != m_componentsProp.size() ) {
    ATH_MSG_ERROR("Size of configured components "<< m_componentsProp.size() <<" does not match size of configured calibrations "<< m_calibrationProp.size());
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  }

  for ( unsigned idx = 0, max_idx = m_calibrationProp.size();  idx < max_idx; ++idx ) {
    std::string name(m_componentsProp[idx]);
    name.resize(9, ' '); // Uff, the MET object has labeled components, but names contain invisible characters (spaces) so each lablel has size of 9 chars, 
    m_comps[name] = m_calibrationProp[idx];
  }

  for (CompSelection::const_iterator i = m_comps.begin(); i != m_comps.end(); ++i ){
    ATH_MSG_DEBUG("Using component " << i->first << " with calibration " << i->second);
  }

  return HLT::OK; 
}


ETSumHypo::~ETSumHypo() {
}


HLT::ErrorCode ETSumHypo::hltFinalize( ) { return HLT::OK; }

HLT::ErrorCode ETSumHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
  // the code is increadibly trivial
  pass = true;

  m_sum_Et_passing = -1; // this will only get assigned when we passwget 
  m_sum_Et = 0; // this must be 0 because our loop starts from it

  const TrigMissingET* met;
  if ( getFeature(outputTE, met) == HLT::OK && met != 0 ){
    ATH_MSG_DEBUG("Got MET object successfully");
  } else {
    ATH_MSG_DEBUG("The MET object inaccessible");
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
  }
  

  for ( unsigned comp_index =0, max_comp = met->getNumOfComponents(); comp_index < max_comp; ++comp_index ) {

    CompSelection::const_iterator compIt = m_comps.find(met->getNameOfComponent(comp_index));
    if ( compIt != m_comps.end() ) { // cool we have found component we look for
      m_sum_Et += met->getSumEtComponent(comp_index) 
	*compIt->second; // this is the calibration      
      ATH_MSG_DEBUG("Got Et from component "<< compIt->first << " val: " << met->getSumEtComponent(comp_index) << " overall sum " << m_sum_Et );
    }

  }
  // now cutting
  if ( m_upper > 0. && m_sum_Et > m_upper ) {
    pass = false;
    return HLT::OK;
  }

  if ( m_lower > 0. && m_lower > m_sum_Et ) {
    pass = false;
    return HLT::OK;
  }
 
  m_sum_Et_passing = m_sum_Et;
  return HLT::OK;
}
