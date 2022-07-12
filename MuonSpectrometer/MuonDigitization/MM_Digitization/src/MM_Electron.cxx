/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MM_Digitization/MM_Electron.h"

MM_Electron::MM_Electron(float x, float y) : m_initialPosition(x, y) {}

void MM_Electron::setOffsetPosition(float x, float y) { m_offsetPosition.Set(x, y); }

void MM_Electron::propagateElectron(float lorentzAngle, float driftVel) {
    float tanLorentzAngle = std::tan(lorentzAngle);
    if (tanLorentzAngle == tanLorentzAngle)  // checking that it's not NAN
        m_offsetPosition.Set(m_offsetPosition.X() + tanLorentzAngle * (m_offsetPosition.Y() + m_initialPosition.Y()),
                             m_offsetPosition.Y() + m_initialPosition.Y());

    if (driftVel > 0.)
        m_time = m_offsetPosition.Mod() / driftVel;
    else
        m_time = -1.;
}

void MM_Electron::setTime(float Time) { m_time = Time; }
void MM_Electron::setCharge(float Charge) { m_charge = Charge; }

TVector2 MM_Electron::getOffsetPosition() const { return m_offsetPosition; }
float MM_Electron::getCharge() const { return m_charge; }
float MM_Electron::getTime() const { return m_time; }
float MM_Electron::getX() const { return m_offsetPosition.X() + m_initialPosition.X(); }
float MM_Electron::getY() const { return m_offsetPosition.Y() + m_initialPosition.Y(); }
float MM_Electron::getInitialX() const { return m_initialPosition.X(); }
float MM_Electron::getInitialY() const { return m_initialPosition.Y(); }
