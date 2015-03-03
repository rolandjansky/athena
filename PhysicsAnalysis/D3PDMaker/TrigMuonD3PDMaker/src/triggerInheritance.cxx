/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: triggerInheritance.cxx 332521 2010-11-16 15:39:21Z kanno $

// Gaudi/Athena include(s):
#include "SGTools/BaseInfo.h"

// EDM include(s):
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEvent/IsoMuonFeature.h"
#include "TrigMuonEvent/TileTrackMuFeature.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "EventKernel/INavigable4Momentum.h"

#include "MuonTrigCoinData/TgcCoinDataContainer.h"
#include "MuonTrigCoinData/TgcCoinDataCollection.h"
#include "MuonTrigCoinData/RpcCoinDataContainer.h"
#include "MuonTrigCoinData/RpcCoinDataCollection.h"


SG_ADD_BASE( CombinedMuonFeature, INavigable4Momentum );
SG_ADD_BASE( IsoMuonFeature,      INavigable4Momentum );
SG_ADD_BASE( TileTrackMuFeature,  INavigable4Momentum );
SG_ADD_BASE( TrigMuonEFTrack,     INavigable4Momentum );
SG_ADD_BASE( TrigMuonEFCbTrack,   TrigMuonEFTrack );
SG_ADD_BASE( TrigMuonEFCbTrack,   INavigable4Momentum );

SG_ADD_BASE( Muon::TgcCoinDataContainer,  Muon::MuonCoinDataContainer< Muon::TgcCoinDataCollection >);
SG_ADD_BASE( Muon::TgcCoinDataCollection, Muon::MuonCoinDataCollection< Muon::TgcCoinData > );
SG_ADD_BASE( Muon::RpcCoinDataContainer,  Muon::MuonCoinDataContainer< Muon::RpcCoinDataCollection >);
SG_ADD_BASE( Muon::RpcCoinDataCollection, Muon::MuonCoinDataCollection< Muon::RpcCoinData > );
