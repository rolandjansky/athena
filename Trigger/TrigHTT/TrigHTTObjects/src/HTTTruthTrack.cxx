/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTObjects/HTTTruthTrack.h"

HTTTruthTrack::HTTTruthTrack() :
  TObject(),
  m_x(0.),
  m_y(0.),
  m_z(0.),
  m_d0(0.),
  m_vtx_z(0.),
  m_q(0),
  m_px(0.),
  m_py(0.),
  m_pz(0.),
  m_pdgcode(0),
  m_barcode(0),
  m_evtindex(0)
{
  // nothing special to do
}
