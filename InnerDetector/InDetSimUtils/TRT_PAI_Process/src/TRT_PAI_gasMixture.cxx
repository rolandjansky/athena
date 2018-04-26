/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_PAI_gasMixture.h"
#include "TRT_PAI_gasComponent.h"

#include <iostream>
#include <cmath>

//____________________________________________________________________________

void TRT_PAI_gasMixture::addComponent(TRT_PAI_gasComponent* pgc,
				      double frac) {
  if ( m_gasFrozen ) {
    if (msgLevel(MSG::ERROR)) msg(MSG::ERROR)
      << "gasMixture::addComponent: "
      << "cannot add new gasComponent - gas already frozen"
      << endreq;
    return;
  }

  m_compFracs.push_back(frac);
  m_pcomp.push_back(pgc);
  return;
}

//____________________________________________________________________________

void TRT_PAI_gasMixture::freezeGas() {
  if ( m_gasFrozen ) return;

  int    nComp = m_pcomp.size();
  double wtot  = 0.;
  for (int j=0; j<nComp; j++) {
    if ( m_compFracs[j] <= 0. ) {
      if (msgLevel(MSG::ERROR)) msg(MSG::ERROR)
	<< "gasMixture::freezeGas: "
	<< "A gasComponent has non-positive fraction"
	<< endreq;
      return;
    }
    wtot += m_compFracs[j];
  }

  if ( std::abs(wtot-1.) > 1e-5 ) {
    if (msgLevel(MSG::WARNING)) msg(MSG::WARNING)
      << "gasMixture::freezeGas: "
      << "Gas fractions do not add to unity but " << wtot
      << ". Re-normalizing!!"
      << endreq;
  }

  for (int j=0; j<nComp; ++j ) {
    m_compFracs[j] /= wtot;
  }

  m_gasFrozen = 1;

  // Now, find the composition in terms of elements

  double w;
  TRT_PAI_element* pe;
  wtot = 0.;
  for (int j=0; j<nComp; j++) {
    for(int i=0; i<m_pcomp[j]->getNElementTypes(); i++) {
      pe = m_pcomp[j]->getElement(i);
      w  = m_compFracs[j] * m_pcomp[j]->getElementMultiplicity(i);
      wtot += w;
      for ( unsigned int k=0; k<m_pelem.size(); k++ ) {
	if ( pe == m_pelem[k] ) {
	  m_elemWeights[k] += w;
	  w = -999.;
	  break;
	}
      }
      if ( w>0. ) {
	m_elemWeights.push_back(w);
	m_pelem.push_back(pe);
      }
    }
  }
  return;
}

//____________________________________________________________________________

void TRT_PAI_gasMixture::showStructure() {

  if ( !m_gasFrozen ) {
    if (msgLevel(MSG::WARNING)) msg(MSG::WARNING)
      << "gasMixture::showStructure: "
      << "Showing structure of non-frozen gas"
      << endreq;
  }

  if (msgLevel(MSG::INFO)) msg(MSG::INFO)
    << "The gas named '" << m_name
    << "' has the following components:"
    << endreq;
  for (unsigned int i=0; i<m_compFracs.size(); i++) {
    if (msgLevel(MSG::INFO)) msg(MSG::INFO)
      << " - " << m_compFracs[i]*100. << " percent "
      << m_pcomp[i]->getName() << " consisting of: ";
    for (int j=0; j<m_pcomp[i]->getNElementTypes(); j++) {
      if ( j>0 ) if (msgLevel(MSG::INFO)) msg(MSG::INFO) << ",";
      if (msgLevel(MSG::INFO)) msg(MSG::INFO)
	<< " " << m_pcomp[i]->getElementMultiplicity(j)
	<< " atoms " << m_pcomp[i]->getElement(j)->getName();
    }
    if (msgLevel(MSG::INFO)) msg(MSG::INFO) << endreq;
  }
  return;
}

//____________________________________________________________________________

TRT_PAI_gasComponent* TRT_PAI_gasMixture::getComponent(unsigned int n) {

  if ( n >= m_pcomp.size() ) {
    if (msgLevel(MSG::ERROR)) msg(MSG::ERROR)
      << "gasMixture::getComponent: out of bounds"
      << endreq;
    return m_pcomp[0];
  };

  return m_pcomp[n];
}

//____________________________________________________________________________

double TRT_PAI_gasMixture::getCompFraction(unsigned int n) {

  if ( n >= m_compFracs.size() ) {
    if (msgLevel(MSG::ERROR)) msg(MSG::ERROR)
      << "gasMixture::getCompFraction: out of bounds"
      << endreq;
    return m_compFracs[0];
  }

  return m_compFracs[n];
}

//____________________________________________________________________________

TRT_PAI_element* TRT_PAI_gasMixture::getElement(unsigned int n) {

  if ( n >= m_pelem.size() ) {
    if (msgLevel(MSG::ERROR)) msg(MSG::ERROR)
      << "TRT_PAI_gasMixture::getElement: out of bounds"
      << endreq;
    return m_pelem[0];
  };

  return m_pelem[n];
}

//____________________________________________________________________________

double TRT_PAI_gasMixture::getElemWeight(unsigned int n) {

  if ( n >= m_elemWeights.size() ) {
    if (msgLevel(MSG::ERROR)) msg(MSG::ERROR)
      << "gasMixture::getElemFraction:Error:out of bounds"
      << endreq;
    return 0;
  };

  return m_elemWeights[n];
}

//____________________________________________________________________________



void
TRT_PAI_gasMixture::display (const std::string& msg, int lvl) const
{
  this->msg() << static_cast<MSG::Level>(lvl) << msg << endreq;
}

void 
TRT_PAI_gasMixture::setLvl (int lvl)
{
  msg().setLevel ((MSG::Level)lvl);
}

void 
TRT_PAI_gasMixture::setLvl (const std::string& lvl)
{
  MSG::Level new_lvl = MSG::INFO;
  if (lvl=="debug")          { new_lvl = MSG::DEBUG;
  } else if (lvl=="info")    { new_lvl = MSG::INFO;
  } else if (lvl=="warning") { new_lvl = MSG::WARNING;
  } else if (lvl=="error")   { new_lvl = MSG::ERROR;
  } else {
    msg() << MSG::WARNING << "lvl [" << lvl << "] UNKNOWN !" << endreq;
    return;
  }
  msg().setLevel (new_lvl);
}
