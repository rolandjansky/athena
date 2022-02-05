/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MicromegasHitIdHelper_H
#define MicromegasHitIdHelper_H

#include <string>

//base class
#include "HitManagement/HitIdHelper.h"

class MicromegasHitIdHelper: public HitIdHelper {
public:

  static MicromegasHitIdHelper* GetHelper();
  std::string GetStationName(const int& hid) const;
  void SetStationName(std::string name, int& hid) const;
  int GetPhiSector(const int& hid) const;
  int GetZSector(const int& hid) const;
  int GetMultiLayer(const int& hid) const;
  int GetLayer(const int& hid) const;
  int GetSide(const int& hid) const;

  int BuildMicromegasHitId (const std::string&, const int, const int, const int,
                            const int, const int) const;

private:
  MicromegasHitIdHelper();
  void Initialize();
  void InitializeStationName();
  static MicromegasHitIdHelper* m_help;

};

#endif
