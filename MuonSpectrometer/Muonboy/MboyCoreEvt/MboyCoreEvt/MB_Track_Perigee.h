/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MB_Track_Perigee_H
#define MB_Track_Perigee_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MB_Track_Base.h"

  /**
   @class MB_Track_Perigee

   This class is the class to manage track data at perigee

  @author samusog@cern.ch
  
  */

class MB_Track_Perigee: public MB_Track_Base {
public:
   MB_Track_Perigee();
  ~MB_Track_Perigee();

   void SetTrackPerigee(
      double ImpactParameterAtPerigee,
       double ZAtPerigee,
        double PhiAtPerigee,
	 double CotgAtPerigee,
	  double InverseTransverseMomentum,
      double Cov11, double Cov21, double Cov22,
      double Cov31, double Cov32, double Cov33,
      double Cov41, double Cov42, double Cov43,
      double Cov44, double Cov51, double Cov52,
      double Cov53, double Cov54, double Cov55,
      double XPerigeePoint ,double YPerigeePoint ,double ZPerigeePoint,
      double UXUnitVector ,double UYUnitVector ,double UZUnitVector,
      double Momentum
   );

public:
///////////////////////////////////

   /**Get Tracks Parameters */
   double ImpactParameterAtPerigee()  const ; 
   double ZAtPerigee()                const ;
   double PhiAtPerigee()              const ;
   double CotgAtPerigee()             const ;
   double InverseTransverseMomentum() const ;

   /**Get X Coordinate of the Point on track at the Perigee */
   double XPerigeePoint() const ; 

   /**Get Y Coordinate of the Point on track at the Perigee */
   double YPerigeePoint() const ;

   /**Get Z Coordinate of the Point on track at the Perigee */
   double ZPerigeePoint() const ;
   
};
#endif
