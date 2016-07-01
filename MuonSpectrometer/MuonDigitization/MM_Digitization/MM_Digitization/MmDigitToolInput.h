/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_DIGITIZATION_MMDIGITTOOLINPUT_H
#define MM_DIGITIZATION_MMDIGITTOOLINPUT_H
#include "Identifier/Identifier.h"
#include "GeoPrimitives/GeoPrimitives.h"
/*-----------------------------------------------
 
Created March 2013 by Nektarios Chr. Benekos
   Karakostas Konstantinos   <Konstantinos.Karakostas@cern.ch>
   
Class to store input needed for the MM_Digitization tools:
- G4 driftradius
- position along tube
- magnetic field strength at hit position   <-- ToDo: get the lorentz angle from the mag field
- local temperature
- electric charge
- gamma factor
- hit Identifier	

-----------------------------------------------*/
/*******************************************************************************/ 
class MmDigitToolInput {
 public:
 
 MmDigitToolInput(int stripIdLocal, double posx, double incomingAngle, const Amg::Vector3D& field, int stripMaxId)   // 27/05/2015 T.Saito
   //  MmDigitToolInput(int stripIdLocal, double posx, double incomingAngle, double field, int stripMaxId)
   :  m_stripIDLocal(stripIdLocal),
      m_xpos(posx),
      m_incomingAngle(incomingAngle),
      m_field(field),
      m_stripMaxId(stripMaxId)
  { }
    
      
  ~MmDigitToolInput() {}

      int    stripIDLocal()        const { return m_stripIDLocal; }
      double positionWithinStrip() const { return m_xpos; }
      double incomingAngle()       const { return m_incomingAngle; }
      const Amg::Vector3D& magneticField()       const { return m_field; }// kT unit, local cordinate
      int    stripMaxID()          const { return m_stripMaxId; }
      Identifier getHitID()        const { return m_hitID; }
       
 private:
      int    m_stripIDLocal;
      double m_xpos; 
      double m_incomingAngle;
      Amg::Vector3D m_field;
      int    m_stripMaxId;
      Identifier m_hitID;
};
/*******************************************************************************/ 	 
#endif  
