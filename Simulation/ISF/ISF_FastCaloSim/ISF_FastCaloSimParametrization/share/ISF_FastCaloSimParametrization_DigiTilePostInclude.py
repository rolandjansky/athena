from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.OutputLevel= VERBOSE
#rel 19 is configuring this via a tool!!
ToolSvc.TileHitVecToCntTool.HitTimeFlag=1 

ServiceMgr.TileInfoLoader.NPhElec=0
ServiceMgr.TileInfoLoader.NPhElecA=0
ServiceMgr.TileInfoLoader.NPhElecC=0
ServiceMgr.TileInfoLoader.NPhElecD=0
ServiceMgr.TileInfoLoader.NPhElecE=0

###########
print ServiceMgr.TileInfoLoader
print topSequence.TileDigitsMaker
print ToolSvc.TileHitVecToCntTool
topSequence.TileDigitsMaker.OutputLevel=1
topSequence.TileDigitsMaker.IntegerDigits=True
TileHitVecToCntTool.OutputLevel=1
ToolSvc.TileRawChannelBuilderOptATLAS.OutputLevel=1
########### 

