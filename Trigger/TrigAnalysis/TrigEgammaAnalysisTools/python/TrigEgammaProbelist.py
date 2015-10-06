# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# this file contains the following probelist of triggers
#
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

default2 = ['e24_lhmedium_idperf_L1EM20VH',#,'e24_lhmedium_iloose_L1EM18VH','L1_EM18VH','e24_lhmedium_L1EM20VHI']# Primary LH electron triggers
        'e24_medium_idperf_L1EM20VH']
        #'e24_lhtight_iloose',
        #'L1_EM18VH',
        #'g20_tight',
        #]
default = ['e24_lhmedium_iloose_L1EM18VH',# Primary LH electron triggers
        'e24_lhmedium_iloose_L1EM20VH',
        'e24_lhtight_iloose',
        'e24_lhtight_iloose_L1EM20VH',
        'e26_lhtight_iloose',
        'e26_lhtight_smooth_iloose',
        'e60_lhmedium',
        'e120_lhloose',
        'e140_lhloose',
        # Primary cut-based electron triggers
        'e24_medium_iloose_L1EM18VH',
        'e24_medium_iloose_L1EM20VH',
        'e24_tight_iloose',
        'e26_tight_iloose',
        'e60_medium',
        'e120_loose',
        'e140_loose',
        # Non-isolated
        'e24_lhmedium_L1EM18VH',
        'e24_medium_L1EM18VH',
        'e24_lhtight_L1EM20VH',
        'e24_tight_L1EM20VH',
        'e24_lhmedium_L1EM20VHI',
        # Single photon
        'g120_loose',
        'g140_loose',
        # Suppporting triggers for alignment studies / LH ineffiency
        'e24_lhmedium_cutd0dphideta_iloose_L1EM18VH',
        'e24_lhmedium_nod0_iloose_L1EM18VH',
        'e24_lhmedium_nodeta_iloose_L1EM18VH',
        'e24_lhmedium_nodphires_iloose_L1EM18VH',
        'e24_lhmedium_cutd0dphideta_iloose_L1EM20VH',
        'e24_lhmedium_nod0_iloose_L1EM20VH',
        'e24_lhmedium_nodeta_iloose_L1EM20VH',
        'e24_lhmedium_nodphires_iloose_L1EM20VH',
        'e24_lhmedium_iloose_HLTCalo_L1EM18VH',
        'e24_lhmedium_iloose_HLTCalo_L1EM20VH',
        'e24_lhmedium_iloose_L2EFCalo_L1EM18VH',
        'e24_lhmedium_iloose_L2EFCalo_L1EM20VH',
        'e26_lhtight_cutd0dphideta_iloose',
        'e26_lhtight_iloose_HLTCalo',
        'e26_lhtight_iloose_L2EFCalo',
        'e24_lhtight_iloose_HLTCalo_L1EM20VH',
        'e24_lhtight_iloose_L2EFCalo_L1EM20VH',
        'e60_lhmedium_HLTCalo',
        'e60_lhmedium_L2EFCalo',
        'e60_lhmedium_nod0',
        'e60_lhmedium_cutd0dphideta',
        # Supporting trigger for background
        #'e24_vloose_L1EM18VH', 
        #'e24_vloose_L1EM20VH',  
        #'e24_lhvloose_L1EM18VH', 
        #'e24_lhvloose_L1EM20VH',  
        #'e26_lhvloose_L1EM20VH',
        #'e26_vloose_L1EM20VH',
        #'e60_lhvloose',
        # Rerun mode for diobject
        'e12_lhloose_L1EM10VH',
        'e15_lhloose_L1EM13VH',
        'e17_loose'
        'e17_lhloose',
        'e12_loose_L1EM10VH',
        'e15_loose_L1EM13VH',
        'e17_loose_L1EM15',
        'e17_lhloose_L1EM15',
        'e13_etcut_trkcut',
	# Rerun mode for photon
        'g35_loose_L1EM15',
        'g25_loose_L1EM15',
        'g35_loose',
        'g25_loose',
	"g25_medium",
	"g35_medium",
        'g50_loose',
        'g20_tight',
        # Performance and supporting triggers 
        #"g0_perf_L1EM3_EMPTY",
        #"e0_perf_L1EM3_EMPTY",
        #"e0_perf_L1EM15",
        #"g0_perf_L1EM15",
        #"g10_etcut",
        #"g10_loose",
        #"e5_etcut",
        #"e10_etcut_L1EM7",
        #"e5_lhloose_idperf",
        #"e5_loose_idperf",
        #"e5_loose_L2Star_idperf",
        #"e5_loose",
        #'e24_medium_idperf_L1EM20VH',
        #'e24_lhmedium_idperf_L1EM20VH',
        #'e25_etcut_L1EM15',
        #'e30_etcut_L1EM15',
        #'L1_EM18VH',
        #'L1_EM20VH',
        #'L1_EM20VHI',
       ]

