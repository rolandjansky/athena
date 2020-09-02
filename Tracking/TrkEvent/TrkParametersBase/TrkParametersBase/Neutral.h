/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Neutral.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKPARAMETERSBASE_NEUTRAL_H
#define TRKPARAMETERSBASE_NEUTRAL_H 1

#include <iostream>
#include <cmath>

namespace Trk
{
  /**
     @class Neutral

     Simple helper class for defining track parameters for neutral particles

     @author Christian Gumpert <christian.gumpert@cern.ch>
   */
  class Neutral
  {
  public:
    /** Default constructor */
    Neutral() = default;

    /** Default constructor with dummy parameter */
    Neutral(const double&){}

    /** Copy constructor */
    Neutral(const Neutral&) = default;

    /** Move constructor */
    Neutral(Neutral&&) = default;

    /** Default destructor */
    ~Neutral() = default;

    /** Assignment operator */
    Neutral& operator=(const Neutral&) = default;

    /** Move assignment operator */
    Neutral& operator=(Neutral&&) = default;

    /** Charge for neutral particles is always 0 */
    double charge() const {return 0.;}

    /** Setting the charge for a neutral particle has no effect */
    void setCharge(double) {}

    /** Equality operator yields always true */
    bool operator==(const Neutral&) const
    {
      return true;
    }

    /** Inequality operator */
    bool operator!=(const Neutral& rhs) const
    {
      return !(*this == rhs);
    }
  };
} //end of namespace Trk
#endif
