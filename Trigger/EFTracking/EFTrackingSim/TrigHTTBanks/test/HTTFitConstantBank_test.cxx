// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#undef NDEBUG
#include <cassert>
#include <string>
#include <iostream>
#include <numeric>
#include <filesystem>

#include "AthenaKernel/getMessageSvc.h"
#include "TrigHTTMaps/HTTPlaneMap.h"
#include "TrigHTTObjects/HTTTrack.h"
#include "../src/HTTFitConstantBank.h"
#include "PathResolver/PathResolver.h"

static const int ncoords=9;
static const int npix=1;
static const float x[8] = {  267.582993,  364.800979,  514.460438,  520.356103,  698.459816,  704.303125,  917.730445,  923.5662};
static const float y[8] = {  113.13225,  154.23536,  217.51039,  220.00306,  295.30410,  297.77457,  388.01020, 390.4775};
static const float z[8] = { 58.103,  79.213,  111.710,  112.990,  151.664,  152.933,  199.277,  200.544};

int main(int, char**) {

  // remove local file since filesystem copy overwrite doesn't always work if there is a duplicate, even with flags
  std::filesystem::remove("./step3_01eta03_03phi05_fit.pmap");
  std::filesystem::remove("./test.gcon");
  
  // find the file
  std::string pmap_path = PathResolver::find_file("step3_01eta03_03phi05.pmap","DATAPATH", PathResolver::RecursiveSearch);
  std::string const_path = PathResolver::find_file("test.gcon","DATAPATH", PathResolver::RecursiveSearch);
  
  //copy it over
  std::filesystem::copy(pmap_path,"./step3_01eta03_03phi05_fit.pmap",std::filesystem::copy_options::overwrite_existing);
  std::filesystem::copy(const_path,"./test.gcon",std::filesystem::copy_options::overwrite_existing);

  
  HTTPlaneMap pmap("./step3_01eta03_03phi05_fit.pmap", 0, 1);
  HTTFitConstantBank fitconstants(&pmap, 9, "./test.gcon", false, 0);
  HTTTrack track;
  track.setNLayers(ncoords - npix);
  track.setTrackCorrType(TrackCorrType::Second);
  for (unsigned layer=0; layer < (ncoords-npix); layer++) {
    HTTHit hit;
    hit.setLayer(layer);
    hit.setX(x[layer]);
    hit.setY(y[layer]);
    hit.setZ(z[layer]);
    if (layer < npix) hit.setDetType(SiliconTech::pixel);
    else hit.setDetType(SiliconTech::strip);
    track.setHTTHit(layer, hit);
  }
  fitconstants.linfit(0, track, false);
  std::cerr << "q/pt = " << track.getQOverPt() << " phi = " << track.getPhi() << " d0 = " << track.getD0() << " z0 = " << track.getZ0() << " eta = " << track.getEta() << " and chi2/ndf = " << track.getChi2ndof() << std::endl;
  return 0;
}
  
