/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ReadSiClusters_h
#define ReadSiClusters_h
#include <string>
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "StoreGate/DataHandle.h"
//#include "TrkPrepRawData/PrepRawDataCLASS_DEF.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetPrepRawData/SiClusterCollection.h"
#include "InDetPrepRawData/SiCluster.h"

class StoreGateSvc;

class ReadSiClusters : public Algorithm {

  typedef InDet::SiClusterContainer SiClusterContainer;
  typedef InDet::SiClusterCollection SiClusterCollection;
  typedef InDet::SiCluster SiCluster;

public:
  ReadSiClusters (const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode initialize();

  virtual StatusCode execute();

  virtual StatusCode finalize();

private:

  StatusCode RetrieveSiClusters();


  StatusCode BookSiNtuple();
  StatusCode FillSiNtuple();

private:

  bool m_readSiClusters;

  std::string m_SiClustersName;
  std::string m_ntupleDirName;

  int m_nDet; 

  StoreGateSvc* m_StoreGate; 

  const DataHandle<SiClusterContainer> m_clusters;
  //const DataHandle<SiClusterCollection> m_clusters;
  int m_eventNumber;
  int m_numberOfEvents;

  NTuple::Tuple *p_ntuple;
  NTuple::Item<double> m_r;
  NTuple::Item<double> m_phi;
  NTuple::Item<double> m_z;
  NTuple::Item<long> m_rowWidth;
  NTuple::Item<long> m_colWidth;
  NTuple::Item<double> m_phiWidth;
  NTuple::Item<double> m_etaWidth;
  NTuple::Item<double> m_errPhi;
  NTuple::Item<double> m_errZ;
};

#endif
