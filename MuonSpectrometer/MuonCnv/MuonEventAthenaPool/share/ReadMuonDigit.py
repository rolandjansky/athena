# Algs for CBNT
theApp.Dlls += [ "CBNT_Athena", "CBNT_Utils" ] 
theApp.TopAlg += [ "CBNT_Athena/CBNT_MuonDigit" ]
CBNT_MuonDigit = Algorithm( "CBNT_MuonDigit" )
CBNT_MuonDigit.members = [
 "ReadMdtDigit",
 "ReadRpcDigit",
 "ReadTgcDigit",
 "ReadCscDigit"
]
ReadMdtDigit = Algorithm( "ReadMdtDigit" )
ReadMdtDigit.NtupleLocID = "/FILE1/MDT/103" 
ReadRpcDigit = Algorithm( "ReadRpcDigit" )
ReadRpcDigit.NtupleLocID = "/FILE1/RPC/104" 
ReadTgcDigit = Algorithm( "ReadTgcDigit" )
ReadTgcDigit.NtupleLocID = "/FILE1/TGC/105" 
ReadCscDigit = Algorithm( "ReadCscDigit" )
ReadCscDigit.NtupleLocID = "/FILE1/CSC/106" 
ReadMdtDigit.WriteMdtNtuple = TRUE
ReadRpcDigit.WriteRpcNtuple = TRUE
ReadTgcDigit.WriteTgcNtuple = TRUE
ReadCscDigit.WriteCscNtuple = TRUE