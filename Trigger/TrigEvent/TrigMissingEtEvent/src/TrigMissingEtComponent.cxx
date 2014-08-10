/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     TrigMissingEtComponent.cxx
// PACKAGE:  Trigger/TrigEvent/TrigMissingEtEvent
// 
// AUTHOR:   Diego Casadei
// CREATED:  April, 2008
// 
// Description: MET trigger slice components (auxiliary information).
//
// ********************************************************************

#include "TrigMissingEtEvent/TrigMissingEtComponent.h"
#include <math.h>
#include <stdio.h>

TrigMissingEtComponent::TrigMissingEtComponent(){
  m_name = "empty";
  reset();
}

TrigMissingEtComponent::~TrigMissingEtComponent(){
}


void TrigMissingEtComponent::reset(){
  m_status = m_usedChannels = m_sumOfSigns = 0; 
  m_calib0 = 0;
  m_calib1 = 1; 
  m_ex = m_ey = m_ez = m_sumEt = m_sumE = 0;
}


std::string TrigMissingEtComponent::getFormattedHeader(){
  std::string s="_name__status_usedChannels__sumOfSigns__calib1_calib0/MeV";
  s+="___ex/MeV_____ey/MeV_____ez/MeV___sumE/MeV__sumEt/MeV";
  return s;
}


std::string TrigMissingEtComponent::getFormattedValues(){
  char buff[256];
  sprintf(buff,
	       "%s %7d %12d %11d %7.3f %7.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n", 
	      m_name.c_str(),
	      m_status, m_usedChannels, m_sumOfSigns, m_calib1, m_calib0,
	      m_ex, m_ey, m_ez, m_sumEt, m_sumE);
  return std::string(buff);
}


bool operator== ( const TrigMissingEtComponent& a, const TrigMissingEtComponent& b ) {

   const double DELTA=1e-3; // distance used to compare floats

   if( a.name() != b.name() )
      return false;
   if( a.status() != b.status() )
      return false;
   if( a.sumOfSigns() != b.sumOfSigns() )
      return false;
   if( a.usedChannels() != b.usedChannels() )
      return false;

   if( fabsf(a.ex() - b.ex()) > DELTA )
      return false;
   if( fabsf(a.ey() - b.ey()) > DELTA )
      return false;
   if( fabsf(a.ez() - b.ez()) > DELTA )
      return false;
   if( fabsf(a.sumEt() - b.sumEt()) > DELTA )
      return false;
   if( fabsf(a.sumE() - b.sumE()) > DELTA )
      return false;
   if( fabsf(a.calib0() - b.calib0()) > DELTA )
      return false;
   if( fabsf(a.calib1() - b.calib1()) > DELTA )
      return false;

   return true;
}
