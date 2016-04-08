/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRANSVERSEENERGY_H
#define TRANSVERSEENERGY_H

#include <cmath>
#include <TMath.h>

class TransverseEnergy
{
 public:

  /** default constructor */
  TransverseEnergy();

  /** destructor */
  ~TransverseEnergy();

  /** constructor */
  TransverseEnergy( double etx, double ety , double etsum = -999. );

  /** copy constructor */
  TransverseEnergy( const TransverseEnergy& transEn );

  /** assignment operator */
  TransverseEnergy &operator=(TransverseEnergy const &transEn)
    {
      if(this != &transEn)
	{
	  this->TransverseEnergy::~TransverseEnergy();
	  
	  new(this) TransverseEnergy(transEn);
	}

	return *this;
    };
  
  /** add metod */
  void Add( double etx, double ety, double etsum, double scaleFactor=1. );

  /** add metod */
  void Add( double etx, double ety );

  /** add method */
  void Add( const TransverseEnergy& et , double scaleFactor=1.);

  void SetEtX(double etx)    {m_etx = etx;};
  void SetEtY(double ety)    {m_ety = ety;};
  void SetEtSum(double etsum){m_etsum = etsum;};

  /** returns the x component of the transverse energy */
  double etx()   const { return m_etx; };

  /** returns the y component of the transverse energy */
  double ety()   const { return m_ety; };

  /** returns the transverse energy */
  double et()    const { return m_et;  };

  /** returns the direction of the transverse energy */
  double phi()   const { return m_phi; };

  /** returns the scalar sum of the transverse energy */
  double etsum() const { return m_etsum; };

 private:
  /** sets all the member variables in a coherent way.
   It calculates and set (et, phi) starting from (etx, ety) */
  void SetEtPhi();
  
  //protected:
  /** transverse energy (x component) */
  double m_etx;
  
  /** transverse energy (y component) */
  double m_ety;

  /** transverse energy */
  double m_et;

  /** direction of the transverse energy */
  double m_phi;

 /** scalar sum of the transverse energy */
  double m_etsum;

};

#endif //TRANSVERSEENERGY_H

