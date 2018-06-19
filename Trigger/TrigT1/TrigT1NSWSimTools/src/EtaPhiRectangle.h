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
      m_etaMin(0.), m_etaMax(0.), m_etaCen(0.), m_etaHw(0.),
      m_phiMin(0.), m_phiMax(0.), m_phiCen(0.), m_phiHw(0.),
      m_wrapsAroundPi(false) {};

      EtaPhiRectangle(const double &etaMin, const double &etaMax,
                      const double &phiMin, const double &phiMax);
      double area() const { return 2.0*m_etaHw*2.0*m_phiHw; };
      double eta() const { return m_etaCen; };
      double phi() const { return m_phiCen; };
      double etaMin() const { return m_etaMin; };
      double etaMax() const { return m_etaMax; };
      double phiMin() const { return m_phiMin; };
      double phiMax() const { return m_phiMax; };
      double etaHalfWidth() const {return m_etaHw; };
      double phiHalfWidth() const {return m_phiHw; };
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
      double m_etaMin; //!< minimum eta
      double m_etaMax; //!< maximum eta
      double m_etaCen; //!< central eta
      double m_etaHw;  //!< eta half width
      double m_phiMin; //!< minimum phi
      double m_phiMax; //!< maximum phi
      double m_phiCen; //!< central phi
      double m_phiHw;  //!< phi half width
      bool m_wrapsAroundPi; //!< whether the rectangle crosses over \phi = \pi
  }; // end EtaPhiRectangle

  std::ostream& operator<< (std::ostream& stream, const EtaPhiRectangle &epr);
} // end namespace nsw

#endif

