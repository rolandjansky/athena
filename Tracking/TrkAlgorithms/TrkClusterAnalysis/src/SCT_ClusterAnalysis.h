/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKING_TRKCLUSTERANALYSIS_SCT_CLUSTERANALYSIS_H
#define TRACKING_TRKCLUSTERANALYSIS_SCT_CLUSTERANALYSIS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/ReadHandleKey.h"

#include "InDetIdentifier/SCT_ID.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"

#include <string>
#include <vector>
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"

class TTree;
class SCT_ID;

class SCT_ClusterAnalysis : public AthAlgorithm {

public:
  SCT_ClusterAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
  ~SCT_ClusterAnalysis(){}

  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual unsigned int cardinality() const override { return 1; }

private:
  SG::ReadHandleKey<InDet::SCT_ClusterContainer> m_inputKey{this, "InputKey", "SCT_Clusters", "Key of input sct clusters"};
  const SCT_ID *m_stripID{};

  std::vector<int>* m_barrelEndcap{};
  std::vector<int>* m_layerDisk{};
  std::vector<int>* m_phiModule{};
  std::vector<int>* m_etaModule{};
  std::vector<int>* m_sideModule{};
  std::vector<double>* m_eta{};
  std::vector<double>* m_globalX{};
  std::vector<double>* m_globalY{};
  std::vector<double>* m_globalZ{};
  std::vector<double>* m_localX{};
  std::vector<double>* m_localY{};
  std::vector<double>* m_localCovXX{};
  std::vector<double>* m_localCovXY{};
  std::vector<double>* m_localCovYY{};
  std::vector<double>* m_localCovYX{};
  std::vector<int>* m_sizeX{};
  std::vector<int>* m_sizeY{};

  TH2* m_h_globalZR{};
  TH1* m_h_etaCluster{};

  TTree* m_tree{};
  StringProperty m_ntupleFileName{this, "NtupleFileName", "/ntuples/file1", "Output filename"};
  StringProperty m_ntupleDirName{this, "NtupleDirectoryName", "/StripClusterAnalysis/", "Directory name in output file"};
  StringProperty m_ntupleTreeName{this, "NtupleTreeName", "StripClusterAna", "Tree name in output file"};
  StringProperty m_path{this, "HistPath", "/StripClusterAnalysis/", "Full path in output file"};
  ServiceHandle<ITHistSvc> m_thistSvc { this, "THistSvc", "THistSvc", "Histogramming svc" };

};

#endif // TRACKING_TRKCLUSTERANALYSIS_SCT_CLUSTERANALYSIS_H
