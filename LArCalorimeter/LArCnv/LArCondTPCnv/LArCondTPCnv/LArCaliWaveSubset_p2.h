/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArCaliWaveSubset_p2.h
 *
 * @brief This file defines the class used for persistent container of
 * LArCaliWave
 *
 * @author Ilija Vukotic  <ivukotic@cern.ch>
 *
 */
#ifndef LARCONDTPCNV_LARCALIWAVESUBSET_P2
#define LARCONDTPCNV_LARCALIWAVESUBSET_P2

/**
 * @class LArCaliWaveSubset_p2
 *
 * @brief persistent class container of 
 * LArCaliWave data.
 *
 *
**/

#include "LArCondTPCnv/LArConditionsSubset_p1.h"
class LArCaliWaveSubset_p2
{

public:
    LArCaliWaveSubset_p2()
      : m_samples(0)  {}
    LArConditionsSubset_p1  m_subset;       // stores febIDs etc..
    std::vector<float>		m_dt;           // one per channel
    std::vector<float> 	    m_vAmplitudes; 	// m_samples per channel
    std::vector<float> 	    m_vErrors; 		// m_samples per channel
    std::vector<int>	 	m_vTriggers; 	// m_samples per channel
    std::vector<unsigned short>	 	m_vDAC; // how many dac values per channel
    std::vector<unsigned>	m_flag;         // one per channel
    std::vector<int>		m_DAC;			// DAC values
    unsigned int 			m_samples; 		// usualy 768 per channel
};

     
#endif // LARCONDTPCNV_LARCALIWAVESUBSET_P2
