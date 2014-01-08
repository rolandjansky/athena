/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MuonRecDigit.h"

MuonRecDigit::MuonRecDigit():MbIOObject()
{ 

 Type_PRD () ;

 m_ParticleCode = 6     ;

 m_X = 0. ;
 m_Y = 0. ;
 m_Z = 0. ;

}

MuonRecDigit::~MuonRecDigit(){}

void MuonRecDigit::PrintCout() const
{ 
 std::cout << " Dummy implementation of MuonRecDigit::PrintCout " << std::endl;
}

// Set All

void MuonRecDigit::SetX            ( const double& X         ) { m_X            = X            ; }
void MuonRecDigit::SetY            ( const double& Y         ) { m_Y            = Y            ; }
void MuonRecDigit::SetZ            ( const double& Z         ) { m_Z            = Z            ; }
void MuonRecDigit::SetParticleCode ( const int& ParticleCode ) { m_ParticleCode = ParticleCode ; }

// Get All

double MuonRecDigit::GetX            () const { return m_X            ; }
double MuonRecDigit::GetY            () const { return m_Y            ; }
double MuonRecDigit::GetZ            () const { return m_Z            ; }
int    MuonRecDigit::GetParticleCode () const { return m_ParticleCode ;}
