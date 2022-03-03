/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

/**
 * @file LArCaliWave.h
 *
 * @brief 
 *
 * @author Marco Delmastro <Marco.Delmastro@cern.ch>
 * @author RD Schaffer  <R.D.Schaffer@cern.ch>
 *
 * $Id: LArCaliWave.h,v 1.17 2008-02-21 21:02:19 wlampl Exp $
 *
 * Modifcation:
 * 03 Dec, 2006: G. Rosenbaum
 * Add functions to support putting four bits of info regarding which calib lines are pulsed into m_DAC
 * First 16 bits are DAC value, bits 17-20 are which of four possible calib lines are pulsed.
 * 15 May 2021: P. Strizenec
 * Separating DAC and pulsedInt into two variables, for SuperCell processing needs more bits for DAC


 */

#ifndef LARRAWCONDITIONS_LARCALIWAVE_H
#define LARRAWCONDITIONS_LARCALIWAVE_H
/**
 * @class LArCaliWave
 *
 * @brief 
 *
 *
**/

#include "LArRawConditions/LArWave.h"
#include "LArRawConditions/LArWaveCumul.h"
#include <vector>
#include <iostream>
#include <stdint.h>

// Need real class for CaliWave vector:
class LArCaliWave;
class LArCaliWaveVec : public std::vector<LArCaliWave> {
 public:
  LArCaliWaveVec() {};
  bool isEmpty() const;
};

class LArCaliWave : public LArWaveCumul {

public:

    /// \name structors
    //@{
    LArCaliWave();

    LArCaliWave(const std::vector<double>& 
		theVector,
		double dt,
		int DAC, 
                int isPulsed,
		unsigned flag=0);

    LArCaliWave(unsigned nSamples,
		double dt,
		int DAC, 
                int isPulsed,
		unsigned flag=0);

    LArCaliWave(const std::vector<double>& vAmpl, 
		const std::vector<double>& vErr, 
		const std::vector<int>& vTrig,
		double dt,
		int DAC, 
                int isPulsed,
		unsigned flag=0);

    virtual ~LArCaliWave() = default;
    //@}

    /// \name accumulation  accessors
    //@{
    /// DAC value
    int getDAC() const;
    /// isPulsed value
    int getIsPulsedInt() const;
    //@}

 protected:

  int m_DAC;
  int m_isPulsed;

};


// INLINE FUNCTIONS


inline 
bool LArCaliWaveVec::isEmpty() const {
  return (this->size()==0);
}


inline
LArCaliWave::LArCaliWave() 
  : LArWaveCumul(), m_DAC(0), m_isPulsed(false)
{}

inline
LArCaliWave::LArCaliWave(const std::vector<double>& theVector,
			 double dt,
			 int DAC, 
                         int isPulsed,
			 unsigned flag)
	:  
	LArWaveCumul(theVector,dt,flag),
	m_DAC(DAC),m_isPulsed(isPulsed) 
{}

inline
LArCaliWave::LArCaliWave(unsigned nSamples,
			 double dt,
			 int DAC, 
                         int isPulsed,
			 unsigned flag) 
	:  
	LArWaveCumul(nSamples,dt,flag), 
	m_DAC(DAC),m_isPulsed(isPulsed)
{}

inline
LArCaliWave::LArCaliWave(const std::vector<double>& vAmpl, 
			 const std::vector<double>& vErr, 
			 const std::vector<int>& vTrig,
			 double dt,
			 int DAC, 
                         int isPulsed,
			 unsigned flag)
	:  
	LArWaveCumul(vAmpl,vErr,vTrig,dt,flag), 
	m_DAC(DAC),m_isPulsed(isPulsed)
{}
 
/*old one*/
//inline
//int 
//LArCaliWave::getDAC() const   
//{ return m_DAC; }


inline
int 
LArCaliWave::getDAC() const   
{
    return m_DAC;
}

inline
int 
LArCaliWave::getIsPulsedInt() const   
{ 
  return m_isPulsed;
}




#endif // LARRAWCONDITIONS_LARCALIWAVE_H
