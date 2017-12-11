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

#ifndef ISF_FATRASEVENT_PLANARCLUSTERCOLLECTION_H
#define ISF_FATRASEVENT_PLANARCLUSTERCOLLECTION_H

#include "AthenaKernel/CLASS_DEF.h"
#include "TrkPrepRawData/PrepRawDataCollection.h"
#include "ISF_FatrasEvent/PlanarCluster.h"

// Containers
// numbers obtained using clid.db

namespace iFatras {
  
  typedef Trk::PrepRawDataCollection< PlanarCluster > PlanarClusterCollection;
  
  /**Overload of << operator for MsgStream for debug output*/
  MsgStream& operator << ( MsgStream& sl, const PlanarClusterCollection& coll);
  
  /**Overload of << operator for std::ostream for debug output*/ 
  std::ostream& operator << ( std::ostream& sl, const PlanarClusterCollection& coll);
}

CLASS_DEF(iFatras::PlanarClusterCollection, 1076543790, 1)

#endif // ISF_FATRASEVENT_PLANARCLUSTERCOLLECTION_H
