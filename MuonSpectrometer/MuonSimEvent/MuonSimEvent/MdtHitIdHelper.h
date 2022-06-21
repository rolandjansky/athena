/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MdtHitIdHelper_H
#define MdtHitIdHelper_H

#include <string>

//base class
#include "HitManagement/HitIdHelper.h"

class MdtHitIdHelper: public HitIdHelper {
public:

  static const MdtHitIdHelper* GetHelper(unsigned int nTubes=78); // all MDT/sMDT chambers used in Run1/2 have maximum 78 tubes
  std::string GetStationName(const int& hid) const;
  void SetStationName(const std::string& name, int& hid) const;
  int GetPhiSector(const int& hid) const;
  int GetZSector(const int& hid) const;
  int GetMultiLayer(const int& hid) const;
  int GetLayer(const int& hid) const;
  int GetTube(const int& hid) const;

  int BuildMdtHitId (const std::string&, const int, const int, const int,
                     const int, const int) const;

private:
  MdtHitIdHelper(unsigned int nTubes);
  void Initialize(unsigned int nTubes);
  void InitializeStationName();
};

#endif
