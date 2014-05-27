/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArCaliWaveSubset_p1.h
 *
 * @brief This file defines the class used for persistent container of
 * LArCaliWave
 *
 * @author Ilija Vukotic  <ivukotic@cern.ch>
 *
 */
#ifndef LARCONDTPCNV_LARCALIWAVESUBSET_P1
#define LARCONDTPCNV_LARCALIWAVESUBSET_P1

/**
 * @class LArCaliWaveSubset_p1
 *
 * @brief persistent class container of 
 * LArCaliWave data.
 *
 *
**/

#include "LArCondTPCnv/LArConditionsSubset_p1.h"
class LArCaliWaveSubset_p1
{

public:
    LArCaliWaveSubset_p1()
      : m_samples(0) {}
    LArConditionsSubset_p1      m_subset;       // stores febIDs etc.
    std::vector<float>		m_dt;           // one per channel
    std::vector<float> 	        m_vAmplitudes; 	// m_samples per channel
    std::vector<float> 	        m_vErrors; 	// m_samples per channel
    std::vector<int>	 	m_vTriggers; 	// m_samples per channel
    std::vector<unsigned>	m_flag;         // one per channel
    std::vector<int>		m_DAC;
    unsigned int 		m_samples; // usualy 768 per channel
};

     
#endif // LARCONDTPCNV_LARWAVE_P1
