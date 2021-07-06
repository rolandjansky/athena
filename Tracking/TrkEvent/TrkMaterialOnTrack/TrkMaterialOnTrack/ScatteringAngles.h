/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

class ScatteringAngles final
{

public:
  //! default constructor for POOL
  ScatteringAngles() = default;
  //! full constructor passing two deflection angles and their uncertainties
  ScatteringAngles(double deltaPhi,
                   double deltaTheta,
                   double sigmaDeltaPhi,
                   double sigmaDeltaTheta)
    : m_deltaPhi(deltaPhi)
    , m_deltaTheta(deltaTheta)
    , m_sigmaDeltaPhi(sigmaDeltaPhi)
    , m_sigmaDeltaTheta(sigmaDeltaTheta)
  {}

  ScatteringAngles(const ScatteringAngles&) = default;
  ScatteringAngles(ScatteringAngles&&) = default;
  ScatteringAngles& operator=(const ScatteringAngles&) = default;
  ScatteringAngles& operator=(ScatteringAngles&&) = default;
  //!  destructor
  ~ScatteringAngles() = default;

  //! returns the \f$ \Delta\phi \f$
  double deltaPhi() const;

  //! returns the \f$ \Delta\theta \f$
  double deltaTheta() const;

  //! returns the \f$ \sigma\Delta\phi \f$
  double sigmaDeltaPhi() const;

  //! returns the \f$ \sigma\Delta\theta \f$
  double sigmaDeltaTheta() const;

private:
  //! holds deflection \f$ \Delta\phi \f$
  double m_deltaPhi{ 0.0 };
  //! holds deflection \f$ \Delta\theta \f$
  double m_deltaTheta{ 0.0 };
  //! holds the error \f$ \sigma\Delta\phi \f$ on the phi deflection
  double m_sigmaDeltaPhi{ 0.0 };
  //! holds the error \f$ \sigma\Delta\theta \f$ on the theta deflection
  double m_sigmaDeltaTheta{ 0.0 };
};

//! Overload of << operator for MsgStream for debug output
MsgStream&
operator<<(MsgStream& sl, const Trk::ScatteringAngles& saos);
//! Overload of << operator for std::ostream for debug output
std::ostream&
operator<<(std::ostream& sl, const Trk::ScatteringAngles& saos);

} // end of Trk ns

inline double
Trk::ScatteringAngles::deltaPhi() const
{
  return m_deltaPhi;
}

inline double
Trk::ScatteringAngles::deltaTheta() const
{
  return m_deltaTheta;
}

inline double
Trk::ScatteringAngles::sigmaDeltaPhi() const
{
  return m_sigmaDeltaPhi;
}

inline double
Trk::ScatteringAngles::sigmaDeltaTheta() const
{
  return m_sigmaDeltaTheta;
}

#endif

