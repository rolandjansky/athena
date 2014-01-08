/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MbPOT_H
#define MbPOT_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MbIOObject.h"



  /**
   @class MbPOT

   This class is an absract class for managing a digit-rec data

  @author samusog@cern.ch
  
  */

class MbPOT: public MbIOObject{
public:
    MbPOT();
    virtual ~MbPOT();
    MbPOT( const MbPOT& aMbPOT );

public:
///////////////////////////////////

   /**Set XYZ coordinate of point at device level */
   void SetXdca (double Xdca ) ;
   void SetYdca (double Ydca ) ;
   void SetZdca (double Zdca ) ;

   /**Set XYZ coordinate of direction near device  */
   void SetXdir (double Xdir ) ;
   void SetYdir (double Ydir ) ;
   void SetZdir (double Zdir ) ;

   /**Get XYZ coordinate of point at device level */
   double GetXdca()  const;
   double GetYdca()  const;
   double GetZdca()  const;
   
   /**Get XYZ coordinate of direction at device level */
   double GetXdir()  const;
   double GetYdir()  const;
   double GetZdir()  const;
   
   /**Get XYZ coordinate of point near device  */
   double GetXdcb()  const;
   double GetYdcb()  const;
   double GetZdcb()  const;
   
   /**Set Energy at device level */
   void SetEner (double Ener ) ;

   /**Get Energy at device level */
   double GetEner()  const;

   /**Set S coordinate of point at device level */
   void SetSdca (double Sdca ) ;

   /**Get S coordinate of point at device level */
   double GetSdca()  const;
   
   /**Set Reso at device level */
   void SetReso (double Reso ) ;

   /**Get Reso at device level */
   double GetReso()  const;
      
   /** Set Reversal flag */
   void SetIsToBeReversed(int IsToBeReversed ) ;

   /** Get Reversal flag */
   int IsToBeReversed() const;

      
      
protected:
///////////////////////////////////
 double m_Xdca  ; //!< X coordinate of point at device level
 double m_Ydca  ; //!< Y coordinate of point at device level
 double m_Zdca  ; //!< Z coordinate of point at device level
 double m_Xdir  ; //!< X coordinate of direction at device level
 double m_Ydir  ; //!< Y coordinate of direction at device level
 double m_Zdir  ; //!< Z coordinate of direction at device level
 double m_Ener  ; //!< Energy at device level

 double m_Sdca  ; //!< S coordinate of point at device level
 double m_Reso  ; //!< Reso at device level

 int m_IsToBeReversed ; //!< Tell if the track is to be reversed (cosmics in upper part)

};

#endif
