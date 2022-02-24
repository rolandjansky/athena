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

private:
  SG::ReadHandleKey<SpacePointContainer> m_inputKey;
  const PixelID *m_pixelID;
  const SCT_ID *m_stripID;

  std::vector<int>* m_barrelEndcap;
  std::vector<int>* m_layerDisk;
  std::vector<int>* m_phiModule;
  std::vector<int>* m_etaModule;
  std::vector<int>* m_sideModule;
  std::vector<int>* m_isInnermost;
  std::vector<int>* m_isNextToInnermost;
  std::vector<double>* m_eta;
  std::vector<double>* m_globalX;
  std::vector<double>* m_globalY;
  std::vector<double>* m_globalZ;
  std::vector<double>* m_globalCovXX;
  std::vector<double>* m_globalCovYY;
  std::vector<double>* m_globalCovZZ;
  std::vector<double>* m_globalCovXY;
  std::vector<double>* m_globalCovXZ;
  std::vector<double>* m_globalCovYX;
  std::vector<double>* m_globalCovYZ;
  std::vector<double>* m_globalCovZX;
  std::vector<double>* m_globalCovZY;

  TH2* m_h_globalZR;
  TH1* m_h_etaSpacePoint;

  TTree* m_tree;
  std::string m_ntupleFileName;
  std::string m_ntupleDirName;
  std::string m_ntupleTreeName;
  std::string m_path;
  ServiceHandle<ITHistSvc> m_thistSvc;

  bool m_usePixel;

};

#endif // TRACKING_TRKCLUSTERANALYSIS_SPACEPOINTANALYSIS_H
