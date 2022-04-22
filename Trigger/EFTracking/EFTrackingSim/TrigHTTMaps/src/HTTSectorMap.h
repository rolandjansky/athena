/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTSECTORMAP_H
#define HTTSECTORMAP_H

#include <vector>
#include <map>

/* 
   A simple map that links sectors in different cases with different layers
*/
class HTTSectorMap {
  typedef std::map<int,int> mapint;
  typedef std::map<int,mapint> mapint2;
private:
  mapint2 m_data;
public:
  HTTSectorMap() = default;
  HTTSectorMap(const char *fname) { LoadFromFile(fname); }
  ~HTTSectorMap() = default;
  void SetSector(int, int, int);
  int GetSector(int, int);
  void LoadFromFile(const char *);
  void CreateFile(const char *,const char *, const char *);
  void Dump();
  static std::map<int,int> makeLookup(const char* fname);
};

#endif
