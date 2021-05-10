/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOFEXPERF_FWDCELL_H
#define TRIGT1CALOFEXPERF_FWDCELL_H

#include <vector>
#include <TMath.h>
#include <TString.h>
#include <TObjArray.h>
#include <TLorentzVector.h>
#include "CaloEvent/CaloCellContainer.h"

class FwdCell{

 public:

  // constructors
  FwdCell();
  FwdCell(float eta, float deta, float phi, float dphi);

  // destructor
  virtual ~FwdCell();

  float Eta() const {return m_eta;};
  float Phi() const {return m_phi;};
  float dEta() const {return m_deta;};
  float dPhi() const {return m_dphi;};
  float sampling() const {return m_sampling;};
  std::vector<int> GetSCIndices() const {return m_scellIndices;};
  std::vector<int> GetTileIndices() const {return m_tileIndices;};
  void SetSCIndices(int sc_id)  {m_scellIndices.push_back(sc_id);};
  void SetTileIndices(int tile_id)  {m_tileIndices.push_back(tile_id);};
  void SetSampling(int sampling) {m_sampling=sampling;};
 private:

  float m_eta;
  float m_deta;
  float m_phi;
  float m_dphi;
  int m_sampling;
  std::vector<int> m_scellIndices;
  std::vector<int> m_tileIndices;
  void Initialize(float eta, float deta, float phi, float dphi);

};

  bool isInBox(float eta1, float eta2, float deta, float phi1, float phi2, float dphi);
  bool inOnBox(float eta1, float eta2, float deta, float phi1, float phi2, float dphi);
  bool isInBoxXY(float x1, float x2, float dx, float y1, float y2, float dy, float z1, float z2);
  float percentageInWindowEtaPhi(float etaWindow, float etaCell, float detaWindow, float detaCell, float phiWindow, float phiCell, float dphiWindow, float dphiCell);
  std::vector<float> fromEtaPhiLayerToXY(float eta, float phi, int layer);
  float percentageInWindowFCAL(float etaWindow, float etaCell, float detaWindow, float detaCell, float phiWindow, float phiCell, float dphiWindow, float dphiCell);
  float deltaPhiAtlas(float phi1, float phi2);
  float deltaR(float phi1, float phi2, float eta1, float eta2);
  float roundp1(float value);
  std::vector<CaloCell*> sortCells(std::vector<CaloCell*> cells);
  std::vector<float> fromXYtoEtaPhi(float x, float y, float z);
  float sumOfEnergy(std::vector<CaloCell*> cells);
  float rectangeOverlapSeedCell(float seed_x, float seed_y, float seed_dx, float seed_dy, CaloCell* cell);
  std::vector<float> firstlayerShowerCentre(std::vector<CaloCell*> cells);
  std::vector<float> varianceFromCentre(std::vector<CaloCell*> cells);
  float fractionMaxEnergyCell(std::vector<CaloCell*> cells);
  float showerDepthZ(std::vector<CaloCell*> cells);
  std::vector<float> projectXYCoordinates(float x,float y,float z,float eta,float phi);
#endif
