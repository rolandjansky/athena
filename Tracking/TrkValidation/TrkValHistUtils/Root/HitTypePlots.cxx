/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkValHistUtils/HitTypePlots.h"

namespace Trk {
  HitTypePlots::HitTypePlots(PlotBase *pParent, std::string sHitType, std::string sHitLabel, int iMin,
                             int iMax) : PlotBase(pParent, ""),
    m_sHitType(sHitType), m_sHitLabel(sHitLabel), m_iMin(iMin), m_iMax(iMax) {
    Hits = NULL;
    HitsVsEta = NULL;
    HitsVsPhi = NULL;
  }

  void
  HitTypePlots::initializePlots() {
    int iBins = m_iMax - m_iMin + 1;

    Hits = Book1D(m_sHitType, m_sHitLabel + ";" + m_sHitLabel + ";Entries", iBins, m_iMin - 0.5, m_iMax + 0.5);
    HitsVsEta = Book2D(m_sHitType + "vsEta", m_sHitLabel + ";" + m_sHitLabel + ";#eta;Entries", iBins, m_iMin - 0.5,
                       m_iMax + 0.5, 54, -2.7, 2.7);
    HitsVsPhi = Book2D(m_sHitType + "vsPhi", m_sHitLabel + ";" + m_sHitLabel + ";#phi;Entries", iBins, m_iMin - 0.5,
                       m_iMax + 0.5, 54, -2.7, 2.7);
  }

  void
  HitTypePlots::fill(int iHits, float fEta, float fPhi) {
    Hits->Fill(iHits);
    HitsVsEta->Fill(iHits, fEta);
    HitsVsPhi->Fill(iHits, fPhi);
  }
}
