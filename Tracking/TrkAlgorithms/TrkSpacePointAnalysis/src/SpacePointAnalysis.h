/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKING_TRKCLUSTERANALYSIS_SPACEPOINTANALYSIS_H
#define TRACKING_TRKCLUSTERANALYSIS_SPACEPOINTANALYSIS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/ReadHandleKey.h"

#include "TrkSpacePoint/SpacePointContainer.h"
#include "TrkSpacePoint/SpacePointOverlapCollection.h"

#include <string>
#include <vector>
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"

class TTree;
class PixelID;
class SCT_ID;

class SpacePointAnalysis : public AthAlgorithm {

public:
  SpacePointAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
  ~SpacePointAnalysis(){}

  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual unsigned int cardinality() const override { return 1; }

private:
  SG::ReadHandleKey<SpacePointContainer> m_inputKey{this, "InputKey", "PixelSpacePoints", "Key of input space points"};
  SG::ReadHandleKey<SpacePointOverlapCollection> m_inputOverlapKey{this, "InputOverlapKey", "OverlapSpacePoints", "Key of input for overlap space points"};
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
  std::vector<double>* m_globalCovXX{};
  std::vector<double>* m_globalCovYY{};
  std::vector<double>* m_globalCovZZ{};
  std::vector<double>* m_globalCovXY{};
  std::vector<double>* m_globalCovXZ{};
  std::vector<double>* m_globalCovYX{};
  std::vector<double>* m_globalCovYZ{};
  std::vector<double>* m_globalCovZX{};
  std::vector<double>* m_globalCovZY{};

  TH2* m_h_globalZR{};
  TH1* m_h_etaSpacePoint{};

  TTree* m_tree{};
  StringProperty m_ntupleFileName{this, "NtupleFileName", "/ntuples/file1", "Output filename"};
  StringProperty m_ntupleDirName{this, "NtupleDirectoryName", "/SpacePointAnalysis/", "Directory name in output file"};
  StringProperty m_ntupleTreeName{this, "NtupleTreeName", "SpacePointAna", "Tree name in output file"};
  StringProperty m_path{this, "HistPath", "/SpacePointAnalysis/", "Full path in output file"};
  ServiceHandle<ITHistSvc> m_thistSvc { this, "THistSvc", "THistSvc", "Histogramming svc" };

  BooleanProperty m_usePixel{this, "UsePixel", true, "enable use of pixel ID or SCT ID"};
  BooleanProperty m_useOverlap{this, "UseOverlap", true, "enable use of overlap for SCT ID"};

};

#endif // TRACKING_TRKCLUSTERANALYSIS_SPACEPOINTANALYSIS_H
