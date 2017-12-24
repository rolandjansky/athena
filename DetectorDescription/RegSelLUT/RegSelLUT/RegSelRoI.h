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

  ~RegSelRoI() { } 

  double getzMin()   const { return mzMin; } 
  double getzMax()   const { return mzMax; } 
  double getphiMin() const { return mphiMin; } 
  double getphiMax() const { return mphiMax; } 
  double getetaMin() const { return metaMin; } 
  double getetaMax() const { return metaMax; } 
  double getaMin()   const { return maMin; } 
  double getaMax()   const { return maMax; } 
  bool   getsplit()  const { return msplit; } 

  double getphiWidth() const { return ( mphiMax>=mphiMin ? mphiMax-mphiMin : mphiMax-mphiMin+2*M_PI ); } 
  double getzWidth()   const { return mzMax-mzMin; } 

  // given a radius calculate the min and max z values of the RoI 
  double getzMin(const double r) const { return  r*maMin+mzMin; } 
  double getzMax(const double r) const { return  r*maMax+mzMax; } 

  // given a z position calculate the min and max r values of the RoI 
  double getrMin(const double z) const { return (z-mzMin)*minvaMin; } 
  double getrMax(const double z) const { return (z-mzMax)*minvaMax; } 

  
  /// find the min eta limits for an object fully contained 
  /// within an roi, bounded with the specifed radius
  double etaMinLimit(double r) const {
    double x = r*maMin-getzWidth();
    if ( x==0 ) return 0;
    double tantheta = r/x;
    double theta = std::atan(tantheta);
    if ( theta<0 ) theta += M_PI;
    return -std::log(std::tan(0.5*theta));    
  }
  
  /// find the max eta limits for an object fully contained 
  /// within an roi, bounded with the specifed radius
  double etaMaxLimit(double r) const { 
    double x = r*maMax+getzWidth();
    if ( x==0 ) return 0;
    double tantheta = r/x;
    double theta = std::atan(tantheta);
    if ( theta<0 ) theta += M_PI;
    return -std::log(std::tan(0.5*theta));    
  }

private:

  double mzMin,    mzMax;    // z positions at front and back of RoI
  double mphiMin,  mphiMax;  // phi at either side
  double metaMin,  metaMax;  // eta at front and back
  double minvaMin, minvaMax; // gradient dr/dz
  double maMin,    maMax;    // gradient dz/dr 

  bool   msplit;             // is the roi split over the pi boundary

};



std::ostream& operator<<(std::ostream& s, const RegSelRoI& r);


#endif  /* REGSELROI_H */










