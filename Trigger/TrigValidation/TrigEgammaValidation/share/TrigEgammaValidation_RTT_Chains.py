from TriggerMenu import useNewTriggerMenu 
use_new_tm = useNewTriggerMenu() 

def electronChains():

  egammaTrigChainlist = ['e24_lhmedium_iloose_L1EM18VH',# Primary LH electron triggers
                         'e24_lhmedium_iloose_L1EM20VH',
                         'e24_lhtight_iloose',
                         'e26_lhtight_iloose',
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
                         'e24_vloose_L1EM18VH', 
                         'e24_vloose_L1EM20VH',  
                         'e24_lhvloose_L1EM18VH', 
                         'e24_lhvloose_L1EM20VH',  
                         'e26_lhvloose_L1EM20VH',
                         'e26_vloose_L1EM20VH',
                         'e60_lhvloose',
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
                         "g0_perf_L1EM3_EMPTY",
                         "e0_perf_L1EM3_EMPTY",
                         "e0_perf_L1EM15",
                         "g0_perf_L1EM15",
                         "g10_etcut",
                         "g10_loose",
                         "e5_etcut",
                         "e10_etcut_L1EM7",
                         "e5_lhloose_idperf",
                         "e5_loose_idperf",
                         "e5_loose_L2Star_idperf",
                         "e5_loose",
                         'e24_medium_idperf_L1EM20VH',
                         'e24_lhmedium_idperf_L1EM20VH',
                         'e25_etcut_L1EM15',
                         'e30_etcut_L1EM15',
                         'L1_EM18VH',
                         'L1_EM20VH',
                         'L1_EM20VHI',
                         ]

  return (egammaTrigChainlist)
