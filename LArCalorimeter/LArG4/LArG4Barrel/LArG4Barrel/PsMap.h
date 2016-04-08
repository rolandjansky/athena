/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PsMap_h
#define PsMap_h

#include "LArG4Barrel/CurrMap.h"

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
   void SetMap(int module);
   CurrMap* Map() {return m_curr;}
  private:
   PsMap();
   static PsMap* s_thePointer;
   curr_map m_theMap;
   std::string m_directory;
// current map info
   CurrMap* m_curr;
   int m_module;
};
#endif
