/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTgcHitIdHelper_H
#define sTgcHitIdHelper_H

#include <string>

//base class
#include "HitManagement/HitIdHelper.h"

class sTgcHitIdHelper: public HitIdHelper {
public:

  static sTgcHitIdHelper* GetHelper();
  std::string GetStationName(const int& hid) const;
  void SetStationName(std::string name, int& hid) const;
  int GetPhiSector(const int& hid) const;
  int GetZSector(const int& hid) const;
  int GetMultiLayer(const int& hid) const;
  int GetLayer(const int& hid) const;
  int GetSide(const int& hid) const;

  int BuildsTgcHitId (const std::string&, const int, const int, const int,
                      const int, const int) const;

private:
  sTgcHitIdHelper();
  void Initialize();
  void InitializeStationName();
  static sTgcHitIdHelper* m_help;

};

#endif
