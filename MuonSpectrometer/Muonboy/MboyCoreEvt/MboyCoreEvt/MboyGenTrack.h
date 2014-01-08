/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyGenTrack_H
#define MboyGenTrack_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

  /**
   @class MboyGenTrack

   This class is for managing a gen track data 

  @author samusog@cern.ch
  
  */

class MboyGenTrack {
public:
    MboyGenTrack();
    ~MboyGenTrack();

public:
///////////////////////////////////

   /**Set PDG Code  */
   void SetPDGCode(int    PDGCode); 
   
   /**Set Position  */
   void SetPosX   (double PosX   );
   void SetPosY   (double PosY   );
   void SetPosZ   (double PosZ   );
   
   /**Set momentum  */
   void SetPX     (double PX	 );
   void SetPY     (double PY	 ); 
   void SetPZ     (double PZ	 );
 
   /**Get PDG Code  */
   int    GetPDGCode() const ; 
   
   /**Get Position  */
   double GetPosX   () const ;
   double GetPosY   () const ;
   double GetPosZ   () const ;
   
   /**Get momentum  */
   double GetPX     () const ;
   double GetPY     () const ; 
   double GetPZ     () const ;
 

   /**Print the data */
   void Print(std::ostream* out) const;

private:
///////////////////////////////////
 int    m_PDGCode ; //!< PDG Code
 
 double m_PosX    ; //!< position
 double m_PosY    ; //!< position
 double m_PosZ    ; //!< position
 
 double m_PX      ; //!< momentum
 double m_PY      ; //!< momentum 
 double m_PZ      ; //!< momentum

};

#endif
