/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMT_FINDER_H
#define MMT_FINDER_H

#include "AthenaBaseComps/AthMessaging.h"

#include "MMT_struct.h"


class MMT_Finder : public AthMessaging {
 public:
  MMT_Finder(std::shared_ptr<MMT_Parameters> par, int nUVRoads = 1);
  int Coincidence_Gate(const std::vector<bool>& plane_hits, std::shared_ptr<MMT_Parameters> par) const;
  void set_roads(int roads) { m_nRoads=roads; }
  int get_roads() const {return m_nRoads;}
  void fillHitBuffer(std::map<std::pair<int,int>,finder_entry>& hitBuffer, const Hit& hit, std::shared_ptr<MMT_Parameters> par) const;
  void checkBufferForHits(std::vector<bool>& plane_is_hit, std::vector<Hit>& track, int road, std::map<std::pair<int,int>,finder_entry> hitBuffer, std::shared_ptr<MMT_Parameters> par) const;

 private:
  std::vector<int> q_planes(const std::string& type) const;
  //Finder components
  double m_clock,m_max_age;
  int m_nRoads;
  double m_slope_min;
  double m_slope_max;
  std::vector<std::vector<double> > m_gateFlags;
  std::vector<std::vector<finder_entry> > m_finder;

  int m_nUVRoads;
};
#endif
