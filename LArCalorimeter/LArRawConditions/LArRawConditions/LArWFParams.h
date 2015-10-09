//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARWFPARAMS_H
#define LARWFPARAMS_H

#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/CaloGain.h"

#include "LArRawConditions/LArCaliPulseParamsComplete.h"
#include "LArRawConditions/LArDetCellParamsComplete.h"
#include "LArRawConditions/LArPhysCaliTdiffComplete.h"
#include "LArRawConditions/LArTdriftComplete.h"

class LArWFParams
{

public:

    /// default constructor: empty, "invalid" parameters set:
    LArWFParams();
    
    /// constructors for "valid" parameters sets:

    LArWFParams(double tcal, double fstep, double tdrift, double omega0, 
		double taur, double tshaper, double ampli, double tdiff);
    LArWFParams(unsigned flag,
		double tcal, double fstep, double tdrift, double omega0, 
		double taur, double tshaper, double ampli, double tdiff);
    
    /// same constructors, but without Tdiff specification (kept for
    /// backward compatibility)

    LArWFParams(double tcal, double fstep, double tdrift, double omega0, double taur, 
		double tshaper, double ampli);
    LArWFParams(unsigned flag,
		double tcal, double fstep, double tdrift, double omega0, 
		double taur, double tshaper, double ampli);
    
    //// constructor that retrieves a parameters set from
    //// LArCaliPulseParamsComplete, LArDetCellParamsComplete,
    //// LArPhysCaliTdiffComplete, LArTdriftComplete collections

    LArWFParams(HWIdentifier chID, CaloGain::CaloGain gain, 
                const LArCaliPulseParamsComplete& caliPar,
		const LArDetCellParamsComplete& detPar, 
		const LArPhysCaliTdiffComplete& tdiff,
		const LArTdriftComplete& tdrift);
    // set methods

    void setFlag(unsigned flag);

    void setTcal(double tcal);
    void setFstep(double fstep);
    void setTdrift(double tdrift); // change of Tdrift value
    void setOmega0(double omega0);
    void setTaur(double taur);
    void setTshaper(double tsh);
    void setAmplitude(double ampli);
    void setTdiff (double tdiff);
  
    void setValid();
    void setBad();

    /// the followings pick up values from smaller Complete structures,
    /// return value is false if parameters were not found

    bool set(HWIdentifier chID, CaloGain::CaloGain gain, 
             const LArCaliPulseParamsComplete& caliPar);
    bool set(HWIdentifier chID, CaloGain::CaloGain gain, 
             const LArDetCellParamsComplete& detPar);
    bool set(HWIdentifier chID, CaloGain::CaloGain gain, 
             const LArPhysCaliTdiffComplete& tdiff);
    bool set(HWIdentifier chID, CaloGain::CaloGain gain, 
             const LArTdriftComplete& tdrift);
  
    // get methods

    unsigned int getFlag()       const;
    double tcal()                const;  // exp time constant in calib pulse
    double fstep()               const;  // asymptotic value of calib pulse (fraction of peak)
    double tdrift()              const;  // drift time
    double omega0()              const;  // resonance angular frequency ( = 1/sqrt(LC) )
    double taur()                const;  // resonance damping factor    ( = rC )
    double tshaper()             const;  // shaper time constant
    double amplitude()           const;  // usable for Mphys/Mcal
    double tdiff()               const;  // time difference between phys and calib pulses
    bool   isValid()             const;  

private:
    //Parameters needed for Physics prediction:
    unsigned int m_flag ;   /* tell method used for parameters extraction */
    double m_Tcal      ;
    double m_Fstep     ;
    double m_Tdrift    ;
    double m_Omega0    ;
    double m_Taur      ;
    double m_Tshaper   ;
    double m_Amplitude ;
    double m_Tdiff     ;
    bool m_isValid ;
public:

};

//////////////////////
// Inline definitions
//////////////////////

inline
LArWFParams::LArWFParams()
	: 
        m_flag(0),
	m_Tcal(0), 
	m_Fstep(0), 
	m_Tdrift(0), 
	m_Omega0(0), 
	m_Taur(0), 
	m_Tshaper(0), 
	m_Amplitude(0),
	m_Tdiff(0),
	m_isValid(false) {}

inline
LArWFParams::LArWFParams(double tcal, double fstep, double tdrift, double omega0,
			 double taur, double tshaper, double ampli, 
			 double tdiff)
	:
        m_flag(0),
	m_Tcal(tcal), 
	m_Fstep(fstep), 
	m_Tdrift(tdrift), 
	m_Omega0(omega0), 
	m_Taur(taur), 
	m_Tshaper(tshaper), 
	m_Amplitude(ampli), 
	m_Tdiff(tdiff), 
	m_isValid(true) {}

inline
LArWFParams::LArWFParams(unsigned flag,
			 double tcal, double fstep, double tdrift, double omega0, 
			 double taur, double tshaper, double ampli,
			 double tdiff)
	:
	m_flag(flag), 
	m_Tcal(tcal), 
	m_Fstep(fstep), 
	m_Tdrift(tdrift), 
	m_Omega0(omega0), 
	m_Taur(taur), 
	m_Tshaper(tshaper), 
	m_Amplitude(ampli), 
	m_Tdiff(tdiff), 	
	m_isValid(true) {}

inline
LArWFParams::LArWFParams(double tcal, double fstep, double tdrift, double omega0, 
			 double taur, double tshaper, double ampli)
	:
        m_flag(0),
	m_Tcal(tcal), 
	m_Fstep(fstep), 
	m_Tdrift(tdrift), 
	m_Omega0(omega0), 
	m_Taur(taur), 
	m_Tshaper(tshaper), 
	m_Amplitude(ampli), 
        m_Tdiff(0),
	m_isValid(true) {}

