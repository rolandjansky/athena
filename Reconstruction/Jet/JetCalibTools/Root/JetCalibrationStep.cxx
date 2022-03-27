// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

/*
JetCalibrationStep
A base class for individual pieces of the jet calibration sequence

Author: Bill Balunas (balunas@cern.ch)
*/

#include "JetCalibTools/JetCalibrationStep.h"

JetCalibrationStep::JetCalibrationStep(const char* name):
  asg::AsgMessaging(std::string(name)),
  m_GeV(1000),
  m_name(name)
{}

JetCalibrationStep::~JetCalibrationStep()
{}

StatusCode JetCalibrationStep::setStartP4(xAOD::Jet& jet) const {
  xAOD::JetFourMom_t tmp;
  if ( m_jetStartScale.compare("Default") == 0 ) {
    return StatusCode::SUCCESS;
  } else if ( jet.getAttribute<xAOD::JetFourMom_t>(m_jetStartScale.c_str(),tmp) ) {
    jet.setJetP4(tmp);
    return StatusCode::SUCCESS;
  }
  ATH_MSG_WARNING("Jet does not have the requested momentum state: " << m_jetStartScale);
  return StatusCode::FAILURE;
}

StatusCode JetCalibrationStep::getNominalResolutionData(const xAOD::Jet&, double&) const{
  ATH_MSG_ERROR("Nominal data resolution requested from a jet calibration step that doesn't define it.");
  return StatusCode::FAILURE;
}

StatusCode JetCalibrationStep::getNominalResolutionMC(const xAOD::Jet&, double&) const{
  ATH_MSG_ERROR("Nominal MC resolution requested from a jet calibration step that doesn't define it.");
  return StatusCode::FAILURE;
}