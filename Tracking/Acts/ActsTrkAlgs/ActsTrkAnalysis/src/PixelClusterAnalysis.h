/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSTRKANALYSIS_PIXELCLUSTERANALYSIS_H
#define ACTSTRKANALYSIS_PIXELCLUSTERANALYSIS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/ReadHandleKey.h"

#include "InDetIdentifier/PixelID.h"
#include "xAODInDetMeasurement/PixelClusterContainer.h"

#include <string>
#include <vector>
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"

namespace ActsTrk {

  class PixelClusterAnalysis : public AthAlgorithm {

  public:
    PixelClusterAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
    ~PixelClusterAnalysis(){}

    virtual StatusCode initialize() override final;
    virtual StatusCode execute() override final;
    virtual unsigned int cardinality() const override { return 1; }

  private:
    SG::ReadHandleKey<xAOD::PixelClusterContainer> m_pixelClusterContainerKey{this, "ClusterContainerKey", "ITkPixelClusters", "Key of input pixel clusters"};
    const PixelID *m_pixelID{};

    std::vector<int>* m_barrelEndcap{};
    std::vector<int>* m_layerDisk{};
    std::vector<int>* m_phiModule{};
    std::vector<int>* m_etaModule{};
    std::vector<int>* m_isInnermost{};
    std::vector<int>* m_isNextToInnermost{};
    std::vector<double>* m_eta{};
    std::vector<float>* m_globalX{};
    std::vector<float>* m_globalY{};
    std::vector<float>* m_globalZ{};
    std::vector<float>* m_localX{};
    std::vector<float>* m_localY{};
    std::vector<float>* m_localCovXX{};
    std::vector<float>* m_localCovYY{};
    std::vector<int>* m_sizeX{};
    std::vector<int>* m_sizeY{};

    TH2* m_h_globalZR{};
    TH1* m_h_etaCluster{};

    TTree* m_tree{};
    Gaudi::Property<std::string> m_histPath {this, "HistPath", "/ActsTrkAnalysis/PixelClusters/", ""};
    Gaudi::Property<std::string> m_ntuplePath {this, "NtuplePath", "/ActsTrkAnalysis/ntuples/", ""};
    Gaudi::Property<std::string> m_ntupleName {this, "NtupleName", "PixelClusters", ""};

    ServiceHandle<ITHistSvc> m_thistSvc { this, "THistSvc", "THistSvc", "Histogramming svc" };

  };

}

#endif // ACTSTRKANALYSIS_PIXELCLUSTERANALYSIS_H
