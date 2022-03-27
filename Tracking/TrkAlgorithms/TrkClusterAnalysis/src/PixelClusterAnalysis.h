/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKING_TRKCLUSTERANALYSIS_PIXELCLUSTERANALYSIS_H
#define TRACKING_TRKCLUSTERANALYSIS_PIXELCLUSTERANALYSIS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/ReadHandleKey.h"

#include "InDetIdentifier/PixelID.h"
#include "InDetPrepRawData/PixelClusterContainer.h"

#include <string>
#include <vector>
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"

class TTree;
class PixelID;

class PixelClusterAnalysis : public AthAlgorithm {

public:
  PixelClusterAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
  ~PixelClusterAnalysis(){}

  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual unsigned int cardinality() const override { return 1; }

private:
  SG::ReadHandleKey<InDet::PixelClusterContainer> m_inputKey{this, "InputKey", "PixelClusters", "Key of input pixel clusters"};
  const PixelID *m_pixelID{};

  std::vector<int>* m_barrelEndcap{};
  std::vector<int>* m_layerDisk{};
  std::vector<int>* m_phiModule{};
  std::vector<int>* m_etaModule{};
  std::vector<int>* m_isInnermost{};
  std::vector<int>* m_isNextToInnermost{};
  std::vector<double>* m_eta{};
  std::vector<double>* m_globalX{};
  std::vector<double>* m_globalY{};
  std::vector<double>* m_globalZ{};
  std::vector<double>* m_localX{};
  std::vector<double>* m_localY{};
  std::vector<double>* m_localCovXX{};
  std::vector<double>* m_localCovYY{};
  std::vector<int>* m_sizeX{};
  std::vector<int>* m_sizeY{};

  TH2* m_h_globalZR{};
  TH1* m_h_etaCluster{};

  TTree* m_tree{};
  StringProperty m_ntupleFileName{this, "NtupleFileName", "/ntuples/file1", "Output filename"};
  StringProperty m_ntupleDirName{this, "NtupleDirectoryName", "/PixelClusterAnalysis/", "Directory name in output file"};
  StringProperty m_ntupleTreeName{this, "NtupleTreeName", "PixelClusterAna", "Tree name in output file"};
  StringProperty m_path{this, "HistPath", "/PixelClusterAnalysis/", "Full path in output file"};
  ServiceHandle<ITHistSvc> m_thistSvc { this, "THistSvc", "THistSvc", "Histogramming svc" };

};

#endif // TRACKING_TRKCLUSTERANALYSIS_PIXELCLUSTERANALYSIS_H
