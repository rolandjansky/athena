/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_PAI_element.h"
#include "TRT_PAI_gasComponent.h"

//___________________________________________________________________________

void TRT_PAI_gasComponent::addElement(TRT_PAI_element* pe,
				      unsigned int n) {
  m_pelements.push_back(pe);
  m_elementMultiplicity.push_back(n);
  return;
}

//___________________________________________________________________________

double TRT_PAI_gasComponent::getDensity(double tempK) {
  double mass = 0.;
  int n = m_pelements.size();
  for ( int i=0; i<n; ++i ) {
    mass += m_elementMultiplicity[i] * m_pelements[i]->getAtomicA();
  }
  double density = mass/22400.;
  if ( tempK>0. ) density = density*293./tempK;
  return density;
}

//___________________________________________________________________________
