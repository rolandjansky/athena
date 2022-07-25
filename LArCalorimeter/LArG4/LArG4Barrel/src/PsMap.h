/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4BARREL_PsMap_h
#define LARG4BARREL_PsMap_h

#include "CurrMap.h"

#include <map>
#include <vector>
#include <string>

typedef std::map<int, CurrMap*> curr_map;

class PsMap {
  public:
   ~PsMap() = default;
   static const PsMap* GetPsMap();
   void Reset();
   const CurrMap* GetMap (int module) const;
  private:
   PsMap();
   curr_map m_theMap;

#ifdef LARG4_STAND_ALONE
public:
  void SetDirectory(const std::string& dir) { m_directory = dir; }
private:
  std::string m_directory{"/afs/cern.ch/atlas/offline/data/lar/calo_data"};
#endif

};
#endif // LARG4BARREL_PsMap_h
