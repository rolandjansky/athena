/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
//Tool to initialize MonGroups only Once per run
//Nov. 2009
//Author Justin Griffiths <griffith@cern.ch>
///////////////////////////////////////////////////////////////////////////

#include "MDTMonGroupStruct.h"

MDTMonGroupStruct::MDTMonGroupStruct(ManagedMonitorToolBase *b, const std::string& title, ManagedMonitorToolBase::Interval_t low_stat_interval,  ManagedMonitorToolBase::MgmtAttr_t m, const std::string& lowStat_suffix):

  //These last two arguments are meant to place histograms in normal run_level folder
  //if running online

  //Overview Plots
  mongroup_overview_shift(b, "Muon/MuonRawDataMonitoring/"+title+"/Overview", /*ManagedMonitorToolBase::shift,*/  ManagedMonitorToolBase::run, m),
  mongroup_overview_shift_geometry(b, "Muon/MuonRawDataMonitoring/"+title+"/Overview/Expert/Geometry", /*ManagedMonitorToolBase::shift,*/  ManagedMonitorToolBase::run, m),
  mongroup_overview_recoMon(b, "Muon/MuonRawDataMonitoring/"+title+"/Overview/Expert/RecoMonitoring", /*ManagedMonitorToolBase::shift,*/  ManagedMonitorToolBase::run, m),
  mongroup_overview_shiftLumi(b, "Muon/MuonRawDataMonitoring/"+title+"/Overview"+lowStat_suffix, /*ManagedMonitorToolBase::shift,*/  low_stat_interval, m),
  mongroup_overview_shiftLumi_recoMon(b, "Muon/MuonRawDataMonitoring/"+title+"/Overview"+lowStat_suffix+"/Expert/RecoMonitoring", /*ManagedMonitorToolBase::shift,*/  low_stat_interval, m),
  mongroup_overview_expert(b , "Muon/MuonRawDataMonitoring/"+title+"/Overview", /*ManagedMonitorToolBase::expert,*/  ManagedMonitorToolBase::run, m, "",  "mergeRebinned"),
  mongroup_sectors_expert(b, "Muon/MuonRawDataMonitoring/"+title+"/Overview/HitOccupancies_Sectors", /*ManagedMonitorToolBase::expert,*/  ManagedMonitorToolBase::run, m, "", "mergeRebinned"),

  //Overview Regional Plots Shift
  mongroup_brA_shift(b, "Muon/MuonRawDataMonitoring/"+title+"/MDTBA/Overview", /*ManagedMonitorToolBase::shift,*/  ManagedMonitorToolBase::run, m ),
  mongroup_brC_shift(b, "Muon/MuonRawDataMonitoring/"+title+"/MDTBC/Overview", /*ManagedMonitorToolBase::shift,*/  ManagedMonitorToolBase::run, m ),
  mongroup_ecA_shift(b, "Muon/MuonRawDataMonitoring/"+title+"/MDTEA/Overview", /*ManagedMonitorToolBase::shift,*/  ManagedMonitorToolBase::run, m ),
  mongroup_ecC_shift(b, "Muon/MuonRawDataMonitoring/"+title+"/MDTEC/Overview", /*ManagedMonitorToolBase::shift,*/  ManagedMonitorToolBase::run, m ),
  mongroup_brA_shiftLumi(b, "Muon/MuonRawDataMonitoring/"+title+"/MDTBA/Overview"+lowStat_suffix, /*ManagedMonitorToolBase::shift,*/  low_stat_interval, m ),
  mongroup_brC_shiftLumi(b, "Muon/MuonRawDataMonitoring/"+title+"/MDTBC/Overview"+lowStat_suffix, /*ManagedMonitorToolBase::shift,*/  low_stat_interval, m ),
  mongroup_ecA_shiftLumi(b, "Muon/MuonRawDataMonitoring/"+title+"/MDTEA/Overview"+lowStat_suffix, /*ManagedMonitorToolBase::shift,*/  low_stat_interval, m ),
  mongroup_ecC_shiftLumi(b, "Muon/MuonRawDataMonitoring/"+title+"/MDTEC/Overview"+lowStat_suffix, /*ManagedMonitorToolBase::shift,*/  low_stat_interval, m ),
  //Overview Regional Plots Expert
  mongroup_brA_hits_expert(b, "Muon/MuonRawDataMonitoring/"+title+"/MDTBA/Overview/Hits", /*ManagedMonitorToolBase::expert,*/  ManagedMonitorToolBase::run, m ),
  mongroup_brC_hits_expert(b, "Muon/MuonRawDataMonitoring/"+title+"/MDTBC/Overview/Hits", /*ManagedMonitorToolBase::expert,*/  ManagedMonitorToolBase::run, m ),
  mongroup_ecA_hits_expert(b, "Muon/MuonRawDataMonitoring/"+title+"/MDTEA/Overview/Hits", /*ManagedMonitorToolBase::expert,*/  ManagedMonitorToolBase::run, m ),
  mongroup_ecC_hits_expert(b, "Muon/MuonRawDataMonitoring/"+title+"/MDTEC/Overview/Hits", /*ManagedMonitorToolBase::expert,*/  ManagedMonitorToolBase::run, m ),
  mongroup_brA_tdc_expert(b, "Muon/MuonRawDataMonitoring/"+title+"/MDTBA/Overview/TDC", /*ManagedMonitorToolBase::expert,*/  ManagedMonitorToolBase::run, m ),
  mongroup_brC_tdc_expert(b, "Muon/MuonRawDataMonitoring/"+title+"/MDTBC/Overview/TDC", /*ManagedMonitorToolBase::expert,*/  ManagedMonitorToolBase::run, m ),
  mongroup_ecA_tdc_expert(b, "Muon/MuonRawDataMonitoring/"+title+"/MDTEA/Overview/TDC", /*ManagedMonitorToolBase::expert,*/  ManagedMonitorToolBase::run, m ),
  mongroup_ecC_tdc_expert(b, "Muon/MuonRawDataMonitoring/"+title+"/MDTEC/Overview/TDC", /*ManagedMonitorToolBase::expert,*/  ManagedMonitorToolBase::run, m ),

  //Chamber by Chamber Plots
  mongroup_chambers_expert_MDTBA(b , "Muon/MuonRawDataMonitoring/"+title+"/MDTBA/Chambers", /*ManagedMonitorToolBase::expert,*/  ManagedMonitorToolBase::run, m ),
  mongroup_chambers_expert_MDTBC(b , "Muon/MuonRawDataMonitoring/"+title+"/MDTBC/Chambers", /*ManagedMonitorToolBase::expert,*/  ManagedMonitorToolBase::run, m ),
  mongroup_chambers_expert_MDTEA(b , "Muon/MuonRawDataMonitoring/"+title+"/MDTEA/Chambers", /*ManagedMonitorToolBase::expert,*/  ManagedMonitorToolBase::run, m ),
  mongroup_chambers_expert_MDTEC(b , "Muon/MuonRawDataMonitoring/"+title+"/MDTEC/Chambers", /*ManagedMonitorToolBase::expert,*/  ManagedMonitorToolBase::run, m )


{  
}

MDTMonGroupStruct::~MDTMonGroupStruct(){}

