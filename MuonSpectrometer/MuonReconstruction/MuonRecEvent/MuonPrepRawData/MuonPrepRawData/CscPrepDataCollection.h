/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef MUONPREPRAWDATA_CSCPREPDATACOLLECTION_H
#define MUONPREPRAWDATA_CSCPREPDATACOLLECTION_H

#include "MuonPrepRawData/CscPrepData.h"
#include "MuonPrepRawData/MuonPrepDataCollection.h"
#include "CLIDSvc/CLASS_DEF.h"

namespace Muon {
    
    typedef MuonPrepDataCollection< CscPrepData > CscPrepDataCollection;

    /**Overload of << operator for MsgStream for debug output*/
    MsgStream& operator << ( MsgStream& sl, const CscPrepDataCollection& coll);

    /**Overload of << operator for std::ostream for debug output*/ 
    std::ostream& operator << ( std::ostream& sl, const CscPrepDataCollection& coll);
    
}


CLASS_DEF(Muon::CscPrepDataCollection, 2081800031, 1)

#endif
