// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File:         RegSelRoI.h  
 **
 **   Description:  a useful RoI descriptor that can define an RoI with   
 **                 arbitrary extent in z and phi, and with independent  
 **                 RoI slopes at either end in z  
 ** 
 **   Author:       M.Sutton  
 **
 **   Created:      Thu Feb 15 00:15:26 GMT 2007
 **   Modified:     
 **                   
 **                   
 **
 **************************************************************************/ 


#ifndef REGSELROI_H
#define REGSELROI_H

#include <iostream>

#include <cmath>

class RegSelRoI { 

public:

  RegSelRoI() { } 

  RegSelRoI(double zMin,   double zMax, 
	    double phiMin, double phiMax, 
	    double etaMin, double etaMax);

  ~RegSelRoI() = default;

  double getzMin()   const { return m_zMin; } 
  double getzMax()   const { return m_zMax; } 
  double getphiMin() const { return m_phiMin; } 
  double getphiMax() const { return m_phiMax; } 
  double getetaMin() const { return m_etaMin; } 
  double getetaMax() const { return m_etaMax; } 
  double getaMin()   const { return m_aMin; } 
  double getaMax()   const { return m_aMax; } 
  bool   getsplit()  const { return m_split; } 

  double getphiWidth() const { return ( m_phiMax>=m_phiMin ? m_phiMax-m_phiMin : m_phiMax-m_phiMin+2*M_PI ); } 
  double getzWidth()   const { return m_zMax-m_zMin; } 

  // given a radius calculate the min and max z values of the RoI 
  double getzMin(const double r) const { return  r*m_aMin+m_zMin; } 
  double getzMax(const double r) const { return  r*m_aMax+m_zMax; } 

  // given a z position calculate the min and max r values of the RoI 
  double getrMin(const double z) const { return (z-m_zMin)*m_invaMin; } 
  double getrMax(const double z) const { return (z-m_zMax)*m_invaMax; } 

  
  /// find the min eta limits for an object fully contained 
  /// within an roi, bounded with the specifed radius
  double etaMinLimit(double r) const {
    double x = r*m_aMin-getzWidth();
    if ( x==0 ) return 0;
    double tantheta = r/x;
    double theta = std::atan(tantheta);
    if ( theta<0 ) theta += M_PI;
    return -std::log(std::tan(0.5*theta));    
  }
  
  /// find the max eta limits for an object fully contained 
  /// within an roi, bounded with the specifed radius
  double etaMaxLimit(double r) const { 
    double x = r*m_aMax+getzWidth();
    if ( x==0 ) return 0;
    double tantheta = r/x;
    double theta = std::atan(tantheta);
    if ( theta<0 ) theta += M_PI;
    return -std::log(std::tan(0.5*theta));    
  }

private:

  double m_zMin,    m_zMax;    // z positions at front and back of RoI
  double m_phiMin,  m_phiMax;  // phi at either side
  double m_etaMin,  m_etaMax;  // eta at front and back
  double m_invaMin, m_invaMax; // gradient dr/dz
  double m_aMin,    m_aMax;    // gradient dz/dr 

  bool   m_split;             // is the roi split over the pi boundary

};



std::ostream& operator<<(std::ostream& s, const RegSelRoI& r);


#endif  /* REGSELROI_H */










