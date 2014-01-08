/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MB_Track_H
#define MB_Track_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MB_Track_SpecEntr.h"
#include "MboyCoreEvt/MB_Track_CaloEntr.h"
#include "MboyCoreEvt/MB_Track_Perigee.h"
#include "MboyCoreEvt/MbIOObjectStore.h"
#include "MboyCoreEvt/AccessorToMbIOObjectStore.h"

  /**
   @class MB_Track

   This class is the class to manage track data at different location

   It maintains the set of the MbIOObject objects associated to the track in a MbIOObjectStore data menber

  @author samusog@cern.ch
  
  */

class MB_Track: public AccessorToMbIOObjectStore{
public:
   MB_Track();
   MB_Track(const MB_Track_SpecEntr& SpectrometerEntrance,
            const MB_Track_CaloEntr& CalorimeterEntrance,
            const MB_Track_Perigee&  Perigee,
	                 double QualityFactor ,
                         int    NberOfDOF ,
                         double Chi2Typ0 ,
                         double Chi2Typ1 ,
                         double Chi2Typ2 ,
			 MbIOObjectStore&  aMbIOObjectStore
   );
   virtual ~MB_Track();

public:
///////////////////////////////////

   /** Get Reversal flag */
   int IsToBeReversed() const;

   /** Get StraightLine flag */
   int IsStraightLine() const;

   /**Get Tracks validities*/
   int IsValidMS() const;
   int IsValidKA() const;
   int IsValidVT() const;  
   
   /**Get Tracks at Spectro entrance*/
   MB_Track_SpecEntr  GetTrackAtSpectrometerEntrance() const;
   const MB_Track_SpecEntr*  pGetTrackAtSpectrometerEntrance() const;

   /**Get Tracks at calo entrance*/
   MB_Track_CaloEntr  GetTrackAtCalorimeterEntrance() const;
   const MB_Track_CaloEntr*  pGetTrackAtCalorimeterEntrance() const;

   /**Get Tracks at perigee*/
   MB_Track_Perigee   GetTrackAtPerigee() const;
   const MB_Track_Perigee*  pGetTrackAtPerigee() const;

   /**Get Quality Factor*/
   double GetQualityFactor() const;

   /**Get the Nber of d.o.f*/
   int GetNberOfDOF() const;

   /**Get the Chi2*/
   double GetChi2(int FitKind) const;

   /**Set MS and Eloss points*/
   void Setenmsxx(double enmsxx_i) ;
   
   /**Get MS and Eloss points*/
   double Getenmsxx()              const ;
   
private:
///////////////////////////////////
//Data

   MB_Track_SpecEntr m_SpectrometerEntrance ; //!< track at Spectrometer Entrance
   MB_Track_CaloEntr m_CalorimeterEntrance  ; //!< track at Calorimeter Entrance 
   MB_Track_Perigee  m_Perigee              ; //!< track at Perigee             

   double m_QualityFactor ; //!< Quality factor
   int    m_NberOfDOF     ; //!< Nber of d.o.f
   double m_Chi2Typ0      ; //!< Chi2
   double m_Chi2Typ1      ; //!< Chi2 
   double m_Chi2Typ2      ; //!< Chi2 

   double               m_enmsxx     ; //!< MS and Eloss point data 

};
#endif
