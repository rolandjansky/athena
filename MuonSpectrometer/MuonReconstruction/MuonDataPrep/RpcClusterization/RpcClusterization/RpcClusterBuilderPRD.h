/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RpcClusterBuilderPRD_H
#define RpcClusterBuilderPRD_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/DataHandle.h"
#include "Identifier/Identifier.h"
//#include "CLHEP/Geometry/Point3D.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/RpcPrepData.h"

#include "MuonDigitContainer/RpcDigitContainer.h"
#include "MuonDigitContainer/RpcDigitCollection.h"
#include "MuonDigitContainer/RpcDigit.h"
#include <vector>
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "EventPrimitives/EventPrimitives.h"

class StoreGateSvc;
class ActiveStoreSvc;
class MsgStream;
class RpcIdHelper;

using namespace std;
using namespace MuonGM;

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

  
  vector<Muon::RpcPrepDataCollection*> m_coll_vect; 	 
  Muon::RpcPrepDataCollection* m_temp_coll;

  std::string m_colKey;
  std::string m_colKeyIn;
  typedef map<int, Muon::RpcPrepData*, less<int> > pattern;
  map<Identifier,pattern> m_digits;
  StatusCode retrieve_rpcClusterContainer() const;
  float m_timeSpread;

protected:

  Muon::RpcPrepDataContainer * m_rpcClusterContainer;
  ActiveStoreSvc* m_activeStore;
  StoreGateSvc *m_EvtStore;
  const MuonDetectorManager * m_muonMgr;
  const RpcIdHelper * m_rpcId;

};

#endif
