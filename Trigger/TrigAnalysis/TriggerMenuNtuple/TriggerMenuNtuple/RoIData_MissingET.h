/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __RoIData_MissingET_h__
#define __RoIData_MissingET_h__
/*
  RoIData_MissingET.h
*/
#include "TriggerMenuNtuple/RoIData.h"

class MissingETObjectsInRoI;

class RoIData_MissingET : public RoIData {
public:
  RoIData_MissingET(int create=0);
  virtual ~RoIData_MissingET();

  void setL2MissingET(float sumEt, float et, float ex, float ey, int flow, std::vector<TrigStatus> status);
  void setEFMissingET(float sumEt, float et, float ex, float ey, std::vector<TrigStatus> status);

public:
  //
  bool l2exist() const;
  float l2pt() const;

  bool efexist() const;
  float efpt() const;
  //

  std::vector<TrigStatus> L2MissingETChains;
  std::vector<TrigStatus> EFMissingETChains;

  bool L2MissingET;
  float L2MissingET_SumEt;
  float L2MissingET_Et;
  float L2MissingET_Ex;
  float L2MissingET_Ey;
  int   L2overflowFlag;

  bool EFMissingET;
  float EFMissingET_SumEt;
  float EFMissingET_Et;
  float EFMissingET_Ex;
  float EFMissingET_Ey;

};

//bool convertMissingETData(RoIData_MissingET& out, const MissingETObjectsInRoI& in);

#endif // __RoIData_MissingET_h__
