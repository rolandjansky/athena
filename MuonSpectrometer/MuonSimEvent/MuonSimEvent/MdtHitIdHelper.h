/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MdtHitIdHelper_H
#define MdtHitIdHelper_H

#include <string>

//base class
#include "HitManagement/HitIdHelper.h"

class MdtHitIdHelper: public HitIdHelper {
public:

  static MdtHitIdHelper* GetHelper();
  static MdtHitIdHelper* GetHelper(const unsigned int nTubes);
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
  MdtHitIdHelper(const unsigned int nTubes);
  void Initialize(const unsigned int nTubes=78); // all MDT/sMDT chambers used in Run1/2 have maximum 78 tubes
  void InitializeStationName();
  static MdtHitIdHelper* m_help;

};

#endif
