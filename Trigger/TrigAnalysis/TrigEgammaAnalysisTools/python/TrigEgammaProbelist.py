# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# this file contains the useful lists of triggers for quick configuration
# and a default monitoring configuration
# First set of lists useful for monitoring
# Second set of lists useful for validation 
# The final configuration should be placed in TrigEgammaMonitoring
# Create dictionary for MaM

#Tag items
triggerTags = [ 
  'HLT_e24_lhmedium_iloose_L1EM18VH',
  'HLT_e24_lhmedium_iloose_L1EM20VH',
  'HLT_e24_lhtight_iloose',
  'HLT_e26_lhtight_iloose',
  # Primary cut-based electron triggers
  'HLT_e24_medium_iloose_L1EM18VH',
  'HLT_e24_medium_iloose_L1EM20VH',
  'HLT_e24_tight_iloose',
  'HLT_e26_tight_iloose'
]


monitoring_mam = {
  'primary_single_ele'      : 'HLT_e26_lhtight_iloose',
  'primary_single_cut_ele'  : 'HLT_e24_tight_iloose',
  'primary_single_pho'      : 'HLT_g120_loose'}

# L1 Items
monitoring_L1Calo = [
  'L1_EM20VH',
  'L1_EM20VHI']

# High pt electron triggers that cannot be monitored with TP Analysis
monitoring_electron=[
  'HLT_e60_lhmedium',
  'HLT_e60_medium',
  'HLT_e120_lhloose',
  'HLT_e120_loose',
  'HLT_e140_lhloose'
]

# Full list of monitoring for photons
monitoring_photon=[
  'HLT_g25_loose',
  'HLT_g35_loose',
  "HLT_g25_medium",
  "HLT_g35_medium",
  'HLT_g20_tight',
  'HLT_g120_loose',
  'HLT_g140_loose'
]

# Lowest single electron triggers for TP analysis
monitoringTP_electron =[
  'HLT_e24_lhmedium_L1EM18VH',
  'HLT_e24_lhmedium_L1EM20VH',
  'HLT_e24_medium_L1EM20VH',
  'HLT_e24_lhmedium_iloose',
  'HLT_e24_medium_iloose',
  'HLT_e24_lhmedium_nod0_iloose',
  'HLT_e24_lhtight_iloose',
  'HLT_e24_tight_iloose',
  'HLT_e26_lhtight_iloose',
  'HLT_e26_lhtight_nod0_iloose',
  'HLT_e26_lhtight_nod0_ivarloose',
  'HLT_e26_lhtight_nod0_ringer_iloose',
  'HLT_e26_lhtight_nod0_ringer_ivarloose',
  'HLT_e28_lhtight_nod0_iloose',
]

#Zee TP triggers
monitoringTP_electronZee = [
  'HLT_e24_lhtight_L1EM20VH_e15_etcut_Zee',
  'HLT_e26_lhtight_e15_etcut_Zee',
]

#Jpsiee TP triggers
monitoringTP_electronJpsiee = [
  "HLT_e5_loose",
  "HLT_e5_lhloose",
  "HLT_e5_vloose",
  "HLT_e5_lhvloose",
]

