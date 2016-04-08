/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIEVENTSHAPEMAP_H
#define HIEVENTSHAPEMAP_H

#include "xAODHIEvent/HIEventShape.h"
#include "HIEventUtils/HIEventShapeIndex.h"
#include <string>

class HIEventShapeMap
{

public:

  static const HIEventShapeIndex* insert(std::string key, const HIEventShapeIndex& index, bool clobber=false);
  static const HIEventShapeIndex* getIndex(std::string key);
  static bool hasKey(std::string key);
  static HIEventShapeMap* getMap();

private:
  HIEventShapeMap(){};
  std::map<std::string,HIEventShapeIndex> m_map;
  static HIEventShapeMap s_obj;  

};


#endif
