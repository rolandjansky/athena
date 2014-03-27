/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// ResidualPull.cxx
//   Source file for class ResidualPull
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#include "TrkEventPrimitives/ResidualPull.h"

// default constructor
Trk::ResidualPull::ResidualPull():
        m_residual(0),
        m_pull(0),
        m_pullValid(false),
        m_residualType(Trk::ResidualPull::Biased),
        m_parameterKey(0),
        m_sineStripAngle(0.0) {}

// copy constructor
Trk::ResidualPull::ResidualPull(const Trk::ResidualPull& resPull):
  m_residual(resPull.m_residual),
  m_pull(resPull.m_pull),
  m_pullValid(resPull.m_pullValid),
  m_residualType(resPull.m_residualType),
  m_parameterKey(resPull.m_parameterKey),
  m_sineStripAngle(resPull.m_sineStripAngle)  {}

// constructor with parameters
Trk::ResidualPull::ResidualPull(const std::vector<double> &residual,
                                const std::vector<double> &pull,
                                const bool pullValid,
                                const Trk::ResidualPull::ResidualType type,
                                const int parameterKey,
                                const double sineStripAngle):
        m_residual(residual),
        m_pull(pull),
        m_pullValid(pullValid),
        m_residualType(type),
        m_parameterKey(parameterKey),
        m_sineStripAngle(sineStripAngle) {
    if (m_residual.size() != m_pull.size()) {
        //return 0;
        // throw something....
    }

}

//return the parameter key of the Trk::LocalParameters (of the measurement) used for the calculation
int Trk::ResidualPull::parameterKey()  const {
    return m_parameterKey;
}

// return the dimension of the residual and pull
int Trk::ResidualPull::dimension() const {
    return m_residual.size();
}