default = [
  'HLT_e24_lhmedium_iloose_L1EM18VH',# Primary LH electron triggers
  'HLT_e24_lhmedium_iloose_L1EM20VH',
  'HLT_e24_lhtight_iloose',
  'HLT_e24_lhtight_iloose_L1EM20VH',
  'HLT_e26_lhtight_iloose',
  'HLT_e26_lhtight_smooth_iloose',
  'HLT_e60_lhmedium',
  'HLT_e120_lhloose',
  'HLT_e140_lhloose',
  # Primary cut-based electron triggers
  'HLT_e24_medium_iloose_L1EM18VH',
  'HLT_e24_medium_iloose_L1EM20VH',
  'HLT_e24_tight_iloose',
  'HLT_e26_tight_iloose',
  'HLT_e60_medium',
  'HLT_e120_loose',
  'HLT_e140_loose',
  # Non-isolated
  'HLT_e24_lhmedium_L1EM18VH',
  'HLT_e24_medium_L1EM18VH',
  'HLT_e24_lhtight_L1EM20VH',
  'HLT_e24_tight_L1EM20VH',
  'HLT_e24_lhmedium_L1EM20VHI',
  # Single photon
  'HLT_g120_loose',
  'HLT_g140_loose',
  # Suppporting triggers for alignment studies / LH ineffiency
  'HLT_e24_lhmedium_cutd0dphideta_iloose_L1EM18VH',
  'HLT_e24_lhmedium_nod0_iloose_L1EM18VH',
  'HLT_e24_lhmedium_nodeta_iloose_L1EM18VH',
  'HLT_e24_lhmedium_nodphires_iloose_L1EM18VH',
  'HLT_e24_lhmedium_cutd0dphideta_iloose_L1EM20VH',
  'HLT_e24_lhmedium_nod0_iloose_L1EM20VH',
  'HLT_e24_lhmedium_nodeta_iloose_L1EM20VH',
  'HLT_e24_lhmedium_nodphires_iloose_L1EM20VH',
  'HLT_e24_lhmedium_iloose_HLTCalo_L1EM18VH',
  'HLT_e24_lhmedium_iloose_HLTCalo_L1EM20VH',
  'HLT_e24_lhmedium_iloose_L2EFCalo_L1EM18VH',
  'HLT_e24_lhmedium_iloose_L2EFCalo_L1EM20VH',
  'HLT_e26_lhtight_cutd0dphideta_iloose',
  'HLT_e26_lhtight_iloose_HLTCalo',
  'HLT_e26_lhtight_iloose_L2EFCalo',
  'HLT_e24_lhtight_iloose_HLTCalo_L1EM20VH',
  'HLT_e24_lhtight_iloose_L2EFCalo_L1EM20VH',
  'HLT_e60_lhmedium_HLTCalo',
  'HLT_e60_lhmedium_L2EFCalo',
  'HLT_e60_lhmedium_nod0',
  'HLT_e60_lhmedium_cutd0dphideta',
  # Supporting trigger for background
  #'HLT_e24_vloose_L1EM18VH', 
  #'HLT_e24_vloose_L1EM20VH',  
  #'HLT_e24_lhvloose_L1EM18VH', 
  #'HLT_e24_lhvloose_L1EM20VH',  
  #'HLT_e26_lhvloose_L1EM20VH',
  #'HLT_e26_vloose_L1EM20VH',
  #'HLT_e60_lhvloose',
  # Rerun mode for diobject
  'HLT_e12_lhloose_L1EM10VH',
  'HLT_e15_lhloose_L1EM13VH',
  'HLT_e17_loose'
  'HLT_e17_lhloose',
  'HLT_e12_loose_L1EM10VH',
  'HLT_e15_loose_L1EM13VH',
  'HLT_e17_loose_L1EM15',
  'HLT_e17_lhloose_L1EM15',
  'HLT_e13_etcut_trkcut',
	# Rerun mode for photon
  # Performance and supporting triggers 
  #"HLT_g0_perf_L1EM3_EMPTY",
  #"HLT_e0_perf_L1EM3_EMPTY",
  #"HLT_e0_perf_L1EM15",
  #"HLT_g0_perf_L1EM15",
  #"HLT_g10_etcut",
  #"HLT_g10_loose",
  #"HLT_e5_etcut",
  #"HLT_e10_etcut_L1EM7",
  #"HLT_e5_lhloose_idperf",
  #"HLT_e5_loose_idperf",
  #"HLT_e5_loose_L2Star_idperf",
  #"HLT_e5_loose",
  #'HLT_e24_medium_idperf_L1EM20VH',
  #'HLT_e24_lhmedium_idperf_L1EM20VH',
  #'HLT_e25_etcut_L1EM15',
  #'HLT_e30_etcut_L1EM15',
  #'L1_EM18VH',
  #'L1_EM20VH',
  #'L1_EM20VHI',
]


