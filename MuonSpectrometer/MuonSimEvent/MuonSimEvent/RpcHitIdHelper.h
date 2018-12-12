/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RpcHitIdHelper_H
#define RpcHitIdHelper_H

#include <string>

//base class
#include "HitManagement/HitIdHelper.h"

class RpcHitIdHelper: public HitIdHelper {
public:

  static RpcHitIdHelper* GetHelper();
  std::string GetStationName(const int& hid) const;
  void SetStationName(std::string name, int& hid) const;
  int GetPhiSector(const int& hid) const;
  int GetZSector(const int& hid) const;
  int GetDoubletR(const int& hid) const;
  int GetGasGapLayer(const int& hid) const;
  int GetDoubletPhi(const int& hid) const;
  int GetDoubletZ(const int& hid) const;
  int GetMeasuresPhi(const int& hid) const;

  int BuildRpcHitId (const std::string, const int, const int, const int,
                     const int, const int, const int, const int) const;

private:
  RpcHitIdHelper();
  void Initialize();
  void InitializeStationName();
  static RpcHitIdHelper* m_help;

};

#endif
