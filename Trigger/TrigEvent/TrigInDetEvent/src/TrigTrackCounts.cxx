/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigTrackCounts.h"

//---------------------------------------------------------------

TrigTrackCounts::TrigTrackCounts(): m_z0_pt(),
				    m_eta_phi() {
}

//---------------------------------------------------------------

TrigTrackCounts::TrigTrackCounts(const TrigHisto2D& z0_pt,
				 const TrigHisto2D& eta_phi)
  : m_z0_pt(z0_pt),
    m_eta_phi(eta_phi)
{
}

//---------------------------------------------------------------

TrigTrackCounts::TrigTrackCounts(TrigHisto2D&& z0_pt,
				 TrigHisto2D&& eta_phi)
  : m_z0_pt(std::move(z0_pt)),
    m_eta_phi(std::move(eta_phi))
{
}

//---------------------------------------------------------------

TrigTrackCounts::~TrigTrackCounts() {
}

//---------------------------------------------------------------

const TrigHisto2D& TrigTrackCounts::z0_pt(void) const {
  return m_z0_pt;
}

const TrigHisto2D& TrigTrackCounts::eta_phi(void) const {
  return m_eta_phi;
}

//---------------------------------------------------------------
