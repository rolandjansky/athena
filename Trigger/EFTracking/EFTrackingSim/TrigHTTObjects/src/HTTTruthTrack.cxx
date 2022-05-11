/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTObjects/HTTTruthTrack.h"

HTTTruthTrack::HTTTruthTrack() :
  m_d0(0.), m_z0(0.),
  m_vtx_x(0.), m_vtx_y(0.), m_vtx_z(0.), m_px(0.), m_py(0.), m_pz(0.),
  m_q(0),
  m_pdgcode(0), m_barcode(0), m_evtindex(0), m_barcode_frac_offline(0.)
{
  // nothing special to do
}
