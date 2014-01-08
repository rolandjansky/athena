/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MuonRecDigitMDT.h"

MuonRecDigitMDT::MuonRecDigitMDT():MuonRecDigit(),MbIDTagMDT(),
m_RsLine("XXX")
{

 Tech_MDT   () ;

 m_TDCcount  = 0  ;
 m_ADCcount  = 0  ;
 m_DriftTime = 0. ;

 m_Status    =-1  ;

}
MuonRecDigitMDT::~MuonRecDigitMDT(){}

void MuonRecDigitMDT::Print(std::ostream* out) const{PrintAmdIds(out);}
void MuonRecDigitMDT::PrintAmdIds(std::ostream* out) const{

    *out
                    << "TUB "
                    << "  "
                    << std::setw(3)<< m_StationName
                    << std::setw(4)<< m_JFF
                    << std::setw(4)<< m_JZZ
                    << std::setw(4)<< m_JOB
                    << std::setw(4)<< m_JSL
                    << std::setw(4)<< m_JTUBE
                    << "        "
                    << " " <<std::setw(10)<<std::setprecision(3)<< m_X
                    << " " <<std::setw(10)<<std::setprecision(3)<< m_Y
                    << " " <<std::setw(10)<<std::setprecision(3)<< m_Z
                    << " " <<std::setw(10)<<std::setprecision(3)<< m_DriftTime
                    << " " <<std::setw(10)<< m_ADCcount
                    << std::setw(5)<< m_ParticleCode
                    << " " <<std::setw(10)<< m_TDCcount;

}
void MuonRecDigitMDT::PrintAthIds(std::ostream* out) const{

    *out
                    << "MDT "
                    << "  "
                    << std::setw(3)<< m_StationName
                    << std::setw(4)<< m_SGStationEta
                    << std::setw(4)<< m_SGStationPhi
                    << std::setw(4)<< m_SGMultilayer
                    << std::setw(4)<< m_SGTubeLayer
                    << std::setw(4)<< m_SGTube
                    << "        "
                    << " " <<std::setw(10)<<std::setprecision(3)<< m_X
                    << " " <<std::setw(10)<<std::setprecision(3)<< m_Y
                    << " " <<std::setw(10)<<std::setprecision(3)<< m_Z
                    << " " <<std::setw(10)<<std::setprecision(3)<< m_DriftTime
                    << " " <<std::setw(10)<< m_ADCcount
                    << std::setw(5)<< m_ParticleCode
                    << " " <<std::setw(10)<< m_TDCcount;

}


// Set All
void MuonRecDigitMDT::SetTDCcount	 ( const int&	 TDCcount  ){ m_TDCcount  = TDCcount  ; }
void MuonRecDigitMDT::SetADCcount	 ( const int&	 ADCcount  ){ m_ADCcount  = ADCcount  ; }
void MuonRecDigitMDT::SetSignalDriftTime ( const double& DriftTime ){ m_DriftTime = DriftTime ; }
void MuonRecDigitMDT::SetRsLine          ( std::string   RsLine    ){ m_RsLine    = RsLine    ; }
void MuonRecDigitMDT::SetStatus	         ( const int&	 Status    ){ m_Status    = Status    ; }

// Get All

int         MuonRecDigitMDT::GetTDCcount  () const { return m_TDCcount  ; }
int         MuonRecDigitMDT::GetADCcount  () const { return m_ADCcount  ; }
double      MuonRecDigitMDT::GetDriftTime () const { return m_DriftTime ; }
std::string MuonRecDigitMDT::GetRsLine    () const { return m_RsLine    ; }
int         MuonRecDigitMDT::GetStatus    () const { return m_Status    ; }
