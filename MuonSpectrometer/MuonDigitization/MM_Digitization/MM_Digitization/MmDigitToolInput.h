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
- magnetic field strength at hit position
- local temperature
- electric charge
- gamma factor
- hit Identifier

-----------------------------------------------*/
/*******************************************************************************/
class MmDigitToolInput {
 public:

 MmDigitToolInput(int stripIdLocal, double posx, double incomingAngleXZ, double incomingAngleYZ, const Amg::Vector3D& magneticField, int stripMaxId, int gasgap, float eventTime)

   :  m_stripIDLocal(stripIdLocal),
      m_xpos(posx),
      m_incomingAngleXZ(incomingAngleXZ),
      m_incomingAngleYZ(incomingAngleYZ),
      m_magneticField(magneticField),
      m_stripMaxId(stripMaxId),
      m_gasgap(gasgap),
      m_eventTime(eventTime)
  { }


  ~MmDigitToolInput() {}

      int    stripIDLocal()        const { return m_stripIDLocal; }
      double positionWithinStrip() const { return m_xpos; }
      double incomingAngleXZ()       const { return m_incomingAngleXZ; }
      double incomingAngleYZ()       const { return m_incomingAngleYZ; }
      const Amg::Vector3D& magneticField()       const { return m_magneticField; } // kT unit, local cordinate
      int    stripMaxID()          const { return m_stripMaxId; }
      int    gasgap()              const { return m_gasgap; }
      Identifier getHitID()        const { return m_hitID; }
      float  eventTime()           const { return m_eventTime; }

 private:
      int    m_stripIDLocal;
      double m_xpos;
      double m_incomingAngleXZ;
      double m_incomingAngleYZ;
      Amg::Vector3D m_magneticField;
      int    m_stripMaxId;
      int    m_gasgap;
      Identifier m_hitID;
      float m_eventTime;
};
/*******************************************************************************/
#endif
