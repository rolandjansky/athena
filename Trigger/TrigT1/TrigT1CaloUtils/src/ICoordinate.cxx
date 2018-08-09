/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          ICoordinate.cxx  -  description
                             -------------------
    begin                : 26/02/2002
    email                : e.moyse@qmul.ac.uk
 ***************************************************************************/



#ifndef  TRIGGERSPACE
// running in Athena
#include "TrigT1CaloUtils/ICoordinate.h"
#else
// running in TRIGGERSPACE
#include "ICoordinate.h"
#endif

namespace LVL1{

ICoordinate::ICoordinate(int phi, int eta, double dEta):
	m_phi(phi), m_eta(eta), m_dEta(dEta){
	checkBounds();
}


// Destructor
ICoordinate::~ICoordinate(){
} 

/** change coords of an existing ICoordinate object*/
void ICoordinate::setCoords(int phi, int eta){
	m_phi=phi; m_eta=eta;
	checkBounds();
	return;
}

/** Ensure coords are sensible*/
void ICoordinate::checkBounds(){
	return;
}

/** return eta*/
int ICoordinate::eta() const{
	return m_eta;
}

/** return phi*/
int ICoordinate::phi() const{
	return m_phi;
}

/** return double eta*/
double ICoordinate::dEta() const{
	return m_dEta;
}

} // end of LVL1 namespace bracket
