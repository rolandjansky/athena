/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#pragma once

#include "EventContainers/IdentifiableCache.h"

#include "MuonPrepRawData/CscPrepDataCollection.h"
#include "MuonPrepRawData/CscStripPrepDataCollection.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "MuonPrepRawData/sTgcPrepDataCollection.h"
#include "MuonPrepRawData/MMPrepDataCollection.h"
 
// Typedef the different templates which are likely to be used later on
typedef EventContainers::IdentifiableCache <Muon::CscPrepDataCollection> CscPrepDataCollection_Cache;
typedef EventContainers::IdentifiableCache <Muon::CscStripPrepDataCollection> CscStripPrepDataCollection_Cache;
typedef EventContainers::IdentifiableCache <Muon::MdtPrepDataCollection> MdtPrepDataCollection_Cache;
typedef EventContainers::IdentifiableCache <Muon::RpcPrepDataCollection> RpcPrepDataCollection_Cache;
typedef EventContainers::IdentifiableCache <Muon::TgcPrepDataCollection> TgcPrepDataCollection_Cache;
typedef EventContainers::IdentifiableCache <Muon::sTgcPrepDataCollection> sTgcPrepDataCollection_Cache;
typedef EventContainers::IdentifiableCache <Muon::MMPrepDataCollection> MMPrepDataCollection_Cache;

CLASS_DEF( CscPrepDataCollection_Cache, 1232420816, 1 )
CLASS_DEF( CscStripPrepDataCollection_Cache, 1331156947, 1 )
CLASS_DEF( MdtPrepDataCollection_Cache, 1174391008, 1)
CLASS_DEF( RpcPrepDataCollection_Cache, 1256660324, 1)
CLASS_DEF( TgcPrepDataCollection_Cache, 1086963067, 1)
CLASS_DEF( sTgcPrepDataCollection_Cache, 1222037689, 1)
CLASS_DEF( MMPrepDataCollection_Cache, 1084684386, 1)
