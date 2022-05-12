from AthenaCommon.AppMgr import ServiceMgr

puAlg = job.StandardPileUpToolsAlg
puAlg.PileUpTools["TileHitVecToCntTool"].HitTimeFlag=1

puAlg.PileUpTools["TileHitVecToCntTool"].usePhotoStatistics=False

topSequence.TileDigitsMaker.IntegerDigits=True

###########
print(ServiceMgr.TileInfoLoader)
print(topSequence.TileDigitsMaker)
###########
