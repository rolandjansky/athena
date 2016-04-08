#
# default parameters for testbeam reconstruction
#

# use only Fit method by default

if not 'doTileFlat' in dir():
    doTileFlat = False

if not 'doTileOpt' in dir():
    doTileOpt = False

if not 'doTileOpt2' in dir():
    doTileOpt2 = False

if not 'doTileOptATLAS' in dir():
    doTileOptATLAS = False

if not 'doTileFit' in dir():
    doTileFit = True; # fit method by default for CTB reconstruction

if not 'doTileFitCool' in dir():
    doTileFitCool = False

# prepare to reconstruct physics run
if not 'TileCisRun' in dir():
    TileCisRun = False

if not 'TilePedRun' in dir():
    TilePedRun = False

if not 'TileLasRun' in dir():
    TileLasRun = False

if not 'TileRunType' in dir():
    TileRunType = 0; # unknown run type by default

if not 'TileCalibrateEnergy' in dir():
    TileCalibrateEnergy = True; # convert ADC counts to pCb and then to GeV for normal runs

# expect 2004 data by default
if not 'Tile2004' in dir():
    Tile2004 = True

# expect 3 barrels configuration for 2003 data or in simulation
if not 'TileBarrelOnly' in dir():
    TileBarrelOnly = True

# take OFCs from COOL, no on the fly calculations
if not 'OfcFromCOOL' in dir():
    OfcFromCOOL = True

# this is real data
if not 'doSim' in dir():
    doSim = False

# don't need to run any optimal filtering on simulated data
# this is already done at digitization step
if doSim:
    doTileFlat = False
    doTileOpt  = False
    doTileOpt2 = False
    doTileOptATLAS = False
    doTileFit  = False
    doTileFitCool = False
    TileCisRun = False
    TilePedRun = False

# various configurations with ext.barrels
# we assume here that in case of barrel-only configuration
# correct GeoModel tag (3 or 5 modules) has been set already
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
detDescrVersion = GeoModelSvc.AtlasVersion
#from AthenaCommon.GlobalFlags import jobproperties as globaljobproperties
#detDescrVersion = globaljobproperties.Global.DetDescrVersion()
if (detDescrVersion.startswith("ctb") or
    detDescrVersion.startswith("CTB") or
    detDescrVersion.startswith("ATLAS-CTB") ) :
    if Tile2004:
        if 'RunNumber' in dir():
            if 1000000 < RunNumber and RunNumber < 1002000:
                TileBarrelOnly = False

        if not TileBarrelOnly:
            GeoModelSvc.TileVersionOverride = "TileTB-3B3EB-00"
    else:
        if not TileBarrelOnly:
            GeoModelSvc.TileVersionOverride = "TileTB-2B2EB-00"

# set options for TileRawChannelMaker
from TileRecUtils.TileRecFlags import jobproperties

jobproperties.TileRecFlags.doTileFlat=doTileFlat
jobproperties.TileRecFlags.doTileFit=doTileFit
jobproperties.TileRecFlags.doTileFitCool=doTileFitCool
jobproperties.TileRecFlags.doTileOpt=doTileOpt
jobproperties.TileRecFlags.doTileOpt2=doTileOpt2
jobproperties.TileRecFlags.doTileOptATLAS=doTileOptATLAS
jobproperties.TileRecFlags.doTileManyAmps=False

jobproperties.TileRecFlags.TileRunType=TileRunType
jobproperties.TileRecFlags.calibrateEnergy=TileCalibrateEnergy
jobproperties.TileRecFlags.correctTime=True; # apply time correction in physics runs
jobproperties.TileRecFlags.correctAmplitude=False; # don't do parabolic correction
jobproperties.TileRecFlags.OfcFromCOOL=OfcFromCOOL
