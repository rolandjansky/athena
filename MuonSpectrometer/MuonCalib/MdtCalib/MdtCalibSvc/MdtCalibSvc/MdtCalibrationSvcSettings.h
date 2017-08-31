/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTCALIBSVC_MDTCALIBRATIONSVCSETTINGS_H
#define MDTCALIBSVC_MDTCALIBRATIONSVCSETTINGS_H

/*
 * 
 * Authors Niels van Eldik & Martin Woudstra
 *
 */

#include <cmath>

/**
  @class MdtCalibrationSvcSettings
  This class is used to select which type of corrections should be applied by the calibration service.
  @author Martin Woudstra, Niels van Eldik
*/
class MdtCalibrationSvcSettings {
public:
  
  static const int dummyTimeWindow = -1000000;

  MdtCalibrationSvcSettings() :
    windowSetting(1),windowLowerBound(dummyTimeWindow),windowUpperBound(dummyTimeWindow),
    doTof(true),doProp(true),doTemp(false),doField(false),
    doWireSag(false),doSlew(false),doBkg(false) {
    }  

  /** Initialization function.
      This should be called after setting the window upper and lower bounds
      to their desired values. If they are set and the class is not (re-)initialized,
      unexpected behaviour may result.
      If either window is already set (by hand or otherwise),
      it will not be modified, but this->windowSetting will be set to 0 if inconsistencies are found.
  */
  void initialize() {
    // Check if either of the window values are already set,
    // in which case they will not be modified by this function.
    bool setLower = timeWindowSetToValue(windowLowerBound);
    bool setUpper = timeWindowSetToValue(windowUpperBound);

    double valueLower = 0.;
    double valueUpper = 0.;

    switch (windowSetting) {
    case 0:
      break;
    case 1:
      valueLower = 1000.;
      valueUpper = 2000.;
      break;
    case 2:
      valueLower = 20.;
      valueUpper = 30.;
      break;
    case 3:
      valueLower = 10.;
      valueUpper = 30.;
      break;
    case 4:
      valueLower = 50.;
      valueUpper = 100.;
      break;
    default:
      // Index out of range: do not change any member variables
      // Note the return statement, not break
      return;
    } // End of switch

    if (setLower) {
      // Change the lower bound
      windowLowerBound = valueLower;
    } else {
      // Don't touch the lower bound, but check if its value is correct
      if (!timeWindowSetToValue(windowLowerBound, valueLower)) {
	windowSetting = 0;
      }
    }
    
    if (setUpper) {
      // Change the upper bound
      windowUpperBound = valueUpper;
    } else {
      // Don't touch the upper bound, but check if its value is correct
      if (!timeWindowSetToValue(windowUpperBound, valueUpper)) {
	windowSetting = 0;
      }
    }
  }

  /** Internal method to check consistency of time window values.
   */
  bool timeWindowSetToValue(double &window, double value = dummyTimeWindow) const {
    return std::abs(window - value) < 1e-3;
  }

  /** const access to configuration */

  /** returns the integer value of the time window.
  */
  double timeWindowSetting() const {
    return windowSetting;
  }

  /** returns the lower bound of the matching window used in the calibration service.
      The number should be positive.
  */
  double timeWindowLowerBound() const {
    return windowLowerBound;
  }

  /** returns the upper bound of the matching window used in the calibration service.
      The number should be positive.
  */
  double timeWindowUpperBound() const {
    return windowUpperBound;
  }

  /** toggle on/off tof correction */
  bool doTofCorrection() const { return doTof; }

  /** toggle on/off propagation correction */
  bool doPropCorrection() const { return doProp; }

  /** toggle on/off time corrections, returns the OR of all corrections */
  bool doCorrections() const { return doWireSag || doField || doTemp || doSlew || doBkg; }

  /** toggle on/off temperature correction */
  bool doTemperatureCorrection() const { return doTemp; }

  /** toggle on/off magnetic fieldcorrection */
  bool doMagneticFieldCorrection() const { return doField; }

  /** toggle on/off wire sag correction */
  bool doWireSagCorrection() const { return doWireSag; }

  /** toggle on/off slewing correction */
  bool doSlewingCorrection() const { return doSlew; }

  /** toggle on/off background correction */
  bool doBackgroundCorrection() const { return doBkg; }

  /** data members */
  int windowSetting;
  double windowLowerBound; //!< After setting, make sure to call MdtCalibrationSvcSettings::initialize()!!!
  double windowUpperBound; //!< After setting, make sure to call MdtCalibrationSvcSettings::initialize()!!!
  bool doTof;
  bool doProp;
  bool doTemp;
  bool doField;
  bool doWireSag;
  bool doSlew;
  bool doBkg;
};

#endif
