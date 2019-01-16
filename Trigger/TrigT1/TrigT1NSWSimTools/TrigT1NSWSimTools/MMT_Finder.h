/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMT_FINDER_H
#define MMT_FINDER_H


#include "AthenaKernel/MsgStreamMember.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "MMT_struct.h"


class MMT_Finder{
 public:
  MMT_Finder(MMT_Parameters *par, int nUVRoads = 1);
  ~MMT_Finder(){}
  int Coincidence_Gate(const std::vector<bool>& plane_hits) const;
  void set_roads(int roads) { m_nRoads=roads; }
  int get_roads() const {return m_nRoads;}
  void fillHitBuffer(std::map<std::pair<int,int>,finder_entry>& hitBuffer, const Hit& hit) const;
  void checkBufferForHits(std::vector<bool>& plane_is_hit, std::vector<Hit>& track, int road, std::map<std::pair<int,int>,finder_entry> hitBuffer) const;


  // / Log a message using the Athena controlled logging system
  MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
  // / Check whether the logging system is active at the provided verbosity level
  bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }

 private:
  std::vector<int> q_planes(const std::string& type) const;
  //Finder components
  double m_clock,m_max_age;
  int m_nRoads;
  double m_slope_min;
  double m_slope_max;
  std::vector<std::vector<double> > m_gateFlags;
  std::vector<std::vector<finder_entry> > m_finder;
  MMT_Parameters* m_par;

  int m_nUVRoads;

  /// Private message stream member
  mutable Athena::MsgStreamMember m_msg;
};
#endif
