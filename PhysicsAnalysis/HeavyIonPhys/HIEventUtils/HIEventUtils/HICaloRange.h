/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIEVENTUTILS_HICALORANGE_H
#define HIEVENTUTILS_HICALORANGE_H

#include "xAODHIEvent/HIEventShapeContainer.h"
#include "HIEventUtils/HIEventDefs.h"
#include <map>
#include <cmath>
#include <iostream>

class TH2;


class HICaloRange
{
public:
  //methods to access static member
  static const HICaloRange& getRange();
  static void initializeRange();
  static void initializeRange( const TH2* h2);
  static void initializeRange( const xAOD::HIEventShapeContainer* shape);

  //public class memeber functions
  bool LayerInRange(float eta, int layer);
  inline float getRangeMin(int layer) const {return m_range.find(layer)->second.first;};
  inline float getRangeMax(int layer) const {return m_range.find(layer)->second.second;};

private:
  //private constructor for singleton
  HICaloRange();
  HICaloRange(const HICaloRange& cr);
  void initializeRange_Internal();
  void initializeRange_Internal(const TH2* h2);
  void initializeRange_Internal(const xAOD::HIEventShapeContainer* shape);
  void updateRange(float eta_low, float eta_up, int layer);

  //members
  typedef std::pair<float,float> range_t;
  std::map<int, range_t > m_range;
  static HICaloRange s_range;
  static bool s_init;


};


#endif
