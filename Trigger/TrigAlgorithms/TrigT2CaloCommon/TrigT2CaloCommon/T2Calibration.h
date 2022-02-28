/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

 NAME:     T2Calibration.h
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloCommon

 AUTHOR:   D.O. Damazio

 PURPOSE:   Object only designed to provide cluster calibration
 Should be used by EM and Tau people in the near future
 It should be possible to introduce the calibration configuration
 from a jobOption file. Not possible now.

 *******************************************************************/

#ifndef TRIGT2CALOCOMMON_T2CALIBRATION
#define TRIGT2CALOCOMMON_T2CALIBRATION
#include <vector>
#include <math.h>
#include <iostream>
#include <cmath>

/** Class to perform calibration on LVL2 Calorimeter Trigger Clusters */
class T2Calibration {
public:
	/** Constructor */
	T2Calibration();
	/** Destructor */
	virtual ~T2Calibration();
	/** @brief tool initialization
	* @param[in] Eta limits (normally [0,2.5] for EM Calo).
	* @param[in] Dimensions - granularity of the calibration.
	* @param[in] corrections - vector with eta positions and
	*	calibration constants for each position.
	*/
	virtual void initialize(const std::vector<float>& limit, const std::vector<int>&
		dimension, const std::vector<float>& correction);

	/** @brief for a given cluster eta and energy (not being used yet),
	* provides the calibration constant 
	* @param[in] ClusterEta Eta position of the cluster.
	* @param[in] EnergyCluster Energy of the cluster (when Energy
	*	dependent calibration is used (not yet)).
	* @return calibration constant to be multiplied by cluster
	*	Energy.
	*/
	virtual double Calib( const double ClusterEta, const double EnergyCluster) const;
private:
	double m_factor;
	/** limits of the calibrated region (normally [0,2.5] */
	std::vector<float> m_limit;
	/** dimensions of the calibration matrix */
	std::vector<int> m_dimension;
	/** correction factor */
	std::vector<float> m_correction;

};

inline double T2Calibration::Calib ( const double ClusterEta,
		const double /*EnergyCluster*/) const {
	// If no calibration was defined in jobOption,
	// this should return 1.0 imediately
	if ( m_dimension.size() == 0 ) return 1.0;
        int jump = m_dimension[0];
        // search for correction
        double eta_search = (std::abs(ClusterEta) - m_limit[0]);
        eta_search *= m_factor;
        eta_search = floorf(eta_search);
        // Calculates correction factor
        if ( eta_search >=0 && eta_search < m_dimension[1] )
                return m_correction[(int)(jump*eta_search+1)];
	else // Avoid playing with calibration outside the limites
		return 1.0;
} // end of Calib method

#endif
