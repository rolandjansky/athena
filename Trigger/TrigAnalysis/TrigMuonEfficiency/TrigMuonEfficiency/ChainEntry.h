/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ChainEntry_h
#define ChainEntry_h
// defined in Trigger/TrigAnalaysis/TriggerMenuNtuple/ChainEntry.h
namespace ChainEntry {
  enum RoIType {
    kUnknown = 0,
    kMuonRoIType = 1,
    kElectronRoIType = 2,
    kTauRoIType = 3,
    kJetRoIType = 4,
    kBjetRoIType = 5,
    kMETRoIType = 6,
    kBphysicsRoIType = 7,

    kRoIType_L2_mu = 1001,
    kRoIType_L2_e = 1002,
    kRoIType_L2_tau = 1003,
    kRoIType_L2_j = 1004,
    kRoIType_L2_b = 1005,
    kRoIType_L2_xe = 1006,
    kRoIType_L2_DiMu = 1007,
    kRoIType_L2_TileMu = 1010,

    kRoIType_EF_mu = 2001,
    kRoIType_EF_e = 2002,
    kRoIType_EF_tau = 2003,
    kRoIType_EF_j = 2004,
    kRoIType_EF_b = 2005,
    kRoIType_EF_xe = 2006,
    kRoIType_EF_DiMu = 2007,
    kRoIType_EF_TileMu = 2010
  };
  const int kEFChainIdOffset = 10000;
}
#endif
