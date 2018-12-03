/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruth/TrackBarcodeInfo.h"

TrackBarcodeInfo::TrackBarcodeInfo(int bc, const ISF::ISFParticle* baseIsp):VTrackInformation(BarcodeOnly),m_theBaseISFParticle(baseIsp),m_barcode(bc),m_returnedToISF(false)
{
}

int TrackBarcodeInfo::GetParticleBarcode() const
{
  return m_barcode;
}

void TrackBarcodeInfo::SetBaseISFParticle(const ISF::ISFParticle* isp)
{
  m_theBaseISFParticle=isp;
}

const ISF::ISFParticle* TrackBarcodeInfo::GetBaseISFParticle() const
{
  return m_theBaseISFParticle;
}

void TrackBarcodeInfo::SetReturnedToISF(bool returned)
{
  m_returnedToISF = returned;
}

bool TrackBarcodeInfo::GetReturnedToISF() const
{
  return m_returnedToISF;
}
