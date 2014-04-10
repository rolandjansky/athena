/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PrepRawDataCLASS_DEF.h
//   Header file for class PrepRawDataCLASS_DEF
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to contain all the CLASS_DEF for Containers and Collections
///////////////////////////////////////////////////////////////////
// Version 1.0 25/09/2002 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef PIXELCLUSTERCOLLECTION_H
#define PIXELCLUSTERCOLLECTION_H

#include "SGTools/CLASS_DEF.h"
#include "TrkPrepRawData/PrepRawDataCollection.h"
#include "InDetPrepRawData/PixelCluster.h"

// Containers
// numbers obtained using clid.db
namespace InDet {
  typedef Trk::PrepRawDataCollection< PixelCluster > PixelClusterCollection;

  /**Overload of << operator for MsgStream for debug output*/
  MsgStream& operator << ( MsgStream& sl, const PixelClusterCollection& coll);

  /**Overload of << operator for std::ostream for debug output*/ 
  std::ostream& operator << ( std::ostream& sl, const PixelClusterCollection& coll);
}

CLASS_DEF(InDet::PixelClusterCollection, 1270617816, 1)

#endif // TRKPREPRAWDATA_PREPRAWDATACLASS_DEF_H
