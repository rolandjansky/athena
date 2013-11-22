/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonThresholds_h
#define MuonThresholds_h

#include "TrigMuonEfficiency/MuonHypothesis.h"

namespace MuonHypoThresholds {

const float hypo_etaregion[] = {1.05, 1.5, 2.0, 9.9};
size_t hypo_binsize = sizeof(hypo_etaregion)/sizeof(hypo_etaregion[0]);

// EF combined muon
const double ef_cb_4[]  =  {  3.93,  3.91,  3.88,  3.88};
const double ef_cb_6[]  =  {  5.88,  5.81,  5.78,  5.76};
const double ef_cb_8[]  =  {  7.82,  7.74,  7.70,  7.72};
const double ef_cb_10[] =  {  9.77,  9.67,  9.62,  9.57};
const double ef_cb_13[] =  { 12.67, 12.55, 12.49, 12.46};
const double ef_cb_15[] =  { 14.63, 14.49, 14.42, 14.38};
const double ef_cb_18[] =  { 17.53, 17.39, 17.34, 17.28};
const double ef_cb_24[] =  { 23.34, 23.19, 23.14, 23.06};
const double ef_cb_36[] =  { 34.96, 34.78, 34.69, 34.63};
const double ef_cb_40[] =  { 38.76, 38.54, 38.38, 38.31};

const MuonHypothesis::EF_thresholdPair ef_cb_pair[] = {
  MuonHypothesis::EF_thresholdPair("4GeV",   ef_cb_4),
  MuonHypothesis::EF_thresholdPair("6GeV",   ef_cb_6),
  MuonHypothesis::EF_thresholdPair("8GeV",   ef_cb_8),
  MuonHypothesis::EF_thresholdPair("10GeV",  ef_cb_10),
  MuonHypothesis::EF_thresholdPair("13GeV",  ef_cb_13),
  MuonHypothesis::EF_thresholdPair("15GeV",  ef_cb_15),
  MuonHypothesis::EF_thresholdPair("18GeV",  ef_cb_18),
  MuonHypothesis::EF_thresholdPair("24GeV",  ef_cb_24),
  MuonHypothesis::EF_thresholdPair("36GeV",  ef_cb_36),
  MuonHypothesis::EF_thresholdPair("40GeV",  ef_cb_40),
};

const MuonHypothesis::EF_thresholdMap ef_cb_map(ef_cb_pair, ef_cb_pair + sizeof(ef_cb_pair)/sizeof(ef_cb_pair[0]));




// EF stand-alone muon
const double ef_sa_40[] =            { 31.5, 30.0, 28.5, 32.5};
const double ef_sa_40_barrelOnly[] = { 31.5,1000.0,1000.0,1000.0};
const double ef_sa_40_uptoEC2[] =    { 31.5, 30.0, 28.5,  1000.0};
const double ef_sa_50_barrelOnly[] = { 45.0,1000.0,1000.0,1000.0};

const MuonHypothesis::EF_thresholdPair ef_sa_pair[] = {
  MuonHypothesis::EF_thresholdPair("40GeV",            ef_sa_40),
  MuonHypothesis::EF_thresholdPair("40GeV_barrelOnly", ef_sa_40_barrelOnly),
  MuonHypothesis::EF_thresholdPair("40GeV_uptoEC2",    ef_sa_40_uptoEC2),
  MuonHypothesis::EF_thresholdPair("50GeV_barrelOnly", ef_sa_50_barrelOnly),
};

const MuonHypothesis::EF_thresholdMap ef_sa_map(ef_sa_pair, ef_sa_pair + sizeof(ef_sa_pair)/sizeof(ef_sa_pair[0]));
}
#endif
// eof
