/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELGANGEDCLUSTERAMBIGUITIES_P1_H
#define PIXELGANGEDCLUSTERAMBIGUITIES_P1_H

/*

Author: Andreas Wildauer

*/

#include <vector>
#include <inttypes.h>

namespace InDet{
class PixelGangedClusterAmbiguities_p1   
{
 public:
/// Default constructor
  PixelGangedClusterAmbiguities_p1 ();
 
  std::string m_pixelClusterContainerName;
  std::vector< std::pair<uint32_t, std::vector<uint32_t> > >  m_ambiguityMap;
};

// inlines
inline
PixelGangedClusterAmbiguities_p1::PixelGangedClusterAmbiguities_p1() : m_pixelClusterContainerName("") {}
}

#endif
