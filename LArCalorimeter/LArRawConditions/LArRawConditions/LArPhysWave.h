/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 
#ifndef LARPHYSWAVE_H

#define LARPHYSWAVE_H

#include "LArWave.h"
#include "LArWaveCumul.h"
#include <vector>

class LArPhysWave : public LArWaveCumul {

 public:

    LArPhysWave();

    /// keep double instances for each case: with and without
    /// timeOffset. If without, set a default value of 0.
    LArPhysWave(const std::vector<double>& theVector,
		double dt, 
		int timeOffset,
		unsigned flag=0);
    
    LArPhysWave(const std::vector<double>& theVector, 
		double dt, 
		unsigned flag=0);
    
    LArPhysWave(unsigned nSamples,
		double dt, 
		int timeOffset, 
		unsigned flag=0);

    LArPhysWave(unsigned nSamples,
		double dt, 
		unsigned flag=0);

    LArPhysWave(const std::vector<double>& vAmpl, 
		const std::vector<double>& vErr, 
		const std::vector<int>& vTrig,
		double dt, 
		int timeOffset, 
		unsigned flag=0);
    
    LArPhysWave(const std::vector<double>& vAmpl, 
		const std::vector<double>& vErr, 
		const std::vector<int>& vTrig,
		double dt, 
		unsigned flag=0);
    
    virtual ~LArPhysWave();

    int  getTimeOffset() const;
    void setTimeOffset(int timeOffset);

 protected:

    int m_timeOffset;

} ;

//////////////////////
// Inline definitions
//////////////////////

inline LArPhysWave::LArPhysWave() : LArWaveCumul(), m_timeOffset(0)  {}

inline LArPhysWave::LArPhysWave(const std::vector<double>& theVector, double dt, 
				int timeOffset, unsigned flag) 
	:  LArWaveCumul(theVector,dt,flag), m_timeOffset(timeOffset) {}

inline LArPhysWave::LArPhysWave(const std::vector<double>& theVector, 
				double dt, unsigned flag) 
	:  LArWaveCumul(theVector,dt,flag), m_timeOffset(0) {}
  
inline LArPhysWave::LArPhysWave(unsigned nSamples, double dt, 
				int timeOffset, unsigned flag) 
	:  LArWaveCumul(nSamples, dt, flag), m_timeOffset(timeOffset) {}

inline LArPhysWave::LArPhysWave(unsigned nSamples, double dt, unsigned flag) 
	:  LArWaveCumul(nSamples,dt,flag), m_timeOffset(0) {}

inline LArPhysWave::LArPhysWave(const std::vector<double>& vAmpl, 
				const std::vector<double>& vErr, 
				const std::vector<int>& vTrig,
				double dt, int timeOffset, unsigned flag)
	:  LArWaveCumul(vAmpl,vErr,vTrig,dt,flag), m_timeOffset(timeOffset) {}

inline LArPhysWave::LArPhysWave(const std::vector<double>& vAmpl,
				const std::vector<double>& vErr,
				const std::vector<int>& vTrig,
				double dt, unsigned flag)
	:  LArWaveCumul(vAmpl,vErr,vTrig,dt,flag), m_timeOffset(0) {}
  
inline LArPhysWave::~LArPhysWave() { } 

inline int  LArPhysWave::getTimeOffset() const {return m_timeOffset; } 
inline void LArPhysWave::setTimeOffset(int timeOffset) { m_timeOffset = timeOffset; } 

#endif // LARPHYSWAVE_H
