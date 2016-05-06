/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//

#include <math.h> 

inline double phicorr(double a)
{ 
  if (a <= -M_PI)   
    {     
      return a+(2*M_PI*floor(-(a-M_PI)/(2*M_PI)));
    }
  else if (a > M_PI) 
    {
      return a-(2*M_PI*floor((a+M_PI)/(2*M_PI)));
    }
  else
    {
      return a;
    }
}
