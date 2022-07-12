/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_DIGITIZATION_MM_DIGITTOOLINPUT_H
#define MM_DIGITIZATION_MM_DIGITTOOLINPUT_H
#include "GeoPrimitives/GeoPrimitives.h"
#include "Identifier/Identifier.h"
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
class MM_DigitToolInput {
public:
    MM_DigitToolInput(int stripIdLocal, double posx, double incomingAngleXZ, double incomingAngleYZ, const Amg::Vector3D& magneticField,
                      int stripMinId, int stripMaxId, int gasgap, float eventTime)

        :
        m_stripIDLocal(stripIdLocal),
        m_xpos(posx),
        m_incomingAngleXZ(incomingAngleXZ),
        m_incomingAngleYZ(incomingAngleYZ),
        m_magneticField(magneticField),
        m_stripMinId(stripMinId),
        m_stripMaxId(stripMaxId),
        m_gasgap(gasgap),
        m_eventTime(eventTime) {}

    ~MM_DigitToolInput() = default;
    int stripIDLocal() const { return m_stripIDLocal; }
    double positionWithinStrip() const { return m_xpos; }
    double incomingAngleXZ() const { return m_incomingAngleXZ; }
    double incomingAngleYZ() const { return m_incomingAngleYZ; }
    const Amg::Vector3D& magneticField() const { return m_magneticField; }  // kT unit, local cordinate
    int stripMinID() const { return m_stripMinId; }
    int stripMaxID() const { return m_stripMaxId; }
    int gasgap() const { return m_gasgap; }
    Identifier getHitID() const { return m_hitID; }
    float eventTime() const { return m_eventTime; }

private:
    int m_stripIDLocal{0};
    double m_xpos{0.};
    double m_incomingAngleXZ{0.};
    double m_incomingAngleYZ{0.};
    Amg::Vector3D m_magneticField{0., 0., 0.};
    int m_stripMinId{0};
    int m_stripMaxId{0};
    int m_gasgap{0};
    Identifier m_hitID{0};
    float m_eventTime{0.f};
};
/*******************************************************************************/
#endif
