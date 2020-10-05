/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>

namespace Trk {


void cfClstPnt( double *par, double *Vrt, double *ClstPnt)
{
  double e[3];   //Track direction at perigee
  e[0] = cos(par[3]);
  e[1] = sin(par[3]);
  e[2] = 1. / tan(par[2]);
  double e2 = 1.+e[2]*e[2];  // vector length squared

  
  double Per[3];   //Perigee position
  Per[0] =   sin(par[3]) * par[0];
  Per[1] = - cos(par[3]) * par[0];
  Per[2] =                 par[1];


  double u = (Vrt[0]-Per[0])*e[0]
            +(Vrt[1]-Per[1])*e[1]
	    +(Vrt[2]-Per[2])*e[2];
  u=u/e2;
  
  ClstPnt[0]=Per[0] + u*e[0];
  ClstPnt[1]=Per[1] + u*e[1];
  ClstPnt[2]=Per[2] + u*e[2];
}


} /* End of namespace */
