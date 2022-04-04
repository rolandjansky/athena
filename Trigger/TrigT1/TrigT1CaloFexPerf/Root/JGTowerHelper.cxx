/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloFexPerf/JGTowerHelper.h"
#include "TMath.h"
#include "TVector2.h"

namespace LVL1
{
  // Constructor
  JGTowerHelper::JGTowerHelper(float eta, float deta, float phi, float dphi)
  {
    m_eta = eta;
    m_deta = deta;
    m_phi = phi;
    m_dphi = dphi;
  }

  //Destructor
  JGTowerHelper::~JGTowerHelper() {}

  //Tools
  bool JGTowerHelper::inBox(float eta1, float eta2, float deta, float phi1, float phi2, float dphi)
  {

    bool inEtaBox = std::abs(eta1 - eta2) < deta;
    if (phi2 > TMath::Pi())
      phi2 = phi2 - 2 * TMath::Pi();
    float deltaPhi = std::abs(phi1 - phi2);
    if (deltaPhi > TMath::Pi())
      deltaPhi = 2 * TMath::Pi() - deltaPhi;
    bool inPhiBox = deltaPhi < dphi;

    return inEtaBox && inPhiBox;
  }

  bool JGTowerHelper::inBox(float eta, float phi) const
  {
    return inBox(Eta(), eta, dEta() / 2, Phi(), phi, dPhi() / 2);
  }
} // namespace LVL1
