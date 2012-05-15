theApp.Dlls += [ "TrigNtMuon" ]

#CBNT_AthenaAware = Algorithm( "CBNT_AthenaAware" )
#CBNT_AthenaAware.Members += [ "CBNTAA_TrigmuFast/CBNT_TrigmuFast" ]
#CBNT_AthenaAware.Members += [ "CBNTAA_TrigmuComb/CBNT_TrigmuComb" ]

from TrigNtMuon.TrigNtMuonConf import *

if TriggerFlags.MuonSlice.doEFMuon():
    from AthenaCommon.Configurable import Configurable
    if   "MooHLTAlgo_MS_Muon" in Configurable.allConfigurables.keys():
        Configurable.allConfigurables.get("MooHLTAlgo_MS_Muon").doAANtuples = True
    if   "MooHLTAlgo_SA_Muon" in Configurable.allConfigurables.keys():
        Configurable.allConfigurables.get("MooHLTAlgo_SA_Muon").doAANtuples = True
    if   "MooHLTAlgo_CB_Muon" in Configurable.allConfigurables.keys():
        Configurable.allConfigurables.get("MooHLTAlgo_CB_Muon").doAANtuples = True


