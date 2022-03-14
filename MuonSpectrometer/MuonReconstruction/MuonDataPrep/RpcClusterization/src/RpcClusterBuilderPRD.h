/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RpcClusterBuilderPRD_H
#define RpcClusterBuilderPRD_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "Identifier/Identifier.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/RpcPrepData.h"

#include "MuonDigitContainer/RpcDigitContainer.h"
#include "MuonDigitContainer/RpcDigitCollection.h"
#include "MuonDigitContainer/RpcDigit.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "EventPrimitives/EventPrimitives.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

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

  void buildClusters(Identifier elementId, const MuonGM::MuonDetectorManager* MuonDetMgr);
  int buildPatterns(const Muon::RpcPrepDataCollection* rpcCollection);
  void push_back(Muon::RpcPrepData *& newCluster);

  
  std::vector<Muon::RpcPrepDataCollection*> m_coll_vect;
  Muon::RpcPrepDataCollection* m_temp_coll;

  typedef std::map<int, Muon::RpcPrepData*> pattern;
  std::map<Identifier,pattern> m_digits;
  // StatusCode retrieve_rpcClusterContainer() const;
  float m_timeSpread;

protected:

  Muon::RpcPrepDataContainer * m_rpcClusterContainer;

  SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey {this, "DetectorManagerKey", 
      "MuonDetectorManager", 
      "Key of input MuonDetectorManager condition data"};    

  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  SG::ReadHandleKey<Muon::RpcPrepDataContainer> m_colKeyIn {this, "InputCollectionName", "RPC_Measurements"};
  SG::WriteHandleKey<Muon::RpcPrepDataContainer> m_colKey {this, "CollectionName", "rpcClusters"};
};

#endif
