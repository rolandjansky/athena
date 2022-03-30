/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSTRKANALYSIS_SPACEPOINTANALYSIS_H
#define ACTSTRKANALYSIS_SPACEPOINTANALYSIS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/ReadHandleKey.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"

#include "ActsTrkEvent/SpacePoint.h"
#include "xAODInDetMeasurement/PixelClusterContainer.h"
#include "xAODInDetMeasurement/StripClusterContainer.h"

#include <string>
#include <vector>
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"

namespace ActsTrk {

  class SpacePointAnalysis : public AthAlgorithm {

  public:
    SpacePointAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
    ~SpacePointAnalysis(){}

    virtual StatusCode initialize() override final;
    virtual StatusCode execute() override final;
    virtual unsigned int cardinality() const override { return 1; }

  private:
    SG::ReadHandleKey<ActsTrk::SpacePointContainer> m_spacePointContainerKey{this, "SpacePointContainerKey", "ITkPixelSpacePoints", "Key of input space points"};
    SG::ReadHandleKey<xAOD::PixelClusterContainer> m_pixelClusterContainerKey{this, "PixelClusterContainerKey", "ITkPixelClusters", "Key of input pixel clusters"};
    SG::ReadHandleKey<xAOD::StripClusterContainer> m_stripClusterContainerKey{this, "StripClusterContainerKey", "ITkStripClusters", "Key of input strip clusters"};
    const PixelID *m_pixelID{};
    const SCT_ID *m_stripID{};

    std::vector<int>* m_barrelEndcap{};
    std::vector<int>* m_layerDisk{};
    std::vector<int>* m_phiModule{};
    std::vector<int>* m_etaModule{};
    std::vector<int>* m_sideModule{};
    std::vector<int>* m_isInnermost{};
    std::vector<int>* m_isNextToInnermost{};
    std::vector<int>* m_isOverlap{};
    std::vector<double>* m_eta{};
    std::vector<double>* m_globalX{};
    std::vector<double>* m_globalY{};
    std::vector<double>* m_globalZ{};

    TH2* m_h_globalZR{};
    TH1* m_h_etaSpacePoint{};

    TTree* m_tree{};
    Gaudi::Property<std::string> m_histPath {this, "HistPath", "/ActsTrkAnalysis/", ""};
    Gaudi::Property<std::string> m_histName {this, "HistName", "PixelSpacePoints", ""};
    Gaudi::Property<std::string> m_ntuplePath {this, "NtuplePath", "/ActsTrkAnalysis/ntuples/", ""};
    Gaudi::Property<std::string> m_ntupleName {this, "NtupleName", "PixelSpacePoints", ""};

    ServiceHandle<ITHistSvc> m_thistSvc { this, "THistSvc", "THistSvc", "Histogramming svc" };

    BooleanProperty m_usePixel{this, "UsePixel", true, "enable use of pixel ID or SCT ID"};
    BooleanProperty m_useOverlap{this, "UseOverlap", false, "looking at strip strip space points"};

  };
}

#endif // ACTSTRKANALYSIS_SPACEPOINTANALYSIS_H