# probeListLowMidPtSupportingTriggers
# probeListLowMidPtPhysicsTriggers
# probeListHighPtSupportingTriggers
# probeListHighPtPhysicsTriggers
# probeListPhotonTriggers
# probeListLHTriggers -- short lilst to compare with IsEM triggers
# probeListIsEMTriggers
# probeListAltL1Seeds -- compare with same triggers with nominal L1 seed
# probeListPerfTriggers -- id performance triggers for ID tracking studies
# probeListRun1TrkTriggers -- compare with same items without L2Star in name
# probeListRun1PidTriggers -- compare with sames items without 1 in name
# probeListAltSequence -- compare with same trigers without HLTCalo or L2EFCalo in name
# probeListMisAlignmentTriggers -- comapre with same trigger without misalignment configuration in name
# ProbeList 1 - Low/mid pt supporting triggers
# Tools to Run
# EmulationTool,Counts,Efficiency,Perf
# Samples to run - Zee and a background such as dijet or JF17 or any JF sample

probeListLowMidPtSupportingTriggers = [
	"HLT_e0_L2Star_perf_L1EM15",
	"HLT_e0_L2Star_perf_L1EM3",
	"HLT_e0_perf_L1EM15",
	"HLT_e0_perf_L1EM3",
	"HLT_e10_etcut_L1EM7",
	"HLT_e13_etcut_L1EM10_W-MT25",
	"HLT_e13_etcut_L1EM10_W-MT30",
	"HLT_e13_etcut_L1W-NOMATCH",
	"HLT_e13_etcut_L1W-NOMATCH_W-05RO-XEEMHT",
	"HLT_e13_etcut_trkcut",
	"HLT_e14_etcut",
	"HLT_e17_etcut_L1EM15",
	"HLT_e17_etcut_L2StarA_L1EM15",
	"HLT_e17_etcut_L2StarB_L1EM15",
	"HLT_e17_etcut_L2StarC_L1EM15",
	"HLT_e17_etcut_L2Star_idperf_L1EM15",
	"HLT_e17_etcut_idperf_L1EM15",
	"HLT_e18_etcut_L1EM15_W-MT35",
	"HLT_e18_etcut_trkcut",
	"HLT_e20_etcut_L1EM12",
	"HLT_e30_etcut_L1EM15",
	"HLT_e3_etcut",
	"HLT_e3_etcut_L1EM3_EMPTY",
	"HLT_e4_etcut",
	"HLT_e5_etcut",
	"HLT_e9_etcut",
  "HLT_e24_etcut",
  "HLT_e25_etcut_L1EM15",
]

# ProbeList 2 Low/mid pt physics triggers
# EmulationTool,Eff,Counts,Perf
# Samples to run Zee and any background sample dijet, JFXX
probeListLowMidPtPhysicsTriggers = [
	"HLT_e10_lhvloose_L1EM7",
	"HLT_e10_vloose_L1EM7",
	"HLT_e12_lhloose",
	"HLT_e12_loose",
	"HLT_e12_lhvloose_L1EM10VH",
	"HLT_e12_vloose_L1EM10VH",
	"HLT_e15_lhloose_L1EM13VH",
	"HLT_e15_lhvloose_L1EM13VH",
	"HLT_e15_lhvloose_L1EM7",
	"HLT_e15_loose_L1EM13VH",
	"HLT_e15_vloose_L1EM13VH",
	"HLT_e15_vloose_L1EM7",
	"HLT_e17_lhloose",
	"HLT_e17_lhmedium",
	"HLT_e17_lhmedium_iloose",
	"HLT_e17_lhvloose",
	"HLT_e17_loose",
	"HLT_e17_medium",
	"HLT_e17_medium_iloose",
	"HLT_e17_vloose",
	"HLT_e20_lhmedium",
	"HLT_e20_lhvloose",
	"HLT_e20_medium",
	"HLT_e20_vloose",
	"HLT_e20_vloose_L1EM12",
	"HLT_e24_lhmedium_L1EM15VH",
	"HLT_e24_lhmedium_L1EM18VH",
	"HLT_e24_lhmedium_iloose_L1EM18VH",
	"HLT_e24_lhmedium_iloose_L1EM20VH",
	"HLT_e24_lhtight_iloose",
	"HLT_e24_lhvloose_L1EM18VH",
	"HLT_e24_lhvloose_L1EM20VH",
	"HLT_e24_medium_L1EM15VH",
	"HLT_e24_medium_L1EM18VH",
	"HLT_e24_medium_iloose_L1EM18VH",
	"HLT_e24_medium_iloose_L1EM20VH",
	"HLT_e24_tight_L1EM20VH",
	"HLT_e24_tight_iloose",
	"HLT_e24_vloose_L1EM18VH",
	"HLT_e24_vloose_L1EM20VH",
	"HLT_e25_lhvloose_L1EM15",
	"HLT_e25_vloose_L1EM15",
	"HLT_e26_lhtight_iloose",
	"HLT_e26_lhvloose_L1EM20VH",
	"HLT_e26_tight_iloose",
	"HLT_e26_vloose_L1EM20VH",
	"HLT_e30_lhvloose_L1EM15",
	"HLT_e30_vloose_L1EM15",
]

