/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SamplePointHXX
#define SamplePointHXX

namespace MuonCalib { 

/**
@class SamplePoint
This class provides a sample point for the BaseFunctionFitter.
@author Oliver.Kortner@cern.ch
@date 04.04.2005
*/

  class SamplePoint {

  private:
    // sample point triplet //
    double m_x1; //!< x1 coordinate of the sample point
    double m_x2; //!< x2 coordinate of the sample point
    double m_sigma; //!< error of the x2 coordinate of the sample point

    // inline methods //
    /**initialization method, 

    triplet[0] = x1 coordinate of the sample point,

    triplet[1] = x2 coordinate of the sample point,

    triplet[2] = error of the x2 coordinate of the sample point
    */
    inline void init(double triplet[3]);

    /** initialization method, 

    mx1 = x1 coordinate of the sample point,

    mx2 = x2 coordinate of the sample point,

    msigma = error of the x2 coordinate of the sample point
    */
    inline void init(const double & mx1, const double & mx2,
		       const double & msigma); 

  public:
    // Constructors
    SamplePoint(void) {
      m_x1 = 0.0; m_x2 = 0.0; m_sigma = 0.0;
    }				//!< default constructor
    /** constructor,

    triplet[0] = x1 coordinate of the sample point,

    triplet[1] = x2 coordinate of the sample point,

    triplet[2] = error of the x2 coordinate of the sample point
    */
    SamplePoint(double triplet[3]) {
      init(triplet);
    }

    /** constructor,

    mx1 = x1 coordinate of the sample point,

    mx2 = x2 coordinate of the sample point,

    msigma = error of the x2 coordinate of the sample point
    */
    SamplePoint(const double & mx1, const double & mx2,
		const double & msigma) {
      init(mx1, mx2, msigma);
    }

    // Methods
    // get-methods //
    inline double x1(void) const; //!< get the x1 coordinate of the sample point
    inline double x2(void) const; //!< get the x2 coordinate of the sample point
    inline double error(void) const; //!< get the error on the x2 coordinate of the sample point

    // set-methods //
    /** set the sample point,

    x[0] = x1 coordinate of the sample point,

    x[1] = x2 coordinate of the sample point,

    x[2] = error of the x2 coordinate of the sample point
    */
    inline void set_triplet(const double x[3]); 
    inline void set_x1(const double & mx1); //!< set the x1 coordinate of the sample point to mx1
    inline void set_x2(const double & mx2); //!< set the x2 coordinate of the sample point to mx2
    inline void set_error(const double & merror); //!< set the error of the x2 coordinate sample point to merror

  };

}

//////////////////////////////////////////////////
// INCLUDE IMPLEMENTATION OF THE INLINE METHODS //
//////////////////////////////////////////////////

#include "SamplePoint.ixx"

#endif