# ProbeList 1 - Low/mid pt supporting triggers
# Tools to Run
# EmulationTool,Counts,Efficiency,Perf
# Samples to run - Zee and a background such as dijet or JF17 or any JF sample

probeListLowMidPtSupportingTriggers = [
	"e0_L2Star_perf_L1EM15",
	"e0_L2Star_perf_L1EM3",
	"e0_perf_L1EM15",
	"e0_perf_L1EM3",
	"e10_etcut_L1EM7",
	"e13_etcut_L1EM10_W-MT25",
	"e13_etcut_L1EM10_W-MT30",
	"e13_etcut_L1W-NOMATCH",
	"e13_etcut_L1W-NOMATCH_W-05RO-XEEMHT",
	"e13_etcut_trkcut",
	"e14_etcut",
	"e17_etcut_L1EM15",
	"e17_etcut_L2StarA_L1EM15",
	"e17_etcut_L2StarB_L1EM15",
	"e17_etcut_L2StarC_L1EM15",
	"e17_etcut_L2Star_idperf_L1EM15",
	"e17_etcut_idperf_L1EM15",
	"e18_etcut_L1EM15_W-MT35",
	"e18_etcut_trkcut",
	"e20_etcut_L1EM12",
	"e30_etcut_L1EM15",
	"e3_etcut",
	"e3_etcut_L1EM3_EMPTY",
	"e4_etcut",
	"e5_etcut",
	"e9_etcut",
        "e25_etcut_L1EM15",
]

# ProbeList 2 Low/mid pt physics triggers
# EmulationTool,Eff,Counts,Perf
# Samples to run Zee and any background sample dijet, JFXX

probeListLowMidPtPhysicsTriggers = [
	"e10_lhvloose_L1EM7",
	"e10_vloose_L1EM7",
	"e12_lhloose",
	"e12_loose",
	"e12_lhvloose_L1EM10VH",
	"e12_vloose_L1EM10VH",
	"e15_lhloose_L1EM13VH",
	"e15_lhvloose_L1EM13VH",
	"e15_lhvloose_L1EM7",
	"e15_loose_L1EM13VH",
	"e15_vloose_L1EM13VH",
	"e15_vloose_L1EM7",
	"e17_lhloose",
	"e17_lhmedium",
	"e17_lhmedium_iloose",
	"e17_lhvloose",
	"e17_loose",
	"e17_medium",
	"e17_medium_iloose",
	"e17_vloose",
	"e20_lhmedium",
	"e20_lhvloose",
	"e20_medium",
	"e20_vloose",
	"e20_vloose_L1EM12",
	"e24_lhmedium_L1EM15VH",
	"e24_lhmedium_L1EM18VH",
	"e24_lhmedium_iloose_L1EM18VH",
	"e24_lhmedium_iloose_L1EM20VH",
	"e24_lhtight_iloose",
	"e24_lhvloose_L1EM18VH",
	"e24_lhvloose_L1EM20VH",
	"e24_medium_L1EM15VH",
	"e24_medium_L1EM18VH",
	"e24_medium_iloose_L1EM18VH",
	"e24_medium_iloose_L1EM20VH",
	"e24_tight_L1EM20VH",
	"e24_tight_iloose",
	"e24_vloose_L1EM18VH",
	"e24_vloose_L1EM20VH",
	"e25_lhvloose_L1EM15",
	"e25_vloose_L1EM15",
	"e26_lhtight_iloose",
	"e26_lhvloose_L1EM20VH",
	"e26_tight_iloose",
	"e26_vloose_L1EM20VH",
	"e30_lhvloose_L1EM15",
	"e30_vloose_L1EM15",
]