# Compare LHTrigger to IsEMTrigger
probeListLHTriggers = [
  "HLT_e10_lhvloose_L1EM7",
  "HLT_e12_lhloose",
  "HLT_e17_lhloose",
  "HLT_e17_lhmedium",
  "HLT_e17_lhmedium_iloose",
  "HLT_e17_lhvloose",
  "HLT_e20_lhmedium",
  "HLT_e20_lhvloose",
  "HLT_e24_lhtight_iloose",
  "HLT_e26_lhtight_iloose",
  "HLT_e30_lhvloose_L1EM15",
  "HLT_e60_lhmedium",
]

# Compare with LH trigger above
probeListIsEMTriggers = [
  "HLT_e10_vloose_L1EM7",
  "HLT_e12_loose",
  "HLT_e17_loose",
	"HLT_e17_medium",
	"HLT_e17_medium_iloose",
  "HLT_e17_vloose",
	"HLT_e20_medium",
	"HLT_e20_vloose",
  "HLT_e24_tight_iloose",
  "HLT_e26_tight_iloose",
  "HLT_e30_vloose_L1EM15",
  "HLT_e60_medium",
]

# Compare with same items without L1 seed
probeListAltL1Seeds = [
	"HLT_e12_lhloose_L1EM10VH",
  "HLT_e12_loose_L1EM10VH",
  "HLT_e17_lhloose_L1EM15",
  "HLT_e17_loose_L1EM15",
  "HLT_e20_lhvloose_L1EM12",
  "HLT_e24_lhtight_L1EM20VH",
	"HLT_e24_lhtight_iloose_L1EM20VH",
	"HLT_e24_tight_iloose_L1EM20VH",
	"HLT_e24_tight_iloose_etisem_L1EM20VH",
]

# Special triggers for hlt id performance
probeListPerfTriggers = [
	"HLT_e24_lhmedium_L2Star_idperf_L1EM20VH",
	"HLT_e24_lhmedium_idperf_L1EM20VH",
  "HLT_e24_medium1_L2Star_idperf_L1EM18VH",
  "HLT_e24_medium1_idperf_L1EM18VH",
  "HLT_e24_medium_L2Star_idperf_L1EM20VH",
  "HLT_e24_medium_idperf_L1EM20VH",
]

# Run1 tracking triggers, for rate estimates
# Compare with same triggers w/o L2StarA
probeListRun1TrkTriggers = [
  "HLT_e24_lhmedium_iloose_L2StarA_L1EM20VH",
  "HLT_e24_medium1_iloose_L2StarA_L1EM18VH",
  "HLT_e24_medium_iloose_L2StarA_L1EM20VH",
]

# Run1 Pid, compare with same Run2 items
probeListRun1PidTriggers = [
	"HLT_e24_medium1_L1EM18VH",
	"HLT_e24_medium1_iloose_L1EM18VH",
	"HLT_e24_tight1_iloose",
	"HLT_e24_tight1_iloose_L1EM20VH",
  "HLT_e26_tight1_iloose",
]

# To be compared with the default items
probeListMisalignmentTriggers = [
	"HLT_e24_lhmedium_cutd0dphideta_L1EM18VH",
	"HLT_e24_lhmedium_cutd0dphideta_iloose_L1EM18VH",
	"HLT_e24_lhmedium_cutd0dphideta_iloose_L1EM20VH",
	"HLT_e24_lhmedium_nod0_L1EM18VH",
	"HLT_e24_lhmedium_nod0_iloose_L1EM18VH",
	"HLT_e24_lhmedium_nodeta_L1EM18VH",
	"HLT_e24_lhmedium_nodeta_iloose_L1EM18VH",
	"HLT_e24_lhmedium_nodphires_L1EM18VH",
	"HLT_e24_lhmedium_nodphires_iloose_L1EM18VH",
	"HLT_e24_lhtight_cutd0dphideta_iloose",
	"HLT_e24_lhtight_cutd0dphideta_iloose_L1EM20VH",
  "HLT_e26_lhtight_cutd0dphideta_iloose",
  "HLT_e60_lhmedium_cutd0dphideta",
]

