/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
//Tool to initialize MonGroups only Once per run
//Nov. 2009
//Author Justin Griffiths <griffith@cern.ch>
///////////////////////////////////////////////////////////////////////////

#ifndef MDTMONGROUPSTUCT_H
#define MDTMONGROUPSTUCT_H
#include <AthenaMonitoring/ManagedMonitorToolBase.h>
#include <string>

class MDTMonGroupStruct {

 public:
  
  MDTMonGroupStruct(ManagedMonitorToolBase*, const std::string& title, ManagedMonitorToolBase::Interval_t, ManagedMonitorToolBase::MgmtAttr_t, const std::string& );
  virtual ~MDTMonGroupStruct();
  
  //Overview Plots
  ManagedMonitorToolBase::MonGroup mongroup_overview_shift;
  ManagedMonitorToolBase::MonGroup mongroup_overview_shift_geometry;
  ManagedMonitorToolBase::MonGroup mongroup_overview_recoMon;
  ManagedMonitorToolBase::MonGroup mongroup_overview_shiftLumi;
  ManagedMonitorToolBase::MonGroup mongroup_overview_shiftLumi_recoMon;
  ManagedMonitorToolBase::MonGroup mongroup_overview_expert;
  ManagedMonitorToolBase::MonGroup mongroup_sectors_expert;

  //Overview Regional Plots Shift
  ManagedMonitorToolBase::MonGroup mongroup_brA_shift;
  ManagedMonitorToolBase::MonGroup mongroup_brC_shift;
  ManagedMonitorToolBase::MonGroup mongroup_ecA_shift;
  ManagedMonitorToolBase::MonGroup mongroup_ecC_shift;
  ManagedMonitorToolBase::MonGroup mongroup_brA_shiftLumi;
  ManagedMonitorToolBase::MonGroup mongroup_brC_shiftLumi;
  ManagedMonitorToolBase::MonGroup mongroup_ecA_shiftLumi;
  ManagedMonitorToolBase::MonGroup mongroup_ecC_shiftLumi;
  //Overview Regional Plots Expert
  ManagedMonitorToolBase::MonGroup mongroup_brA_hits_expert;
  ManagedMonitorToolBase::MonGroup mongroup_brC_hits_expert;
  ManagedMonitorToolBase::MonGroup mongroup_ecA_hits_expert;
  ManagedMonitorToolBase::MonGroup mongroup_ecC_hits_expert;
  ManagedMonitorToolBase::MonGroup mongroup_brA_tdc_expert;
  ManagedMonitorToolBase::MonGroup mongroup_brC_tdc_expert;
  ManagedMonitorToolBase::MonGroup mongroup_ecA_tdc_expert;
  ManagedMonitorToolBase::MonGroup mongroup_ecC_tdc_expert;

  //Chamber by Chamber Plots
  ManagedMonitorToolBase::MonGroup mongroup_chambers_expert_MDTBA;
  ManagedMonitorToolBase::MonGroup mongroup_chambers_expert_MDTBC;
  ManagedMonitorToolBase::MonGroup mongroup_chambers_expert_MDTEA;
  ManagedMonitorToolBase::MonGroup mongroup_chambers_expert_MDTEC;

};
#endif
