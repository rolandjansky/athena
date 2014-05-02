/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArWaveCumul.h
 *
 * @brief 
 *
 * @author Marco Delmastro <Marco.Delmastro@cern.ch>
 * @author RD Schaffer  <R.D.Schaffer@cern.ch>
 *
 * $Id: LArWaveCumul.h,v 1.10 2006-07-11 07:43:34 schaffer Exp $
 */

#ifndef LARRAWCONDITIONS_LARWAVECUMUL_H
#define LARRAWCONDITIONS_LARWAVECUMUL_H
/**
 * @class LArWaveCumul 
 *
 * @brief 
 *
 *
**/

#include "LArRawConditions/LArWave.h"
#include "LArIdentifier/LArOnlineID.h"
#include <vector>

class LArWaveCumul : public LArWave {

 public:

    /// \name structors
    //@{
    LArWaveCumul();

    LArWaveCumul(const std::vector<double>& theVector,
		 double dt, 
		 unsigned flag=0);

    LArWaveCumul(const std::vector<double>& vAmpl, 
		 const std::vector<double>& vErr, 
		 const std::vector<int>& vTrig,
		 double dt, 
		 unsigned flag=0);

    LArWaveCumul(unsigned nSamples,
		 double dt,
		 unsigned flag=0);

    virtual ~LArWaveCumul();
    //@}

    /// \name accumulation  accessors
    //@{

    /// error vector
    const std::vector<double>&     getErrors()             const;

    /// error per time bin
    double                         getError(unsigned i)    const;

    /// trigger vector
    const std::vector<int>&        getTriggers()           const;

    /// trigger per time bin
    int                            getTrigger(unsigned i)  const;	

    //@}
    

    /// \name accumulation  setters
    //@{

    /// add in set of channel responses for a given step/delay
    void addEvent( int delay , int step , const std::vector<double>& Samples ) ;

    /// used when building from LArAccumulatedDigits
    void addAccumulatedEvent( int delay , 
			      int step , 
			      const std::vector<double>& SamplesSum , 	
			      const std::vector<double>& Samples2Sum , 
			      unsigned nTriggers ) ;
    //@}

 protected:

    std::vector<double>   m_errors ;
    std::vector<int>      m_triggers ;

};


inline
LArWaveCumul::LArWaveCumul()
	: 
	LArWave() 
{}

inline
LArWaveCumul::LArWaveCumul(const std::vector<double>& theVector,
			   double dt, 
			   unsigned flag)
	:
	LArWave(theVector,dt,flag)
{
    unsigned nSamples = theVector.size();
    m_errors.resize(nSamples,0); 
    m_triggers.resize(nSamples,0);
}

inline
LArWaveCumul::LArWaveCumul(const std::vector<double>& vAmpl, 
			   const std::vector<double>& vErr, 
			   const std::vector<int>& vTrig,
			   double dt,
			   unsigned flag) 
	:
	LArWave(vAmpl,dt,flag), 
	m_errors(vErr),
	m_triggers(vTrig)
{} 

inline
LArWaveCumul::LArWaveCumul(unsigned nSamples,
			   double dt, 
			   unsigned flag)
	:
	LArWave(nSamples,dt,flag)
{
    m_errors.resize(nSamples,0) ; 
    m_triggers.resize(nSamples,0) ;
}

inline
LArWaveCumul::~LArWaveCumul() 
{}

inline
const std::vector<double>& 
LArWaveCumul::getErrors()  const 
{ return m_errors ; }

inline
double 
LArWaveCumul::getError(unsigned i) const
{ 
    return (i<getSize()) ? m_errors[i] : -9.9999E+99 ; 
}

inline
const std::vector<int>& 
LArWaveCumul::getTriggers()   const
{
    return m_triggers;
}

inline
int 
LArWaveCumul::getTrigger(unsigned i) const
{ return (i<getSize()) ? m_triggers[i] : -999999999; }


#endif // LARWAVECUMUL_H