# Compare with default items
probeListAltSequence = [
	"HLT_e24_lhmedium_iloose_HLTCalo_L1EM18VH",
	"HLT_e24_lhmedium_iloose_HLTCalo_L1EM20VH",
	"HLT_e24_lhtight_iloose_HLTCalo",
	"HLT_e24_lhtight_iloose_HLTCalo_L1EM20VH",
  "HLT_e24_lhtight_iloose_L2EFCalo_L1EM20VH",
  "HLT_e24_tight_iloose_HLTCalo_L1EM20VH",
  "HLT_e24_tight_iloose_L2EFCalo_L1EM20VH",
  "HLT_e26_lhtight_iloose_HLTCalo",
]

#ProbeList 3 High pt triggers
#Should run with Zprime and / or WPrime samples
#For Wprime - Perf
#For Zprime - Perf,Counts,Eff,EmulationTool
#If we have Zprime, check the Z'->ee mass and change the mass window to select events in the Z' mass window.
#Separate into supporting and primary triggers

# Etcut Trigger list
probeListHighPtSupportingTriggers = [
	"HLT_e100_etcut",
	"HLT_e120_etcut",
	"HLT_e40_etcut_L1EM15",
	"HLT_e60_etcut",
	"HLT_e80_etcut",
	"HLT_e50_etcut_L1EM15",
]

# The Primary Trigger List
probeListHighPtPhysicsTriggers = [
	"HLT_e120_lhloose",
	"HLT_e120_lhloose_HLTCalo",
	"HLT_e120_lhvloose",
	"HLT_e120_loose",
	"HLT_e120_loose1",
	"HLT_e120_vloose",
	"HLT_e140_lhloose",
	"HLT_e140_lhloose_HLTCalo",
	"HLT_e140_loose",
	"HLT_e40_lhvloose_L1EM15",
	"HLT_e40_vloose_L1EM15",
	"HLT_e50_lhvloose_L1EM15",
	"HLT_e50_vloose_L1EM15",
	"HLT_e60_lhloose",
	"HLT_e60_lhmedium",
	"HLT_e60_lhmedium_HLTCalo",
	"HLT_e60_lhmedium_cutd0dphideta",
	"HLT_e60_lhvloose",
	"HLT_e60_loose",
	"HLT_e60_medium",
	"HLT_e60_medium1",
	"HLT_e60_vloose",
	"HLT_e70_lhloose",
	"HLT_e70_lhvloose",
	"HLT_e70_loose",
	"HLT_e70_vloose",
	"HLT_e80_lhvloose",
	"HLT_e80_vloose",
]

# ProbeList 4 Photon triggers
# EmulationTool,Perf
# Sample ggH125 and background dijet / JFXX

probeListPhotonTriggers = [
	"HLT_g0_hiptrt_L1EM18VH",
	"HLT_g0_hiptrt_L1EM20VH",
	"HLT_g0_hiptrt_L1EM20VHI",
	"HLT_g0_hiptrt_L1EM22VHI",
	"HLT_g0_perf_L1EM15",
	"HLT_g0_perf_L1EM3",
	"HLT_g100_loose",
	"HLT_g10_etcut",
	"HLT_g10_loose",
	"HLT_g10_medium",
	"HLT_g120_loose",
	"HLT_g120_loose1",
	"HLT_g120_loose_HLTCalo",
	"HLT_g140_loose",
	"HLT_g15_loose",
	"HLT_g15_loose_L1EM3",
	"HLT_g15_loose_L1EM7",
	"HLT_g200_etcut",
	"HLT_g20_etcut_L1EM12",
	"HLT_g20_loose",
	"HLT_g20_loose_L1EM12",
	"HLT_g20_loose_L1EM15",
	"HLT_g20_tight",
	"HLT_g25_loose",
	"HLT_g25_loose_L1EM15",
	"HLT_g25_medium",
	"HLT_g35_loose",
	"HLT_g35_loose_L1EM15",
	"HLT_g35_loose_L1EM15_g25_loose_L1EM15",
	"HLT_g35_loose_g25_loose",
	"HLT_g35_medium",
	"HLT_g3_etcut",
	"HLT_g3_etcut_L1EM3_EMPTY",
	"HLT_g40_loose_L1EM15",
	"HLT_g40_tight",
	"HLT_g45_loose_L1EM15",
	"HLT_g45_tight",
	"HLT_g50_loose",
	"HLT_g50_loose_L1EM15",
	"HLT_g60_loose",
	"HLT_g60_loose_L1EM15VH",
	"HLT_g70_loose",
	"HLT_g80_loose",
]

