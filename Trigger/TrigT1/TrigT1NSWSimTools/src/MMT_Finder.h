/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMT_FINDER_H
#define MMT_FINDER_H

#include "AthenaKernel/MsgStreamMember.h"

#include "MMT_struct.h"
#include "MMT_Fitter.h"


class MMT_Finder{
 public:
  MMT_Finder(MMT_Parameters *par, int nUVRoads = 1);
  ~MMT_Finder(){}
  int Coincidence_Gate(const vector<bool>& plane_hits) const;
  void set_roads(int _roads) { m_nRoads=_roads; }
  int get_roads() const {return m_nRoads;}
  void fillHitBuffer(map<pair<int,int>,finder_entry>& hitBuffer, const Hit& hit) const;
  void checkBufferForHits(vector<bool>& plane_is_hit, vector<Hit>& track, int road, map<pair<int,int>,finder_entry> hitBuffer) const;


  /// Log a message using the Athena controlled logging system
  MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
  /// Check whether the logging system is active at the provided verbosity level
  bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }

 private:
  vector<int> q_planes(const string& type) const;
  //Finder components
  double m_clock,m_max_age;
  int m_nRoads;
  double m_slope_min;
  double m_slope_max;
  vector<vector<double> > m_gateFlags;
  vector<vector<finder_entry> > m_finder;
  MMT_Parameters* m_par;

  int m_nUVRoads;

  /// Private message stream member
  mutable Athena::MsgStreamMember m_msg;
};
#endif
