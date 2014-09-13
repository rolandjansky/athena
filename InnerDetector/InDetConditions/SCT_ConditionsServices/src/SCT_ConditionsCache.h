/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ConditionsCache.h
 * @author shaun.roe@cern.ch
**/
#ifndef SCT_ConditionsCache_h
#define SCT_ConditionsCache_h

#include <map>
#include <utility>

class SCT_ConditionsCache{
public:
  void store(const int idx, const bool status);
  std::pair<bool, bool> retrieve(const int idx) const;
  int size() const;
  void reset();
  
private:
  typedef std::map<int, bool> Map_t;
  Map_t m_map;
  
};


#endif
