/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MB_Track_Base_H
#define MB_Track_Base_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

  /**
   @class MB_Track_Base

   This class is the base class to manage track data

  @author samusog@cern.ch
  
  */

class MB_Track_Base{
public:
   MB_Track_Base();
  ~MB_Track_Base();

   /** Set track parameters */
   void SetTrack(
      double P1,double P2,double P3,double P4,double P5,
      double Cov11, double Cov21, double Cov22,
      double Cov31, double Cov32, double Cov33,
      double Cov41, double Cov42, double Cov43,
      double Cov44, double Cov51, double Cov52,
      double Cov53, double Cov54, double Cov55,
      double XPointOnTrack ,double YPointOnTrack ,double ZPointOnTrack,
      double UXPointOnTrack ,double UYPointOnTrack ,double UZPointOnTrack,
      double Momentum
   );

   /** Get covariance matrix elements */
   double Covij(int i, int j) const;

   /** Get validity flag */
   int IsValid() const;

   /** Set and Get  Reversed flag */
   void SetIsToBeReversed(int IsToBeReversed ) ;
   int IsToBeReversed() const;

   /** Set and Get  StraightLine flag */
   void SetIsStraightLine(int IsStraightLine ) ;
   int IsStraightLine() const;

   /**Get Covariance Matrix elements */
   double Cov11() const; 
   double Cov21() const;
   double Cov22() const;
   double Cov31() const;
   double Cov32() const;
   double Cov33() const;
   double Cov41() const;
   double Cov42() const;
   double Cov43() const;
   double Cov44() const;
   double Cov51() const;
   double Cov52() const;
   double Cov53() const;
   double Cov54() const;
   double Cov55() const;
   
   /**Get X Coordinate of the Crossing Point */
   double XCrossingPoint() const; 

   /**Get Y Coordinate of the Crossing Point */
   double YCrossingPoint() const;

   /**Get Z Coordinate of the Crossing Point */
   double ZCrossingPoint() const;
   
   /**Get X Coordinate of the track Unit vector at the Crossing Point */
   double XUnitVector() const; 

   /**Get Y Coordinate of the track Unit vector at the Crossing Point */
   double YUnitVector() const;

   /**Get Z Coordinate of the track Unit vector at the Crossing Point */
   double ZUnitVector() const;
   
   /**Get Momentum at the Crossing Point */
   double Momentum() const;
   
protected:
///////////////////////////////////
//Data

   int m_IsValid ; //!< Tell if the track is valid

   int m_IsToBeReversed ; //!< Tell if the track is to be reversed (cosmics in upper part)
   int m_IsStraightLine ; //!< Tell if the track is a StraightLine

   double m_P1 ; //!< Track parameter
   double m_P2 ; //!< Track parameter
   double m_P3 ; //!< Track parameter
   double m_P4 ; //!< Track parameter
   double m_P5 ; //!< Track parameter

   double m_Cov11 ; //!<cov. matrix element 
   double m_Cov21 ; //!<cov. matrix element  
   double m_Cov22 ; //!<cov. matrix element
   double m_Cov31 ; //!<cov. matrix element  
   double m_Cov32 ; //!<cov. matrix element  
   double m_Cov33 ; //!<cov. matrix element
   double m_Cov41 ; //!<cov. matrix element  
   double m_Cov42 ; //!<cov. matrix element  
   double m_Cov43 ; //!<cov. matrix element
   double m_Cov44 ; //!<cov. matrix element  
   double m_Cov51 ; //!<cov. matrix element  
   double m_Cov52 ; //!<cov. matrix element
   double m_Cov53 ; //!<cov. matrix element  
   double m_Cov54 ; //!<cov. matrix element  
   double m_Cov55 ; //!<cov. matrix element

   double m_XPointOnTrack ; //!< X Coordinate of the track Point
   double m_YPointOnTrack ; //!< Y Coordinate of the track Point
   double m_ZPointOnTrack ; //!< Z Coordinate of the track Point

   double m_UXPointOnTrack ; //!< X Coordinate of the track Unit vector
   double m_UYPointOnTrack ; //!< Y Coordinate of the track Unit vector
   double m_UZPointOnTrack ; //!< Z Coordinate of the track Unit vector

   double m_Momentum  ; //!< Momentum at the Point on the track

};
#endif
