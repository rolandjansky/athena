/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruth/TrackBarcodeInfo.h"

TrackBarcodeInfo::TrackBarcodeInfo(int bc, const ISF::ISFParticle* isp):VTrackInformation(BarcodeOnly),theISFParticle(isp),barcode(bc)
{
}

int TrackBarcodeInfo::GetParticleBarcode() const
{
  return barcode;
}

void TrackBarcodeInfo::SetISFParticle(const ISF::ISFParticle* isp)
{
  theISFParticle=isp;
}

const ISF::ISFParticle* TrackBarcodeInfo::GetISFParticle() const
{
  return theISFParticle;
}
