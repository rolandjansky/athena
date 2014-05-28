/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     T2CalibrationTau.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloTau
//
// AUTHOR:   D.O. Damazio
//
// Object only designed to understand cluster calibration
// Should be used by EM and Tau people in the near future
// It should be possible to introduce the calibration configuration
// from a jobOption file. Not possible now.
//
// ********************************************************************

#ifndef TRIGT2CALOTAU_T2CALIBRATIONTAU
#define TRIGT2CALOTAU_T2CALIBRATIONTAU
#include <vector>
#include <math.h>
#include "TrigT2CaloCommon/T2Calibration.h"

class T2CalibrationTau : public T2Calibration {
public:

/** Constructor */
	T2CalibrationTau() : T2Calibration(){}
/** Destructor */
	~T2CalibrationTau(){}

        // The two methods of this class, initialize and Calib, are virtual 
        // in the base class T2Calibration and implemented there as for the 
        // moment calibration is the same for egamma/tau. For an specific 
        // calibration override these methods.        
 
	// In the initialize, one should provide a vector with the
	// Eta limits (eg.: 0-2.5), the dimensions of the correction
	// vector (eg: 2 100 - for 2 lines of one hundred constants - the
	// first line is the eta of that bin and the second brings
	// the calibration constant for that bin
//	void initialize(const std::vector<float>& limit, const std::vector<int>&
//		dimension, const std::vector<float>& correction);

	// This, for a given cluster eta and energy (not being used yet),
	// provides the calibration constant
//	double Calib( const double ClusterEta, const double EnergyCluster);

};

#endif
