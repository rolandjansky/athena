/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CcawUtils_H
#define CcawUtils_H

#include <cmath>

namespace ccaw {

  inline double sqr( double a) {return a*a;}

  inline double area( double R) {return M_PI*R*R;}

  inline double gaugeDiam( int gauge) { return 0.127e-3*pow(92, double(36-gauge)/39.0);}

  inline double gaugeSurface( int gauge) { return M_PI*sqr(gaugeDiam(gauge))/4.;}

  /// Units: kg/m3
  inline double ccawDensity() {
    double dens_cu = 8.94e3;  // kg/m3
    double dens_al = 2.70e3;  // kg/m3
    return 0.1*dens_cu + 0.9*dens_al;
  } 

  /// Weight per unit of length for the CCAW conductor, in kg/meter.
  inline double conductorWeight( int gauge) { return gaugeSurface(gauge) * ccawDensity();}

  /** Weight per unit of length for the insulator. The defaults correspond to Quadruple ML insulation (check density!)
      Units are meters for thickness, kg/m3 for density, and kg/m for the result
   */
  inline double insulatorWeight( int gauge, double thick=90.e-6, double dens=1.27e3) {
    double radius = gaugeDiam(gauge) / 2.;
    return (area( radius+thick) - area( radius)) * dens;
  }

  inline double ccawResistivity(int gauge) { 
    double res_cu = 16.78e-9; // Ohm*m
    double res_al = 28.2e-9;  // Ohm*m

    double s = gaugeSurface( gauge);
    double R_al = res_al/(s*0.9);
    double R_cu = res_cu/(s*0.1);

    double invR = 1/R_al + 1/R_cu;
    return 1./invR;
  }

  inline int closestResistivityCcawGauge( double res)
  {
    // start arbitrarily with gauge 22
    int gauge = 22;
    double oldres = ccawResistivity(gauge);
    double newres;
    if (res > oldres) { 
      do {
	newres = ccawResistivity(++gauge);
	if (newres > res) break;
	oldres = newres;
      } while (gauge < 37);
      if ( fabs( 1-newres/res) < fabs( 1-res/oldres)) return gauge;
      else return --gauge;  // old gauge was closer
    }
    else {
      do {
	newres = ccawResistivity(--gauge);
	if (newres < res) break;
	oldres = newres;
      } while (gauge > 14);
      if ( fabs( 1-newres/res) < fabs( 1-res/oldres)) return gauge;
      else return ++gauge;  // old gauge was closer
    }
  }

}
#endif
