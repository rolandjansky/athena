/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RpcClusterBuilderPRD_H
#define RpcClusterBuilderPRD_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "Identifier/Identifier.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/RpcPrepData.h"

#include "MuonDigitContainer/RpcDigitContainer.h"
#include "MuonDigitContainer/RpcDigitCollection.h"
#include "MuonDigitContainer/RpcDigit.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "EventPrimitives/EventPrimitives.h"

class RpcIdHelper;

class RpcClusterBuilderPRD:public AthAlgorithm {

public:

  RpcClusterBuilderPRD (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  typedef Muon::RpcPrepDataContainer::const_iterator cont_iter;
  typedef Muon::RpcPrepDataCollection::const_iterator coll_iter;

private:

  StatusCode fill_rpcClusterContainer();

  void buildClusters(Identifier elementId);
  int buildPatterns(const Muon::RpcPrepDataCollection* rpcCollection);
  void push_back(Muon::RpcPrepData *& newCluster);

  
  std::vector<Muon::RpcPrepDataCollection*> m_coll_vect;
  Muon::RpcPrepDataCollection* m_temp_coll;

  std::string m_colKey;
  std::string m_colKeyIn;
  typedef std::map<int, Muon::RpcPrepData*> pattern;
  std::map<Identifier,pattern> m_digits;
  StatusCode retrieve_rpcClusterContainer() const;
  float m_timeSpread;

protected:

  Muon::RpcPrepDataContainer * m_rpcClusterContainer;
  const MuonGM::MuonDetectorManager * m_muonMgr;
  const RpcIdHelper * m_rpcId;

};

#endif
