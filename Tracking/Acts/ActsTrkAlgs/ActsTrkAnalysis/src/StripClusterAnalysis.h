/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSTRKANALYSIS_STRIPCLUSTERANALYSIS_H
#define ACTSTRKANALYSIS_STRIPCLUSTERANALYSIS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/ReadHandleKey.h"

#include "InDetIdentifier/SCT_ID.h"
#include "xAODInDetMeasurement/StripClusterContainer.h"

#include <string>
#include <vector>
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"

namespace ActsTrk {

  class StripClusterAnalysis : public AthAlgorithm {

  public:
    StripClusterAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
    ~StripClusterAnalysis(){}

    virtual StatusCode initialize() override final;
    virtual StatusCode execute() override final;
    virtual unsigned int cardinality() const override { return 1; }

  private:
    SG::ReadHandleKey<xAOD::StripClusterContainer> m_stripClusterContainerKey{this, "ClusterContainerKey", "ITkStripClusters", "Key of input pixel clusters"};
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
    std::vector<double>* m_localCovXX{};
    std::vector<int>* m_sizeX{};

    TH2* m_h_globalZR{};
    TH1* m_h_etaCluster{};

    TTree* m_tree{};
    Gaudi::Property<std::string> m_histPath {this, "HistPath", "/ActsTrkAnalysis/StripClusters/", ""};
    Gaudi::Property<std::string> m_ntuplePath {this, "NtuplePath", "/ActsTrkAnalysis/ntuples/", ""};
    Gaudi::Property<std::string> m_ntupleName {this, "NtupleName", "StripClusters", ""};

    ServiceHandle<ITHistSvc> m_thistSvc { this, "THistSvc", "THistSvc", "Histogramming svc" };

  };

}

#endif // ACTSTRKANALYSIS_STRIPCLUSTERANALYSIS_H