# Compare LHTrigger to IsEMTrigger
probeListLHTriggers = [
        "e10_lhvloose_L1EM7",
        "e12_lhloose",
        "e17_lhloose",
	"e17_lhmedium",
	"e17_lhmedium_iloose",
        "e17_lhvloose",
	"e20_lhmedium",
	"e20_lhvloose",
        "e24_lhtight_iloose",
        "e26_lhtight_iloose",
        "e30_lhvloose_L1EM15",
        "e60_lhmedium",
        ]

# Compare with LH trigger above
probeListIsEMTriggers = [
        "e10_vloose_L1EM7",
        "e12_loose",
        "e17_loose",
	"e17_medium",
	"e17_medium_iloose",
        "e17_vloose",
	"e20_medium",
	"e20_vloose",
        "e24_tight_iloose",
        "e26_tight_iloose",
        "e30_vloose_L1EM15",
        "e60_medium",
        ]

# Compare with same items without L1 seed
probeListAltL1Seeds = [
	"e12_lhloose_L1EM10VH",
        "e12_loose_L1EM10VH",
        "e17_lhloose_L1EM15",
        "e17_loose_L1EM15",
        "e20_lhvloose_L1EM12",
        "e24_lhtight_L1EM20VH",
	"e24_lhtight_iloose_L1EM20VH",
	"e24_tight_iloose_L1EM20VH",
	"e24_tight_iloose_etisem_L1EM20VH",
        ]

# Special triggers for hlt id performance
probeListPerfTriggers = [
	"e24_lhmedium_L2Star_idperf_L1EM20VH",
	"e24_lhmedium_idperf_L1EM20VH",
        "e24_medium1_L2Star_idperf_L1EM18VH",
        "e24_medium1_idperf_L1EM18VH",
        "e24_medium_L2Star_idperf_L1EM20VH",
        "e24_medium_idperf_L1EM20VH",
        ]

# Run1 tracking triggers, for rate estimates
# Compare with same triggers w/o L2StarA
probeListRun1TrkTriggers = [
    "e24_lhmedium_iloose_L2StarA_L1EM20VH",
    "e24_medium1_iloose_L2StarA_L1EM18VH",
    "e24_medium_iloose_L2StarA_L1EM20VH",
        ]

# Run1 Pid, compare with same Run2 items
probeListRun1PidTriggers = [
	"e24_medium1_L1EM18VH",
	"e24_medium1_iloose_L1EM18VH",
	"e24_tight1_iloose",
	"e24_tight1_iloose_L1EM20VH",
        "e26_tight1_iloose",
        ]

# To be compared with the default items
probeListMisalignmentTriggers = [
	"e24_lhmedium_cutd0dphideta_L1EM18VH",
	"e24_lhmedium_cutd0dphideta_iloose_L1EM18VH",
	"e24_lhmedium_cutd0dphideta_iloose_L1EM20VH",
	"e24_lhmedium_nod0_L1EM18VH",
	"e24_lhmedium_nod0_iloose_L1EM18VH",
	"e24_lhmedium_nodeta_L1EM18VH",
	"e24_lhmedium_nodeta_iloose_L1EM18VH",
	"e24_lhmedium_nodphires_L1EM18VH",
	"e24_lhmedium_nodphires_iloose_L1EM18VH",
	"e24_lhtight_cutd0dphideta_iloose",
	"e24_lhtight_cutd0dphideta_iloose_L1EM20VH",
        "e26_lhtight_cutd0dphideta_iloose",
        "e60_lhmedium_cutd0dphideta",
        ]

