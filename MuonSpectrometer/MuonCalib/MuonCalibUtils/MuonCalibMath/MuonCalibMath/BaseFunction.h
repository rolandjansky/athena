/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef MUONCALIB_BASEFUNCTION_H
#define MUONCALIB_BASEFUNCTION_H

namespace MuonCalib {

/**

@class BaseFunction

This is an abstract base class for a set of base functions for fits to 
sample points. 

@author Oliver.Kortner@cern.ch

@date 04.04.2005

*/
  class BaseFunction {

  public:
    // Constructor and destructor
    BaseFunction(void) {}			//!< default constructor
    virtual ~BaseFunction(void) {}		//!< destructor

    // Methods
    virtual double value(const int & k, const double & x) const = 0; //!< get the value of the k-th base function at x

  };

}

#endif
