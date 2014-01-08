/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/MuonboyControl.h"

#include "MuonboyCore/GetSetMuonboyVariable.h"

MuonboyControl::MuonboyControl(){}

MuonboyControl::~MuonboyControl(){}


int  MuonboyControl::GetAtCosm()                {std::string Name="AtCosm"    ; return GetMuboIntVar (Name,NotDef,NotDef,NotDef);}
void MuonboyControl::SetAtCosm(int    AtCosmi)  {std::string Name="AtCosm"    ; SetMuboIntVar (Name,NotDef,NotDef,NotDef,AtCosmi);}

