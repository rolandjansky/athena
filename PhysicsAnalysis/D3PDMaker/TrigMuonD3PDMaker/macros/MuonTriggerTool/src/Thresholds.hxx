#ifndef Thresholds_hxx
#define Thresholds_hxx

#include "../MuonTriggerTool/MuonHypothesis.h"

namespace MuonHypoThresholds {

const float hypo_etaregion[] = {1.05, 1.5, 2.0, 9.9};
size_t hypo_binsize = sizeof(hypo_etaregion)/sizeof(hypo_etaregion[0]);


// EF combined muon
const double ef_cb_4[] =   {  3.93,  3.91,  3.88,  3.88};
const double ef_cb_6[] =   {  5.88,  5.81,  5.78,  5.76};
const double ef_cb_10[] =  {  9.77,  9.67,  9.62,  9.57};
const double ef_cb_13[] =  { 12.67, 12.55, 12.49, 12.46};
const double ef_cb_15[] =  { 14.63, 14.49, 14.42, 14.38};
const double ef_cb_18[] =  { 17.53, 17.39, 17.34, 17.28};
const double ef_cb_20[] =  { 19.47, 19.33, 19.30, 19.22};
const double ef_cb_22[] =  { 21.40, 21.27, 21.25, 21.16};
const double ef_cb_24[] =  { 23.34, 23.19, 23.14, 23.06};
const double ef_cb_30[] =  { 29.17, 29.03, 29.00, 28.92};

const MuonHypothesis::EF_thresholdPair ef_cb_pair[] = {
  MuonHypothesis::EF_thresholdPair("4GeV",   ef_cb_4),
  MuonHypothesis::EF_thresholdPair("6GeV",   ef_cb_6),
  MuonHypothesis::EF_thresholdPair("10GeV",  ef_cb_10),
  MuonHypothesis::EF_thresholdPair("13GeV",  ef_cb_13),
  MuonHypothesis::EF_thresholdPair("15GeV",  ef_cb_15),
  MuonHypothesis::EF_thresholdPair("18GeV",  ef_cb_18),
  MuonHypothesis::EF_thresholdPair("20GeV",  ef_cb_20),
  MuonHypothesis::EF_thresholdPair("22GeV",  ef_cb_22),
  MuonHypothesis::EF_thresholdPair("24GeV",  ef_cb_24),
  MuonHypothesis::EF_thresholdPair("30GeV",  ef_cb_30),
};

const MuonHypothesis::EF_thresholdMap ef_cb_map(ef_cb_pair, ef_cb_pair + sizeof(ef_cb_pair)/sizeof(ef_cb_pair[0]));



// EF stand-alone muon
const double ef_sa_40[] =            { 31.5, 30.0, 28.5, 32.5};
const double ef_sa_40_barrelOnly[] = { 31.5,1000.0,1000.0,1000.0};

const MuonHypothesis::EF_thresholdPair ef_sa_pair[] = {
  MuonHypothesis::EF_thresholdPair("40GeV",            ef_sa_40),
  MuonHypothesis::EF_thresholdPair("40GeV_barrelOnly", ef_sa_40_barrelOnly),
};

const MuonHypothesis::EF_thresholdMap ef_sa_map(ef_sa_pair, ef_sa_pair + sizeof(ef_sa_pair)/sizeof(ef_sa_pair[0]));
}
#endif
// eof
