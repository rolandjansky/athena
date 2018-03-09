# Author: N. Gollub (nils.gollub@cern.ch)
# main job option to setup TileConditions
#
include.block ("TileConditions/TileConditions_jobOptions.py")
from AthenaCommon.Logging import logging
msg = logging.getLogger( 'TileConditions_jobOptions.py' )

from TileConditions.TileInfoConfigurator import TileInfoConfigurator
tileInfoConfigurator = TileInfoConfigurator()

import re
from AthenaCommon.GlobalFlags import globalflags
gbltg=globalflags.DetDescrVersion()
patt = re.compile(r'-([A-Z]+)-')
ss = patt.search(gbltg)
if (type(ss) != type(None)):
    version = ss.group(1)
    if (version=='CSC'):
        msg.warning("Old geometry tag detected %s - will not use COOL DB for TileCal calibration" % gbltg)
        TileUseCOOL = False
        TileFrameLength = 9
    if (version=='GEO') and (not 'TileCablingType' in dir()):
        if not 'RunNumber' in dir():
            from RecExConfig.AutoConfiguration import GetRunNumber
            rn=GetRunNumber()
        else:
            rn=RunNumber
        if rn>219651: # choose RUN2 cabling for old geometry tags starting from 26-MAR-2013 
            TileCablingType = 4 
            msg.warning("Forcing RUN2 cabling for run %s with geometry %s" % (rn,gbltg) )

if 'TileCablingType' in dir():
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    svcMgr.TileCablingSvc.CablingType = TileCablingType

if not 'TileFrameLength' in dir():
    TileFrameLength = 7; # unique default value for simulation and reconstruction

if not 'TileUseCOOL' in dir():
    TileUseCOOL=True; # use COOL DB by default for everything

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
if not 'TileUseDCS' in dir():
    TileUseDCS = TileUseCOOL and (not athenaCommonFlags.isOnline()) and globalflags.DataSource()=='data'

    if TileUseDCS:
        from AthenaCommon.DetFlags import DetFlags
        from RecExConfig.RecFlags import rec
        TileUseDCS = DetFlags.dcs.Tile_on() and rec.doESD and (not rec.readESD)
        TileCheckOFC=True

    if TileUseDCS:
        from RecExConfig.AutoConfiguration import GetRunNumber
        rn=GetRunNumber()
        TileUseDCS = ((rn>171194 and rn<222222) or rn>232498); # use DCS only for 2011 data and later, excluding shutdown period

if TileUseDCS or ('TileCheckOFC' in dir() and TileCheckOFC) or ('RunOflOFC' in dir()):
    if not 'RunNumber' in dir():
        from RecExConfig.AutoConfiguration import GetRunNumber
        rn=GetRunNumber()
    else:
        rn=RunNumber
    if not 'RunOflOFC' in dir():
        RunOflOFC=314450
    if rn<RunOflOFC: # use OFC stored in online folder for all runs before 2017
        from TileConditions.TileCoolMgr import tileCoolMgr
        tileCoolMgr.addSource('OfcOf2Phy', '/TILE/ONL01/FILTER/OF2/PHY', 'TILE', "", '/TILE/ONL01/FILTER/OF2/PHY', 'SplitMC')
        tileCoolMgr.addSource('OfcOf1Phy', '/TILE/ONL01/FILTER/OF1/PHY', 'TILE', "", '/TILE/ONL01/FILTER/OF1/PHY', 'SplitMC')

msg.info("Adjusting TileInfo for %s samples" % TileFrameLength )
tileInfoConfigurator.NSamples = TileFrameLength
tileInfoConfigurator.OFPhysicsNSamples = TileFrameLength
tileInfoConfigurator.TrigSample = (TileFrameLength-1)/2

if athenaCommonFlags.isOnline():
    #=== setup reading from COOL DB
    msg.info("setting up COOL for TileCal online conditions data")
    tileInfoConfigurator.setupCOOL()
    tileInfoConfigurator.setupCOOLOFC()
elif TileUseCOOL:
    #=== setup reading from COOL DB
    msg.info("setting up COOL for TileCal conditions data")
    TileGapTiming=""
    if (('TileCisRun' in dir()) and (TileCisRun) or
        ('TileMonoRun' in dir()) and (TileMonoRun) or
        ('TileRampRun' in dir()) and (TileRampRun)):
        TilePulse="CIS"
    elif ('TileLasRun' in dir()) and (TileLasRun):
        TilePulse="LAS"
        if ('TilePhysTiming' in dir()) and (TilePhysTiming):
            TileGapTiming="GAP"
    else:
        TilePulse="PHY"
    if not 'TileUseCOOLOFC' in dir():
        TileUseCOOLOFC=True; # read OFC from COOL
    if not 'TileUseCOOLPULSE' in dir() or not TileUseCOOLOFC:
        TileUseCOOLPULSE=True; # read pulse from COOL

    tileInfoConfigurator.setupCOOL(type=(TileGapTiming+TilePulse))

    if TileUseDCS:
        tileInfoConfigurator.setupCOOLDCS();
else:
    msg.warning("COOL is not used for TileCal conditions data")
    TileUseCOOLOFC=False
    tileInfoConfigurator.setupCOOLPHYPULSE()
    tileInfoConfigurator.setupCOOLAutoCr()

    from AthenaCommon.AppMgr import ToolSvc
    from TileConditions.TileCondProxyConf import getTileCondProxy
    from TileConditions.TileConditionsConf import TileBadChanTool

    if not 'TileBchList' in dir():
        TileBchList='TileNoBad.oflBch'; # dummy file without bad channels

    #=== set file name with bad channels
    msg.info("Reading TileCal bad channel list from %s" % TileBchList )
    ToolSvc.TileBadChanTool.ProxyOflBch = getTileCondProxy('FILE','Bch',TileBchList,'TileCondProxyFile_OflBch')

# load optimal filter weights if needed
if ('doTileOpt' in dir()) and (doTileOpt):
    tileInfoConfigurator.LoadOptFilterWeights=True
    tileInfoConfigurator.filenameDeltaCISSuffix="of2_Delta_CIS_7Samples"

    if ('TileRunType' in dir()) and (TileRunType==2):
        if TileFrameLength==9:
            tileInfoConfigurator.filenameDeltaPhysicsSuffix="of2_Delta_Laser_9Samples"
        if TileFrameLength==7:
            tileInfoConfigurator.filenameDeltaPhysicsSuffix="of2_Delta_Laser_7Samples"
    else:
        if TileFrameLength==9:
            tileInfoConfigurator.filenameDeltaPhysicsSuffix="of2_Delta_Phys_9Samples"
        if TileFrameLength==7:
            tileInfoConfigurator.filenameDeltaPhysicsSuffix="of2_Delta_Phys_7Samples"

# fine-tune CellNoise values depending on beam type
if not 'TileCommissioning' in dir():
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.beamType != 'collisions':
        TileCommissioning = True
    else:
        try:
            from RecExConfig.RecFlags import rec
            TileCommissioning = rec.Commissioning()
        except:
            msg.info("No RecFlags available - looks like a simulation job")
            TileCommissioning = False
        
if TileCommissioning:
    msg.info("Adjusting TileInfo to return cell noise for Opt.Filter with iterations")
    tileInfoConfigurator.NoiseScaleIndex = 2; # Noise for Optimal Filter with iterations
else:
    msg.info("Adjusting TileInfo to return cell noise for Opt.Filter without iterations")
    tileInfoConfigurator.NoiseScaleIndex = 1; # Noise for Optimal Filter without iterations
