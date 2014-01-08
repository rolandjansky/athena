/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MB_Segment_H
#define MB_Segment_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MbIOObjectStore.h"
#include "MboyCoreEvt/AccessorToMbIOObjectStore.h"
   
  /**
   @class MB_Segment

   This class is managing the segment data.
   
   It maintains the set of the MbIOObject objects associated to the segment in a MbIOObjectStore data menber   

  @author samusog@cern.ch
  
  */

class MB_Segment: public AccessorToMbIOObjectStore{
public:
   MB_Segment();
   MB_Segment(
      double Xpt,double Ypt,double Zpt,
      double Xvec,double Yvec,double Zvec,
      double X_dx,double Y_dx,double Z_dx,
      double X_dy,double Y_dy,double Z_dy,
      int E_Valid,
      double E_dx,double E_Axz,double E_dxAxz,
      double E_dy,double E_Ayz,double E_dyAyz,
      double QualityFactor,
      int NberOfMultiLayer,
      int TrackSharing,
      std::string StationName,
      int         ItypStationName,
      double FittedT0  ,
      double eFittedT0 ,
      MbIOObjectStore&  aMbIOObjectStore,
      bool isStrict=false 
   );
   virtual ~MB_Segment();

   /** Get Fitted T0 and error  */
   void GetFittedT0(double & FittedT0,double & eFittedT0 ) const;

   /** Get Segment Point */
   void GetPoint(double & Xpt,double & Ypt,double & Zpt) const;

   /** Get Segment Direction */
   void GetDirection(double & Xvec,double & Yvec,double & Zvec) const;

   /** Get x vector of local system */
   void Getdx(double & X_dx,double & Y_dx,double & Z_dx) const;

   /** Get y vector of local system */
   void Getdy(double & X_dy,double & Y_dy,double & Z_dy) const;

   /** Get Weigth Matrix validity index */
   int    ErrorValidity() const;

   /** Get Weigth Matrix */
   double Wij(int i, int j) const;

   /** Get Weigth Matrix validity and elements*/
   void GetErrorsComponents(
    int & E_Valid,
    double & E_dx, double & E_Axz , double & E_dxAxz,
    double & E_dy, double & E_Ayz , double & E_dyAyz) const;

   /** Get Quality Factor */
   double GetQualityFactor() const;

   /** Get Nber of Multilayers */
   int GetNberOfMultiLayer() const;

   /** Get Sharing with track status */
   int GetTrackSharing() const;

   /** Get Name  of associated station */
   std::string GetStationName() const;

   /** Get Index of associated station */
   int         GetItypStationName() const;

   /** Get Nber of precise coordinate Digis */
   int GetNberOfDigiCSCPrecise   () const;

   bool isStrict() const; //!< Losseness

private:
///////////////////////////////////
   
   double m_FittedT0  ; //!< Fitted T0
   double m_eFittedT0 ; //!< Error on Fitted T0
   
   double m_Xpt ; //!< X Coordinate of the Segment Point
   double m_Ypt ; //!< Y Coordinate of the Segment Point
   double m_Zpt ; //!< Z Coordinate of the Segment Point

   double m_Xvec ; //!< X Coordinate of the Segment direction
   double m_Yvec ; //!< Y Coordinate of the Segment direction
   double m_Zvec ; //!< Z Coordinate of the Segment direction

   double m_X_dx ; //!< X global Coordinate of x vector of local system
   double m_Y_dx ; //!< Y global Coordinate of x vector of local system
   double m_Z_dx ; //!< Z global Coordinate of x vector of local system
   double m_X_dy ; //!< X global Coordinate of y vector of local system
   double m_Y_dy ; //!< Y global Coordinate of y vector of local system
   double m_Z_dy ; //!< Z global Coordinate of y vector of local system

   int    m_E_Valid ; //!< Weigth Matrix validity index
   double m_E_dx    ; //!< Weigth Matrix xz elements
   double m_E_Axz   ; //!< Weigth Matrix xz elements
   double m_E_dxAxz ; //!< Weigth Matrix xz elements
   double m_E_dy    ; //!< Weigth Matrix yz elements
   double m_E_Ayz   ; //!< Weigth Matrix yz elements
   double m_E_dyAyz ; //!< Weigth Matrix yz elements

   double m_QualityFactor ; //!< Quality factor

   int m_KounterMultiLayer ; //!< Nber of multilayer involved

   int m_TrackSharing ; //!< Sharing with track status

   std::string m_StationName     ; //!< Name  of associated station
   int         m_ItypStationName ; //!< Index of associated station

   bool m_isStrict; //!< Losseness

};

 inline bool MB_Segment::isStrict() const {
   return m_isStrict;
 }

#endif