inline
LArWFParams::LArWFParams(unsigned flag,
			 double tcal, double fstep, double tdrift, double omega0, 
			 double taur, double tshaper, double ampli)
	:
	m_flag(flag), 
	m_Tcal(tcal), 
	m_Fstep(fstep), 
	m_Tdrift(tdrift), 
	m_Omega0(omega0), 
	m_Taur(taur), 
	m_Tshaper(tshaper), 
	m_Amplitude(ampli), 
        m_Tdiff(0),
	m_isValid(true) {}

inline
LArWFParams::LArWFParams(HWIdentifier chID, CaloGain::CaloGain gain, 
	      const LArCaliPulseParamsComplete& caliPar,
	      const LArDetCellParamsComplete& detPar, 
	      const LArPhysCaliTdiffComplete& tdiff,
	      const LArTdriftComplete& tdrift) 
{
    m_flag = 0;
    m_Amplitude = 0;
    m_isValid = true ;
    int igain = (int)gain ;

    m_Tshaper = 15 ; 

    if (  (m_Tcal=caliPar.Tcal(chID,igain))     == (float)ILArCaliPulseParams::ERRORCODE )
      m_isValid = false ;
    if (  (m_Fstep=caliPar.Fstep(chID,igain))   == (float)ILArCaliPulseParams::ERRORCODE )
      m_isValid = false ;

    if (  (m_Omega0=detPar.Omega0(chID,igain))  == (float)ILArDetCellParams::ERRORCODE )
      m_isValid = false ;
    if (  (m_Taur=detPar.Taur(chID,igain))      == (float)ILArDetCellParams::ERRORCODE )
      m_isValid = false ;

    if (  (m_Tdiff=tdiff.Tdiff(chID,igain))     == (float)ILArPhysCaliTdiff::ERRORCODE )
      m_isValid = false ;

    if (  (m_Tdrift=tdrift.Tdrift(chID))        == (float)ILArTdrift::ERRORCODE )
      m_isValid = false ;

}

inline void LArWFParams::setFlag(unsigned flag)           { m_flag      = flag ; } 

inline void LArWFParams::setTcal(double tcal)       { m_Tcal      = tcal ;   }
inline void LArWFParams::setFstep(double fstep)     { m_Fstep     = fstep ;  }
inline void LArWFParams::setTdrift(double tdrift)   { m_Tdrift    = tdrift ; } 
inline void LArWFParams::setOmega0(double omega0)   { m_Omega0    = omega0 ; }
inline void LArWFParams::setTaur(double taur)       { m_Taur      = taur ;   }
inline void LArWFParams::setTshaper(double tsh)     { m_Tshaper   = tsh ;    } 
inline void LArWFParams::setAmplitude(double ampli) { m_Amplitude = ampli ;  } 
inline void LArWFParams::setTdiff (double tdiff)    { m_Tdiff     = tdiff ;  }
  
inline void LArWFParams::setValid() { m_isValid = true ; } 
inline void LArWFParams::setBad()   { m_isValid = false ; }

inline bool LArWFParams::set(HWIdentifier chID, CaloGain::CaloGain gain, 
                             const LArCaliPulseParamsComplete& caliPar) {
    m_Tcal  = caliPar.Tcal(chID, (int)gain) ;
    m_Fstep = caliPar.Fstep(chID,(int)gain) ;
    return ( m_Tcal != ILArCaliPulseParams::ERRORCODE && 
	     m_Fstep != ILArCaliPulseParams::ERRORCODE ) ;
}

inline bool LArWFParams::set(HWIdentifier chID, CaloGain::CaloGain gain, 
                             const LArDetCellParamsComplete& detPar) {
    m_Omega0 = detPar.Omega0(chID, (int)gain) ;
    m_Fstep  = detPar.Taur(chID,(int)gain) ;
    return ( m_Omega0 != ILArDetCellParams::ERRORCODE && 
	     m_Taur != ILArDetCellParams::ERRORCODE ) ;
}

inline bool LArWFParams::set(HWIdentifier chID, CaloGain::CaloGain gain,
                             const LArPhysCaliTdiffComplete& tdiff) {
    m_Tdiff = tdiff.Tdiff(chID, (int)gain) ;
    return ( m_Tdiff != ILArPhysCaliTdiff::ERRORCODE ) ;
}

inline bool LArWFParams::set(HWIdentifier chID, CaloGain::CaloGain /*gain*/, 
                             const LArTdriftComplete& tdrift) {
    m_Tdrift = tdrift.Tdrift(chID) ;
    return ( m_Tdrift != ILArTdrift::ERRORCODE ) ;
}

inline unsigned int       LArWFParams::getFlag()         const { return m_flag ;     }  
inline double             LArWFParams::tcal()            const { return m_Tcal;      }  
inline double             LArWFParams::fstep()           const { return m_Fstep;     }  
inline double             LArWFParams::tdrift()          const { return m_Tdrift;    }  
inline double             LArWFParams::omega0()          const { return m_Omega0;    }  
inline double             LArWFParams::taur()            const { return m_Taur;      }  
inline double             LArWFParams::tshaper()         const { return m_Tshaper;   }  
inline double             LArWFParams::amplitude()       const { return m_Amplitude; }  
inline double             LArWFParams::tdiff()           const { return m_Tdiff;     }  
inline bool               LArWFParams::isValid()         const { return m_isValid ;  }  


#endif
