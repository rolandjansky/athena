/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKSECTORMAP_H
#define FTKSECTORMAP_H

#include <vector>
#include <map>

/* 
   A simple map that links sectors in 8L, 4L, and 11L cases:
   FTKSectorMap::GetSector(sector_8L,sector_4L) returns sector_11L
*/
class FTKSectorMap {
  typedef std::map<int,int> mapint;
  typedef std::map<int,mapint> mapint2;
private:
  mapint2 m_data;
public:
  FTKSectorMap() {};
  FTKSectorMap(const char *fname) { LoadFromFile(fname); }
  ~FTKSectorMap() {};
  void SetSector(int, int, int);
  int GetSector(int, int);
  int LoadFromFile(const char *);
  int CreateFile(const char *,const char *, const char *);
  void Dump();
  static std::map<int,int> makeLookup(const char* fname);
};

#endif
