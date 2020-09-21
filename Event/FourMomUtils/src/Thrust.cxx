/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "FourMomUtils/Thrust.h"
#include "FourMomUtils/P4Sorters.h"

// AthAnalysisBase/ManaCore doesn't currently include the Trigger Service
#ifndef XAOD_ANALYSIS

#include <cmath>

namespace FourMomUtils {

using std::abs;
using std::exp;

CLHEP::Hep3Vector
thrust( const I4MomIter_t& iBeg, const I4MomIter_t& iEnd,
  double& thrust_major, double& thrust_minor, bool useThreeD)
{
  /*
     Finding the thrust axis in an event is not trivial.

     Here, we follow the procedure described in the PYTHIA manual JHEP 05 (2006) 026,
     also hep-ph/0603175. The approach is to use an iterative method, which usually
     converges quickly. As the minimization can find just a local minimum, different
     starting points for the thrust axis are tried. By default, first the direction
     of the four most energetic particles are tried, if their result disagrees, all
     16 permutations of the sum of all 4 particles are tried (with coefficients +- 1)

     Note, that the thrust is calculated for _ALL_ particles. If you want only a subset
     of particles, you have to apply a cut beforehand.
     See e.g. Reconstruction/EventShapes/EventShapeTools for examples.
  */

  CLHEP::Hep3Vector thrust(0,0,0);

  int agree=0;
  int disagree=0;

  CLHEP::Hep3Vector n_0[20];
  short add0[20] = { 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,-1,-1,-1,-1,-1,-1,-1,-1 };
  short add1[20] = { 0, 1, 0, 0, 1, 1, 1, 1,-1,-1,-1,-1, 1, 1, 1, 1,-1,-1,-1,-1 };
  short add2[20] = { 0, 0, 1, 0, 1, 1,-1,-1, 1, 1,-1,-1, 1, 1,-1,-1, 1, 1,-1,-1 };
  short add3[20] = { 0, 0, 0, 1, 1,-1, 1,-1, 1,-1, 1,-1, 1,-1, 1,-1, 1,-1, 1,-1 };

  std::vector<const INavigable4Momentum*> v_copy(4);

  // partial_sort_copy sorts a copy of the collection according to energy and
  //  returns only the first four elements (minimum of input collection size and
  //  pre-allocated output vector v_copy
  partial_sort_copy( iBeg, iEnd,
         v_copy.begin(), v_copy.end(),
         P4Sorters::Descending::Ene() );

  int n_tests=0;
  int max_tests=std::min<int>(20, std::distance(iBeg, iEnd));
  do{
    n_0[n_tests]=CLHEP::Hep3Vector(0,0,0);

    // assign direction of four most energetic particles
    n_0[n_tests] +=
      add0[n_tests] * CLHEP::Hep3Vector(v_copy.at(0)->px(), v_copy.at(0)->py(), v_copy.at(0)->pz()) +
      add1[n_tests] * CLHEP::Hep3Vector(v_copy.at(1)->px(), v_copy.at(1)->py(), v_copy.at(1)->pz()) +
      add2[n_tests] * CLHEP::Hep3Vector(v_copy.at(2)->px(), v_copy.at(2)->py(), v_copy.at(2)->pz()) +
      add3[n_tests] * CLHEP::Hep3Vector(v_copy.at(3)->px(), v_copy.at(3)->py(), v_copy.at(3)->pz());

    if(!useThreeD)
      n_0[n_tests].setZ(0);

    /* // my convention : x is always positive (thrust axis has two fold ambiguity)
       if(n_0[n_tests].x()<0)
       n_0[n_tests] = - n_0[n_tests]; */

    // protect against small number of input particles (smaller than 4!)
    if(n_0[n_tests].mag()>0)
      n_0[n_tests] /= n_0[n_tests].mag();

    int loop=0;
    bool run=false;
    do{
      CLHEP::Hep3Vector n_1(0,0,0);
      for ( I4MomIter_t itr = iBeg; itr != iEnd; ++itr )
        {
          //  if(((*itr)->hlv()).vect().dot(n_0[n_tests])>0)
          if((*itr)->px() * n_0[n_tests].x() +
             (*itr)->py() * n_0[n_tests].y() +
             (*itr)->pz() * n_0[n_tests].z() > 0 )
            n_1 += CLHEP::Hep3Vector((*itr)->px(), (*itr)->py(), (*itr)->pz() );
          else
            n_1 -= CLHEP::Hep3Vector((*itr)->px(), (*itr)->py(), (*itr)->pz() );
        }

      if(!useThreeD)
        n_1.setZ(0);

      // protect against few number of input particles (smaller than 4!)
      if(n_1.mag()>0)
        n_1 /= n_1.mag();

      // has axis changed ? if so, try at most ten times (thrust axis has two fold ambiguity)
      run = ( n_0[n_tests]!=n_1 ) && ( -n_0[n_tests]!=n_1 ) && loop++<10;
      n_0[n_tests] = n_1;
    }while( run );

    // agrees or disagrees with first result ?
    //  thrust has a sign ambiguity
    if( n_tests>0 && ( n_0[0] == n_0[n_tests] || n_0[0] == -n_0[n_tests] ) ) agree++;
    if( n_tests>0 &&   n_0[0] != n_0[n_tests] && n_0[0] != -n_0[n_tests] )   disagree++;

    // stop if four first tries give same result (no test for first try! )
    //  if not, try at most max_tests combinations
  }while( ( disagree>0 || agree<4 ) && ++n_tests < max_tests);

  // now that we have the thrust axis, we determine the thrust value
  //  if the various calculations of the thrust axes disagree, try all
  //  and take the maximum, calculate minor and mayor axis
  n_tests=0;
  do{
    double denominator = 0;
    double numerator_t = 0;
    double numerator_m = 0;
    for ( I4MomIter_t itr = iBeg; itr != iEnd; ++itr )
      {
        CLHEP::Hep3Vector c((*itr)->hlv().vect());
        c.setZ(0);
        numerator_t += abs(c.dot(n_0[n_tests]));
        numerator_m += (c.cross(n_0[n_tests])).mag();
        denominator += c.mag();
      }
    const double inv_denominator = 1. / denominator;
    if( numerator_t * inv_denominator > thrust_major )
      {
        thrust_major = numerator_t * inv_denominator;
        thrust_minor = numerator_m * inv_denominator;
        thrust=n_0[n_tests];
      }
  }while(disagree>0 && ++n_tests < max_tests);

  /*  std::cout << "Calculation of Thrust gave: ( "
      << thrust.x() << " | "
      << thrust.y() << " | "
      << thrust.z() << " )\n"; */

  // return StatusCode::SUCCESS;
  return thrust;
}

} //> end namespace FourMomUtils

#endif
