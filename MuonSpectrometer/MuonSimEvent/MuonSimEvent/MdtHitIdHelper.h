/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MdtHitIdHelper_H
#define MdtHitIdHelper_H

#include <string>

//base class
#include "HitManagement/HitIdHelper.h"

class MdtHitIdHelper: public HitIdHelper {
public:

  static MdtHitIdHelper* GetHelper();
  std::string GetStationName(const int& hid) const;
  void SetStationName(std::string name, int& hid) const;
  int GetPhiSector(const int& hid) const;
  int GetZSector(const int& hid) const;
  int GetMultiLayer(const int& hid) const;
  int GetLayer(const int& hid) const;
  int GetTube(const int& hid) const;

  int BuildMdtHitId (const std::string, const int, const int, const int,
                     const int, const int) const;

private:
  MdtHitIdHelper();
  void Initialize();
  void InitializeStationName();
  static MdtHitIdHelper* m_help;

};

#endif
