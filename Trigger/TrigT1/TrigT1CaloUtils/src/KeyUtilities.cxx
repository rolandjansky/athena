/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          KeyUtilities.cpp  -  description
                             -------------------
    begin                : Tue Sep 26 2000
    email                : e.moyse@qmw.ac.uk
 ***************************************************************************/


#ifndef  TRIGGERSPACE
// running in Athena
#include "TrigT1CaloUtils/KeyUtilities.h"

#else
// running in TRIGGERSPACE
#include "KeyUtilities.h"

#endif

#include <cmath>

namespace LVL1 {

/** takes phi and eta and constructs a KeyUtilities object
which can return the key for this coord
*/
//Del by KDevelop: KeyUtilities::KeyUtilities(double phi, double eta) {
//Del by KDevelop: 	m_phi=phi;
//Del by KDevelop: 	m_eta=eta;
//Del by KDevelop: 	convertCoordsToIntegers();
//Del by KDevelop: 	//std::cout << "m_ieta : "<<m_ieta<<std::endl;
//Del by KDevelop: }

/** constructs a KeyUtilities object*/
KeyUtilities::KeyUtilities() :
  m_iphi(0),
  m_ieta(0),
  m_eta(0.0),
  m_phi(0.0),
  m_phiBin(0),
  m_etaBin(0),
  m_centralPhi(0.0),
  m_centralEta(0.0),
  m_debug(false)
{
}

/** constructs a KeyUtilities object*/
KeyUtilities::KeyUtilities(double phi, double eta) :
  m_iphi(0),
  m_ieta(0),
  m_eta(eta),
  m_phi(phi),
  m_phiBin(0),
  m_etaBin(0),
  m_centralPhi(0.0),
  m_centralEta(0.0),
  m_debug(false)
{
}

KeyUtilities::~KeyUtilities(){
}

/** returns key */
//Del by KDevelop: unsigned int KeyUtilities::key() {
//Del by KDevelop: 	calculateTriggerBin();
//Del by KDevelop: 	unsigned int temp=(m_etaBin<<6) + m_phiBin;
//Del by KDevelop: 	//int temp=m_phiBin;
//Del by KDevelop: 	return temp;
//Del by KDevelop: }

/** calculates a map key from passed phi, eta
coordinates */
unsigned int KeyUtilities::key( double phi, double eta) {
  m_eta=eta;
  m_phi=phi;
  ICoordinate* iCoord = convertCoordsToIntegers(m_phi, m_eta);
  BinAndCoord* bandc  = calculateTriggerBin(iCoord);
  if (m_debug) std::cout << "KU : key: got bandc "<<std::endl;
  m_phiBin=bandc->phiBin();
  m_etaBin=bandc->etaBin();
  m_centralPhi=bandc->coords()->phi();
  m_centralEta=bandc->coords()->eta();
  delete bandc;
  delete iCoord;
  int temp=((m_etaBin+50)<<6) + m_phiBin;

  if (m_debug) std::cout << "KU : key: returning - "<<temp<<std::endl;
 	return static_cast<unsigned int>(temp);
}

/** returns trigger tower key of passed Coordinate */
unsigned int KeyUtilities::key(const Coordinate& coord) {
  return key(coord.phi(), coord.eta());
}

/** converts the coordinates and corrects for overflows etc.
For instance, if phi is negative this routine converts to the
equivalent positive position.
\*todo tidy up a bit.
*/
ICoordinate* KeyUtilities::convertCoordsToIntegers(double phi, double eta){

	double phiBinWidth=((2*M_PI)/64.0);
	// there are a maximum of 64 cells in the phi direction
	// stretching from 0 to ~2*PI.
	if (m_debug) std::cout << "KU : (phi, eta) ("<<phi<<","<<eta<<")"<<std::endl;
	int iphi=sharpRound(phi/phiBinWidth);
	int ieta=0;

	// Now check that m_iphi is between 0 and 64.
  iphi = iphi%64;
	if (iphi<0) iphi = 64+iphi;

	// Make m_ieta symmetrical:
	// o no cell at m_ieta=0
	// o first cells at +-1
	// o last cells at about +- 50
	// this gives us a more useful numerical symmetry.

	// So to get integer value we do:
	int temp_sign=sign(eta);
	double temp_abs=eta*temp_sign;

	double temp_double=(temp_abs*10.0+1.0)*temp_sign;
  if (m_debug) std::cout << "KU : temp_abs, temp_sign, temp_double : "<<temp_abs<<","<<temp_sign<<","<<temp_double<<std::endl;
//	if (m_eta>=0.0){
//	  temp_double=m_eta*10.0+1.0;
//	}else{
//	  temp_double=m_eta*10.0-1.0;
//	}

	ieta=static_cast<int>(temp_double);
	if (m_debug) std::cout << "KU : (iphi, ieta) ("<<iphi<<","<<ieta<<")"<<std::endl;
	ICoordinate* iCoord=new ICoordinate(iphi, ieta, eta);
	return iCoord;
}


/** returns the eta coord of the centre
of the relevent trigger tower */
double LVL1::KeyUtilities::eta() const{
  return m_centralEta;
}




/** returns phi coordinate of the centre of the relevent
trigger tower. */
double LVL1::KeyUtilities::phi() const{
  return m_centralPhi;
}

/** returns -1 if temp is -ve and +1 if it is +ve. returns 0 if temp =0 */
int LVL1::KeyUtilities::sign(int temp) const{
  return ((temp >= 0) ? 1 : -1);
}

/** returns -1 if temp is -ve and +1 if it is +ve. returns 0 if temp =0 */
int LVL1::KeyUtilities::sign(double temp) const{
  return ((temp >= 0.0) ? 1 : -1);
}

void LVL1::KeyUtilities::setupThisKeyValues(){
  ICoordinate* iCoord = convertCoordsToIntegers(m_phi, m_eta);
  BinAndCoord* bandc  = calculateTriggerBin(iCoord);
  m_phiBin=bandc->phiBin();
  m_etaBin=bandc->etaBin();
  m_centralPhi=bandc->coords()->phi();
  m_centralEta=bandc->coords()->eta();
  delete bandc;
  delete iCoord;
}

/** rounds number to nearest whole number */
int LVL1::KeyUtilities::round(double a) const {
  return static_cast<int>(a<0.0?ceil(a-0.5):a>0.0?floor(a+0.5):0.0);
}

/** rounds number as follows (-1.0 to 0.0) -> -1, (0.0 to 1.0) -> 0, (1.0 to 2.0)->1 etc.*/
int LVL1::KeyUtilities::sharpRound(double a) const {
  return static_cast<int>(a<0.0?(a-1.0):a>0.0?(a):0.0);
}

int LVL1::KeyUtilities::sharpRound2(int a, int b) const {
  int temp=(abs(a)-1)/b+1;
  return temp*sign(a);
}

} // end LVL1 namespace



/** return central coords of current key value. */
LVL1::Coordinate LVL1::KeyUtilities::coord() const{
  return Coordinate(m_centralPhi,m_centralEta);
}
