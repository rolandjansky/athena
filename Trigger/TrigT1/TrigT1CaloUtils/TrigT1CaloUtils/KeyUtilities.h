/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          KeyUtilities.h  -  description
                             -------------------
    begin                : Monday August 20 2001
    email                : e.moyse@qmw.ac.uk
 ***************************************************************************/


#ifndef KeyUtilities_H
#define KeyUtilities_H

#ifndef  TRIGGERSPACE
// running in Athena
#include <iostream>
//#include "GaudiKernel/MsgStream.h"
#include <math.h>
#include "TrigT1CaloUtils/ICoordinate.h"
#include "TrigT1CaloUtils/BinAndCoord.h"
#include "TrigT1Interfaces/Coordinate.h"


#else
// running in TRIGGERSPACE
#include <iostream>
#include <math.h>
#include "ICoordinate.h"
#include "BinAndCoord.h"
#include "Coordinate.h"

#endif




namespace LVL1 {

/**
  The KeyUtilities object provides the key for each trigger tower
  depending on its eta,phi coords <p>
  The key is an integer number that uniquely
  identifies each tower, and is needed by the STL map container: the map holds
  trigger towers and we retrieve them using their keys.
  *@author Edward Moyse
  */
class KeyUtilities {
public:
  virtual ~KeyUtilities();
  /** returns the centre of the TT at eta_coord:*/
  virtual double eta() const;
  /** returns the width at eta_coord:*/
  virtual double dEta(const Coordinate& coord) const=0;
  /** returns phi coordinate of centre of relevant
	trigger tower. */
  virtual double phi() const;
  /** returns the height at eta_coord:*/
  virtual double dPhi(const Coordinate& coord) const=0;
  /** calculates a map key from passed phi, eta
  coordinates */
  unsigned int key( double phi, double eta) ;
   /** returns the key of the passed Coordinate */
  unsigned int key(const Coordinate& coord) ;
  /** return central coords of current key value. */
  Coordinate coord() const;
 
protected: // Protected methods
  /** the constructor is protected so a user can never make a KeyUtilities object*/
  KeyUtilities();
  /** constructs a KeyUtilities object*/
  KeyUtilities(double phi, double eta) ;

  /** converts the coordinates and corrects for overflows etc.
  For instance, if phi is negative this routine converts to the
  equivalent positive position. */
  ICoordinate* convertCoordsToIntegers(double phi, double eta);
  /** converts integer phi, eta 
  coordinates to phi, eta trigger bins. */
  virtual BinAndCoord* calculateTriggerBin(ICoordinate* iCoord)=0;
  /** returns -1 if temp is -ve and +1 if it is +ve. returns 0 if temp =0 */
  virtual int sign(int temp) const;
  /** returns -1 if temp is -ve and +1 if it is +ve. returns 0 if temp =0 */
  virtual int sign(double temp) const;
  /** rounds number to nearest whole number */
  int round(double a) const;
  /** rounds number as follows (-1.0 to 0.0) -> -1, (0.0 to 1.0) -> 0, (1.0 to 2.0)->1 etc.*/
  int sharpRound(double a) const ;
   /** divides a/b and returns a number as follows (where +b means +ve b):
   if 0<a<+b, then n=1, if +b<a<+2b, n=2, if -b<a<0, n=-1, if -2b<a<-b, n=-2*/
  int sharpRound2(int a, int b) const ;
  void setupThisKeyValues();

protected:
  /** integer phi coord*/
  int m_iphi;
  /** integer eta coord */
  int m_ieta;
  /** eta coordinate of key */
  double m_eta;
  /** phi coordinate of key */
  double m_phi;
  /**  phi bin*/
  int m_phiBin;
  /**  eta bin*/
  int m_etaBin;
  /** this is the coordinate of the centre of the
	relevant trigger tower. */
  double m_centralPhi;
  /** this is the coordinate of the centre of the
	relevant trigger tower. */
  double m_centralEta;
	/** turns DEBUG code on and off */
	bool m_debug;
};
}
#endif
