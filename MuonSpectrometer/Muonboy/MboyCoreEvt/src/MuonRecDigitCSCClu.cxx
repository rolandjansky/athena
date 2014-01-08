/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MuonRecDigitCSCClu.h"

MuonRecDigitCSCClu::MuonRecDigitCSCClu():MuonRecDigit(),MbIDTagCSCclu()
{

 Tech_CSCclu() ;

 m_Error           = 0. ;
 m_DepositedCharge = 0. ;
 m_Time            = 0. ;
 m_TimeStatus      = 0  ;
 m_Status          = 0  ;
 
}
MuonRecDigitCSCClu::~MuonRecDigitCSCClu(){}

void MuonRecDigitCSCClu::Print(std::ostream* out) const{PrintAmdIds(out);}
void MuonRecDigitCSCClu::PrintAmdIds(std::ostream* out) const{

    *out
                    << "CLU "
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
                    << " " <<std::setw(10)<<std::setprecision(3)<< m_Error
                    << " " <<std::setw(10)<<std::setprecision(3)<< m_SZFLAG
                    << std::setw(5)<< m_ParticleCode;

}
void MuonRecDigitCSCClu::PrintAthIds(std::ostream* out) const{

    *out
                    << "CLU "
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
                    << " " <<std::setw(10)<<std::setprecision(3)<< m_Error
                    << std::setw(5)<< m_ParticleCode;

}

// Set All

void MuonRecDigitCSCClu::SetError           ( const double& Error           ) { m_Error           = Error           ; }
void MuonRecDigitCSCClu::SetDepositedCharge ( const double& DepositedCharge ) { m_DepositedCharge = DepositedCharge ; }
void MuonRecDigitCSCClu::SetTime            ( const double& Time            ) { m_Time            = Time            ; }
void MuonRecDigitCSCClu::SetStatus          ( const int&    Status          ) { m_Status          = Status          ; }
void MuonRecDigitCSCClu::SetTimeStatus      ( const int&    TimeStatus      ) { m_TimeStatus      = TimeStatus      ; }

// Get All

double MuonRecDigitCSCClu::GetError           () const { return m_Error           ; }
double MuonRecDigitCSCClu::GetDepositedCharge () const { return m_DepositedCharge ; }
double MuonRecDigitCSCClu::GetTime            () const { return m_Time            ; }
int    MuonRecDigitCSCClu::GetStatus          () const { return m_Status          ; }
int    MuonRecDigitCSCClu::GetTimeStatus      () const { return m_TimeStatus      ; }
 
