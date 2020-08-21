/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FitQuality.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTPRIMITIVES_TRKFITQUALITY_H
#define TRKEVENTPRIMITIVES_TRKFITQUALITY_H

#include <cmath>
#include <iostream>

class MsgStream;

namespace Trk {

/** @class FitQuality
    @brief Base class to represent and store fit qualities from track
           reconstruction in terms of @f$\chi^2@f$ and number of
           degrees of freedom.

        Its main use is to describe the trajectory fit quality at
        a measurement and of the overall track. However, it can
        be extended as necessary.

        @author Edward.Moyse@cern.ch, Andreas.Salzburger@cern.ch
*/

class FitQuality
{
public:
  /** default ctor for POOL*/
  FitQuality() = default;

  FitQuality(const FitQuality&) = default;
  FitQuality(FitQuality&&) = default;
  FitQuality& operator=(const FitQuality&) = default;
  FitQuality& operator=(FitQuality&&) = default;
  virtual ~FitQuality() = default;

  /** Constructor with @f$ \chi^2 @f$ and @f$ n_{dof} @f$ */
  FitQuality(double chiSquared, int numberDoF);

  /**Constructor hadling double type of NDF*/
  FitQuality(double chiSquared, double numberDoF);

  /**Virtual constructor */
  virtual FitQuality* clone() const;

  /** returns the @f$ \chi^2 @f$ of the overall track fit*/
  double chiSquared() const;

  /** returns the number of degrees of freedom of the overall track or
  vertex fit as integer */
  int numberDoF() const;

  /** returns the number of degrees of freedom of the overall track or
  vertex fit as double */
  double doubleNumberDoF() const;

  /** set the @f$ \chi^2 @f$*/
  void setChiSquared(double chiSquared);

  /** set the number of degrees of freedom*/
  void setNumberDoF(double numberDoF);


private:
  double m_chiSquared;
  double m_numberDoF;

}; // end of class definitions

/**Overload of << operator for MsgStream for debug output*/
MsgStream&
operator<<(MsgStream& sl, const FitQuality& fq);
/**Overload of << operator for std::ostream for debug output*/
std::ostream&
operator<<(std::ostream& sl, const FitQuality& fq);

} // end ns
#include "TrkEventPrimitives/FitQuality.icc"
#endif // TRKEVENTPRIMITIVES_FITQUALITY_H
