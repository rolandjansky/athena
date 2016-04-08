/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSPSHOWERBUILDER_H
#define MUONSPSHOWERBUILDER_H
/*****************************************************************************
Name    : MuonSpShowerBuilder.h
Package : offline/PhysicsAnalysis/EventViewBuilder/EventViewAlgs
Author  : Frank Paige, Stephane Willocq
Created : August 2006
Note    : 

DESCRIPTION:

High energy jets sometimes leak from the calorimeter, producing showers
in the muon system and missing ET tails. This algorithms sums the MDT
hits and muon segments around each jet axis and creates a MuonSpShower
object. For segments a standard cone is used. Since the phi for an MDT
hit corresponds to the center of the tube, a rectangle in eta-phi space
is used instead. The MuonSpShower object is saved only if it passes the
hit or segment cut. For most events the MuonSpShowerContainer should be
empty.

RPC and TGC hits might also be used.

*****************************************************************************/

#include "AthenaBaseComps/AthAlgorithm.h"

namespace MuonGM {
class MuonDetectorManager;
}
class MuonIdHelper;

#include <vector>
#include <string>

class MuonSpShowerBuilder : public AthAlgorithm {
 public:

  MuonSpShowerBuilder(const std::string& name, ISvcLocator* pSvcLocator);
  ~MuonSpShowerBuilder();

  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();

 private:

  std::string m_JetCollectionKey;
  double m_jetPtCut;

  std::string m_RpcPrepDataKey;
  std::string m_TgcPrepDataKey;
  std::string m_MdtPrepDataKey;
  std::string m_MuonSegmentKey;

  double m_etaCut;
  double m_phiCut;
  double m_rCut;
  int m_triggerHitCut;
  int m_mdtHitCut;
  int m_muonSegmentCut;  

  std::string m_MuonSpShowerKey;

  const MuonGM::MuonDetectorManager* m_MuonMgr;
  const MuonIdHelper* m_muonHelper;

};

#endif // MUONSPSHOWERBUILDER_H
