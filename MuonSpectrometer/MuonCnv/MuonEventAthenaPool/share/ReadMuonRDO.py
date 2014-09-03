# Algs for CBNT
theApp.Dlls += [ "CBNT_Athena", "CBNT_Utils" ] 
theApp.TopAlg += [ "CBNT_Athena/CBNT_MuonRDO" ]
CBNT_MuonRDO = Algorithm( "CBNT_MuonRDO" )
CBNT_MuonRDO.members = [
 "ReadMdtRDO",
 "ReadRpcRDO",
 "ReadTgcRDO",
 "ReadCscRDO"
]
ReadMdtRDO = Algorithm( "ReadMdtRDO" )
ReadMdtRDO.NtupleLocID = "/FILE1/MDT/103" 
ReadRpcRDO = Algorithm( "ReadRpcRDO" )
ReadRpcRDO.NtupleLocID = "/FILE1/RPC/104" 
ReadTgcRDO = Algorithm( "ReadTgcRDO" )
ReadTgcRDO.NtupleLocID = "/FILE1/TGC/105" 
ReadCscRDO = Algorithm( "ReadCscRDO" )
ReadCscRDO.NtupleLocID = "/FILE1/CSC/106" 
ReadMdtRDO.WriteMdtNtuple = TRUE
ReadRpcRDO.WriteRpcNtuple = TRUE
ReadTgcRDO.WriteTgcNtuple = TRUE
ReadCscRDO.WriteCscNtuple = TRUE
