/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
   ~PsMap();
   static PsMap* GetPsMap();
   void SetDirectory(std::string dir);
   void Reset();
   CurrMap* GetMap (int module) const;
  private:
   PsMap();
   static PsMap* s_thePointer;
   curr_map m_theMap;
   std::string m_directory;
};
#endif // LARG4BARREL_PsMap_h
