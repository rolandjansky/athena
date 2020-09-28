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

#ifndef ITK_STRIP_CLUSTERCOLLECTION_H
#define ITK_STRIP_CLUSTERCOLLECTION_H

#include "SGTools/CLASS_DEF.h"
#include "TrkPrepRawData/PrepRawDataCollection.h"
#include "InDetPrepRawData/ITk_Strip_Cluster.h"

// Containers
// numbers obtained using clid.db
namespace InDet {
  typedef Trk::PrepRawDataCollection< ITk_Strip_Cluster > ITk_Strip_ClusterCollection;

  /**Overload of << operator for MsgStream for debug output*/
  MsgStream& operator << ( MsgStream& sl, const ITk_Strip_ClusterCollection& coll);

  /**Overload of << operator for std::ostream for debug output*/ 
  std::ostream& operator << ( std::ostream& sl, const ITk_Strip_ClusterCollection& coll);
}

CLASS_DEF(InDet::ITk_Strip_ClusterCollection, 1206433936, 1)

#endif // TRKPREPRAWDATA_PREPRAWDATACLASS_DEF_H
