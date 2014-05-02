/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

/**
 * @file LArWave.h
 *
 * @brief 
 *
 * @author Marco Delmastro <Marco.Delmastro@cern.ch>
 * @author RD Schaffer  <R.D.Schaffer@cern.ch>
 *
 * $Id: LArWave.h,v 1.9 2008-12-10 17:37:54 mdelmast Exp $
 */

#ifndef LARRAWCONDITIONS_LARWAVE_H
#define LARRAWCONDITIONS_LARWAVE_H
/**
 * @class LArWave 
 *
 * @brief 
 *
 *
**/

#include <vector>
#include <cstddef>

class LArWave {
  
 public:
  
    /// \name structors
    //@{
    LArWave();
    LArWave(const unsigned nSamples, const double dt, const unsigned flag=0);
    LArWave(const std::vector<double>& theVector, const double dt, const unsigned flag=0);
    virtual ~LArWave();

    LArWave& operator= (const LArWave&);
    //@}
    
    /// \name accumulation  accessors
    //@{
    /// Wave parameters
    const std::vector<double>& getWave()               const;

    /// delta time
    const double&               getDt()                const { return m_dt; }

    /// Amplitude  per time bin
    inline const double&       getSample(const unsigned int i) const {return m_amplitudes[i];}

    /// Amplitude  per time bin 
    inline const double&       operator[](const unsigned int i) const {return m_amplitudes[i];}

    /// time
    double                     getTime(const unsigned i)     const;

    /// number of time samples
    inline size_t              getSize() const { return m_amplitudes.size() ; } 

    /// index for a time value
    unsigned                   getIndex(double aTime)  const;

    /// flag: ...
    unsigned                   getFlag()               const;

    /// is LArWave uninitialized?
    bool                       isEmpty()               const;
    //@}
    

    /// \name accumulation setters
    //@{
    /// Amplitude  per time bin - NOT const
    inline double&             operator[](const unsigned int i) {return m_amplitudes[i];}

    /// resize the number of time bin samples
    void setSize     (const unsigned& nSize);

    /// set the delta time
    void setDt       (const double dt);

    /// set the amplitude for time bin i
    void setSample   (const unsigned& i, const double& aVal) { m_amplitudes[i] = aVal ; } 

    /// set flag
    void setFlag     (const unsigned flag);
    //@}
 

    /// \name accumulation alegebra operations
    //@{
    LArWave operator+ (const LArWave& bWave) const ; // point-to-point +
    LArWave operator- (const LArWave& bWave) const ; // point-to-point -
    LArWave operator* (const LArWave& bWave) const ; // point-to-point *
    LArWave operator/ (const LArWave& bWave) const ; // point-to-point /
    LArWave operator% (const LArWave& bWave) const ; // convolution
    LArWave operator+ (const double& aBias) const ;   // add a constant 
    LArWave operator* (const double& aScale) const ;  // multiply by a constant

    LArWave& operator+=(const LArWave& bWave);
    LArWave& operator-=(const LArWave& bWave);
    //@}

 protected:

    double              m_dt;
    std::vector<double> m_amplitudes;
    unsigned            m_flag ;

    // construct wave with m_dt from two waves and long as the shortest wave
    LArWave(const LArWave& theWave1 , const LArWave& theWave2) ;

public:

    // definition of flags for different kinds of waveforms:
    enum { 
	meas        = 01 ,  // measured (cumulated from several triggers)
	mwf         = 02 ,  // calibration master waveform
	mwfN        = 03 ,  // as mwf, but normalized to have peak=1
	dac0        = 04 ,  // dac0 wave
	xtalkcorr   = 05 ,  // cross-talk corrected wave (Strips)

	pred        = 10 ,  // predicted from observed calibration (generic)
	predCali    = 11 ,  // as pred, parameters calibration only (a` la Milano)
	predFitPhys = 12 ,  // as pred, parameters from fit to observed phys (a` la Annecy)
	predMeasPar = 13 ,  // as pred, parameters from independent measurements

	model       = 20 ,  // from some model (generic)

	patch       = 30 ,  // patched from another channel

	unknown     =  0 
    };

private:

};



inline
LArWave::LArWave() : m_dt(0), m_flag(0)
{} 


inline
LArWave::LArWave(const unsigned nSamples, const double dt, const unsigned flag) 
	: 
	m_dt(dt) , m_flag(flag)
{ m_amplitudes.resize(nSamples) ; }


inline
LArWave::LArWave(const std::vector<double>& theVector,const double dt, const unsigned flag) 
	: 
	m_dt(dt) , m_amplitudes(theVector) , m_flag(flag)
{} 


inline
LArWave::~LArWave() 
{} 

inline
const std::vector<double>& 
LArWave::getWave()    const 
{ return m_amplitudes ; } 

inline
double 
LArWave::getTime(const unsigned i)       const 
{ return i * m_dt ; } 


inline
unsigned
LArWave::getFlag()               const 
{ return m_flag ; } 

inline
bool
LArWave::isEmpty()               const
{ return (m_amplitudes.size() == 0); } 

inline
void	
LArWave::setSize(const unsigned& nSize)           
{ m_amplitudes.resize(nSize,0) ; } 

inline
void	
LArWave::setDt(const double dt)                  
{ m_dt = dt ; } 


inline
void 
LArWave::setFlag(const unsigned flag)            
{ m_flag = flag ; } 



#endif // LARRAWCONDITIONS_LARWAVE_H

