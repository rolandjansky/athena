/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTObjects/HTTDataFlowInfo.h"

#include <ostream>

void HTTDataFlowInfo::reset()
{
  nMappedHits_1st_layer.clear();
  nMappedHits_1st_total = 0;

  nClusters_1st_layer.clear();
  nClusters_1st_total = 0;

  nRoads_1st_total = 0;
  nRoads_1st_7hits = 0;
  nRoads_1st_8hits = 0;

  nTracks_1st_total = 0;
  nTracks_1st_passChi2 = 0;
  nTracks_1st_afterOR = 0;

  nTracks_1st_per_patt_total.clear();
  nTracks_1st_per_patt_passChi2.clear();
  nTracks_1st_per_patt_afterOR.clear();

  nConstants_1st_fitter = 0;
  nConstants_1st_passChi2 = 0;
  nConstants_1st_afterOR = 0;

  nRoads_2nd_total = 0;

  nTracks_2nd_total = 0;
  nTracks_2nd_passChi2 = 0;
  nTracks_2nd_afterOR = 0;

  nConstants_2nd_extrapolate = 0;
  nConstants_2nd_fitter = 0;
  nConstants_2nd_passChi2 = 0;
  nConstants_2nd_afterOR = 0;

  nTruthTracks = 0;
  nOfflineTracks = 0;

  nTracks_1st_over_nOfflineTracks = 0;
  nTracks_2nd_over_nOfflineTracks = 0;
}

std::ostream& operator<<(std::ostream& s, HTTDataFlowInfo const& info)
{
  for (unsigned i = 0; i < info.nMappedHits_1st_layer.size(); i++) {
    s << "nMappedHits_1st_L" << i << " = " << info.nMappedHits_1st_layer[i] << ", ";
  }
  s << "nMappedHits_1st_total = " << info.nMappedHits_1st_total << "\n";

  for (unsigned i = 0; i < info.nClusters_1st_layer.size(); i++) {
    s << "nClusters_1st_L" << i << " = " << info.nClusters_1st_layer[i] << ", ";
  }
  s << "nClusters_1st_total = " << info.nClusters_1st_total << "\n";

  s << "nRoads_1st_total = " << info.nRoads_1st_total << ", "
    << "nRoads_1st_7hits = " << info.nRoads_1st_7hits << ", "
    << "nRoads_1st_7hits = " << info.nRoads_1st_8hits << "\n";

  s << "nTracks_1st_total = " << info.nTracks_1st_total << ", "
    << "nTracks_1st_passChi2 = " << info.nTracks_1st_passChi2 << ", "
    << "nTracks_1st_afterOR = " << info.nTracks_1st_afterOR << "\n";

  s << "nTracks_1st_per_patt_total = {";
  for (unsigned i = 0; i < info.nTracks_1st_per_patt_total.size(); i++) {
    s << "pattern #" << i << ": " << info.nTracks_1st_per_patt_total[i];
    s << (i != info.nTracks_1st_per_patt_total.size() - 1 ? ", " : "}\n");
  }

  s << "nTracks_1st_per_patt_passChi2 = {";
  for (unsigned i = 0; i < info.nTracks_1st_per_patt_passChi2.size(); i++) {
    s << "pattern #" << i << ": " << info.nTracks_1st_per_patt_passChi2[i];
    s << (i != info.nTracks_1st_per_patt_passChi2.size() - 1 ? ", " : "}\n");
  }

  s << "nTracks_1st_per_patt_afterOR = {";
  for (unsigned i = 0; i < info.nTracks_1st_per_patt_afterOR.size(); i++) {
    s << "pattern #" << i << ": " << info.nTracks_1st_per_patt_afterOR[i];
    s << (i != info.nTracks_1st_per_patt_afterOR.size() - 1 ? ", " : "}\n");
  }

  s << "nConstants_1st_fitter = " << info.nConstants_1st_fitter << ", "
    << "nConstants_1st_passChi2 = " << info.nConstants_1st_passChi2 << ", "
    << "nConstants_1st_afterOR = " << info.nConstants_1st_afterOR << "\n";

  s << "nRoads_2nd_total = " << info.nRoads_2nd_total << "\n";

  s << "nTracks_2nd_total = " << info.nTracks_2nd_total << ", "
    << "nTracks_2nd_passChi2 = " << info.nTracks_2nd_passChi2 << ", "
    << "nTracks_2nd_afterOR = " << info.nTracks_2nd_afterOR << "\n";

  s << "nConstants_2nd_extrapolate = " << info.nConstants_2nd_extrapolate << ", "
    << "nConstants_2nd_fitter = " << info.nConstants_2nd_fitter << ", "
    << "nConstants_2nd_passChi2 = " << info.nConstants_2nd_passChi2 << ", "
    << "nConstants_2nd_afterOR = " << info.nConstants_2nd_afterOR << "\n";

  s << "nTruthTracks = " << info.nTruthTracks << ", "
    << "nOfflineTracks = " << info.nOfflineTracks << "\n";

  s << "nTracks_1st_over_nOfflineTracks = " << info.nTracks_1st_over_nOfflineTracks << ", "
    << "nTracks_2nd_over_nOfflineTracks = " << info.nTracks_2nd_over_nOfflineTracks;

  return s;
}
