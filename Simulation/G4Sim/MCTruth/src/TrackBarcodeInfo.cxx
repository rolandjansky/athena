/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruth/TrackBarcodeInfo.h"

TrackBarcodeInfo::TrackBarcodeInfo(int bc, const ISF::ISFParticle* baseIsp):VTrackInformation(BarcodeOnly),theBaseISFParticle(baseIsp),barcode(bc),returnedToISF(false)
{
}

int TrackBarcodeInfo::GetParticleBarcode() const
{
  return barcode;
}

void TrackBarcodeInfo::SetBaseISFParticle(const ISF::ISFParticle* isp)
{
  theBaseISFParticle=isp;
}

const ISF::ISFParticle* TrackBarcodeInfo::GetBaseISFParticle() const
{
  return theBaseISFParticle;
}

void TrackBarcodeInfo::SetReturnedToISF(bool returned)
{
  returnedToISF = returned;
}

bool TrackBarcodeInfo::GetReturnedToISF() const
{
  return returnedToISF;
}
