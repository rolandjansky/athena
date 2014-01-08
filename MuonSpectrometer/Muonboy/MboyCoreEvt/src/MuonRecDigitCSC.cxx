/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MuonRecDigitCSC.h"

MuonRecDigitCSC::MuonRecDigitCSC():MuonRecDigit(),MbIDTagCSC()
{
 
 Tech_CSC   () ;

 m_DepositedCharge = 0. ;

}
MuonRecDigitCSC::~MuonRecDigitCSC(){}

void MuonRecDigitCSC::Print(std::ostream* out) const{PrintAmdIds(out);}
void MuonRecDigitCSC::PrintAmdIds(std::ostream* out) const{

    *out
                    << "CSC "
                    << "  "
                    << std::setw(3)<< m_StationName
                    << std::setw(4)<< m_JFF
                    << std::setw(4)<< m_JZZ
                    << std::setw(4)<< m_JOB
                    << std::setw(4)<< m_JSL
                    << std::setw(4)<< m_JSTRI
                    << "        "
                    << " " <<std::setw(10)<<std::setprecision(3)<< m_X
                    << " " <<std::setw(10)<<std::setprecision(3)<< m_Y
                    << " " <<std::setw(10)<<std::setprecision(3)<< m_Z
                    << " " <<std::setw(10)<<std::setprecision(3)<< m_DepositedCharge
                    << " " <<std::setw(10)<<std::setprecision(3)<< m_SZFLAG
                    << std::setw(5)<< m_ParticleCode;

}
void MuonRecDigitCSC::PrintAthIds(std::ostream* out) const{

    *out
                    << "CSC "
                    << "  "
                    << std::setw(3)<< m_StationName
                    << std::setw(4)<< m_SGStationEta  
                    << std::setw(4)<< m_SGStationPhi  
                    << std::setw(4)<< m_SGChamberLayer
                    << std::setw(4)<< m_SGWireLayer   
                    << std::setw(4)<< m_SGMeasuresPhi 
                    << std::setw(4)<< m_SGStrip       
                    << "        "
                    << " " <<std::setw(10)<<std::setprecision(3)<< m_X
                    << " " <<std::setw(10)<<std::setprecision(3)<< m_Y
                    << " " <<std::setw(10)<<std::setprecision(3)<< m_Z
                    << " " <<std::setw(10)<<std::setprecision(3)<< m_DepositedCharge
                    << std::setw(5)<< m_ParticleCode;

}

// Set All

void MuonRecDigitCSC::SetDepositedCharge( const double& DepositedCharge ) { m_DepositedCharge = DepositedCharge ; }

// Get All

double MuonRecDigitCSC::GetDepositedCharge () const { return m_DepositedCharge ; }
