theApp.Dlls += [ "TrigNtCalo" ]

#from TrigNtCalo.TrigNtCaloConf import *
#AANTupleStream += CBNTAA_TrigEFCalo()  
CBNTAA_TrigEFCalo=Algorithm("CBNTAA_TrigEFCalo")
CBNTAA_TrigEFCalo.ClustersInputNames= "HLTAutoKey"
CBNTAA_TrigEFCalo.OutputLevel=VERBOSE
CBNT_AthenaAware  += CBNTAA_TrigEFCalo
CBNTAA_TrigT2Calo=Algorithm("CBNTAA_TrigT2Calo")
CBNT_AthenaAware  += CBNTAA_TrigT2Calo
CBNTAA_TrigT2CaloJet=Algorithm("CBNTAA_TrigT2CaloJet")
CBNT_AthenaAware  += CBNTAA_TrigT2CaloJet

