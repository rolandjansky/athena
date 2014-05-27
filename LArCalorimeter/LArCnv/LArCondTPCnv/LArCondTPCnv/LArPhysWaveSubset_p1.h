/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArPhysWaveSubset_p1.h
 *
 * @brief This file defines the class used for persistent container of
 * LArPhysWave
 *
 * @author Ilija Vukotic  <ivukotic@cern.ch>
 *
 */
#ifndef LARCONDTPCNV_LARPhysWaveSUBSET_P1
#define LARCONDTPCNV_LARPhysWaveSUBSET_P1

/**
 * @class LArPhysWaveSubset_p1
 *
 * @brief persistent class container of 
 * LArPhysWave data.
 *
 *
**/

#include "LArCondTPCnv/LArConditionsSubset_p1.h"
class LArPhysWaveSubset_p1
{

public:
    LArPhysWaveSubset_p1()
      : m_samples(0)  {}
    LArConditionsSubset_p1	m_subset;       // stores febIDs etc.
    std::vector<float>		m_dt;           // one per channel - from LArWave
    std::vector<float> 	    m_vAmplitudes; 	// m_samples per channel  - from LArWave
    std::vector<float> 	    m_vErrors; 		// m_samples per channel  - from LArWaveCumul
    std::vector<int>	 	m_vTriggers; 	// m_samples per channel  - from LArWaveCumul
    std::vector<unsigned>	m_flag;         // one per channel  - from LArWave
    std::vector<int> 		m_timeOffset;	// one per channel	- from LArPhysWave

	unsigned int 		m_samples; //  per channel
};

     
#endif 
