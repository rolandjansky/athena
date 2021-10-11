/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArCaliWaveSubset_p3.h
 *
 * @brief This file defines the class used for persistent container of
 * LArCaliWave
 *
 *
 */
#ifndef LARCONDTPCNV_LARCALIWAVESUBSET_P3
#define LARCONDTPCNV_LARCALIWAVESUBSET_P3

/**
 * @class LArCaliWaveSubset_p3
 *
 * @brief persistent class container of 
 * LArCaliWave data.
 *
 *
**/

#include "LArCondTPCnv/LArConditionsSubset_p1.h"
class LArCaliWaveSubset_p3
{

public:
    LArCaliWaveSubset_p3()
      : m_samples(0)  {}
    LArConditionsSubset_p1      m_subset;       // stores febIDs etc..
    std::vector<float>		m_dt;           // one per channel
    std::vector<float> 	        m_vAmplitudes; 	// m_samples per channel
    std::vector<float> 	        m_vErrors; 	// m_samples per channel
    std::vector<int>	 	m_vTriggers; 	// m_samples per channel
    std::vector<unsigned short>	m_vDAC;         // how many dac values per channel
    std::vector<unsigned>	m_flag;         // one per channel
    std::vector<int>		m_DAC;		// DAC values
    std::vector<int>		m_isPulsed;	// isPulsed values
    unsigned int 		m_samples; 	// usualy 768 per channel
};

     
#endif // LARCONDTPCNV_LARCALIWAVESUBSET_P3
