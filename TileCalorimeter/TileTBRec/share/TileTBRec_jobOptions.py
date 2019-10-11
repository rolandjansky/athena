#----------------------------------
# Read TileCal data from ByteStream
#----------------------------------
from AthenaCommon.AppMgr import ServiceMgr
if not hasattr( ServiceMgr, "ByteStreamAddressProviderSvc" ):
    from ByteStreamCnvSvcBase. ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
    ServiceMgr += ByteStreamAddressProviderSvc()
    
ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += ["TileBeamElemContainer/TileBeamElemCnt",
                                                      "TileDigitsContainer/TileDigitsCnt",
                                                      "TileL2Container/TileL2Cnt",
                                                      "TileLaserObject/TileLaserObj" ]

if not 'TileFrameLength' in dir():
    TileFrameLength = 9; # the value we expect in CTB physics run

ServiceMgr.TileInfoLoader.NSamples = TileFrameLength
ServiceMgr.TileInfoLoader.TrigSample = (TileFrameLength-1)/2

from TileRecUtils.TileRawChannelGetter import *
theTileRawChannelGetter=TileRawChannelGetter()

#jobproperties already loaded in getter
if jobproperties.TileRecFlags.doTileFit():
    ToolSvc.TileRawChannelBuilderFitFilter.FrameLength = TileFrameLength
    
    ToolSvc.TileRawChannelBuilderFitFilter.MaxIterate = 9; # iterate 9 times and choose best chi2
    ToolSvc.TileRawChannelBuilderFitFilter.NoiseThresholdRMS = 3; # 2-param fit forced for pedestal-like events
    ToolSvc.TileRawChannelBuilderFitFilter.SaturatedSampleError = 6; # error (in terms of RMS) assigned to sat.sample

    print ToolSvc.TileRawChannelBuilderFitFilter

if jobproperties.TileRecFlags.doTileOptATLAS():
    ToolSvc.TileRawChannelBuilderOptATLAS.PedestalMode=1

    print ToolSvc.TileRawChannelBuilderOptATLAS

if jobproperties.TileRecFlags.doTileOpt2():
    ToolSvc.TileRawChannelBuilderOpt2Filter.PedestalMode=1

    print ToolSvc.TileRawChannelBuilderOpt2Filter

if jobproperties.TileRecFlags.doTileFlat():
    ToolSvc.TileRawChannelBuilderFlatFilter.FrameLength = TileFrameLength
    ToolSvc.TileRawChannelBuilderFlatFilter.SignalLength = TileFrameLength - 1

    print ToolSvc.TileRawChannelBuilderFlatFilter

print theTileRawChannelGetter.TileRChMaker()
