/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SI_HIT_ANALYSIS_H
#define SI_HIT_ANALYSIS_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/LockedHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

#include "InDetSimEvent/SiHitCollection.h"

#include <string>
#include <vector>

class TH1;
class TH2;
class TTree;


class SiHitAnalysis : public AthAlgorithm
{
public:

  SiHitAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
  ~SiHitAnalysis(){}

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;

private:

  /** Some variables**/
  TH1* m_h_hits_x{};
  TH1* m_h_hits_y{};
  TH1* m_h_hits_z{};
  TH1* m_h_hits_r{};
  TH2* m_h_xy{};
  TH2* m_h_zr{};
  LockedHandle<TH2> m_h_xy_shared{};
  LockedHandle<TH2> m_h_zr_shared{};
  TH1* m_h_hits_time{};
  TH1* m_h_hits_eloss{};
  TH1* m_h_hits_step{};
  TH1* m_h_hits_barcode{};
  TH2* m_h_time_eloss{};
  TH2* m_h_z_eloss{};
  TH2* m_h_r_eloss{};
  TH1* m_h_barrel_endcap{};
  TH1* m_h_layer_disk{};
  TH1* m_h_module_phi{};
  TH1* m_h_module_eta{};

  TTree* m_tree{};
  std::vector<float>* m_hits_x{};
  std::vector<float>* m_hits_y{};
  std::vector<float>* m_hits_z{};
  std::vector<float>* m_hits_r{};
  std::vector<float>* m_hits_time{};
  std::vector<float>* m_hits_eloss{};
  std::vector<float>* m_hits_step{};
  std::vector<float>* m_hits_barcode{};
  std::vector<float>* m_hits_pdgId{};
  std::vector<float>* m_hits_pT{};
  std::vector<float>* m_hits_eta{};
  std::vector<float>* m_hits_phi{};
  std::vector<float>* m_hits_prodVtx_x{};
  std::vector<float>* m_hits_prodVtx_y{};
  std::vector<float>* m_hits_prodVtx_z{};
  std::vector<int>* m_barrel_endcap{};
  std::vector<int>* m_layer_disk{};
  std::vector<int>* m_module_phi{};
  std::vector<int>* m_module_eta{};

  SG::ReadHandleKey<SiHitCollection> m_hitsContainerKey {this, "CollectionName", "", "Input HITS collection name"};

  Gaudi::Property<std::string> m_histPath {this, "HistPath", "/SiHitAnalysis/histos/", ""};
  Gaudi::Property<std::string> m_ntuplePath {this, "NtuplePath", "/SiHitAnalysis/ntuples/", ""};
  Gaudi::Property<bool> m_expert {this, "ExpertMode", false, ""};
  Gaudi::Property<bool> m_extraTruthBranches {this, "ExtraTruthBranches", false, ""};

  ServiceHandle<ITHistSvc> m_thistSvc {this, "HistSvc", "THistSvc", ""};

};

#endif // SI_HIT_ANALYSIS_H
