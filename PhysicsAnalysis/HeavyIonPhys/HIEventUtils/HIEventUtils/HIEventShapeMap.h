/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIEVENTUTILS_HIEVENTSHAPEMAP_H
#define HIEVENTUTILS_HIEVENTSHAPEMAP_H

#include "xAODHIEvent/HIEventShape.h"
#include "HIEventUtils/HIEventShapeIndex.h"
#include <string>

//This class is currently supersed by HIEventShapeMapTool - fully MT safe

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
