/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Charged.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKPARAMETERSBASE_CHARGED_H
#define TRKPARAMETERSBASE_CHARGED_H 1

#include <iostream>
#include <cmath>

namespace Trk
{
  /**
     @class Charged

     Simple helper class for defining track parameters for charged particles

     @todo Do we really need a @c double for the charge? Would an @c int be sufficient?

     @author Christian Gumpert <christian.gumpert@cern.ch>
   */

  class Charged
  {
  public:
    /** Default constructor */
   Charged(const double& charge = 1.):
      m_charge(charge)
    {}

    /** Copy constructor */
    Charged(const Charged&) = default;

    /** Move constructor */
    Charged(Charged&&) = default;

    /** Destructor */
    ~Charged() = default;

    /** Assignment operator */
    Charged& operator=(const Charged&) = default;

    /** Move assignment operator */
    Charged& operator=(Charged&&) = default;

    /** Return the charge */
    double charge() const {return m_charge;}

    /** Set the charge */
    void setCharge(double charge) {m_charge = charge;}

    /** Equality operator */
    bool operator==(const Charged& rOther) const
    {
      constexpr double tolerance = 1e-8;
      return fabs(charge() - rOther.charge()) < tolerance;
    }

    /** Inequality operator */
    bool operator!=(const Charged& rOther) const
    {
      return !(*this == rOther);
    }

  private:
    double m_charge;         //!< the charge value
  };
} // end of namespace Trk

#endif
