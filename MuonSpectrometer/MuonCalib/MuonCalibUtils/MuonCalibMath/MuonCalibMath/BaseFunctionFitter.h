/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#ifndef BaseFunctionFitterHXX
#define BaseFunctionFitterHXX

//////////////////
// HEADER FILES //
//////////////////

// CLHEP //
//#include "CLHEP/config/CLHEP.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "EventPrimitives/EventPrimitives.h"

// standard C++ libraries //
#include <iostream>
#include <iomanip>
#include <fstream>

// STL //
#include <vector>

// MDT calibration utilities //
#include "MuonCalibMath/BaseFunction.h"
#include "MuonCalibMath/SamplePoint.h"

namespace MuonCalib {

/**

@class BaseFunctionFitter

This class performs a fit of a linear combination of base functions to 
a set of sample points.                                                

@author Oliver.Kortner@cern.ch

@date 05.04.2005

*/
  class BaseFunctionFitter {

  private:
    // auxiliary minimization objects //
    int m_nb_coefficients; //!< number of coefficients
    Amg::MatrixX m_A; //!< coefficient matrix for the fit
    Amg::VectorX m_alpha; //!< coefficients of the base functions after the fit
    Amg::VectorX m_b; //!< m_A*m_alpha = m_b;

    // private methods //
    inline void init(void); //!< default initialization method
    /** initialization method: 
	the number of fit parameters (coefficients) is set to nb_coefficients */
    inline void init(const int & nb_coefficients); 

  public:
    // Constructors
    /** default constructor, the number of fit parameters will be set to 5 */
    BaseFunctionFitter(void) {
      init();
    } 
    /** constructor, the number of fit parameters is set to nb_coefficients */
    BaseFunctionFitter(const int & nb_coefficients) {
      init(nb_coefficients);
    }				

    // Methods
    // get-methods //
    /** get the number of fit parameters (coefficients) of the base functions 
	to be fitted */
    inline int number_of_coefficients(void) const;
    /** get the coefficients determined by the fit to the sample points */ 
    inline Amg::VectorX coefficients(void) const;

    // set-methods //
    /** set the number of fit parameters (coefficients) of the base
	functions to nb_coefficients */
    inline void set_number_of_coefficients(const int & nb_coefficients);
    /** perform a fit of the base functions (base_function) to the sample
	points as given in "sample_point" starting at the sample point first_point
	and stopping at the point last_point, 
	1 <= first_point < last_point <= size of the sample_point vector;
	the method returns true, if the fit failed */
    bool fit_parameters(const std::vector<SamplePoint> & sample_point,
			const unsigned int & first_point,
			const unsigned int & last_point,
			BaseFunction * base_function);
  };

}

/////////////////////////////////////////////////
// INCLUDE THE IMPLEMENATION OF INLINE METHODS //
/////////////////////////////////////////////////

#include "BaseFunctionFitter.ixx"

#endif
