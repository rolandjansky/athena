/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef MUONPREPRAWDATA_STGCPREPDATACOLLECTION_H
#define MUONPREPRAWDATA_STGCPREPDATACOLLECTION_H

#include "MuonPrepRawData/sTgcPrepData.h"
#include "MuonPrepRawData/MuonPrepDataCollection.h"
#include "CLIDSvc/CLASS_DEF.h"

namespace Muon {
  
  /** @brief Muon::MuonPrepDataCollection to hold the Muon::sTgcPrepData*/
  typedef MuonPrepDataCollection< sTgcPrepData > sTgcPrepDataCollection;
   
  /**Overload of << operator for MsgStream for debug output*/
  MsgStream& operator << ( MsgStream& sl, const sTgcPrepDataCollection& coll);
  
  /**Overload of << operator for std::ostream for debug output*/ 
  std::ostream& operator << ( std::ostream& sl, const sTgcPrepDataCollection& coll);
    
}

CLASS_DEF(Muon::sTgcPrepDataCollection, 1207304475, 1)

#endif
