/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TgcHitIdHelper_H
#define TgcHitIdHelper_H

#include <string>

//base class
#include "HitManagement/HitIdHelper.h"

class TgcHitIdHelper: public HitIdHelper {
public:

  static const TgcHitIdHelper* GetHelper();
  std::string GetStationName(const int& hid) const;
  void SetStationName(const std::string& name, int& hid) const;
  int GetStationPhi(const int& hid) const;
  int GetStationEta(const int& hid) const;
  int GetGasGap(const int& hid) const;

  int BuildTgcHitId (const std::string&, const int, const int, const int) const;

private:
  TgcHitIdHelper();
  void Initialize();
  void InitializeStationName();
};

#endif
