/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModel/PixelSwitches.h"

PixelSwitches::PixelSwitches()
  : m_services(true),
    m_servicesOnLadder(true), //JBdV
    m_g3CompatibleDigits(false),
    m_initialLayout(false),
    m_dc1Geometry(false),
    m_alignable(true),
    m_slhc(false),
    m_ibl(false),
    m_dbm(false)
{}

void PixelSwitches::setServices(bool flag) {m_services = flag;}
bool PixelSwitches::services() const {return m_services;}

void PixelSwitches::setServicesOnLadder(bool flag) {m_servicesOnLadder = flag;}
bool PixelSwitches::servicesOnLadder() const {return m_servicesOnLadder;}

void PixelSwitches::setG3CompatibleDigits(bool flag) {m_g3CompatibleDigits = flag;} 
bool PixelSwitches::g3CompatibleDigits() const {return m_g3CompatibleDigits;}
  

void PixelSwitches::setInitialLayout(bool flag) {m_initialLayout = flag;}
bool PixelSwitches::initialLayout() const {return m_initialLayout;}
  

void PixelSwitches::setDC1Geometry(bool flag) {m_dc1Geometry = flag;}
bool PixelSwitches::dc1Geometry() const {return m_dc1Geometry;}
  

void PixelSwitches::setAlignable(bool flag) {m_alignable = flag;}
bool PixelSwitches::alignable() const {return m_alignable;}

void PixelSwitches::setSLHC(bool flag) {m_slhc = flag;}
bool PixelSwitches::slhc() const {return m_slhc;}

void PixelSwitches::setIBL(bool flag) {m_ibl = flag;}
bool PixelSwitches::ibl() const {return m_ibl;}
    
void PixelSwitches::setDBM(bool flag) {m_dbm = flag;} //switch to build the DBM
bool PixelSwitches::dbm() const {return m_dbm;}

