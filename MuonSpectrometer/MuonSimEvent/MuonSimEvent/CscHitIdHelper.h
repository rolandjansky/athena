/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CscHitIdHelper_H
#define CscHitIdHelper_H

#include <string>

//base class
#include "HitManagement/HitIdHelper.h"

class CscHitIdHelper: public HitIdHelper {
public:

  static const CscHitIdHelper* GetHelper();
  std::string GetStationName(const int& hid) const;
  void SetStationName(const std::string& name, int& hid) const;
  int GetPhiSector(const int& hid) const;
  int GetZSector(const int& hid) const;
  int GetChamberLayer(const int& hid) const;
  int GetWireLayer (const int& hid) const;

  int BuildCscHitId (const std::string&, const int, const int, const int,
                     const int) const;

private:
  CscHitIdHelper();
  void Initialize();
  void InitializeStationName();
};

#endif
