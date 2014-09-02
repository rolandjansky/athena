/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef MUONPREPRAWDATA_CSCSTRIPPREPDATACOLLECTION_H
#define MUONPREPRAWDATA_CSCSTRIPPREPDATACOLLECTION_H

#include "MuonPrepRawData/CscStripPrepData.h"
#include "MuonPrepRawData/MuonPrepDataCollection.h"
#include "CLIDSvc/CLASS_DEF.h"

namespace Muon {
    /** @brief Muon::MuonPrepDataCollection to hold the Muon::CscStripPrepData */
    typedef MuonPrepDataCollection< CscStripPrepData > CscStripPrepDataCollection;

    /**Overload of << operator for MsgStream for debug output*/
    MsgStream& operator << ( MsgStream& sl, const CscStripPrepDataCollection& coll);

    /**Overload of << operator for std::ostream for debug output*/ 
    std::ostream& operator << ( std::ostream& sl, const CscStripPrepDataCollection& coll);
    
}


CLASS_DEF(Muon::CscStripPrepDataCollection, 1206737261, 1)

#endif
 
