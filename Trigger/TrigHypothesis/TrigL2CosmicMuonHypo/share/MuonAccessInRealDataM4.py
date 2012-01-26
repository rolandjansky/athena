# if RunNumber [0] < 12001:
#   RpcSpecialROBNumber = 0x650001
# else:
#   RpcSpecialROBNumber = 0x650002
# if RunNumber [0] < 12432:
#   TgcSpecialROBNumber = 0x680011
# else:
#   TgcSpecialROBNumber = 0x680019

MdtSpecialROBNumber = 0x0
RpcSpecialROBNumber = 0x650002
TgcSpecialROBNumber = 0x680019

theApp.Dlls += [ "MuonByteStream" ]
theApp.Dlls += [ "MuonTBConditions" ]
theApp.ExtSvc += [ "MuonTBMdtCablingSvc" ]

# set the MDT special mapping options
ToolSvc = Service("ToolSvc")
ToolSvc.MdtColROD_Decoder.UseTBCabling = True
ToolSvc.MdtPRDColROD_Decoder.UseTBCabling = True
ToolSvc.MdtROD_Decoder.UseTBCabling = True

MuonTBMdtCablingSvc=Service("MuonTBMdtCablingSvc")
MuonTBMdtCablingSvc.InputFile= "MdtEleMap.data"

# set the special ROB numbers
#if  MuDetCosmicFlags.doMdtROBNumber:
## ToolSvc.MdtROD_Decoder.SpecialROBNumber = MdtSpecialROBNumber
## ToolSvc.MdtColROD_Decoder.SpecialROBNumber = MdtSpecialROBNumber
## ToolSvc.MdtPRDColROD_Decoder.SpecialROBNumber = MdtSpecialROBNumber

#if  MuDetCosmicFlags.doRpcROBNumber:
ToolSvc.RpcROD_Decoder.SpecialROBNumber = RpcSpecialROBNumber
ToolSvc.RpcColROD_Decoder.SpecialROBNumber = RpcSpecialROBNumber
ToolSvc.RpcPRDColROD_Decoder.SpecialROBNumber = RpcSpecialROBNumber

#if  MuDetCosmicFlags.doTgcROBNumber:
ToolSvc.TgcROD_Decoder.TgcSpecialROBNumber = TgcSpecialROBNumber

#if  MuDetCosmicFlags.doSector13Data:
# ToolSvc.RpcROD_Decoder.Sector13Data = True

