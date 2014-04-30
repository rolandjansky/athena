theApp.Dlls   += [ "TrigT1TGC" ]
theApp.Dlls   += [ "TGCcabling" ]
theApp.ExtSvc += [ "TGCcablingSvc" ]
#// top algorithms to be run ////
#  select Input source
LVL1TGCTrigger = Algorithm( "LVL1TGCTrigger" )
LVL1TGCTrigger.InputData_perEvent = "TGC_DIGITS" 
#  MainAlgorithm for Simulation //
theApp.TopAlg += ["LVL1TGCTrigger::LVL1TGCTrigger/LVL1TGCTrigger"]
# properties
LVL1TGCTrigger.ASDOutDataLocation = "/Event/ASDOutDataLocation"
LVL1TGCTrigger.MuCTPIInput_TGC    = "/Event/L1MuctpiStoreTGC"
# mask file
LVL1TGCTrigger.MaskFileName = "TrigT1TGCMaskedChannel.db"
LVL1TGCTrigger.MaskFileName12 = "TrigT1TGCMaskedChannel._12.db"
