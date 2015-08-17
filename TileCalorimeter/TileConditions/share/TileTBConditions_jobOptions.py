from AthenaCommon.Logging import logging
msg = logging.getLogger( 'TileTBConditions_jobOptions.py' )

from TileConditions.TileInfoConfigurator import TileInfoConfigurator
tileInfoConfigurator = TileInfoConfigurator()

if not 'TileFrameLength' in dir():
    TileFrameLength = 9; # correct frame length for all testbeams
    
TileUseCOOL=False; # do not use COOL DB

msg.info("Adjusting TileInfo for %s samples" % TileFrameLength )
tileInfoConfigurator.NSamples = TileFrameLength
tileInfoConfigurator.OFPhysicsNSamples = TileFrameLength
tileInfoConfigurator.TrigSample = (TileFrameLength-1)/2

msg.info("Adjusting TileInfo to return cell noise for Fit Method")
tileInfoConfigurator.NoiseScaleIndex = 3; # Noise for Fit method

#=== set special CIS and cesium calibraton for 2004 data

if not 'Tile2004' in dir() or Tile2004: # 2004 configuration by default
    from TileConditions.TileCondProxyConf import getTileCondProxy
    ToolSvc.TileCondToolEmscale.ProxyOflCisLin = getTileCondProxy('FILE','Flt','Tile2004.cis','TileCondProxyFile_OflCisLin')
    ToolSvc.TileCondToolEmscale.ProxyOflCisNln = getTileCondProxy('FILE','Flt','Tile2004.lut','TileCondProxyFile_OflCisNln')
    ToolSvc.TileCondToolEmscale.ProxyOflCes    = getTileCondProxy('FILE','Flt','Tile2004.ces','TileCondProxyFile_OflCes')
    if not 'Tile2003Noise' in dir():
        Tile2003Noise=True

if 'Tile2003Noise' in dir() and Tile2003Noise: # special testbeam noise values
    from TileConditions.TileCondProxyConf import getTileCondProxy
    ToolSvc.TileCondToolNoiseSample.ProxyNoiseSample = getTileCondProxy('FILE','Flt','Tile2003.ped','TileCondProxyFile_NoiseSample')

# load optimal filter weights if needed
if ('doTileOpt' in dir()) and (doTileOpt):
    tileInfoConfigurator.LoadOptFilterWeights=True
    tileInfoConfigurator.filenameDeltaCISSuffix="of2_Delta_CIS_7Samples"
    if TileFrameLength==9:
        tileInfoConfigurator.filenameDeltaPhysicsSuffix="of2_Delta_Phys_9Samples"
    if TileFrameLength==7:
        tileInfoConfigurator.filenameDeltaPhysicsSuffix="of2_Delta_Phys_7Samples"


