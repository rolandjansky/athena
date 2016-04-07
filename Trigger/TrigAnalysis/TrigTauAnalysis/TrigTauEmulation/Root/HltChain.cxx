/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2

#include "TrigTauEmulation/HltChain.h"
#include "TrigTauEmulation/HltItem.h"

std::map<std::string, HltChain> HltParsing::chains() 

{

  std::map<std::string, HltChain> ch;

  // Single tau

  // tracktwo chains
  ch["HLT_tau25_perf_tracktwo"]= HltChain({
      HltItem("tau25_perf_tracktwo", "TAU12IM")}, "TAU12IM");

  ch["HLT_tau25_idperf_tracktwo"]= HltChain({
      HltItem("tau25_idperf_tracktwo", "TAU12")}, "TAU12");

  ch["HLT_tau25_perf_tracktwo_L1TAU12"]= HltChain({
      HltItem("tau25_perf_tracktwo", "TAU12")}, "TAU12");

  ch["HLT_tau25_loose1_tracktwo"] = HltChain({
      HltItem("tau25_loose1_tracktwo", "TAU12IM")}, "TAU12IM");

  ch["HLT_tau25_medium1_tracktwo"] = HltChain({
      HltItem("tau25_medium1_tracktwo", "TAU12IM")}, "TAU12IM");

  ch["HLT_tau25_medium1_tracktwo_L1TAU12"] = HltChain({
      HltItem("tau25_medium1_tracktwo", "TAU12")}, "TAU12");

  ch["HLT_tau25_medium1_tracktwo_L1TAU12IL"] = HltChain({
      HltItem("tau25_medium1_tracktwo", "TAU12IL")}, "TAU12IL");

  ch["HLT_tau25_medium1_tracktwo_L1TAU12IT"] = HltChain({
      HltItem("tau25_medium1_tracktwo", "TAU12IT")}, "TAU12IT");

  ch["HLT_tau25_tight1_tracktwo"] = HltChain({
      HltItem("tau25_tight1_tracktwo", "TAU12IM")}, "TAU12IM");

  ch["HLT_tau35_perf_tracktwo"]= HltChain({
      HltItem("tau35_perf_tracktwo", "TAU20IM")}, "TAU20IM");

  ch["HLT_tau35_loose1_tracktwo"] = HltChain({
      HltItem("tau35_loose1_tracktwo", "TAU20IM")}, "TAU20IM");

  ch["HLT_tau35_medium1_tracktwo"] = HltChain({
      HltItem("tau35_medium1_tracktwo", "TAU20IM")}, "TAU20IM");

  ch["HLT_tau35_medium1_tracktwo_L1TAU20"] = HltChain({
      HltItem("tau35_medium1_tracktwo", "TAU20")}, "TAU20");

  ch["HLT_tau35_medium1_tracktwo_L1TAU20IL"] = HltChain({
      HltItem("tau35_medium1_tracktwo", "TAU20IL")}, "TAU20IL");

  ch["HLT_tau35_medium1_tracktwo_L1TAU20IT"] = HltChain({
      HltItem("tau35_medium1_tracktwo", "TAU20IT")}, "TAU20IT");

  ch["HLT_tau35_tight1_tracktwo"] = HltChain({
      HltItem("tau35_tight1_tracktwo", "TAU20IM")}, "TAU20IM");

  ch["HLT_tau80_medium1_tracktwo"]= HltChain({
      HltItem("tau80_medium1_tracktwo", "TAU40")}, "TAU40");

  ch["HLT_tau125_medium1_tracktwo"]= HltChain({
      HltItem("tau125_medium1_tracktwo", "TAU60")}, "TAU60");

  ch["HLT_tau160_medium1_tracktwo"]= HltChain({
      HltItem("tau160_medium1_tracktwo", "TAU60")}, "TAU60");



  // # perf_ptonly 
  ch["HLT_tau0_perf_ptonly_L1TAU12"] = HltChain({
      HltItem("tau0_perf_ptonly", "TAU12")}, "TAU12");

  ch["HLT_tau0_perf_ptonly_L1TAU60"] = HltChain({
      HltItem("tau0_perf_ptonly", "TAU60")}, "TAU60");

  ch["HLT_tau5_perf_ptonly_L1TAU8"] = HltChain({
      HltItem("tau5_perf_ptonly", "TAU8")}, "TAU8");

  ch["HLT_tau25_perf_ptonly"]= HltChain({
      HltItem("tau25_perf_ptonly", "TAU12IM")}, "TAU12IM");

  ch["HLT_tau25_perf_ptonly_L1TAU12"]= HltChain({
      HltItem("tau25_perf_ptonly", "TAU12")}, "TAU12");

  ch["HLT_tau25_loose1_ptonly"]= HltChain({
      HltItem("tau25_loose1_ptonly", "TAU12IM")}, "TAU12IM");

  ch["HLT_tau25_medium1_ptonly"]= HltChain({
      HltItem("tau25_medium1_ptonly", "TAU12IM")}, "TAU12IM");

  ch["HLT_tau25_tight1_ptonly"]= HltChain({
      HltItem("tau25_tight1_ptonly", "TAU12IM")}, "TAU12IM");

  ch["HLT_tau25_medium1_mvonly"]= HltChain({
      HltItem("tau25_medium1_mvonly", "TAU12IM")}, "TAU12IM");

  ch["HLT_tau35_perf_ptonly"]= HltChain({
      HltItem("tau35_perf_ptonly", "TAU20IM")}, "TAU20IM");

  ch["HLT_tau35_loose1_ptonly"]= HltChain({
      HltItem("tau35_loose1_ptonly", "TAU20IM")}, "TAU20IM");

  ch["HLT_tau35_tight1_ptonly"]= HltChain({
      HltItem("tau35_tight1_ptonly", "TAU20IM")}, "TAU20IM");

  // Tau + MET
  ch["HLT_tau35_medium1_tracktwo_xe70_L1XE45"] = HltChain({
      HltItem("tau35_medium1_tracktwo", "TAU20IM"),
      HltItem("xe70", "XE45")}, "TAU20IM_2J20_XE45");


  // e + tau
  ch["HLT_e17_medium_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25"] = HltChain({
      HltItem("e17_medium", "EM15HI"),
      HltItem("tau25_medium1_tracktwo", "TAU12IM")}, "DR-EM15TAU12I-J25");

  ch["HLT_e17_lhmedium_tau25_medium1_tracktwo"] = HltChain({
      HltItem("e17_lhmedium", "EM15HI"), 
      HltItem("tau25_medium1_tracktwo", "TAU12IM")}, "EM15HI_2TAU12IM_J25_3J12");

  ch["HLT_e17_medium_tau25_medium1_tracktwo_xe50"] = HltChain({
      HltItem("e17_medium", "EM15HI"),
      HltItem("tau25_medium1_tracktwo", "TAU12IM"),
      HltItem("xe50", "XE35")}, "EM15HI_2TAU12IM_XE35");

  // e + tau + met
  ch["HLT_e17_lhmedium_tau25_medium1_tracktwo_xe50"] = HltChain({
      HltItem("e17_lhmedium", "EM15HI"),
      HltItem("tau25_medium1_tracktwo", "TAU12IM"),
      HltItem("xe50", "XE35")}, "EM15HI_2TAU12IM_XE35");



  // mu + tau triggers
  ch["HLT_mu14_tau25_medium1_tracktwo"] = HltChain({
      HltItem("mu14", "MU10"),
      HltItem("tau25_medium1_tracktwo", "TAU12IM")}, "MU10_TAU12IM_J25_2J12");

  ch["HLT_mu14_tau35_medium1_tracktwo"] = HltChain({
      HltItem("mu14", "MU10"),
      HltItem("tau35_medium1_tracktwo", "TAU20IM")}, "MU10_TAU20IM_J25_2J12");

  ch["HLT_mu14_tau35_medium1_tracktwo_L1TAU20"] = HltChain({
      HltItem("mu14", "MU10"),
      HltItem("tau35_medium1_tracktwo", "TAU20")}, "MU10_TAU20");

  ch["HLT_mu14_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12-J25"] = HltChain({
      HltItem("mu14", "MU10"), 
      HltItem("tau25_medium1_tracktwo", "TAU12IM")}, "DR-MU10TAU12I_TAU12-J25");

  ch["HLT_mu14_tau25_medium1_tracktwo_L1DR-MU10TAU12I"] = HltChain({
      HltItem("mu14", "MU10"), 
      HltItem("tau25_medium1_tracktwo", "TAU12IM")}, "DR-MU10TAU12I");

  ch["HLT_mu14_tau25_medium1_tracktwo_L1MU10_TAU12I-J25"] = HltChain({
      HltItem("mu14", "MU10"),
      HltItem("tau25_medium1_tracktwo", "TAU12IM")}, "MU10_TAU12I-J25");

  // HLT_mu14_iloose is not in the MC15 menu ...
  // HLT_mu14_iloose_tau25_medium1_tracktwo
  // HLT_mu14_iloose_tau35_medium1_tracktwo

  // mu + tau + met trigger
  ch["HLT_mu14_tau25_medium1_tracktwo_xe50"] = HltChain({
      HltItem("mu14", "MU10"),
      HltItem("tau25_medium1_tracktwo", "TAU12IM"),
      HltItem("xe50", "XE35")}, "MU10_TAU12IM_XE35"); 
  // HLT_mu14_iloose_tau25_medium1_tracktwo_xe50


  // di-tau triggers
  ch["HLT_tau35_loose1_tracktwo_tau25_loose1_tracktwo"] = HltChain({
      HltItem("tau35_loose1_tracktwo", "TAU20IM"),
      HltItem("tau25_loose1_tracktwo", "TAU12IM")}, "TAU20IM_2TAU12IM_J25_2J20_3J12");


  return ch;

}
