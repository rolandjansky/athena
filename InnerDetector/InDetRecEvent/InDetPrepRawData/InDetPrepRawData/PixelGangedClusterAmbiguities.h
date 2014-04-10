/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 CLASS_DEF for InDetRIO_Maker map
 ------------------------------
 ATLAS Collaboration
 ***************************************************************************/

#ifndef PIXELGANGEDCLUSTERAMBIGUITIES_H
#define PIXELGANGEDCLUSTERAMBIGUITIES_H 1

//<<<<<< INCLUDES                                                       >>>>>>
#include "SGTools/CLASS_DEF.h"
#include <map>
#include "InDetPrepRawData/SiCluster.h"

//<<<<<< PUBLIC DEFINES  >>>>>>

namespace InDet{
  struct compare_SiCluster
  {
    bool operator()( const ::InDet::SiCluster* const& a,
                     const ::InDet::SiCluster* const& b) const
    {
      Identifier ida = (a!=0 ? a->identify() : Identifier(0));
      Identifier idb = (b!=0 ? b->identify() : Identifier(0));
      return ida < idb;
    }
  };

  typedef std::multimap<const SiCluster*, const SiCluster*, 
                        ::InDet::compare_SiCluster> 
  PixelGangedClusterAmbiguities;
}

CLASS_DEF( InDet::PixelGangedClusterAmbiguities , 130176466 , 1 )



#endif // PIXELGANGEDCLUSTERAMBIGUITIES_H
