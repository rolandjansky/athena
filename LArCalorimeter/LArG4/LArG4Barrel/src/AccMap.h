/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4BARREL_ACCMAP_H
#define LARG4BARREL_ACCMAP_H

#include "CurrMap.h"

#include <map>
#include <vector>
#include <string>

typedef std::map<int, CurrMap*> curr_map;

class AccMap {
public:
  ~AccMap() = default;
  static const AccMap* GetAccMap();

  void Reset();
  int  Region(int region, int sampling, int eta) const;
  const CurrMap* GetMap(int ifold, int ielecregion) const;
  const CurrMap* GetMap(int ifold, int region, int sampling, int eta) const;
  float GetXmin(int ifold) const {
    if (ifold<m_nmax && ifold >=0) return m_xmin[ifold];
    else return -999.;}
  float GetXmax(int ifold) const {
    if (ifold<m_nmax && ifold >=0) return m_xmax[ifold];
    else return -999.;}
  float GetYmin(int ifold) const {
    if (ifold<m_nmax && ifold >=0) return m_ymin[ifold];
    else return -999.;}
  float GetYmax(int ifold) const {
    if (ifold<m_nmax && ifold >=0) return m_ymax[ifold];
    else return -999.;}
private:
  AccMap();
  curr_map m_theMap;
  const int m_nmax{14};
  std::vector<float> m_xmin;
  std::vector<float> m_xmax;
  std::vector<float> m_ymin;
  std::vector<float> m_ymax;

#ifdef LARG4_STAND_ALONE
public:
  void SetDirectory(const std::string& dir) { m_directory=dir; }
private:
  std::string m_directory{"/afs/cern.ch/atlas/offline/data/lar/calo_data"};
#endif

};
#endif
