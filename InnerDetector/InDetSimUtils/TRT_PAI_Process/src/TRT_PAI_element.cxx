/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_PAI_element.h"
#include <cmath>
#include <vector>
#include <string>
#include <iostream>

//____________________________________________________________________________

TRT_PAI_element::TRT_PAI_element(const std::string & nm,
				 const float *E,
				 const float *S,
				 int          N,
				 double       Z,
				 double       A) :
  m_name(nm),
  m_atomicA(A),
  m_atomicZ(Z)
{
  for ( int i=0; i<N; ++i ) {
    m_lnEnergyLvls.push_back( std::log(E[i]) );
    m_lnCrossScts.push_back( std::log(S[i]) );
  }
  return;
}

//____________________________________________________________________________

double TRT_PAI_element::getDensity(double tempK) {
  double density = m_atomicA/22400.;
  if ( tempK>0. ) density = density*293./tempK;
  return density;
}

//____________________________________________________________________________
