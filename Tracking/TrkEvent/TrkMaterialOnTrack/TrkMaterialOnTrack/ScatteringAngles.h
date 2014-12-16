/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCATTERINGANGLEONTRACK_H
#define SCATTERINGANGLEONTRACK_H

#include <iostream>

class MsgStream;

namespace Trk {
/** @brief  represents a deflection of the track
    caused through multiple scattering in material.

    This class is part of the ATLAS tracking EDM and holds the
    deflection angles wrt. to the track's theta and phi
    and their uncertainties. Like that it can be filled with the full
    estimated deflection or only the additional uncertainty on the
    momentum direction as it is used by Kalman-filter style track fits.

    @author Edward Moyse <http://consult.cern.ch/xwho/people/>
  */

class ScatteringAngles
 {

 public:
   //! default constructor for POOL - do not use in reconstruction!
   ScatteringAngles();
   //! full constructor passing two deflection angles and their uncertainties
   ScatteringAngles(double deltaPhi, 
                    double deltaTheta, 
                    double sigmaDeltaPhi, 
                    double sigmaDeltaTheta);
   
   //! virtual destructor 
   virtual ~ScatteringAngles();
	
   //! returns the \f$ \Delta\phi \f$
   double deltaPhi()   const;
	
   //! returns the \f$ \Delta\theta \f$
   double deltaTheta() const;
	
   //! returns the \f$ \sigma\Delta\phi \f$
   double sigmaDeltaPhi()   const;
	
   //! returns the \f$ \sigma\Delta\theta \f$
   double sigmaDeltaTheta() const;

 private:
   //! holds deflection \f$ \Delta\phi \f$
   double m_deltaPhi;
   //! holds deflection \f$ \Delta\theta \f$
   double m_deltaTheta;
   //! holds the error \f$ \sigma\Delta\phi \f$ on the phi deflection
   double m_sigmaDeltaPhi;
   //! holds the error \f$ \sigma\Delta\theta \f$ on the theta deflection
   double m_sigmaDeltaTheta;
};

//! Overload of << operator for MsgStream for debug output 
MsgStream& operator << ( MsgStream& sl, const Trk::ScatteringAngles& saos);
//! Overload of << operator for std::ostream for debug output 
std::ostream& operator << ( std::ostream& sl, const Trk::ScatteringAngles& saos);

}//end of Trk ns

inline double Trk::ScatteringAngles::deltaPhi() const
{
	return m_deltaPhi;
}

inline double Trk::ScatteringAngles::deltaTheta() const
{
	return m_deltaTheta;
}

inline double Trk::ScatteringAngles::sigmaDeltaPhi() const
{
	return m_sigmaDeltaPhi;
}

inline double Trk::ScatteringAngles::sigmaDeltaTheta() const
{
	return m_sigmaDeltaTheta;
}

#endif

