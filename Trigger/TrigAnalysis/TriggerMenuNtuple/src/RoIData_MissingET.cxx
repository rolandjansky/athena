/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  RoIData_MissingET.cxx
*/
#include "TriggerMenuNtuple/RoIData_MissingET.h"

using namespace std;


RoIData_MissingET::RoIData_MissingET(int create) : 
  RoIData(create), 
  L2MissingETChains(0), EFMissingETChains(0),
  L2MissingET(false), L2MissingET_SumEt(0.0), L2MissingET_Et(0.0), L2MissingET_Ex(0.0), L2MissingET_Ey(0.0), L2overflowFlag(0),
  EFMissingET(false), EFMissingET_SumEt(0.0), EFMissingET_Et(0.0), EFMissingET_Ex(0.0), EFMissingET_Ey(0.0) {
}

RoIData_MissingET::~RoIData_MissingET() {
}

void RoIData_MissingET::setL2MissingET(float sumEt, float et, float ex, float ey, int flow, std::vector<TrigStatus> status) {
  L2MissingET = true;
  L2MissingET_SumEt = sumEt;
  L2MissingET_Et = et;
  L2MissingET_Ex = ex;
  L2MissingET_Ey = ey;
  L2overflowFlag = flow;
  L2MissingETChains = status;
}

void RoIData_MissingET::setEFMissingET(float sumEt, float et, float ex, float ey, std::vector<TrigStatus> status) {
  EFMissingET = true;
  EFMissingET_SumEt = sumEt;
  EFMissingET_Et = et;
  EFMissingET_Ex = ex;
  EFMissingET_Ey = ey;
  EFMissingETChains = status;
}

bool RoIData_MissingET::l2exist() const {
  return L2MissingET;
}
float RoIData_MissingET::l2pt() const {
  return L2MissingET_Et/1000;
}

bool RoIData_MissingET::efexist() const {
  return EFMissingET;
}
float RoIData_MissingET::efpt() const {
  return EFMissingET_Et/1000;
}
