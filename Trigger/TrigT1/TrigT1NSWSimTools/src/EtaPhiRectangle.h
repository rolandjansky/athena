// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef NSW_ETAPHIRECTANGLE_H
#define NSW_ETAPHIRECTANGLE_H

#include <ostream>

namespace nsw {

  /*! \brief A class describing a rectangle in \eta , \phi
   *
   * An EtaPhiRectangle object can be used to keep track of a rectangle
   * in these coordinates. It is meant to be a simple object that takes
   * care of the 2\pi ambigiuties and that can perform simple operations
   * such as compute the overlap between two rectangles.  While the
   * minimum and maximum \phi values can be provided in any range,
   * internally all \phi angles are stored and handled within
   * (-\pi,+\pi].
   * Maybe at some point you could use TVector2.h (but they store x,y) or gsl::polar.
   *
   * davide.gerbaudo@gmail.com, 2012
   */

  //! \todo should probably define an EtaPhiPair instead of EtaPhiPoint...and use it everywhere
  typedef std::pair< double, double > EtaPhiPoint;
  //----------------------------------------------------------
  class EtaPhiRectangle{
  public:
    EtaPhiRectangle():
      etaMin_(0.), etaMax_(0.), etaCen_(0.), etaHw_(0.),
      phiMin_(0.), phiMax_(0.), phiCen_(0.), phiHw_(0.),
      wrapsAroundPi_(false) {};

      EtaPhiRectangle(const double &etaMin, const double &etaMax,
                      const double &phiMin, const double &phiMax);
      double area() const { return 2.0*etaHw_*2.0*phiHw_; };
      double eta() const { return etaCen_; };
      double phi() const { return phiCen_; };
      double etaMin() const { return etaMin_; };
      double etaMax() const { return etaMax_; };
      double phiMin() const { return phiMin_; };
      double phiMax() const { return phiMax_; };
      double etaHalfWidth() const {return etaHw_; };
      double phiHalfWidth() const {return phiHw_; };
      //! determine whether a given point in (\eta,\phi) is inside this EtaPhiRectangle
      bool contains(const EtaPhiPoint &point) const;
      //! same as above, but less safe agaist eta-phi swap
      bool contains(const double &eta, const double &phi) const { return contains(std::make_pair(eta,phi));};
      //! determine whether two rectangles overlap
      static double overlappingArea(const EtaPhiRectangle &lhs,
                                    const EtaPhiRectangle &rhs);
      //! compute the rectangle corresponding to the overlapping area
      static EtaPhiRectangle overlappingRectangle(const EtaPhiRectangle &lhs,
                                                  const EtaPhiRectangle &rhs);
      //! convert any angle to its equivalent in ( -\pi , +\pi ]
      static double phi_mpi_pi(const double &val);
      //! convert any angle to its equivalent in ( 0 , +2\pi ]
      static double phi_zero_2pi(const double &val);
      //! print the rectangle
      void print(std::ostream& stream) const;

  protected:
      //! compute the location of the center of the rectangle
      /*! In fact this method is initializing the internal representation
        of the rectangle, with center, half width, etc.
      */
      void computeCenterCoord();
  private:
      double etaMin_; //!< minimum eta
      double etaMax_; //!< maximum eta
      double etaCen_; //!< central eta
      double etaHw_;  //!< eta half width
      double phiMin_; //!< minimum phi
      double phiMax_; //!< maximum phi
      double phiCen_; //!< central phi
      double phiHw_;  //!< phi half width
      bool wrapsAroundPi_; //!< whether the rectangle crosses over \phi = \pi
  }; // end EtaPhiRectangle

  std::ostream& operator<< (std::ostream& stream, const EtaPhiRectangle &epr);
} // end namespace nsw

#endif

