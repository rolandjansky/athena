/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AccMap_h
#define AccMap_h

#include "LArG4Barrel/CurrMap.h"

#include <map>
#include <vector>
#include <string>

typedef std::map<int, CurrMap*> curr_map;

class AccMap {
  public:
   ~AccMap();
   static AccMap* GetAccMap();
   void SetDirectory(std::string dir);
   void Reset();
   int  Region(int region, int sampling, int eta);
   void SetMap(int ifold, int ielecregion);
   void SetMap(int ifold, int region, int sampling, int eta);
   CurrMap* Map() {return m_curr;}
   float GetXmin(int ifold) {
       if (ifold<m_nmax && ifold >=0) return m_xmin[ifold];
       else return -999.;}
   float GetXmax(int ifold) {
       if (ifold<m_nmax && ifold >=0) return m_xmax[ifold];
       else return -999.;}
   float GetYmin(int ifold) {
       if (ifold<m_nmax && ifold >=0) return m_ymin[ifold];
       else return -999.;}
   float GetYmax(int ifold) {
       if (ifold<m_nmax && ifold >=0) return m_ymax[ifold];
       else return -999.;}
  private:
   AccMap();
   static AccMap* s_thePointer;
   curr_map m_theMap;
   std::string m_directory;
// current map info
   CurrMap* m_curr;
   int m_fold;
   int m_region;
   int m_eta;
   int m_sampling;
   int m_elecregion;
   int m_nmax;
   std::vector<float> m_xmin;
   std::vector<float> m_xmax;
   std::vector<float> m_ymin;
   std::vector<float> m_ymax;
};
#endif
