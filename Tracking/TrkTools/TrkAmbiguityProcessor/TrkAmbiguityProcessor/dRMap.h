/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 CLASS_DEF for InDet dR map
 ------------------------------
 ATLAS Collaboration
 ***************************************************************************/

#ifndef DRMAP_H
#define DRMAP_H 1

//<<<<<< INCLUDES                                                       >>>>>>
#include "AthenaKernel/CLASS_DEF.h"
#include <map>
#include "InDetPrepRawData/PixelCluster.h"


namespace InDet{

  typedef std::map<const InDet::PixelCluster*, std::pair<float,float> > 
  DRMap;
}

CLASS_DEF( InDet::DRMap , 193676466 , 1 )



#endif // DRMAP_H
