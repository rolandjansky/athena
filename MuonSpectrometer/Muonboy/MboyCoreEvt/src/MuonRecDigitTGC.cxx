/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MuonRecDigitTGC.h"

MuonRecDigitTGC::MuonRecDigitTGC():MuonRecDigit(),MbIDTagTGC()
{

 Tech_TGC   () ;

}
MuonRecDigitTGC::~MuonRecDigitTGC(){}

void MuonRecDigitTGC::Print(std::ostream* out) const{PrintAmdIds(out);}
void MuonRecDigitTGC::PrintAmdIds(std::ostream* out) const{

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
void MuonRecDigitTGC::PrintAthIds(std::ostream* out) const{

    *out
                    << "TGC "
                    << "  "
                    << std::setw(3)<< m_StationName
                    << std::setw(4)<< m_SGStationEta
                    << std::setw(4)<< m_SGStationPhi
                    << std::setw(4)<< m_SGGasGap  
                    << std::setw(4)<< m_SGIsStrip 
                    << std::setw(4)<< m_SGChannel   
                    << " " <<std::setw(10)<<std::setprecision(3)<< m_X
                    << " " <<std::setw(10)<<std::setprecision(3)<< m_Y
                    << " " <<std::setw(10)<<std::setprecision(3)<< m_Z
                    << "                      "
                    << std::setw(5)<< m_ParticleCode;

}
