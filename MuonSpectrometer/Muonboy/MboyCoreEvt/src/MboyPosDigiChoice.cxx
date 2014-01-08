/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MboyPosDigiChoice.h"

MboyPosDigiChoice::MboyPosDigiChoice()
{

   m_PosOffMDT = 0;
   m_PosOffRPC = 0;
   m_PosOffTGC = 0;
   m_PosOffCSC = 0;
  
}
MboyPosDigiChoice::~MboyPosDigiChoice(){}

int  MboyPosDigiChoice::GetPosOffMDT () const {return m_PosOffMDT ;}
int  MboyPosDigiChoice::GetPosOffRPC () const {return m_PosOffRPC ;}
int  MboyPosDigiChoice::GetPosOffTGC () const {return m_PosOffTGC ;}
int  MboyPosDigiChoice::GetPosOffCSC () const {return m_PosOffCSC ;}

void MboyPosDigiChoice::SetPosOffMDT (int PosOffMDT) { m_PosOffMDT = PosOffMDT ; }
void MboyPosDigiChoice::SetPosOffRPC (int PosOffRPC) { m_PosOffRPC = PosOffRPC ; }
void MboyPosDigiChoice::SetPosOffTGC (int PosOffTGC) { m_PosOffTGC = PosOffTGC ; }
void MboyPosDigiChoice::SetPosOffCSC (int PosOffCSC) { m_PosOffCSC = PosOffCSC ; }
