/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 CLASS_DEF for InDetRIO_Maker map
 ------------------------------
 ATLAS Collaboration
 ***************************************************************************/

#ifndef PIXELGANGEDCLUSTERAMBIGUITIES_H
#define PIXELGANGEDCLUSTERAMBIGUITIES_H 1

//<<<<<< INCLUDES                                                       >>>>>>
#include "AthenaKernel/CLASS_DEF.h"
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

      // sometimes different clusters have the same identity
      // use the size to determine the order/break degeneracy
      if( ida == idb && ida != Identifier(0) ) { 
        return (a->width() < b->width());
      }
        
      return ida < idb;
    }
  };

  typedef std::multimap<const SiCluster*, const SiCluster*, 
                        ::InDet::compare_SiCluster> 
  PixelGangedClusterAmbiguities;
}

CLASS_DEF( InDet::PixelGangedClusterAmbiguities , 130176466 , 1 )



#endif // PIXELGANGEDCLUSTERAMBIGUITIES_H
