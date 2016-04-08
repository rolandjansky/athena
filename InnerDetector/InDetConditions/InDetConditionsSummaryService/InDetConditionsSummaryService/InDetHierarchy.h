/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetHierarchy.h
 * Header containing the InDetHierarchy enum, to avoid pulling in a class every time it is needed
 * and namespace scoped to avoid clashes
 * @author shaun.roe@cern.ch
 **/
#ifndef InDetHierarchy_h
#define InDetHierarchy_h
namespace InDetConditions{
  enum Hierarchy{ DEFAULT=0, SCT_STRIP=1, SCT_CHIP=2, SCT_SIDE=3, SCT_MODULE=4,
    PIXEL_PIXEL=1, PIXEL_CHIP=2, PIXEL_MODULE=3,
  TRT_STRAW=1, TRT_STRAW_LAYER=2 };
}

#endif


