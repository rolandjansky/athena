/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// inline function for handling two phi angles, to avoid 2PI wrap around. 
//

#include <math.h> 

inline double cycle(double a, double b)
{
  double del = b-a;
  if (del > M_PI) 
    {
      return a+2.0*M_PI;
    }
  else if (del < -M_PI)
    {
      return a-2.0*M_PI;
    }
  else
    {
      return a;
    }
}
