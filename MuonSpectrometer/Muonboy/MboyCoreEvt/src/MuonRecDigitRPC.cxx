/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MuonRecDigitRPC.h"

MuonRecDigitRPC::MuonRecDigitRPC():MuonRecDigit(),MbIDTagRPC()
{

 Tech_RPC   () ;

 m_RpcTime = 0. ;

}
MuonRecDigitRPC::~MuonRecDigitRPC(){}

void MuonRecDigitRPC::Print(std::ostream* out) const{PrintAmdIds(out);}
void MuonRecDigitRPC::PrintAmdIds(std::ostream* out) const{

    *out
                    << "TRI "
                    << "  "
                    << std::setw(3)<< m_StationName
                    << std::setw(4)<< m_JFF
                    << std::setw(4)<< m_JZZ
                    << std::setw(4)<< m_JOB
                    << std::setw(4)<< m_JSPLI
                    << std::setw(4)<< m_JSL
                    << std::setw(4)<< m_JSZ
                    << std::setw(4)<< m_JSTRI
                    << " " <<std::setw(10)<<std::setprecision(3)<< m_X
                    << " " <<std::setw(10)<<std::setprecision(3)<< m_Y
                    << " " <<std::setw(10)<<std::setprecision(3)<< m_Z
                    << "                      "
                    << std::setw(5)<< m_ParticleCode;

}
void MuonRecDigitRPC::PrintAthIds(std::ostream* out) const{

    *out
                    << "RPC "
                    << "  "
                    << std::setw(3)<< m_StationName
                    << std::setw(4)<< m_SGStationEta 
                    << std::setw(4)<< m_SGStationPhi 
                    << std::setw(4)<< m_SGDoubletR   
                    << std::setw(4)<< m_SGDoubletZ   
                    << std::setw(4)<< m_SGDoubletPhi 
                    << std::setw(4)<< m_SGGasGap     
                    << std::setw(4)<< m_SGMeasuresPhi
                    << std::setw(4)<< m_SGStrip      
                    << " " <<std::setw(10)<<std::setprecision(3)<< m_X
                    << " " <<std::setw(10)<<std::setprecision(3)<< m_Y
                    << " " <<std::setw(10)<<std::setprecision(3)<< m_Z
                    << "                      "
                    << std::setw(5)<< m_ParticleCode;

}

// Set All

void MuonRecDigitRPC::SetRpcTime ( const double& RpcTime ) { m_RpcTime = RpcTime ; }

// Get All

double MuonRecDigitRPC::GetRpcTime () const  {return m_RpcTime	; }