# Compare with default items
probeListAltSequence = [
	"e24_lhmedium_iloose_HLTCalo_L1EM18VH",
	"e24_lhmedium_iloose_HLTCalo_L1EM20VH",
	"e24_lhtight_iloose_HLTCalo",
	"e24_lhtight_iloose_HLTCalo_L1EM20VH",
        "e24_lhtight_iloose_L2EFCalo_L1EM20VH",
        "e24_tight_iloose_HLTCalo_L1EM20VH",
        "e24_tight_iloose_L2EFCalo_L1EM20VH",
        "e26_lhtight_iloose_HLTCalo",
        ]
#ProbeList 3 High pt triggers
#Should run with Zprime and / or WPrime samples
#For Wprime - Perf
#For Zprime - Perf,Counts,Eff,EmulationTool
#If we have Zprime, check the Z'->ee mass and change the mass window to select events in the Z' mass window.
#Separate into supporting and primary triggers

probeListHighPtSupportingTriggers = [
# Etcut Trigger list
	"e100_etcut",
	"e120_etcut",
	"e40_etcut_L1EM15",
	"e60_etcut",
	"e80_etcut",
	"e50_etcut_L1EM15",
]

probeListHighPtPhysicsTriggers = [
# The Primary Trigger List
	"e120_lhloose",
	"e120_lhloose_HLTCalo",
	"e120_lhvloose",
	"e120_loose",
	"e120_loose1",
	"e120_vloose",
	"e140_lhloose",
	"e140_lhloose_HLTCalo",
	"e140_loose",
	"e40_lhvloose_L1EM15",
	"e40_vloose_L1EM15",
	"e50_lhvloose_L1EM15",
	"e50_vloose_L1EM15",
	"e60_lhloose",
	"e60_lhmedium",
	"e60_lhmedium_HLTCalo",
	"e60_lhmedium_cutd0dphideta",
	"e60_lhvloose",
	"e60_loose",
	"e60_medium",
	"e60_medium1",
	"e60_vloose",
	"e70_lhloose",
	"e70_lhvloose",
	"e70_loose",
	"e70_vloose",
	"e80_lhvloose",
	"e80_vloose",
]

# ProbeList 4 Photon triggers
# EmulationTool,Perf
# Sample ggH125 and background dijet / JFXX

probeListPhotonTriggers = [
	"g0_hiptrt_L1EM18VH",
	"g0_hiptrt_L1EM20VH",
	"g0_hiptrt_L1EM20VHI",
	"g0_hiptrt_L1EM22VHI",
	"g0_perf_L1EM15",
	"g0_perf_L1EM3",
	"g100_loose",
	"g10_etcut",
	"g10_loose",
	"g10_medium",
	"g120_loose",
	"g120_loose1",
	"g120_loose_HLTCalo",
	"g140_loose",
	"g15_loose",
	"g15_loose_L1EM3",
	"g15_loose_L1EM7",
	"g200_etcut",
	"g20_etcut_L1EM12",
	"g20_loose",
	"g20_loose_L1EM12",
	"g20_loose_L1EM15",
	"g20_tight",
	"g25_loose",
	"g25_loose_L1EM15",
	"g25_medium",
	"g35_loose",
	"g35_loose_L1EM15",
	"g35_loose_L1EM15_g25_loose_L1EM15",
	"g35_loose_g25_loose",
	"g35_medium",
	"g3_etcut",
	"g3_etcut_L1EM3_EMPTY",
	"g40_loose_L1EM15",
	"g40_tight",
	"g45_loose_L1EM15",
	"g45_tight",
	"g50_loose",
	"g50_loose_L1EM15",
	"g60_loose",
	"g60_loose_L1EM15VH",
	"g70_loose",
	"g80_loose",
]

defaultJpsi=[
         "e5_loose",
         "e5_lhloose",
         "e5_vloose",
         "e5_lhvloose",
]
