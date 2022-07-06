/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*
 * HTTRegionSlices_test.cxx: Unit tests for HTTRegionSlices
 */

#include <cstdio>
#include <iostream>
#include <cassert>
#include "TrigHTTConfTools/HTTRegionSlices.h"
#include "TLorentzVector.h"

int main(int, char**)
{

  std::string filePath = "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/HTT/TrigHTTMaps/V1/map_file/slices_v01_Jan21.txt";
  HTTRegionSlices regionSlice(filePath.c_str());

  const double eta(0.2), phi(0.4);
  const double bad_eta(0.7), bad_phi(0.8);
  const double pt(1), m(0);

  TLorentzVector tlv;
  HTTTruthTrack track;

  tlv.SetPtEtaPhiM(pt,eta,phi,m);
  track.setPX(tlv.Px());
  track.setPY(tlv.Py());
  track.setPZ(tlv.Pz());
  std::cout << "Truth track eta = " << track.getEta() << " and phi = " << track.getPhi() << " and in region = " << 
    regionSlice.inRegion(0, track) << std::endl;

  tlv.SetPtEtaPhiM(pt,bad_eta,phi,m);
  track.setPX(tlv.Px());
  track.setPY(tlv.Py());
  track.setPZ(tlv.Pz());
  std::cout << "Truth track eta = " << track.getEta() << " and phi = " << track.getPhi() << " and in region = " << 
    regionSlice.inRegion(0, track) << std::endl;

  tlv.SetPtEtaPhiM(pt,eta,bad_phi,m);
  track.setPX(tlv.Px());
  track.setPY(tlv.Py());
  track.setPZ(tlv.Pz());
  std::cout << "Truth track eta = " << track.getEta() << " and phi = " << track.getPhi() << " and in region = " << 
    regionSlice.inRegion(0, track) << std::endl;
  return 0;
}

