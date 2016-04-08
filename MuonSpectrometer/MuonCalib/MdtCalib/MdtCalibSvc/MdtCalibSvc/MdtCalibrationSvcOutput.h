/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTCALIBSVC_MDTCALIBRATIONSVCOUTPUT_H
#define MDTCALIBSVC_MDTCALIBRATIONSVCOUTPUT_H

/*
 * 
 * Authors Niels van Eldik & Martin Woudstra
 *
 */

/**
  @class MdtCalibrationSvcOutput
  This class is used to pass on the output of the calibration service.
  Currently the drift time, the drift radius and the error on the drift radius are provided.
  @author Martin Woudstra, Niels van Eldik
*/
class MdtCalibrationSvcOutput {
 public:
  MdtCalibrationSvcOutput( double t, double dr, double t) :
  m_radius(r),m_sigmaRadius(dr),m_driftTime(t) {
  }  
    
  double radius() const { return m_radius; }
  double sigmaRadius() const { return m_sigmaRadius; }
  double driftTime() const { return m_driftTime; }
  
 private:
  double m_radius;
  double m_sigmaRadius;
  double m_driftTime;
};

#endif
