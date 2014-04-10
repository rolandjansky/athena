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

#ifndef SCT_CLUSTERCOLLECTION_H
#define SCT_CLUSTERCOLLECTION_H

#include "SGTools/CLASS_DEF.h"
#include "TrkPrepRawData/PrepRawDataCollection.h"
#include "InDetPrepRawData/SCT_Cluster.h"

// Containers
// numbers obtained using clid.db
namespace InDet {
  typedef Trk::PrepRawDataCollection< SCT_Cluster > SCT_ClusterCollection;

  /**Overload of << operator for MsgStream for debug output*/
  MsgStream& operator << ( MsgStream& sl, const SCT_ClusterCollection& coll);

  /**Overload of << operator for std::ostream for debug output*/ 
  std::ostream& operator << ( std::ostream& sl, const SCT_ClusterCollection& coll);
}

CLASS_DEF(InDet::SCT_ClusterCollection, 1334719092, 1)

#endif // TRKPREPRAWDATA_PREPRAWDATACLASS_DEF_H
