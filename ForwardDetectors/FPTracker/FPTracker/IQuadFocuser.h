/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPTRACKER_IQUADFOCUSER_H
#define FPTRACKER_IQUADFOCUSER_H


#include "boost/shared_ptr.hpp"
#include <string>

namespace FPTracker{
  class Point;
  class TransversePoint;
  class IQuadFocuser{
  public:
    virtual void focus(double, 
		       double, 
		       double, 
		       const TransversePoint& displacement, 
		       const Point& direction)             = 0;

    virtual ~IQuadFocuser();
    virtual double xe()                              const = 0;
    virtual double xae()                             const = 0;
    virtual double ye()                              const = 0;
    virtual double yae()                             const = 0;
    virtual std::string label()                      const = 0;

    typedef boost::shared_ptr< IQuadFocuser >        Ptr_t; // not const as focus is not const.... improve me

  };

}
#endif
