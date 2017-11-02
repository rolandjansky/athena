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
  MMT_Finder(MMT_Parameters *par);
  ~MMT_Finder(){}
  int Coincidence_Gate(const vector<bool>& plane_hits) const;
  void set_roads(int _roads) { roads=_roads; }
  int get_roads() const {return roads;}
  void fillHitBuffer(map<pair<int,int>,finder_entry>& evFinder, const Hit& hit) const;
  void checkBufferForHits(vector<bool>& plane_is_hit, vector<Hit>& track, int road, map<pair<int,int>,finder_entry> hitBuffer) const;


  /// Log a message using the Athena controlled logging system
  MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
  /// Check whether the logging system is active at the provided verbosity level
  bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }

 private:
  vector<int> q_planes(const string& type) const;
  //Finder components
  double clock,max_age;
  int roads;
  double slope_min,slope_max;
  vector<vector<double> > Gate_Flags; 
  vector<vector<finder_entry> > Finder;
  MMT_Parameters* m_par;

  /// Private message stream member
  mutable Athena::MsgStreamMember m_msg;
};
#endif
