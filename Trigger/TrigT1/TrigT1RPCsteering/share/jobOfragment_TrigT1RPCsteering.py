#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
theApp.Dlls += [ "TrigT1RPCsteering" ]
#top algorithms to be run
theApp.TopAlg += [ "TrigT1RPC" ]
#--------------------------------------------------------------
# Define configuration file for Level-1 Muon Barrel Trigger
#--------------------------------------------------------------
TrigT1RPC = Algorithm( "TrigT1RPC" )
TrigT1RPC.Hardware          = TRUE
TrigT1RPC.DataDetail        = FALSE
TrigT1RPC.RPCbytestream     = FALSE
TrigT1RPC.RPCbytestreamFile = ""
