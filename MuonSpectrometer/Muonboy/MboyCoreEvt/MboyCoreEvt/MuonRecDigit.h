/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonRecDigit_H
#define MuonRecDigit_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MbIOObject.h"


  /**
   @class MuonRecDigit

   This class is an absract class for managing a digit data

  @author samusog@cern.ch
  
  */

class MuonRecDigit: public MbIOObject{
public:
    MuonRecDigit();
    virtual ~MuonRecDigit();

public:
///////////////////////////////////

   /**Print the data */
   virtual void PrintCout()               const ;
   
   /**Set X coordinate associate to digit */
   void SetX (const double& X );

   /**Set Y coordinate associate to digit */
   void SetY (const double& Y );

   /**Set Z coordinate associate to digit */
   void SetZ (const double& Z );
   
   /**Get X coordinate associate to digit */
   double GetX () const ;

   /**Get Y coordinate associate to digit */
   double GetY () const ;

   /**Get Z coordinate associate to digit */
   double GetZ () const ;

   /**Get Particle Code */
   void SetParticleCode (const int& ParticleCode );

   /**Get Particle Code */
   int  GetParticleCode () const ;


protected:
///////////////////////////////////

 double m_X               ; //!< X coordinate associate to digit
 double m_Y               ; //!< Y coordinate associate to digit
 double m_Z               ; //!< Z coordinate associate to digit

 int    m_ParticleCode    ; //!< Particle code

};

#endif
