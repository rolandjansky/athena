/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RpcHitIdHelper_H
#define RpcHitIdHelper_H

#include <string>

//base class
#include "HitManagement/HitIdHelper.h"

class RpcHitIdHelper: public HitIdHelper {
public:

  static RpcHitIdHelper* GetHelper();
  static RpcHitIdHelper* GetHelper(unsigned int nGasGaps);
  std::string GetStationName(const int& hid) const;
  void SetStationName(std::string name, int& hid) const;
  int GetPhiSector(const int& hid) const;
  int GetZSector(const int& hid) const;
  int GetDoubletR(const int& hid) const;
  int GetGasGapLayer(const int& hid) const;
  int GetDoubletPhi(const int& hid) const;
  int GetDoubletZ(const int& hid) const;
  int GetMeasuresPhi(const int& hid) const;

  int BuildRpcHitId (const std::string&, const int, const int, const int,
                     const int, const int, const int, const int) const;

private:
  RpcHitIdHelper();
  RpcHitIdHelper(unsigned int nGasGaps);
  void Initialize(unsigned int nGasGaps=2); // all non-BI RPCs (Run1+2) have 2 gas gaps, only BI RPCs have 3 gas gaps
  void InitializeStationName();
  static RpcHitIdHelper* m_help;

};

#endif
