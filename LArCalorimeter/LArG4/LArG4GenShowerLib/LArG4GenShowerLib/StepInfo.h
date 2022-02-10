/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4GENSHOWERLIB_STEPINFO_H
#define LARG4GENSHOWERLIB_STEPINFO_H

// STL includes
#include <iostream>

// CLHEP include for Hep3Vector
#include "CLHEP/Vector/ThreeVector.h"


/// Namespace for the ShowerLib related classes
namespace ShowerLib
{

  /**
   *
   *   @short Class to collect information about G4 steps
   *
   *          This class is designed to transfer hit information,
   *          i.e. position, energy deposition and time, from
   *          G4 simulation to the clustering algorithm
   *
   *  @author Wolfgang Ehrenfeld, University of Hamburg, Germany
   *  @author Sasha Glazov, DESY Hamburg, Germany
   *
   * @version $Id: StepInfo.h 511332 2012-07-25 16:15:07Z ssnyder $
   *
   */

  class StepInfo
  {

  public:

    //! empty default constructor
    StepInfo(): m_dep(0.), m_time(0.), m_valid(true) {}

    StepInfo(const StepInfo& first, const StepInfo& second);

    /* access functions */

    //! set position
    inline void setP(const CLHEP::Hep3Vector& p) { m_pos = p; }
    //! set x position
    inline void setX(const double x) { return m_pos.setX(x); }
    //! set y position
    inline void setY(const double y) { return m_pos.setY(y); }
    //! set z position
    inline void setZ(const double z) { return m_pos.setZ(z); }
    //! set depoisted energy
    inline void setE(const double t) { m_dep = t; }
    //! set time
    inline void setTime(const double t) { m_time = t; }
    //! set validity
    inline void setValid(const bool flag) { m_valid = flag; }

    //! return spacial position
    inline CLHEP::Hep3Vector position() const { return m_pos; }
    //! return x position
    inline double x() const { return m_pos.x(); }
    //! return y position
    inline double y() const { return m_pos.y(); }
    //! return z position
    inline double z() const { return m_pos.z(); }
    //! return deposited energy
    inline double dep() const { return m_dep; }
    //! return time of hit
    inline double time() const { return m_time; }
    //! return validity flag
    inline bool valid() const { return m_valid; }

    /* helper functions */

    //! return spactial distance squared
    double diff2(const StepInfo& other) const;

    //! energy weighted sum
    StepInfo& operator+=(const StepInfo& other );

  private:

    // data members
    CLHEP::Hep3Vector m_pos;    //!< spatial position
    double m_dep;        //!< deposited energy
    double m_time;       //!< time
    bool m_valid;        //!< flag, if hit is valid

  };


  // INLINE DEFINITIONS
  inline StepInfo::StepInfo(const StepInfo& first, const StepInfo& second)
  {
    double esum = first.m_dep + second.m_dep;
    double w1 = 0;
    double w2 = 0;

    if (esum > 0) {
      w1 =  first.m_dep/esum;
      w2 =  second.m_dep/esum;
    }

    m_pos = w1*first.m_pos + w2*second.m_pos;
    m_time = w1*first.m_time + w2*second.m_time;
    m_dep = esum;
    m_valid = true;
  }

  inline double StepInfo::diff2(const StepInfo& other) const
  {
    return (this->position().diff2(other.position()));
  }

  inline StepInfo& StepInfo::operator+=(const StepInfo& other )
  {
    double esum = m_dep + other.m_dep;
    double w1 = 0;
    double w2 = 0;

    if (esum > 0) {
      w1 =  m_dep/esum;
      w2 =  other.m_dep/esum;
    } else {
      std::cout << "Error: hit combination: sum of deposited energy is zero!" << std::endl;
    }

    m_pos = w1*m_pos + w2*other.m_pos;
    m_dep += other.m_dep;

    return *this;
  }

} // namespace ShowerLib

#endif // LARG4GENSHOWERLIB_STEPINFO_H