defaultJpsi=[
  "HLT_e5_loose",
  "HLT_e5_lhloose",
  "HLT_e5_vloose",
  "HLT_e5_lhvloose",
]

# Lowest single electron triggers for TP analysis
monitoringTP_electron_extra =[
  'HLT_e24_lhmedium_L1EM18VH',
  'HLT_e24_lhmedium_nod0_L1EM18VH',
  'HLT_e24_lhmedium_nod0_L1EM20VH',
  'HLT_e24_medium_nod0_L1EM20VH',
  'HLT_e24_lhmedium_iloose',
  'HLT_e24_medium_iloose',
  'HLT_e24_lhmedium_nod0_iloose',
  'HLT_e24_lhtight_nod0_iloose',
  'HLT_e24_lhmedium_nod0_ivarloose',
  'HLT_e24_lhtight_nod0_ivarloose',
  'HLT_e24_lhtight_iloose',
  'HLT_e24_tight_iloose',
  'HLT_e26_lhtight_iloose',
  'HLT_e26_lhtight_nod0_iloose',
  'HLT_e26_lhtight_nod0_ivarloose',
  'HLT_e24_lhmedium_nod0_ringer_L1EM20VH',
  'HLT_e24_lhmedium_nod0_ringer_iloose',
  'HLT_e24_lhtight_nod0_ringer_iloose',
  'HLT_e24_lhmedium_nod0_ringer_ivarloose',
  'HLT_e24_lhtight_nod0_ringer_ivarloose',
  'HLT_e26_lhtight_nod0_ringer_iloose',
  'HLT_e26_lhtight_nod0_ringer_ivarloose',
  'HLT_e28_lhtight_nod0_iloose',
  'HLT_e28_lhtight_nod0_ringer_iloose',
  'HLT_e28_lhtight_nod0_ringer_ivarloose',
]



### Make supproting trigger list for the emulation e/g tool
supportingTriggerList = probeListLowMidPtSupportingTriggers+probeListHighPtSupportingTriggers
# Update the supporting list with very loose chains. This will be used in data collisin matches
for trig in probeListLowMidPtPhysicsTriggers+probeListHighPtPhysicsTriggers:
  if 'vloose' in trig:  supportingTriggerList.append(trig)

supportingTriggerList.extend( [
  'HLT_e24_lhvloose_nod0_L1EM20VH',
  'HLT_e26_lhvloose_nod0_L1EM20VH',
  'HLT_e28_lhvloose_nod0_L1EM20VH',
  'HLT_e140_lhvloose_nod0',
  'HLT_e10_lhvloose_nod0_L1EM7',
  'HLT_e10_lhvloose_nod0_L1EM7',
  'HLT_e15_lhvloose_nod0_L1EM7',
  'HLT_e20_lhvloose_nod0_L1EM12',
  'HLT_e25_lhvloose_nod0_L1EM15',
  'HLT_e30_lhvloose_nod0_L1EM15',
  'HLT_e40_lhvloose_nod0_L1EM15',
  'HLT_e50_lhvloose_nod0_L1EM15',
  'HLT_e70_lhvloose_nod0',
  'HLT_e80_lhvloose_nod0',
  'HLT_e100_lhvloose_nod0',
  'HLT_e120_lhvloose_nod0',
  'HLT_e5_lhvloose_nod0',
  'HLT_e12_lhvloose_nod0_L1EM10VH',
  'HLT_e17_lhvloose_nod0',
  'HLT_e20_lhvloose_nod0',
  'HLT_e40_lhvloose_nod0',
  'HLT_e60_lhvloose_nod0',
] )



